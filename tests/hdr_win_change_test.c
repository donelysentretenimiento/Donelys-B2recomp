#include <stdio.h>
#include <string.h>
#include "../runtime/hdr_win.h"

static int checks, fails;
static void expect_(const char *name, int got, int want) {
    checks++;
    if (got != want) { fails++; printf("FAIL %s: %d, expected %d\n", name, got, want); }
}

static HdrWinInfo monitor_(void) {
    HdrWinInfo v; memset(&v, 0, sizeof v);
    v.ok = v.hdr_on = 1; v.color_space = 12; v.bits_per_color = 10;
    v.width = 2560; v.height = 1440; v.refresh_num = 165000; v.refresh_den = 1000;
    v.max_nits = 329.0f; v.min_nits = 0.01f; v.sdr_white_nits = 240.0f;
    strcpy(v.device, "\\\\.\\DISPLAY2");
    strcpy(v.friendly_name, "GIGABYTE GS27Q"); strcpy(v.monitor_path, "MONITOR\\GBT1234");
    return v;
}

int main(void) {
    HdrWinInfo a = monitor_(), b = a;
    expect_("unchanged monitor", hdr_win_changes(&a, &b), HDR_WIN_CHANGE_NONE);
    b.sdr_white_nits = 203.0f;
    expect_("white slider updates levels only", hdr_win_changes(&a, &b), HDR_WIN_CHANGE_LEVELS);
    b = a; b.max_nits = 344.0f;
    expect_("display peak updates levels and metadata", hdr_win_changes(&a, &b), HDR_WIN_CHANGE_LEVELS | HDR_WIN_CHANGE_METADATA);
    b = a; strcpy(b.device, "\\\\.\\DISPLAY1");
    expect_("moving the window recreates swapchain", hdr_win_changes(&a, &b) & HDR_WIN_CHANGE_REMAKE, HDR_WIN_CHANGE_REMAKE);
    b = a; strcpy(b.monitor_path, "MONITOR\\GBT5678");
    expect_("EDID identity change recreates swapchain", hdr_win_changes(&a, &b) & HDR_WIN_CHANGE_REMAKE, HDR_WIN_CHANGE_REMAKE);
    b = a; b.color_space++;
    expect_("desktop colour-space change recreates", hdr_win_changes(&a, &b) & HDR_WIN_CHANGE_REMAKE, HDR_WIN_CHANGE_REMAKE);
    b = a; b.width = 3840;
    expect_("mode resolution change recreates", hdr_win_changes(&a, &b) & HDR_WIN_CHANGE_REMAKE, HDR_WIN_CHANGE_REMAKE);
    printf("hdr_win_change_test: %d checks, %d failed\n", checks, fails);
    return fails != 0;
}
