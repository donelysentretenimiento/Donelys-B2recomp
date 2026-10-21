/* SSBU_HDR_SELFTEST=1 (included by hdr_vk.c): the real conversion pipeline on 12 known pixels, the result read back from the GPU and
 * compared with hdr_convert (hdr_color.h). Prints "hdr: SELFTEST PASS|FAIL". Uses its own command buffer, fence and images. */
static float hdr_half(uint16_t h) { int e = (h >> 10) & 31, m = h & 1023; float v = e == 0 ? ldexpf((float)m, -24) : ldexpf((float)(m + 1024), e - 25); return h & 0x8000 ? -v : v; }
static void hdr_selftest(void) {
    enum { N = 12 };
    static const uint8_t px[N][3] = {{0,0,0},{16,16,16},{32,32,32},{64,64,64},{128,128,128},{192,192,192},{255,255,255},{255,0,0},{0,255,0},{0,0,255},{200,100,50},{10,200,230}};
    VMem im, om, sm_, rm; VkImage in, out; VkImageView iv, ov; VkBuffer sb, rb;
    VkFormat ofmt = H.sc.format; int bpp = ofmt == VK_FORMAT_R16G16B16A16_SFLOAT ? 8 : 4;
    VkImageCreateInfo ci = {VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO}; ci.imageType = VK_IMAGE_TYPE_2D; ci.extent.width = N; ci.extent.height = 1; ci.extent.depth = 1; ci.mipLevels = ci.arrayLayers = 1; ci.samples = VK_SAMPLE_COUNT_1_BIT; ci.tiling = VK_IMAGE_TILING_OPTIMAL;
    ci.format = VK_FORMAT_B8G8R8A8_UNORM; ci.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT; VKC(vkCreateImage(V.device, &ci, NULL, &in));
    VkMemoryRequirements rq; vkGetImageMemoryRequirements(V.device, in, &rq); im = vmem_alloc(rq, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 0); VKC(vkBindImageMemory(V.device, in, im.mem, im.off));
    ci.format = ofmt; ci.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT; VKC(vkCreateImage(V.device, &ci, NULL, &out));
    vkGetImageMemoryRequirements(V.device, out, &rq); om = vmem_alloc(rq, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 0); VKC(vkBindImageMemory(V.device, out, om.mem, om.off));
    VkImageViewCreateInfo vc = {VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO}; vc.viewType = VK_IMAGE_VIEW_TYPE_2D; vc.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT; vc.subresourceRange.levelCount = vc.subresourceRange.layerCount = 1;
    vc.image = in; vc.format = VK_FORMAT_B8G8R8A8_UNORM; VKC(vkCreateImageView(V.device, &vc, NULL, &iv)); vc.image = out; vc.format = ofmt; VKC(vkCreateImageView(V.device, &vc, NULL, &ov));
    sb = vbuf_new(N * 4, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, &sm_); rb = vbuf_new(N * bpp, VK_BUFFER_USAGE_TRANSFER_DST_BIT, &rm);
    for (int i = 0; i < N; i++) { uint8_t *d = (uint8_t *)sm_.map + i * 4; d[0] = px[i][2]; d[1] = px[i][1]; d[2] = px[i][0]; d[3] = 255; }      /* BGRA */
    VkDescriptorImageInfo ii = {H.smp, iv, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL};
    VkWriteDescriptorSet wd = {VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET}; wd.dstSet = H.ds[15]; wd.descriptorCount = 1; wd.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER; wd.pImageInfo = &ii; vkUpdateDescriptorSets(V.device, 1, &wd, 0, NULL);
    VkCommandBuffer cb; VkCommandBufferAllocateInfo ai = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO}; ai.commandPool = V.pool; ai.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY; ai.commandBufferCount = 1; VKC(vkAllocateCommandBuffers(V.device, &ai, &cb));
    VkFenceCreateInfo fi = {VK_STRUCTURE_TYPE_FENCE_CREATE_INFO}; VkFence fence; VKC(vkCreateFence(V.device, &fi, NULL, &fence));
    float maxerr = 0; int fails = 0;
    for (int pass = 0; pass < 2; pass++) {
        float expand = pass ? 1.0f : 0.0f;
        VkCommandBufferBeginInfo bi = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO}; bi.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        VKC(vkResetCommandBuffer(cb, 0)); VKC(vkBeginCommandBuffer(cb, &bi));
        VkImageMemoryBarrier b = {VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER}; b.srcQueueFamilyIndex = b.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED; b.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT; b.subresourceRange.levelCount = b.subresourceRange.layerCount = 1;
