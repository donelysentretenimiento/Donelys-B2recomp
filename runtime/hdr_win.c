/* Edicion mejorada, HDR: Windows' HDR state, luminance and SDR white of the monitor a window is on. DXGI (IDXGIOutput6) gives
 * the colour space of the desktop and the panel's luminance; QueryDisplayConfig gives advanced colour and the SDR white level. */
#define COBJMACROS
#include <windows.h>
#include <dxgi1_6.h>
#include <stdio.h>
#include <string.h>
#include "hdr_win.h"

/* wingdi.h of older SDKs lacks these; the numbers are the documented DISPLAYCONFIG_DEVICE_INFO_TYPE values */
#define HW_GET_SOURCE_NAME 1
#define HW_GET_TARGET_NAME 2
#define HW_GET_ADVANCED_COLOR 9
#define HW_GET_SDR_WHITE 11
typedef struct { DISPLAYCONFIG_DEVICE_INFO_HEADER h; ULONG value; ULONG colorEncoding; ULONG bitsPerColorChannel; } HwAdvColor;      /* value: bit0 supported, bit1 enabled */
typedef struct { DISPLAYCONFIG_DEVICE_INFO_HEADER h; ULONG sdrWhiteLevel; } HwSdrWhite;                                           /* 1000 = 80 nits */

static void hw_displayconfig(const char *gdi, HdrWinInfo *o) {
    UINT32 np = 0, nm = 0;
    o->adv_supported = o->adv_enabled = -1;
    if (GetDisplayConfigBufferSizes(QDC_ONLY_ACTIVE_PATHS, &np, &nm) != ERROR_SUCCESS || !np) return;
    DISPLAYCONFIG_PATH_INFO *p = calloc(np, sizeof *p); DISPLAYCONFIG_MODE_INFO *m = calloc(nm ? nm : 1, sizeof *m);
    if (p && m && QueryDisplayConfig(QDC_ONLY_ACTIVE_PATHS, &np, p, &nm, m, NULL) == ERROR_SUCCESS) {
        for (UINT32 i = 0; i < np; i++) {
            DISPLAYCONFIG_SOURCE_DEVICE_NAME sn; memset(&sn, 0, sizeof sn);
            sn.header.type = (DISPLAYCONFIG_DEVICE_INFO_TYPE)HW_GET_SOURCE_NAME; sn.header.size = sizeof sn; sn.header.adapterId = p[i].sourceInfo.adapterId; sn.header.id = p[i].sourceInfo.id;
            if (DisplayConfigGetDeviceInfo(&sn.header) != ERROR_SUCCESS) continue;
            char name[64]; WideCharToMultiByte(CP_ACP, 0, sn.viewGdiDeviceName, -1, name, sizeof name, NULL, NULL);
            if (_stricmp(name, gdi)) continue;
            DISPLAYCONFIG_TARGET_DEVICE_NAME tn; memset(&tn, 0, sizeof tn);
            tn.header.type = (DISPLAYCONFIG_DEVICE_INFO_TYPE)HW_GET_TARGET_NAME; tn.header.size = sizeof tn; tn.header.adapterId = p[i].targetInfo.adapterId; tn.header.id = p[i].targetInfo.id;
            if (DisplayConfigGetDeviceInfo(&tn.header) == ERROR_SUCCESS) {
                WideCharToMultiByte(CP_UTF8, 0, tn.monitorFriendlyDeviceName, -1, o->friendly_name, sizeof o->friendly_name, NULL, NULL);
                WideCharToMultiByte(CP_UTF8, 0, tn.monitorDevicePath, -1, o->monitor_path, sizeof o->monitor_path, NULL, NULL);
            }
            HwAdvColor ac; memset(&ac, 0, sizeof ac);
            ac.h.type = (DISPLAYCONFIG_DEVICE_INFO_TYPE)HW_GET_ADVANCED_COLOR; ac.h.size = sizeof ac; ac.h.adapterId = p[i].targetInfo.adapterId; ac.h.id = p[i].targetInfo.id;
            if (DisplayConfigGetDeviceInfo(&ac.h) == ERROR_SUCCESS) { o->adv_supported = ac.value & 1; o->adv_enabled = (ac.value >> 1) & 1; o->color_encoding = (int)ac.colorEncoding; if (ac.bitsPerColorChannel) o->bits_per_color = (int)ac.bitsPerColorChannel; }
            HwSdrWhite sw; memset(&sw, 0, sizeof sw);
            sw.h.type = (DISPLAYCONFIG_DEVICE_INFO_TYPE)HW_GET_SDR_WHITE; sw.h.size = sizeof sw; sw.h.adapterId = p[i].targetInfo.adapterId; sw.h.id = p[i].targetInfo.id;
            if (DisplayConfigGetDeviceInfo(&sw.h) == ERROR_SUCCESS && sw.sdrWhiteLevel) o->sdr_white_nits = (float)sw.sdrWhiteLevel / 1000.0f * 80.0f;
            break;
        }
    }
    free(p); free(m);
}

