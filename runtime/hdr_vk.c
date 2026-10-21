/* Edicion mejorada, HDR: the conversion pass between the game's finished SDR picture and an HDR10 / scRGB swapchain.
 * Included at the end of nvn_vk_draw.c (like menu_vk.c): it uses V, R, vmem, image_barrier and the glslang DLL.
 *
 * Why a pass: vkCmdBlitImage cannot change colour space. In HDR mode the swapchain images (V.images, what is acquired and
 * presented) are not drawn on directly. The picture, the frame-time overlay and the Insert menu are composed exactly as in SDR
 * into V.tgt[i] (an 8-bit image of the swapchain's size, one per swapchain image), and one full-screen triangle then reads
 * V.tgt[i] and writes V.images[i] in the HDR encoding (hdr_color.h has the same maths in C, with tests). In SDR mode
 * V.tgt[i] == V.images[i] and nothing here runs. Any failure here leaves the game in SDR (H.disabled).
 *
 * Level 1 only: the SDR picture is converted correctly; the engine's own HDR (before its tone mapping) is not used. */
#include "hdr_win.h"
#include "hdr_color.h"

static struct {
    int disabled;                       /* a failure: SDR for the rest of the run */
    int active;                         /* the current swapchain is HDR */
    int mode;                           /* HDR_HDR10 | HDR_SCRGB while active */
    VkSurfaceFormatKHR sc;              /* format of the HDR swapchain while active */
    int req_mode;                       /* what the swapchain was made for (0 off, 1 auto, 2 hdr10, 3 scrgb), to see a change */
    int win_on_at_make;                 /* Windows' HDR state when it was made */
    HdrWinInfo win;                     /* last query */
    struct { int mode, sdr_white, peak, expand; } cfg;       /* live settings: the menu changes them, graphics.ini gives the start */
    int need_remake;                    /* poll found a different Windows HDR state or a different requested mode */
    LONG64 frames;
    VkPipeline pipe; VkFormat pipe_fmt; VkPipelineLayout pl; VkDescriptorSetLayout dsl; VkDescriptorPool dp; VkSampler smp;
    VkDescriptorSet ds[16];
    VkImage img[16]; VkImageView view[16]; VkImageView outv[16]; VkImage outi[16]; int made;   /* outv: views of the swapchain's own images */
    VMem mem[16];
    PFN_vkSetHdrMetadataEXT set_md;
    char shown[96];                     /* "HDR10 · PQ Rec.2020" for the menu and the launcher log */
} H;
static VkShaderModule menu_shader(int stage, const char *src);
static void hdr_selftest(void);

static float hdr_sdr_white(void) { return H.cfg.sdr_white >= 80 ? (float)H.cfg.sdr_white : H.win.sdr_white_nits >= 80 ? H.win.sdr_white_nits : 203.0f; }
static float hdr_peak(void) { return H.cfg.peak >= 400 ? (float)H.cfg.peak : H.win.max_nits >= 100 ? H.win.max_nits : 1000.0f; }
static void hdr_cfg_init(void) {
    static int done;
    if (done) return;
    done = 1;
    const GfxOptions *g = gfx_options();
    H.cfg.mode = g->hdr_mode; H.cfg.sdr_white = g->hdr_sdr_white; H.cfg.peak = g->hdr_peak; H.cfg.expand = g->hdr_expand;
    const char *e = getenv("SSBU_HDR");                                      /* TEST: off | auto | hdr10 | scrgb wins over the file */
    if (e) H.cfg.mode = !_stricmp(e, "auto") ? 1 : !_stricmp(e, "hdr10") ? 2 : !_stricmp(e, "scrgb") ? 3 : 0;
}

