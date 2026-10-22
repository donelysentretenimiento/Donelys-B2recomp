# Continuidad de la reparación HDR

Actualizado: 2026-10-22. Rama de trabajo: `donelys-hdr-repair`, creada desde `origin/hdr-nativo` (`f5d03e5`). El checkout principal y sus cambios locales no se han modificado.

## Hecho

- Se compararon `main` y `hdr-nativo` antes de modificar código.
- Se confirmó y corrigió el límite fijo de 400 nits en launcher, parser y runtime.
- Se separaron capacidades del monitor y metadatos del contenido: MaxCLL/MaxFALL desconocidos son 0.
- Se añadió detección de cambios de dispositivo, HDR, espacio de color, profundidad, codificación, resolución, frecuencia, luminancia y primarias.
- Se evitó una vista BGRA incompatible cuando la superficie SDR seleccionada es RGBA.
- Pruebas: 41 comprobaciones de color/niveles y 6 de cambios de monitor, todas correctas.
- El launcher MinGW compila. `nvn_vk.c` (que incluye el paso HDR) compila usando las dependencias y generados locales existentes.
- La sonda enumera las tres pantallas. Confirma `DISPLAY1` como GIGABYTE GS27Q (`GBT2721`, serie `24022B002187`), HDR activo, RGB 10 bits, 2560×1440 a 165 Hz y pico 329 nits.
- La ruta real del juego pasó en HDR10 (508 fotogramas) y scRGB (1141): autopruebas GPU correctas, cero fotogramas descartados y cero fallos Vulkan.
- La iGPU AMD pasó 120 fotogramas SDR sin fallos. Como no expone formatos HDR para la superficie conectada a la RTX, `hdr=auto` degradó correctamente a SDR y presentó otros 666 fotogramas sin fallos.

## Siguiente bloque

1. Añadir una captura numérica controlada de fotogramas reales para complementar los patrones conocidos de SDR→scRGB y SDR→HDR10, sin pedir valoración visual.
2. Portar la actualización v0.2.10 y los cambios de 120 FPS en ramas separadas según `docs/tecnico/AUDITORIA_UPSTREAM_0.2.9_0.2.10.md`.
3. Preparar una ruta automatizada larga para audio/cinemáticas; la ruta corta ya confirmó salida continua sin underruns.
4. Continuar por entrada, reubicación y ultrawide. Intel requiere hardware disponible; AMD queda cubierto en SDR y en la degradación HDR→SDR de esta topología.

## Límites conocidos

- No se alteró la configuración HDR de Windows: ya estaba activa en el Gigabyte y desactivada en las otras pantallas.
- El worktree limpio no contiene los archivos generados desde el volcado del usuario; la compilación integral aislada necesita reutilizarlos o regenerarlos localmente, sin versionarlos.
- No hay release ni fusión. La implementación está publicada para revisión en la PR #2.
