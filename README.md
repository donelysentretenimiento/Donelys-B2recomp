# Donelys-B2recomp

Edición mejorada de **B2recomp** (Bayonetta 2 recompilado para Windows) con reparaciones del runtime, un menú dentro del juego y una biblioteca de mods con perfiles.

> Este repositorio contiene **solo código fuente y documentación**. No incluye el juego, ni archivos extraídos de él, ni código generado a partir de él, ni mods de terceros. Cada jugador compila el juego en su propio equipo a partir de su copia legítima (versión 1.2, actualización 131072).

## Origen y licencia

- Base: [B2recomp](https://github.com/K0NL/B2recomp) v0.2.7 (commit `b3a6dc6`), de K0NL. Su historial completo hasta esa versión se conserva en este repositorio.
- B2recomp parte a su vez de [ssbu-recomp](https://github.com/Denkishi/ssbu-recomp), de Denkishi, y de otros proyectos. El detalle está en [NOTICE.md](NOTICE.md) (aviso original, sin modificar) y en [CREDITOS_Y_AUTORIA.md](CREDITOS_Y_AUTORIA.md).
- Licencia: **GPL-3.0-or-later** ([LICENSE](LICENSE)). Las aportaciones de esta edición se publican bajo la misma licencia.
- El README original del proyecto base se conserva en [README.upstream.md](README.upstream.md).

## Qué añade esta edición

| Novedad | Estado | Dónde |
|---|---|---|
| Un estado de audio y un dispositivo `waveOut` por cada `AudioOut`: los vídeos del juego dejan de ir unas 6 veces más lentos | Implementado y probado | `runtime/nnaudio.c` |
| El sonido ya no depende del nombre del ejecutable | Implementado, probado en registro | `runtime/nnaudio.c` |
| La swapchain de Vulkan ya no se recrea por `VK_SUBOPTIMAL_KHR` si el tamaño no ha cambiado | Implementado, **sin verificar con overlay** | `runtime/nvn_vk.c`, `runtime/nvn_vk_draw.c` |
| Menú dentro del juego (Insert o L3+R3) | Implementado y probado con teclado | `runtime/menu_vk.c` y enganches |
| Trucos nativos para la versión 1.2 | Parcialmente probados | `scripts/cheats_patch.py`, `runtime/menu_vk.c` |
| Biblioteca de mods, fichas, conflictos por ruta y perfiles en la ventana **Mods...** | Implementado y probado | `tools/mods/biblioteca.py`, `scripts/pchtxt.py`, `launcher/launcher.c` |

El estado exacto de cada función (implementado, probado, experimental o pendiente) está en [ESTADO_DEL_PROYECTO.md](ESTADO_DEL_PROYECTO.md). Cómo se reparó cada problema, en [REPARACIONES_REALIZADAS.md](REPARACIONES_REALIZADAS.md).

## Compilar

Consulta [GUIA_COMPILACION.md](GUIA_COMPILACION.md) y el [BUILDING.md](BUILDING.md) del proyecto base.

## Documentación

- [CHANGELOG.md](CHANGELOG.md): cambios por fecha.
- [CREDITOS_Y_AUTORIA.md](CREDITOS_Y_AUTORIA.md): quién ha escrito qué.
- [REPARACIONES_REALIZADAS.md](REPARACIONES_REALIZADAS.md): problema, causa, solución y comprobación.
- [EVIDENCIAS_Y_CRONOLOGIA.md](EVIDENCIAS_Y_CRONOLOGIA.md): cronología con fechas reales de GitHub y copias documentales.
- [ESTADO_DEL_PROYECTO.md](ESTADO_DEL_PROYECTO.md): qué funciona, qué está sin probar y qué falta.
