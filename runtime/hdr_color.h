/* Edicion mejorada, HDR: the colour maths of the conversion pass, in C so that tests/hdr_color_test.c can check it with known
 * values. The fragment shader in hdr_vk.c is the same maths in GLSL (same constants, same order); keep the two in step.
 *
 * Input is the game's finished SDR picture as the screen got it: encoded sRGB code values 0..1. Output is
 *   HDR10: PQ (ST 2084) of the Rec.2020 light, 1.0 = 10000 nits;
 *   scRGB: linear Rec.709 light, 1.0 = 80 nits (the Windows convention). */
#ifndef HDR_COLOR_H
#define HDR_COLOR_H
#include <math.h>
enum { HDR_HDR10 = 2, HDR_SCRGB = 3 };
typedef struct { float sdr_white_nits, peak_nits; } HdrLevels;
typedef struct { float max_luminance, min_luminance, max_cll, max_fall; } HdrMetadataValues;
/* Resolve user overrides and Windows' monitor report without assuming a 400-nit minimum.
 * SDR white is constrained to the resolved peak so highlight expansion cannot invert its range. */
static inline HdrLevels hdr_resolve_levels(float cfg_white, float cfg_peak, float win_white, float win_peak) {
    HdrLevels v;
    v.peak_nits = cfg_peak >= 80.0f && cfg_peak <= 10000.0f ? cfg_peak
                : win_peak >= 80.0f && win_peak <= 10000.0f ? win_peak : 1000.0f;
    v.sdr_white_nits = cfg_white >= 80.0f && cfg_white <= 500.0f ? cfg_white
                     : win_white >= 80.0f && win_white <= 500.0f ? win_white : 203.0f;
    if (v.sdr_white_nits > v.peak_nits) v.sdr_white_nits = v.peak_nits;
    return v;
}
/* VkHdrMetadataEXT separates display capabilities from content mastering data. The converted
 * game's content has no measured MaxCLL/MaxFALL, so report those optional fields as unknown (0). */
static inline HdrMetadataValues hdr_metadata_values(float display_peak, float display_min) {
    HdrMetadataValues v = {0};
    if (display_peak > 0.0f && display_peak <= 10000.0f) v.max_luminance = display_peak;
    if (display_min >= 0.0f && display_min <= v.max_luminance) v.min_luminance = display_min;
    return v;
}
static inline float hdr_srgb_to_linear(float c) { return c <= 0.04045f ? c / 12.92f : powf((c + 0.055f) / 1.055f, 2.4f); }
static inline float hdr_pq_encode(float nits) {              /* ST 2084 inverse EOTF; nits >= 0 */
    const float m1 = 2610.0f / 16384.0f, m2 = 2523.0f / 4096.0f * 128.0f, c1 = 3424.0f / 4096.0f, c2 = 2413.0f / 4096.0f * 32.0f, c3 = 2392.0f / 4096.0f * 32.0f;
    float y = powf(nits < 0 ? 0 : nits > 10000.0f ? 1.0f : nits / 10000.0f, m1);
    return powf((c1 + c2 * y) / (1.0f + c3 * y), m2);
}
static inline float hdr_pq_decode(float e) {                 /* ST 2084 EOTF: nits */
    const float m1 = 2610.0f / 16384.0f, m2 = 2523.0f / 4096.0f * 128.0f, c1 = 3424.0f / 4096.0f, c2 = 2413.0f / 4096.0f * 32.0f, c3 = 2392.0f / 4096.0f * 32.0f;
    float p = powf(e < 0 ? 0 : e > 1 ? 1 : e, 1.0f / m2), n = p - c1 < 0 ? 0 : p - c1;
    return 10000.0f * powf(n / (c2 - c3 * p), 1.0f / m1);
}
/* Rec.709 -> Rec.2020 (ITU-R BT.2087, linear light) */
static inline void hdr_709_to_2020(const float in[3], float out[3]) {
    out[0] = 0.627404f * in[0] + 0.329283f * in[1] + 0.043313f * in[2];
    out[1] = 0.069097f * in[0] + 0.919540f * in[1] + 0.011362f * in[2];
    out[2] = 0.016391f * in[0] + 0.088013f * in[1] + 0.895595f * in[2];
}
/* Optional highlight expansion: only light above half of SDR white is lifted, smoothly (squared), by the same factor on the
 * three channels (the hue stays); expand 0..1; the result never goes above peak nits. */
static inline float hdr_expand_gain(float luma, float expand, float sdr_white, float peak) {
    float t = (luma - 0.5f) * 2.0f; t = t < 0 ? 0 : t > 1 ? 1 : t;
    float room = peak / sdr_white - 1.0f; if (room < 0) room = 0;
    return 1.0f + expand * t * t * room;
}
/* encoded sRGB rgb[3] -> out[3] for the mode (HDR_HDR10 | HDR_SCRGB) */
static inline void hdr_convert(int mode, const float rgb[3], float sdr_white, float peak, float expand, float out[3]) {
    float lin[3], y;
    for (int i = 0; i < 3; i++) lin[i] = hdr_srgb_to_linear(rgb[i] < 0 ? 0 : rgb[i] > 1 ? 1 : rgb[i]);      /* the game's picture is 0..1: out of range is clamped */
    y = 0.2126f * lin[0] + 0.7152f * lin[1] + 0.0722f * lin[2];
    float g = expand > 0 ? hdr_expand_gain(y, expand, sdr_white, peak) : 1.0f;
    for (int i = 0; i < 3; i++) lin[i] *= g * sdr_white;       /* now nits */
    if (mode == HDR_SCRGB) { for (int i = 0; i < 3; i++) out[i] = lin[i] / 80.0f; return; }
    float w[3]; hdr_709_to_2020(lin, w);
    for (int i = 0; i < 3; i++) out[i] = hdr_pq_encode(w[i] < 0 ? 0 : w[i]);
}
#endif
