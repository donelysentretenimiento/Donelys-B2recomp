# Informe HDR del monitor objetivo

Actualizado: 2026-10-22. Este documento separa la configuración objetivo comunicada por el propietario de la lectura que Windows entrega al proceso. No supone que «monitor 1» sea `DISPLAY1`: el runtime identifica el monitor que contiene la ventana mediante `MonitorFromWindow` y DXGI.

## Configuración objetivo comunicada

- El «monitor 1» del propietario es el monitor **Gigabyte**.
- El inventario EDID de Windows lo identifica como **GIGABYTE GS27Q**, fabricante `GBT`, producto `2721`, serie `24022B002187` (`DISPLAY\\GBT2721\\...UID4355`). Está activo.
- Windows 11 y NVIDIA GeForce RTX 5090.
- Pantalla objetivo: 165 Hz, RGB, 10 bits y pico aproximado de 329 nits.
- La selección debe seguir la ventana aunque Windows cambie el número o el monitor activo.

## Lectura automática del 2026-10-22

La sonda `tests/hdr_probe.c`, ejecutada sobre la ventana de escritorio, devolvió:

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

Esta lectura corresponde al ASUS, no al Gigabyte objetivo. El inventario gráfico informa además un modo activo de 2560 × 1440 a 164 Hz, coherente con el GS27Q indicado, pero no se asigna aquí a un `DISPLAYn` por conjetura. Cuando la ventana del juego esté en el Gigabyte, el runtime registrará su nombre y ruta EDID reales. Una lectura de capacidades en estado SDR tampoco debe reutilizarse como si fuera la medición HDR activa.

## Comportamiento corregido

- El launcher y el parser aceptan picos manuales desde 80 hasta 10 000 nits; 329 ya no se rechaza ni se transforma en el valor de reserva de 1000 nits.
- Con valor `0`, el runtime usa el pico informado por Windows si está entre 80 y 10 000 nits.
- El blanco SDR nunca supera el pico resuelto.
- Se registran nombre descriptivo, identidad EDID, espacio de color, profundidad, codificación, resolución y frecuencia del monitor que contiene la ventana.
- Cambiar de monitor, modo, HDR, espacio de color o profundidad recrea la swapchain. Cambiar sólo el control de blanco actualiza niveles; cambiar luminancia o primarias actualiza metadatos sin recreaciones innecesarias.

## Validación pendiente automatizable

En una ejecución con HDR de Windows activo, `hdr_probe.exe` y la línea `hdr: ACTIVE` deben registrar el dispositivo real, `hdr_on=1`, 10 bits y el pico que entregue el controlador (aproximadamente 329 nits en la pantalla indicada). No se solicita una comparación visual al usuario: la aceptación se basa en registros, formatos Vulkan, autopruebas numéricas y resultados de presentación.