#define HBAR(img_, ol, nl, sa, da) b.image = img_; b.oldLayout = ol; b.newLayout = nl; b.srcAccessMask = sa; b.dstAccessMask = da; vkCmdPipelineBarrier(cb, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, NULL, 0, NULL, 1, &b)
        HBAR(in, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 0, VK_ACCESS_TRANSFER_WRITE_BIT);
        VkBufferImageCopy bc; memset(&bc, 0, sizeof bc); bc.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT; bc.imageSubresource.layerCount = 1; bc.imageExtent.width = N; bc.imageExtent.height = 1; bc.imageExtent.depth = 1;
        vkCmdCopyBufferToImage(cb, sb, in, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &bc);
        HBAR(in, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT);
        HBAR(out, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, 0, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT);
        VkRenderingAttachmentInfo ca = {VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO}; ca.imageView = ov; ca.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL; ca.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE; ca.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        VkRenderingInfo ri = {VK_STRUCTURE_TYPE_RENDERING_INFO}; ri.renderArea.extent.width = N; ri.renderArea.extent.height = 1; ri.layerCount = 1; ri.colorAttachmentCount = 1; ri.pColorAttachments = &ca;
        vkCmdBeginRendering(cb, &ri);
        VkViewport vpt = {0, 0, N, 1, 0, 1}; VkRect2D scr = {{0, 0}, {N, 1}}; vkCmdSetViewport(cb, 0, 1, &vpt); vkCmdSetScissor(cb, 0, 1, &scr);
        vkCmdBindPipeline(cb, VK_PIPELINE_BIND_POINT_GRAPHICS, H.pipe); vkCmdBindDescriptorSets(cb, VK_PIPELINE_BIND_POINT_GRAPHICS, H.pl, 0, 1, &H.ds[15], 0, NULL);
        float pcv[4] = {(float)H.mode, hdr_sdr_white(), hdr_peak(), expand}; vkCmdPushConstants(cb, H.pl, VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof pcv, pcv);
        vkCmdDraw(cb, 3, 1, 0, 0); vkCmdEndRendering(cb);
        HBAR(out, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, VK_ACCESS_TRANSFER_READ_BIT);
        vkCmdCopyImageToBuffer(cb, out, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, rb, 1, &bc);
        VKC(vkEndCommandBuffer(cb));
        VkSubmitInfo si = {VK_STRUCTURE_TYPE_SUBMIT_INFO}; si.commandBufferCount = 1; si.pCommandBuffers = &cb;
        VKC(vkQueueSubmit(V.queue, 1, &si, fence)); VKC(vkWaitForFences(V.device, 1, &fence, VK_TRUE, 5000000000ull)); VKC(vkResetFences(V.device, 1, &fence));
        for (int i = 0; i < N; i++) {
            float in3[3] = {px[i][0] / 255.0f, px[i][1] / 255.0f, px[i][2] / 255.0f}, want[3], got[3];
            hdr_convert(H.mode, in3, hdr_sdr_white(), hdr_peak(), expand, want);
            const uint8_t *d = (const uint8_t *)rm.map + i * bpp;
            for (int c = 0; c < 3; c++) {
                if (bpp == 8) got[c] = hdr_half(((const uint16_t *)d)[c]);
                else { uint32_t v = *(const uint32_t *)d; got[c] = (float)((v >> (10 * c)) & 1023) / 1023.0f; }      /* A2B10G10R10: R in bits 0..9 */
                float tol = bpp == 8 ? 3e-3f * want[c] + 3e-4f : 1.6e-3f, err = fabsf(got[c] - want[c]);
                if (err > maxerr) maxerr = err;
                if (err > tol) { fails++; printf("hdr: selftest pixel %d (%d,%d,%d) expand %.0f ch %d: GPU %.5f, expected %.5f\n", i, px[i][0], px[i][1], px[i][2], expand, c, got[c], want[c]); }
            }
        }
    }
    printf("hdr: SELFTEST %s (%s, 12 pixels x 2 expansions, max error %.6f)\n", fails ? "FAIL" : "PASS", H.mode == HDR_HDR10 ? "HDR10" : "scRGB", maxerr); fflush(stdout);
    vkDestroyFence(V.device, fence, NULL); vkDestroyImageView(V.device, iv, NULL); vkDestroyImageView(V.device, ov, NULL); vkDestroyImage(V.device, in, NULL); vkDestroyImage(V.device, out, NULL);
    vmem_free(im); vmem_free(om); vkDestroyBuffer(V.device, sb, NULL); vkDestroyBuffer(V.device, rb, NULL); vmem_free(sm_); vmem_free(rm);
}
