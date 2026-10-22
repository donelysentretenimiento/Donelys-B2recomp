# Auditoría selectiva de B2recomp v0.2.9 y v0.2.10

Actualizado: 2026-10-22. Comparación realizada contra la base común v0.2.7 (`b3a6dc6`) y la cabeza previa de `hdr-nativo` (`f5d03e5`). No se ha fusionado ni cherry-pickeado upstream.

## Resultado

No es seguro integrar v0.2.9/v0.2.10 como un bloque. Los cambios útiles se dividen en cuatro grupos y varios pisan directamente el HDR, el launcher bilingüe o reparaciones propias.

| Grupo | Commits upstream | Decisión |
|---|---|---|
| Temporizadores y lógica a 120 FPS | `5827a39`, `66304f7`, `f88f9e1`, `9d60c5c`, `5dce209` | Candidatos a portar en una rama propia. Tocan principalmente `fps_patch.py` y tablas del lifter; requieren las rutas de 60/120 FPS antes de aceptar |
| Presentación y rendimiento Vulkan | `e95cbeb`, `eb269ed`, `38ddc7d`, `f180a00`, `5fb84b1` | Portado manual obligatorio. `display.c`, `gfx_options.c` y `nvn_vk_draw.c` se solapan con selección de monitor, HDR, menú y reparaciones locales |
| Actualización conservando ajustes | `3558192` (v0.2.10) | Adaptar manualmente `setup.py`, `make_package.py` y el launcher. No sustituir el launcher traducido ni sus controles HDR |
| README/release | `61be483`, `5e03296` | Sólo referencia documental; no sustituir identidad, autoría ni documentación bilingüe de DONELYS |

## Regresiones que una fusión directa introduciría

- `runtime/nnaudio.c` de v0.2.10 vuelve a un estado global único. Esta edición necesita varios estados `AudioOut`: el juego mantiene el mezclador abierto y abre otro para el audio CRI de las cinemáticas.
- Upstream no contiene `hdr_color.h`, `hdr_vk.c`, `hdr_win.c`, sus pruebas ni los documentos HDR.
- Se perderían el menú nativo, perfiles/mods, reparaciones de sonido, selección de monitor y documentación de autoría de esta edición.
- El commit de actualización modifica unas 50 líneas del launcher y debe reconciliarse con la traducción y el flujo de DONELYS, no aplicarse a ciegas.

## Orden de integración recomendado

1. Mantener la PR HDR aislada hasta revisión.
2. Crear una rama de actualización v0.2.10 y portar sólo la detección de versión, preservación de configuración y reconstrucción incremental.
3. Crear otra rama de 120 FPS: primero temporizadores/lifter; después presentación y optimizaciones Vulkan, con pruebas comparativas de 60 y 120 FPS.
4. Revalidar audio y cinemáticas tras cada bloque para impedir que reaparezca el estado `AudioOut` compartido.
