/* Edicion mejorada, HDR: what Windows says about the monitor a window is on (hdr_win.c). */
#ifndef HDR_WIN_H
#define HDR_WIN_H
#include <windows.h>
#include <math.h>
#include <string.h>
typedef struct {
    int ok;                       /* the monitor was found through DXGI */
    int hdr_on;                   /* the desktop of that monitor is in HDR (DXGI colour space G2084 / Rec.2020) */
    int adv_supported, adv_enabled;   /* QueryDisplayConfig advanced colour: -1 when it could not be asked */
    float max_nits, max_full_frame_nits, min_nits;   /* panel luminance as the EDID / driver reports it; 0 = unknown */
    float sdr_white_nits;         /* Windows' "SDR content brightness" in nits (80 = the slider at its left); 0 = unknown */
    float prim[8];                /* xy of red, green, blue, white point */
    char device[32];              /* \.\DISPLAYn */
    int color_space, bits_per_color, color_encoding;
    unsigned width, height, refresh_num, refresh_den;
    char friendly_name[128], monitor_path[256];
} HdrWinInfo;
enum { HDR_WIN_CHANGE_NONE = 0, HDR_WIN_CHANGE_REMAKE = 1, HDR_WIN_CHANGE_LEVELS = 2, HDR_WIN_CHANGE_METADATA = 4 };
static inline int hdr_win_changes(const HdrWinInfo *a, const HdrWinInfo *b) {
    int c = HDR_WIN_CHANGE_NONE;
    if (a->ok != b->ok || a->hdr_on != b->hdr_on || strcmp(a->device, b->device) || strcmp(a->monitor_path, b->monitor_path) ||
        a->color_space != b->color_space || a->bits_per_color != b->bits_per_color || a->color_encoding != b->color_encoding ||
        a->width != b->width || a->height != b->height || a->refresh_num != b->refresh_num || a->refresh_den != b->refresh_den)
        c |= HDR_WIN_CHANGE_REMAKE;
    if (fabsf(a->sdr_white_nits - b->sdr_white_nits) > 0.05f || fabsf(a->max_nits - b->max_nits) > 0.05f)
        c |= HDR_WIN_CHANGE_LEVELS;
    if (fabsf(a->max_nits - b->max_nits) > 0.05f || fabsf(a->min_nits - b->min_nits) > 0.0001f ||
        fabsf(a->max_full_frame_nits - b->max_full_frame_nits) > 0.05f || memcmp(a->prim, b->prim, sizeof a->prim))
        c |= HDR_WIN_CHANGE_METADATA;
    return c;
}
int hdr_win_query(HWND hwnd, HdrWinInfo *o);      /* any thread; returns o->ok */
#endif