/* The menu's HDR settings are written back to graphics.ini (the four hdr keys; every other line is kept as it is). */
static void hdr_persist(void) {
    static const char *const k_mode[4] = {"off", "auto", "hdr10", "scrgb"};
    const char *path = gfx_options()->path;
    char key[4][32] = {"hdr", "hdr_sdr_white", "hdr_peak", "hdr_expand"}, val[4][16], *buf = NULL, *out;
    snprintf(val[0], 16, "%s", k_mode[H.cfg.mode & 3]); snprintf(val[1], 16, "%d", H.cfg.sdr_white); snprintf(val[2], 16, "%d", H.cfg.peak); snprintf(val[3], 16, "%d", H.cfg.expand);
    FILE *f = fopen(path, "rb"); long n = 0;
    if (f) { fseek(f, 0, SEEK_END); n = ftell(f); fseek(f, 0, SEEK_SET); buf = malloc((size_t)n + 1); if (buf && fread(buf, 1, (size_t)n, f) != (size_t)n) n = 0; fclose(f); }
    if (!buf) return;
    buf[n] = 0;
    out = malloc((size_t)n + 512); if (!out) { free(buf); return; }
    size_t o = 0; int done[4] = {0, 0, 0, 0};
    for (char *line = buf; *line; ) {
        char *e = strchr(line, '\n'); size_t len = e ? (size_t)(e - line) + 1 : strlen(line);
        int hit = -1;
        for (int k = 0; k < 4; k++) { size_t kl = strlen(key[k]); if (!strncmp(line, key[k], kl) && (line[kl] == ' ' || line[kl] == '=')) hit = k; }
        if (hit >= 0 && !done[hit]) { o += (size_t)sprintf(out + o, "%s = %s\r\n", key[hit], val[hit]); done[hit] = 1; }
        else { memcpy(out + o, line, len); o += len; }
        line += len;
    }
    for (int k = 0; k < 4; k++) if (!done[k]) o += (size_t)sprintf(out + o, "%s = %s\r\n", key[k], val[k]);
    f = fopen(path, "wb"); if (f) { fwrite(out, 1, o, f); fclose(f); }
    free(buf); free(out);
}

