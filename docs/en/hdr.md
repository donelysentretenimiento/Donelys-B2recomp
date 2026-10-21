# HDR10 and scRGB

HDR is available only in the `hdr-nativo` **PREVIEW** channel. It converts the game's final SDR image to an HDR10 or scRGB swapchain; it does not add native engine lighting.

Enable Windows HDR on the target display, open **HDR and advanced color…** in the launcher, and begin with `hdr = auto`. Use `off` if output is wrong. `hdr_sdr_white` controls reference white, `hdr_peak` controls the display peak, and `hdr_expand = 0` disables highlight expansion.

Auto mode attempts HDR10 and then scRGB, with a safe SDR fallback. The path has mathematical and GPU tests, but broad visual validation remains pending. A normal screenshot alone cannot establish the HDR signal sent to the display.
