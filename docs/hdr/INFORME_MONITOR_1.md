# Informe HDR del monitor objetivo

Actualizado: 2026-10-22. Este documento separa la configuración objetivo comunicada por el propietario de la lectura que Windows entrega al proceso. No supone que «monitor 1» sea `DISPLAY1`: el runtime identifica el monitor que contiene la ventana mediante `MonitorFromWindow` y DXGI.

## Configuración objetivo comunicada

- El «monitor 1» del propietario es el monitor **Gigabyte**.
- El inventario EDID de Windows lo identifica como **GIGABYTE GS27Q**, fabricante `GBT`, producto `2721`, serie `24022B002187` (`DISPLAY\\GBT2721\\...UID4355`). Está activo.
- Windows 11 y NVIDIA GeForce RTX 5090.
- Pantalla objetivo: 165 Hz, RGB, 10 bits y pico aproximado de 329 nits.
- La selección debe seguir la ventana aunque Windows cambie el número o el monitor activo.

## Lectura automática del 2026-10-22

La sonda ampliada enumeró las tres pantallas activas y confirmó el objetivo:

| Dispositivo | Nombre | HDR | Señal | Modo | Pico |
|---|---|---:|---|---|---:|
| `\\.\DISPLAY1` | GIGABYTE GS27Q | activo | RGB, 10 bits | 2560 × 1440 a 165,000 Hz | 329 nits |
| `\\.\DISPLAY2` | ASUS VG289Q1A | desactivado | RGB, 8 bits | 3840 × 2160 a 60,000 Hz | 240 nits |
| `\\.\DISPLAY3` | ARZOPA | desactivado | RGB, 8 bits | 2560 × 1440 a 60,000 Hz | 418 nits |

La primera versión de la sonda, ejecutada sólo sobre la ventana de escritorio, había devuelto:

| Campo | Valor observado |
|---|---:|
| Dispositivo GDI | `\\.\DISPLAY2` |
| Nombre / identidad | `ASUS VG289Q1A` / `DISPLAY#AUS28CA#...UID4357` |
| Modo | 3840 × 2160 a 60,000 Hz |
| Espacio DXGI | 0 (`RGB_FULL_G22_NONE_P709`) |
| Codificación / profundidad | RGB, 8 bits por canal |
| HDR avanzado | compatible, desactivado |
| Luminancia máxima / fotograma completo | 240 / 240 nits |
| Blanco SDR de Windows | 80 nits |

Esa lectura correspondía al ASUS, no al Gigabyte objetivo. La enumeración explícita elimina ya la ambigüedad: el GS27Q es `DISPLAY1`. Una lectura de capacidades en estado SDR no se reutiliza como si fuera la medición HDR activa.

## Comportamiento corregido

- El launcher y el parser aceptan picos manuales desde 80 hasta 10 000 nits; 329 ya no se rechaza ni se transforma en el valor de reserva de 1000 nits.
- Con valor `0`, el runtime usa el pico informado por Windows si está entre 80 y 10 000 nits.
- El blanco SDR nunca supera el pico resuelto.
- Se registran nombre descriptivo, identidad EDID, espacio de color, profundidad, codificación, resolución y frecuencia del monitor que contiene la ventana.
- Cambiar de monitor, modo, HDR, espacio de color o profundidad recrea la swapchain. Cambiar sólo el control de blanco actualiza niveles; cambiar luminancia o primarias actualiza metadatos sin recreaciones innecesarias.

## Validación integrada automatizada

Se ejecutó la ruta real del juego sobre `DISPLAY1`, con configuración y partidas aisladas:

| Ruta | Swapchain | Autoprueba GPU | Presentación |
|---|---|---|---|
| HDR10 | formato 64, `HDR10_ST2084`, PQ/Rec.2020 | 12 píxeles × 2 expansiones, error máximo 0,000499 | 508 fotogramas, 0 descartados, 0 fallos Vulkan |
| scRGB | formato 97, `EXTENDED_SRGB_LINEAR`, FP16 | 12 píxeles × 2 expansiones, error máximo 0,001406 | 1141 fotogramas, 0 descartados, 0 fallos Vulkan |

Ambas rutas resolvieron blanco SDR 240 nits y pico 329 nits, detectaron el GS27Q a 10 bits/165 Hz y recrearon la swapchain una sola vez, la transición esperada desde la swapchain inicial SDR. HDR10 envió metadatos; scRGB no los envía por diseño. La capa `VK_LAYER_KHRONOS_validation` no está instalada, por lo que los contadores de validación permanecen inactivos; los resultados anteriores proceden de llamadas Vulkan, lectura GPU y presentación reales.