/* ---- pipeline */
static VkShaderModule hdr_shader(int stage, const char *src) { return menu_shader(stage, src); }
static void hdr_pipe_free(void) {
    if (H.pipe) { vkDestroyPipeline(V.device, H.pipe, NULL); H.pipe = NULL; }
}
static int hdr_pipe_make(VkFormat fmt) {
    if (H.pipe && H.pipe_fmt == fmt) return 1;
    hdr_pipe_free();
    if (!H.pl) {
        VkDescriptorSetLayoutBinding b = {0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, NULL};
        VkDescriptorSetLayoutCreateInfo dl = {VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO}; dl.bindingCount = 1; dl.pBindings = &b;
        if (vkCreateDescriptorSetLayout(V.device, &dl, NULL, &H.dsl) != VK_SUCCESS) return 0;
        VkDescriptorPoolSize ps = {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 16};
        VkDescriptorPoolCreateInfo dp = {VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO}; dp.maxSets = 16; dp.poolSizeCount = 1; dp.pPoolSizes = &ps;
        if (vkCreateDescriptorPool(V.device, &dp, NULL, &H.dp) != VK_SUCCESS) return 0;
        VkDescriptorSetLayout ls[16]; for (int i = 0; i < 16; i++) ls[i] = H.dsl;
        VkDescriptorSetAllocateInfo da = {VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO}; da.descriptorPool = H.dp; da.descriptorSetCount = 16; da.pSetLayouts = ls;
        if (vkAllocateDescriptorSets(V.device, &da, H.ds) != VK_SUCCESS) return 0;
        VkPushConstantRange pr = {VK_SHADER_STAGE_FRAGMENT_BIT, 0, 16};
        VkPipelineLayoutCreateInfo lc = {VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO}; lc.setLayoutCount = 1; lc.pSetLayouts = &H.dsl; lc.pushConstantRangeCount = 1; lc.pPushConstantRanges = &pr;
        if (vkCreatePipelineLayout(V.device, &lc, NULL, &H.pl) != VK_SUCCESS) return 0;
        VkSamplerCreateInfo sc = {VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO};
        sc.magFilter = sc.minFilter = VK_FILTER_NEAREST; sc.addressModeU = sc.addressModeV = sc.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        if (vkCreateSampler(V.device, &sc, NULL, &H.smp) != VK_SUCCESS) return 0;
    }
    static const char *vs = "#version 460\nvoid main(){ vec2 p = vec2((gl_VertexIndex << 1) & 2, gl_VertexIndex & 2); gl_Position = vec4(p * 2.0 - 1.0, 0.0, 1.0); }\n";
    /* the same maths as hdr_color.h (hdr_convert); push constants: x = mode (2 HDR10, 3 scRGB), y = SDR white nits, z = peak nits, w = expansion 0..1 */
    static const char *fs = "#version 460\nlayout(set = 0, binding = 0) uniform sampler2D tex;\n"
        "layout(push_constant) uniform P { vec4 k; } pc;\nlayout(location = 0) out vec4 o;\n"
        "float srgb_lin(float c){ return c <= 0.04045 ? c / 12.92 : pow((c + 0.055) / 1.055, 2.4); }\n"
        "float pq(float n){ const float m1 = 2610.0/16384.0, m2 = 2523.0/4096.0*128.0, c1 = 3424.0/4096.0, c2 = 2413.0/4096.0*32.0, c3 = 2392.0/4096.0*32.0;\n"
        "  float y = pow(clamp(n / 10000.0, 0.0, 1.0), m1); return pow((c1 + c2 * y) / (1.0 + c3 * y), m2); }\n"
        "void main(){ vec3 c = clamp(texelFetch(tex, ivec2(gl_FragCoord.xy), 0).rgb, 0.0, 1.0);\n"
        "  vec3 l = vec3(srgb_lin(c.r), srgb_lin(c.g), srgb_lin(c.b));\n"
        "  float y = dot(l, vec3(0.2126, 0.7152, 0.0722));\n"
        "  if (pc.k.w > 0.0) { float t = clamp((y - 0.5) * 2.0, 0.0, 1.0); l *= 1.0 + pc.k.w * t * t * max(pc.k.z / pc.k.y - 1.0, 0.0); }\n"
        "  l *= pc.k.y;\n"
        "  if (pc.k.x < 2.5) {\n"
        "    vec3 w = vec3(dot(l, vec3(0.627404, 0.329283, 0.043313)), dot(l, vec3(0.069097, 0.919540, 0.011362)), dot(l, vec3(0.016391, 0.088013, 0.895595)));\n"
        "    o = vec4(pq(max(w.r, 0.0)), pq(max(w.g, 0.0)), pq(max(w.b, 0.0)), 1.0);\n"
        "  } else o = vec4(l / 80.0, 1.0); }\n";
    VkShaderModule v = hdr_shader(0, vs), f = hdr_shader(1, fs);
    if (!v || !f) { if (v) vkDestroyShaderModule(V.device, v, NULL); if (f) vkDestroyShaderModule(V.device, f, NULL); return 0; }
    VkPipelineShaderStageCreateInfo st[2] = {{VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO}, {VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO}};
    st[0].stage = VK_SHADER_STAGE_VERTEX_BIT; st[0].module = v; st[0].pName = "main";
    st[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT; st[1].module = f; st[1].pName = "main";
    VkPipelineVertexInputStateCreateInfo vi = {VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO};
    VkPipelineInputAssemblyStateCreateInfo ia = {VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO}; ia.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    VkPipelineViewportStateCreateInfo vp = {VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO}; vp.viewportCount = vp.scissorCount = 1;
    VkPipelineRasterizationStateCreateInfo rs = {VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO}; rs.cullMode = VK_CULL_MODE_NONE; rs.lineWidth = 1.0f;
    VkPipelineMultisampleStateCreateInfo ms = {VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO}; ms.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    VkPipelineColorBlendAttachmentState ba; memset(&ba, 0, sizeof ba); ba.colorWriteMask = 0xf;
    VkPipelineColorBlendStateCreateInfo cb = {VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO}; cb.attachmentCount = 1; cb.pAttachments = &ba;
    VkDynamicState dyn[2] = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
    VkPipelineDynamicStateCreateInfo ds = {VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO}; ds.dynamicStateCount = 2; ds.pDynamicStates = dyn;
    VkPipelineRenderingCreateInfo rinfo = {VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO}; rinfo.colorAttachmentCount = 1; rinfo.pColorAttachmentFormats = &fmt;
    VkGraphicsPipelineCreateInfo gi = {VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO};
    gi.pNext = &rinfo; gi.stageCount = 2; gi.pStages = st; gi.pVertexInputState = &vi; gi.pInputAssemblyState = &ia; gi.pViewportState = &vp;
    gi.pRasterizationState = &rs; gi.pMultisampleState = &ms; gi.pColorBlendState = &cb; gi.pDynamicState = &ds; gi.layout = H.pl;
    VkResult r = vkCreateGraphicsPipelines(V.device, NULL, 1, &gi, NULL, &H.pipe);
    vkDestroyShaderModule(V.device, v, NULL); vkDestroyShaderModule(V.device, f, NULL);
    if (r != VK_SUCCESS) { printf("hdr: vkCreateGraphicsPipelines: %s\n", vk_result_name(r)); H.pipe = NULL; return 0; }
    H.pipe_fmt = fmt;
    return 1;
}

/* ---- choosing the swapchain format (swapchain_create, before vkCreateSwapchainKHR). Returns 1 and the format when HDR is to be used. */
static int hdr_has_format(const VkSurfaceFormatKHR *f, uint32_t n, VkFormat fmt, VkColorSpaceKHR cs) {
    for (uint32_t i = 0; i < n; i++) if (f[i].format == fmt && f[i].colorSpace == cs) return 1;
    return 0;
}
static int hdr_choose(VkSurfaceFormatKHR *out) {
    hdr_cfg_init();
    H.active = 0;
    if (!V.alive) return 0;             /* the first swapchain is made inside nvn_vk_init, before the drawing part's Vulkan calls are loaded: SDR; hdr_poll sees the mode was not served and remakes it at the first present */
    H.req_mode = H.cfg.mode;
    if (H.disabled || H.cfg.mode == 0) return 0;
    hdr_win_query(V.hwnd, &H.win);
    H.win_on_at_make = H.win.ok && H.win.hdr_on;
    if (!H.win.ok || !H.win.hdr_on) {
        static int said; if (said != 1 + H.win.ok) { said = 1 + H.win.ok; printf("hdr: %s: SDR\n", H.win.ok ? "Windows has HDR off for this monitor" : "the monitor's HDR state cannot be read"); fflush(stdout); }
        return 0;
    }
    VkSurfaceFormatKHR f[64]; uint32_t n = 64;
    VkResult r = vkGetPhysicalDeviceSurfaceFormatsKHR(V.gpu, V.surface, &n, f);
    if (r != VK_SUCCESS && r != VK_INCOMPLETE) return 0;
    int can10 = hdr_has_format(f, n, VK_FORMAT_A2B10G10R10_UNORM_PACK32, VK_COLOR_SPACE_HDR10_ST2084_EXT);
    int canf = hdr_has_format(f, n, VK_FORMAT_R16G16B16A16_SFLOAT, VK_COLOR_SPACE_EXTENDED_SRGB_LINEAR_EXT);
    int want = H.cfg.mode == 1 ? (can10 ? HDR_HDR10 : canf ? HDR_SCRGB : 0) : H.cfg.mode == 2 ? (can10 ? HDR_HDR10 : 0) : (canf ? HDR_SCRGB : 0);
    if (!want) { printf("hdr: hdr = %s but the surface offers %s%s: SDR\n", H.cfg.mode == 1 ? "auto" : H.cfg.mode == 2 ? "hdr10" : "scrgb", can10 ? "HDR10 " : "", canf ? "scRGB" : "no HDR format"); fflush(stdout); return 0; }
    VkFormat fmt = want == HDR_HDR10 ? VK_FORMAT_A2B10G10R10_UNORM_PACK32 : VK_FORMAT_R16G16B16A16_SFLOAT;
    /* the formats the pass and the clears need on this GPU */
    if (!fmt_supported(fmt, VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT) || !fmt_supported(V.format.format, VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT)) { printf("hdr: the GPU cannot render to %d: SDR\n", (int)fmt); fflush(stdout); return 0; }
    if (!hdr_pipe_make(fmt)) { H.disabled = 1; printf("hdr: the conversion pipeline could not be made: SDR for this run\n"); fflush(stdout); return 0; }
    out->format = fmt; out->colorSpace = want == HDR_HDR10 ? VK_COLOR_SPACE_HDR10_ST2084_EXT : VK_COLOR_SPACE_EXTENDED_SRGB_LINEAR_EXT;
    H.sc = *out; H.mode = want; H.active = 1;
    return 1;
}

/* ---- the 8-bit images the picture is composed into (one per swapchain image), made after the swapchain's images are known */
static void hdr_images_free(void) {
    for (int i = 0; i < 16; i++) {
        if (H.view[i]) { vkDestroyImageView(V.device, H.view[i], NULL); H.view[i] = NULL; }
        if (H.outv[i]) { vkDestroyImageView(V.device, H.outv[i], NULL); H.outv[i] = NULL; H.outi[i] = NULL; }
        if (H.img[i]) { vkDestroyImage(V.device, H.img[i], NULL); H.img[i] = NULL; vmem_free(H.mem[i]); memset(&H.mem[i], 0, sizeof H.mem[i]); }
    }
    H.made = 0;
}
static void hdr_set_metadata(void) {
    if (!H.set_md || H.mode != HDR_HDR10 || !H.win.ok) return;       /* scRGB carries no metadata; HDR10 without it is valid */
    VkHdrMetadataEXT md = {VK_STRUCTURE_TYPE_HDR_METADATA_EXT};
    md.displayPrimaryRed.x = H.win.prim[0]; md.displayPrimaryRed.y = H.win.prim[1]; md.displayPrimaryGreen.x = H.win.prim[2]; md.displayPrimaryGreen.y = H.win.prim[3];
    md.displayPrimaryBlue.x = H.win.prim[4]; md.displayPrimaryBlue.y = H.win.prim[5]; md.whitePoint.x = H.win.prim[6]; md.whitePoint.y = H.win.prim[7];
    md.maxLuminance = hdr_peak(); md.minLuminance = H.win.min_nits; md.maxContentLightLevel = hdr_sdr_white(); md.maxFrameAverageLightLevel = hdr_sdr_white();
    H.set_md(V.device, 1, &V.swapchain, &md);
}
/* swapchain_create, after its images exist. Returns 0 when the HDR path cannot be set up: the caller then makes an SDR swapchain. */
static int hdr_swapchain_made(void) {
    if (!H.active) { for (uint32_t i = 0; i < V.image_count; i++) V.tgt[i] = V.images[i]; return 1; }
    hdr_images_free();
    for (uint32_t i = 0; i < V.image_count && i < 16; i++) {
        VkImageCreateInfo ci = {VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO};
        ci.flags = VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT;               /* made as the window's 8-bit class, read as UNORM: the stored code values, not decoded */
        ci.imageType = VK_IMAGE_TYPE_2D; ci.format = V.format.format; ci.extent.width = V.extent.width; ci.extent.height = V.extent.height; ci.extent.depth = 1;
        ci.mipLevels = 1; ci.arrayLayers = 1; ci.samples = VK_SAMPLE_COUNT_1_BIT; ci.tiling = VK_IMAGE_TILING_OPTIMAL;
        ci.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; ci.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        if (vkCreateImage(V.device, &ci, NULL, &H.img[i]) != VK_SUCCESS) { H.img[i] = NULL; hdr_images_free(); return 0; }
        VkMemoryRequirements rq; vkGetImageMemoryRequirements(V.device, H.img[i], &rq);
        H.mem[i] = vmem_alloc(rq, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 0);
        if (vkBindImageMemory(V.device, H.img[i], H.mem[i].mem, H.mem[i].off) != VK_SUCCESS) { hdr_images_free(); return 0; }
        VkImageViewCreateInfo vc = {VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
        vc.image = H.img[i]; vc.viewType = VK_IMAGE_VIEW_TYPE_2D; vc.format = VK_FORMAT_B8G8R8A8_UNORM; vc.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT; vc.subresourceRange.levelCount = 1; vc.subresourceRange.layerCount = 1;
        if (vkCreateImageView(V.device, &vc, NULL, &H.view[i]) != VK_SUCCESS) { hdr_images_free(); return 0; }
        VkDescriptorImageInfo ii = {H.smp, H.view[i], VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL};
        VkWriteDescriptorSet wd = {VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET}; wd.dstSet = H.ds[i]; wd.descriptorCount = 1; wd.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER; wd.pImageInfo = &ii;
        vkUpdateDescriptorSets(V.device, 1, &wd, 0, NULL);
        V.tgt[i] = H.img[i];
    }
    H.made = 1;
    if (!H.set_md && g_hdr_has_metadata) H.set_md = (PFN_vkSetHdrMetadataEXT)vkGetDeviceProcAddr(V.device, "vkSetHdrMetadataEXT");
    hdr_set_metadata();
    { static int st; if (!st && getenv("SSBU_HDR_SELFTEST")) { st = 1; hdr_selftest(); hdr_persist(); } }      /* TEST: the selftest also writes the settings back (point SSBU_GRAPHICS_CONFIG at a copy) */
    snprintf(H.shown, sizeof H.shown, "%s", H.mode == HDR_HDR10 ? "HDR10 · PQ · Rec.2020 · 10 bits" : "scRGB · lineal · FP16");
    printf("hdr: ACTIVE %s (swapchain format %d, colour space %d); SDR white %.0f nits, peak %.0f nits, expansion %d %%; monitor %s max %.0f nits; metadata %s\n", H.shown, (int)H.sc.format, (int)H.sc.colorSpace,
           hdr_sdr_white(), hdr_peak(), H.cfg.expand, H.win.device, H.win.max_nits, H.set_md && H.mode == HDR_HDR10 ? "sent" : "not sent");
    fflush(stdout);
    return 1;
}
/* swapchain_destroy, after the device is idle */
static void hdr_swapchain_gone(void) { hdr_images_free(); H.active = 0; for (int i = 0; i < 16; i++) V.tgt[i] = NULL; }
/* the swapchain could not be made in HDR: SDR for the rest of the run */
static void hdr_fail(const char *why) { H.disabled = 1; H.active = 0; printf("hdr: %s: SDR for this run\n", why); fflush(stdout); }

/* ---- once per presented frame, before the image is acquired: Windows' HDR state or the requested mode changed -> the caller makes the swapchain again */
static int hdr_poll(void) {
    hdr_cfg_init();
    if (H.cfg.mode != H.req_mode) { H.disabled = 0; return 1; }       /* the menu changed the mode (this also tries again after a failure) */
    if (H.cfg.mode == 0 || H.disabled || ++H.frames % 120) return 0;  /* Windows is asked every 2 s at 60 fps */
    HdrWinInfo w; hdr_win_query(V.hwnd, &w);
    if (!w.ok) return 0;
    H.win.sdr_white_nits = w.sdr_white_nits;                          /* the slider in Windows' settings: followed live */
    return (w.hdr_on != 0) != (H.win_on_at_make != 0);
}

#include "hdr_selftest.inc"

/* ---- the pass: V.tgt[index] (TRANSFER_DST, composed) -> V.images[index] (UNDEFINED -> PRESENT_SRC). Only when H.active. */
static void hdr_pass(uint32_t index) {
    image_barrier(V.tgt[index], VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    image_barrier(V.images[index], VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    VkImageViewCreateInfo vc = {VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};      /* the swapchain image's own view: the cache below is per index and kept until the swapchain goes */
    if (H.outi[index] != V.images[index] && H.outv[index]) { vkDestroyImageView(V.device, H.outv[index], NULL); H.outv[index] = NULL; }
    if (!H.outv[index]) {
        vc.image = V.images[index]; vc.viewType = VK_IMAGE_VIEW_TYPE_2D; vc.format = H.sc.format; vc.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT; vc.subresourceRange.levelCount = 1; vc.subresourceRange.layerCount = 1;
        VKC(vkCreateImageView(V.device, &vc, NULL, &H.outv[index])); H.outi[index] = V.images[index];
    }
    VkRenderingAttachmentInfo ca = {VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO};
    ca.imageView = H.outv[index]; ca.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL; ca.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE; ca.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    VkRenderingInfo ri = {VK_STRUCTURE_TYPE_RENDERING_INFO};
    ri.renderArea.extent = V.extent; ri.layerCount = 1; ri.colorAttachmentCount = 1; ri.pColorAttachments = &ca;
    vkCmdBeginRendering(R.cmd, &ri);
    VkViewport vpt = {0, 0, (float)V.extent.width, (float)V.extent.height, 0, 1};
    VkRect2D scr = {{0, 0}, V.extent};
    vkCmdSetViewport(R.cmd, 0, 1, &vpt); vkCmdSetScissor(R.cmd, 0, 1, &scr);
    vkCmdBindPipeline(R.cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, H.pipe);
    vkCmdBindDescriptorSets(R.cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, H.pl, 0, 1, &H.ds[index], 0, NULL);
    float pcv[4] = {(float)H.mode, hdr_sdr_white(), hdr_peak(), (float)H.cfg.expand / 100.0f};
    vkCmdPushConstants(R.cmd, H.pl, VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof pcv, pcv);
    vkCmdDraw(R.cmd, 3, 1, 0, 0);
    vkCmdEndRendering(R.cmd);
    image_barrier(V.images[index], VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
    R.pipe_bound = NULL; R.dyn_dirty = 1;                    /* the game's own state tracking must bind again */
}
