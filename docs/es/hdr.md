# HDR10 y scRGB

HDR está disponible únicamente en el canal **PREVIEW** de `hdr-nativo`. Convierte la imagen SDR final del juego a una swapchain HDR10 o scRGB; no modifica la iluminación interna del motor.

## Uso

1. Activa HDR en Windows para el monitor donde se abrirá el juego.
2. En el launcher abre **HDR y color avanzado…**.
3. Empieza con `hdr = auto`; utiliza `off` si aparecen colores incorrectos.
4. Ajusta el blanco SDR (`hdr_sdr_white`) y el pico (`hdr_peak`) conforme a tu pantalla. `hdr_expand = 0` conserva la imagen sin expansión de altas luces.

El modo automático intenta HDR10 y después scRGB; ante incompatibilidad vuelve a SDR. Un cambio de HDR, monitor o tamaño puede recrear la swapchain. La ruta tiene pruebas matemáticas y GPU, pero sigue pendiente de validación visual amplia.

Al informar de un fallo indica GPU, driver, monitor, modo de Windows, formato elegido y valores de `graphics.ini`. Una captura SDR no demuestra por sí sola la señal HDR enviada al monitor.
