/* Known-value tests of runtime/hdr_color.h. Build and run: scripts\test_hdr_color.bat (needs only deps\llvm-mingw). */
#include <stdio.h>
#include "../runtime/hdr_color.h"
static int fails, n;
static void near_(const char *what, float got, float want, float tol) { n++; if (fabsf(got - want) > tol) { fails++; printf("FAIL %s: %.6f, expected %.6f (+-%g)\n", what, got, want, tol); } }
int main(void) {
    HdrLevels levels = hdr_resolve_levels(203.0f, 329.0f, 240.0f, 344.0f);
    near_("configured peak accepts 329-nit display", levels.peak_nits, 329.0f, 1e-6f);
    near_("white never exceeds configured peak", levels.sdr_white_nits, 203.0f, 1e-6f);
    levels = hdr_resolve_levels(340.0f, 329.0f, 240.0f, 344.0f);
    near_("white is clamped to physical peak", levels.sdr_white_nits, 329.0f, 1e-6f);
    levels = hdr_resolve_levels(0.0f, 0.0f, 240.0f, 329.0f);
    near_("automatic peak uses Windows 329-nit report", levels.peak_nits, 329.0f, 1e-6f);
    HdrMetadataValues metadata = hdr_metadata_values(329.0f, 0.01f);
    near_("metadata display peak", metadata.max_luminance, 329.0f, 1e-6f);
    near_("unknown content MaxCLL is zero", metadata.max_cll, 0.0f, 1e-6f);
    near_("unknown content MaxFALL is zero", metadata.max_fall, 0.0f, 1e-6f);
    near_("srgb 0", hdr_srgb_to_linear(0), 0, 1e-7f);
    near_("srgb 1", hdr_srgb_to_linear(1), 1, 1e-6f);
    near_("srgb 0.5", hdr_srgb_to_linear(0.5f), 0.214041f, 1e-5f);
    near_("srgb toe 0.02", hdr_srgb_to_linear(0.02f), 0.02f / 12.92f, 1e-7f);
    near_("pq 0", hdr_pq_encode(0), 0, 1e-6f);
    near_("pq 10000", hdr_pq_encode(10000), 1, 1e-5f);
    near_("pq 100", hdr_pq_encode(100), 0.508078f, 1e-4f);              /* ITU-R BT.2408: 100 nits = 0.508 */
    near_("pq 203", hdr_pq_encode(203), 0.5807f, 1e-3f);                /* BT.2408 reference white 203 nits = 58 % */
    near_("pq 1000", hdr_pq_encode(1000), 0.7518f, 1e-3f);
    for (float nits = 0.5f; nits < 10000.0f; nits *= 3.7f) near_("pq roundtrip", hdr_pq_decode(hdr_pq_encode(nits)) / nits, 1.0f, 2e-3f);
    float w[3] = {1, 1, 1}, o[3];
    hdr_709_to_2020(w, o);
    near_("709->2020 white r", o[0], 1, 1e-4f); near_("709->2020 white g", o[1], 1, 1e-4f); near_("709->2020 white b", o[2], 1, 1e-4f);
    float red[3] = {1, 0, 0}; hdr_709_to_2020(red, o);
    near_("709 red -> 2020 r", o[0], 0.627404f, 1e-6f); near_("709 red -> 2020 g", o[1], 0.069097f, 1e-6f);
    float wh[3] = {1, 1, 1}, bk[3] = {0, 0, 0}, mid[3] = {0.5f, 0.5f, 0.5f};
    hdr_convert(HDR_SCRGB, wh, 203, 1000, 0, o);   near_("scrgb white 203", o[0], 203.0f / 80.0f, 1e-5f);
    hdr_convert(HDR_SCRGB, wh, 80, 1000, 0, o);    near_("scrgb white 80 = 1.0", o[1], 1.0f, 1e-6f);
    hdr_convert(HDR_SCRGB, bk, 203, 1000, 0, o);   near_("scrgb black", o[2], 0, 1e-9f);
    hdr_convert(HDR_SCRGB, mid, 100, 1000, 0, o);  near_("scrgb mid", o[0], 0.214041f * 100.0f / 80.0f, 1e-4f);
    hdr_convert(HDR_HDR10, wh, 203, 1000, 0, o);   near_("hdr10 white 203 = pq", o[0], hdr_pq_encode(203), 1e-4f); near_("hdr10 white neutral", o[0] - o[2], 0, 1e-4f);
    hdr_convert(HDR_HDR10, bk, 203, 1000, 0, o);   near_("hdr10 black", o[0], 0, 1e-6f);
    hdr_convert(HDR_SCRGB, wh, 100, 1000, 1.0f, o); near_("expand full: white reaches peak", o[0] * 80.0f, 1000.0f, 0.5f);
    hdr_convert(HDR_SCRGB, mid, 100, 1000, 1.0f, o); near_("expand leaves mid tones alone", o[0], 0.214041f * 100.0f / 80.0f, 1e-4f);
    float c[3] = {1.0f, 0.5f, 0.0f}; hdr_convert(HDR_SCRGB, c, 100, 1000, 0, o);
    near_("out-of-order channels keep their ratio", o[1] / o[0], hdr_srgb_to_linear(0.5f), 1e-4f);
    float over[3] = {2.0f, -1.0f, 1.0f}; hdr_convert(HDR_SCRGB, over, 80, 1000, 0, o);
    near_("clamps >1", o[0], 1.0f, 1e-6f); near_("clamps <0", o[1], 0.0f, 1e-9f);
    printf("hdr_color_test: %d checks, %d failed\n", n, fails);
    return fails != 0;
}
