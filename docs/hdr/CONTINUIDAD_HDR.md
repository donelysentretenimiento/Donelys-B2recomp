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
- La sonda actual observó que `DISPLAY2` es un ASUS VG289Q1A, 4K60, RGB 8 bits, HDR desactivado y 240 nits; no es el Gigabyte objetivo. Windows identifica el monitor objetivo como GIGABYTE GS27Q (`GBT2721`, serie `24022B002187`).

## Siguiente bloque

1. Ejecutar el binario integrado con HDR de Windows activo y recoger automáticamente la línea `hdr: ACTIVE`, la autoprueba GPU y los contadores de presentación.
2. Añadir una captura numérica controlada para comparar SDR→scRGB y SDR→HDR10 sin pedir valoración visual.
3. Auditar los commits de v0.2.9/v0.2.10 y portar de forma selectiva actualización y temporización, manteniendo separados los cambios HDR.
4. Continuar por audio/cinemáticas, AMD/Intel, entrada, reubicación, ultrawide y 120 FPS según la matriz técnica.

## Límites conocidos

- No se ha activado HDR de Windows ni alterado la configuración del sistema automáticamente.
- El worktree limpio no contiene los archivos generados desde el volcado del usuario; la compilación integral aislada necesita reutilizarlos o regenerarlos localmente, sin versionarlos.
- No hay publicación ni release de esta fase todavía. Debe pasar revisión y una ejecución HDR integrada antes del PR.