int hdr_win_query_monitor(HMONITOR mon, HdrWinInfo *o) {
    memset(o, 0, sizeof *o);
    o->adv_supported = o->adv_enabled = -1;
    HMODULE dx = LoadLibraryA("dxgi.dll");
    HRESULT (WINAPI *create)(REFIID, void **) = dx ? (void *)GetProcAddress(dx, "CreateDXGIFactory1") : NULL;
    IDXGIFactory1 *fac = NULL;
    if (!create || FAILED(create(&IID_IDXGIFactory1, (void **)&fac))) return 0;
    IDXGIAdapter1 *ad;
    for (UINT a = 0; !o->ok && SUCCEEDED(IDXGIFactory1_EnumAdapters1(fac, a, &ad)); a++) {
        IDXGIOutput *out;
        for (UINT k = 0; !o->ok && SUCCEEDED(IDXGIAdapter1_EnumOutputs(ad, k, &out)); k++) {
            DXGI_OUTPUT_DESC d;
            IDXGIOutput6 *o6 = NULL;
            if (SUCCEEDED(IDXGIOutput_GetDesc(out, &d)) && d.Monitor == mon && SUCCEEDED(IDXGIOutput_QueryInterface(out, &IID_IDXGIOutput6, (void **)&o6))) {
                DXGI_OUTPUT_DESC1 d1;
                if (SUCCEEDED(IDXGIOutput6_GetDesc1(o6, &d1))) {
                    o->ok = 1;
                    o->hdr_on = d1.ColorSpace == DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020;
                    o->color_space = (int)d1.ColorSpace; o->bits_per_color = (int)d1.BitsPerColor;
                    o->max_nits = d1.MaxLuminance; o->max_full_frame_nits = d1.MaxFullFrameLuminance; o->min_nits = d1.MinLuminance;
                    for (int i = 0; i < 2; i++) { o->prim[i] = d1.RedPrimary[i]; o->prim[2 + i] = d1.GreenPrimary[i]; o->prim[4 + i] = d1.BluePrimary[i]; o->prim[6 + i] = d1.WhitePoint[i]; }
                    WideCharToMultiByte(CP_ACP, 0, d.DeviceName, -1, o->device, sizeof o->device, NULL, NULL);
                }
                IDXGIOutput6_Release(o6);
            }
            IDXGIOutput_Release(out);
        }
        IDXGIAdapter1_Release(ad);
    }
    IDXGIFactory1_Release(fac);
    if (o->ok && o->device[0]) {
        DEVMODEA dm; memset(&dm, 0, sizeof dm); dm.dmSize = sizeof dm;
        if (EnumDisplaySettingsA(o->device, ENUM_CURRENT_SETTINGS, &dm)) {
            o->width = dm.dmPelsWidth; o->height = dm.dmPelsHeight;
            o->refresh_num = dm.dmDisplayFrequency; o->refresh_den = 1;
        }
        hw_displayconfig(o->device, o);
    }
    return o->ok;
}

int hdr_win_query(HWND hwnd, HdrWinInfo *o) {
    return hdr_win_query_monitor(MonitorFromWindow(hwnd ? hwnd : GetDesktopWindow(), MONITOR_DEFAULTTOPRIMARY), o);
}
