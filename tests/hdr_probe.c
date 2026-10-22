/* Prints what Windows says about the monitor under the desktop window (the HDR state, luminance, SDR white). Phase B probe.
 * Build: deps\llvm-mingw\bin\x86_64-w64-mingw32-clang.exe -O1 -o build\hdr\hdr_probe.exe tests\hdr_probe.c runtime\hdr_win.c -ldxgi -ldxguid -luser32 -lgdi32 */
#include <stdio.h>
#include "../runtime/hdr_win.h"
static int count;
static BOOL CALLBACK print_monitor(HMONITOR monitor, HDC dc, LPRECT rect, LPARAM data) {
    (void)dc; (void)rect; (void)data;
    HdrWinInfo i;
    int ok = hdr_win_query_monitor(monitor, &i);
    printf("monitor=%d ok=%d device=%s name=%s hdr_on=%d advanced_colour supported=%d enabled=%d\nidentity=%s\ncolour_space=%d encoding=%d bits=%d mode=%ux%u @ %.3f Hz\nmax=%.0f nits max_full_frame=%.0f nits min=%.4f nits sdr_white=%.0f nits\nprimaries R(%.4f,%.4f) G(%.4f,%.4f) B(%.4f,%.4f) W(%.4f,%.4f)\n",
           ++count, ok, i.device, i.friendly_name, i.hdr_on, i.adv_supported, i.adv_enabled, i.monitor_path, i.color_space, i.color_encoding, i.bits_per_color,
           i.width, i.height, i.refresh_den ? (double)i.refresh_num / i.refresh_den : 0.0,
           i.max_nits, i.max_full_frame_nits, i.min_nits, i.sdr_white_nits, i.prim[0], i.prim[1], i.prim[2], i.prim[3], i.prim[4], i.prim[5], i.prim[6], i.prim[7]);
    return TRUE;
}
int main(void) {
    if (!EnumDisplayMonitors(NULL, NULL, print_monitor, 0) || !count) return 1;
    return 0;
}
