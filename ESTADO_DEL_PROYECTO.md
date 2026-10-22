# Estado del proyecto

Actualizado el 2026-10-22. Trabajo HDR aislado en `donelys-hdr-repair`, basado en `origin/hdr-nativo` (`f5d03e5`). Entorno: Windows 11, RTX 5090 y GIGABYTE GS27Q (`DISPLAY1`) a 2560×1440/165 Hz, RGB 10 bits, HDR activo y pico 329 nits. Juego versión 1.2.

Leyenda: ✅ probado con evidencia · 🟡 implementado, probado en parte · 🧪 experimental · ⏳ pendiente · ❌ sin probar

## Reparaciones

| Función | Estado | Notas |
|---|---|---|
| Un estado y un dispositivo por `AudioOut` (vídeos a velocidad correcta) | ✅ | Comparación con los fotogramas del propio `.usm` |
| Sonido sin depender del nombre del exe | 🟡 | Compila y se ejecuta; sin probar a oído con el exe renombrado |
| Swapchain sin recreación por `SUBOPTIMAL` sin cambio de tamaño | 🧪 | Sin reproducir el fallo original con overlay o RTX HDR |

## Menú dentro del juego

| Función | Estado |
|---|---|
| Abrir, cerrar y navegar con teclado | ✅ |
| Entrada bloqueada para el juego mientras está abierto | ✅ |
| Ratón (clics y rueda) | ❌ |
| Mando (L3+R3, cruceta, A y B) | ❌ |
| Pérdida de foco y cambio de ventana | ❌ |
| Página Gráficos y Diagnóstico (solo lectura) | 🟡 se ve, no verificada la exactitud de cada dato |
| Página Mods (lee `mods.ini` y fichas) | 🟡 |
| Cámara libre, distancia, FOV, ocultar interfaz | ⏳ (no implementado; aparece como «No disponible») |

## Trucos (versión 1.2)

| Truco | Estado |
|---|---|
| Salud infinita | ✅ |
| Witch Time permanente (tiempo detenido o lento) y atajo F5 | 🧪 |
| Invencibilidad 1P | ❌ |
| Personaje: Balder, Rodin, King Zero, Rosa | ❌ |
| Arma de la ranura A: Umbran Sisters, Arwing, Gatling, híbrida | ❌ |
| Magia infinita, daño multiplicado, golpear a distancia | ⏳ (sin código verificado para la 1.2) |

El usuario indicó que los trucos funcionaron, sin detallar cuáles.

## Mods

| Función | Estado |
|---|---|
| Biblioteca (`biblioteca.py`), fichas, inventario SHA-256 | ✅ |
| Fusiones seguras de `.csv` de combos | 🟡 generadas y comprobadas por diferencia; sin probar en combate |
| Conflictos por ruta y perfiles en la ventana Mods... | ✅ |
| Carga de los mods del perfil «Bayonetta 3 Experience» al arrancar | ✅ (registro: 7 activos) |
| Iconos de Xbox | ✅ |
| Aspecto de modelos, armas y movimientos modificados | ❌ no visto en pantalla |

## Pendiente

1. **Validación integrada del HDR nativo**, ya implementado y en reparación (ver `docs/hdr/CONTINUIDAD_HDR.md`).
2. Probar ratón, mando y cambio de ventana en el menú.
3. Probar el resto de trucos y los mods de combate.
4. Reproducir el fallo de `SUBOPTIMAL` con overlay para confirmar la reparación.
5. Portar selectivamente los cambios útiles de v0.2.9/v0.2.10, sin una fusión indiscriminada.

## Estado de HDR

Existe una ruta HDR10/scRGB con conversión SDR→HDR, selección de formato de superficie y metadatos. En esta fase se repararon el pico de 329 nits, MaxCLL/MaxFALL, el seguimiento del monitor y la compatibilidad RGBA/BGRA. Puntos principales:

- **Formato de la swapchain:** `runtime/nvn_vk.c`, búsqueda del formato de superficie (`B8G8R8A8_UNORM` + `SRGB_NONLINEAR`). Un HDR nativo necesita pedir `VK_EXT_swapchain_colorspace` en la instancia y elegir un formato HDR (por ejemplo `R16G16B16A16_SFLOAT` con `EXTENDED_SRGB_LINEAR`, o `A2B10G10R10` con `HDR10_ST2084`).
- **Presentación:** `runtime/nvn_vk_draw.c`, función `vk_present2`: aquí se copia la imagen del juego a la swapchain con `vkCmdBlitImage`. Con HDR habrá que sustituir la copia por un paso de dibujo que convierta de SDR a HDR (referencia de blanco, curva de transferencia).
- **Metadatos:** `VK_EXT_hdr_metadata` informa capacidades de pantalla; MaxCLL/MaxFALL quedan en 0 porque el contenido convertido no tiene mediciones de masterización.
- **Menú:** `runtime/menu_vk.c` se dibuja después de la copia, sobre el fotograma; con HDR deberá usar el mismo espacio de color.
- **Ajustes:** añadir las opciones a `config/graphics.ini` y a `runtime/gfx_options.c`.
- **Prueba:** 48 comprobaciones unitarias pasan. Ejecución integrada correcta en HDR10 y scRGB sobre el GS27Q: autopruebas GPU correctas, 1649 fotogramas totales, ninguno descartado y cero fallos Vulkan.

Detalles y próximos pasos: `docs/hdr/INFORME_MONITOR_1.md`, `docs/hdr/CONTINUIDAD_HDR.md` y `docs/tecnico/MATRIZ_ERRORES_B2RECOMP.md`.
