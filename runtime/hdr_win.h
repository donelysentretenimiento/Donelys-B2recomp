/* Edicion mejorada, HDR: what Windows says about the monitor a window is on (hdr_win.c). */
#ifndef HDR_WIN_H
#define HDR_WIN_H
#include <windows.h>
typedef struct {
    int ok;                       /* the monitor was found through DXGI */
    int hdr_on;                   /* the desktop of that monitor is in HDR (DXGI colour space G2084 / Rec.2020) */
    int adv_supported, adv_enabled;   /* QueryDisplayConfig advanced colour: -1 when it could not be asked */
    float max_nits, max_full_frame_nits, min_nits;   /* panel luminance as the EDID / driver reports it; 0 = unknown */
    float sdr_white_nits;         /* Windows' "SDR content brightness" in nits (80 = the slider at its left); 0 = unknown */
    float prim[8];                /* xy of red, green, blue, white point */
    char device[32];              /* \.\DISPLAYn */
} HdrWinInfo;
int hdr_win_query(HWND hwnd, HdrWinInfo *o);      /* any thread; returns o->ok */
#endif
