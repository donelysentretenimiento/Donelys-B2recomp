# Estado del proyecto

Actualizado el 2026-10-22. Base: B2recomp v0.2.7 (`b3a6dc6`). Entorno de pruebas: Windows 11, RTX 5090, monitor 4K a 60 Hz, juego versión 1.2.

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

## HDR (rama `hdr-nativo` / PREVIEW)

| Función | Estado | Notas |
|---|---|---|
| HDR10 y scRGB con retorno a SDR | 🟡 | Compila, presenta y supera pruebas técnicas; falta validación visual amplia. |
| Conversión de color CPU | ✅ | `tests/hdr_color_test.c`. |
| Autotest del shader en GPU | 🟡 | Ejecutado en RTX 5090; requiere más fabricantes y drivers. |
| Selección y cambio de monitor | 🟡 | Implementado; necesita pruebas manuales adicionales. |

La salida convierte la imagen SDR final; no es iluminación HDR interna del motor. `main` no incorpora automáticamente esta preview.

## Pendiente

1. Publicar y revisar la documentación bilingüe sin fusionar código HDR en `main`.
2. Probar ratón, mando y cambio de ventana en el menú.
3. Probar el resto de trucos y los mods de combate.
4. Validar HDR visualmente en más pantallas, GPU y drivers.
5. Reproducir el fallo de `SUBOPTIMAL` con overlay para confirmar la reparación.
6. Evaluar por separado la actualización a versiones nuevas del proyecto base.
