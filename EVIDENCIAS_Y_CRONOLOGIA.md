# Evidencias y cronología

Este documento reúne, con fechas y referencias reales de GitHub, las aportaciones hechas al proyecto B2recomp y lo que ocurrió con ellas. Está pensado para conservar la prueba de qué se hizo y cuándo, **sin sacar conclusiones que los datos no respalden**.

Las horas están en **UTC** salvo que se indique. Los datos proceden de la API pública de GitHub y están guardados en bruto en [`docs/autoría-y-contribuciones/copias-de-github/`](docs/autoría-y-contribuciones/copias-de-github/). La carpeta [`docs/autoría-y-contribuciones/`](docs/autoría-y-contribuciones/) explica cómo se conservan.

Convención: **[Hecho]** comprobado con una fuente; **[Coincidencia]** dos hechos próximos en el tiempo sin relación demostrada; **[Interpretación]** lectura que no está demostrada.

## 1. Aportaciones

| Referencia | Fecha (UTC) | Qué contiene |
|---|---|---|
| Commit [`701c6b3`](https://github.com/donelysentretenimiento/B2recomp/commit/701c6b373651084aaf5c96f52f20c50452fdd265) | 2026-10-21 14:38:37 | `runtime/nnaudio.c` (+67/−24), `nvn_vk.c` (+16/−1), `nvn_vk_draw.c` (+2/−2): estado de audio por `AudioOut`, sonido sin depender del nombre del exe, `swapchain_stale()` |
| Commit [`ebb5726`](https://github.com/donelysentretenimiento/B2recomp/commit/ebb5726765536941998660ca758ed4ba9099e34b) | 2026-10-21 14:39:49 | Menú dentro del juego y trucos: `menu_vk.c` (+669), `cheats_patch.py` (+62), enganches en `display.c`, `game_shim.c`, `nnhid.c`, `nvn_vk*.c`, `setup.py`, `.gitignore` |
| Pull request [#26](https://github.com/K0NL/B2recomp/pull/26) | 2026-10-21 15:16:48 | Los dos commits sobre la v0.2.8 (`e95cbeb`): 10 archivos, +835/−27 |
| Incidencia [#27](https://github.com/K0NL/B2recomp/issues/27) | 2026-10-21 15:17:02 | Descripción sencilla de los fallos con enlace al PR |

**[Hecho]** Los dos commits y el PR existen en GitHub con esas fechas y autoría de `donelysentretenimiento` (ver `commit_701c6b3.json`, `commit_ebb5726.json`, `pr26.json`). El contenido exacto enviado está en `pr26_commits.patch`.

**[Hecho]** Las dos fechas de los commits son las de creación. GitHub registra dos reescrituras de la rama (*force push*) a las 15:24:30 y 15:27:48, hechas por la propietaria de la cuenta para quitar de los commits un correo personal y una línea de firma de herramienta. El contenido de los archivos no cambió; solo cambió el nombre y el correo de los autores. Los identificadores de commit son los posteriores a esa reescritura.

**[Hecho]** Los mismos cambios se desarrollaron antes en el repositorio local sobre la v0.2.7. Se conserva copia verificable en la carpeta local del proyecto, fuera de este repositorio: `backups/2026-10-21_cierre/` (paquete `git bundle` con huella SHA-256) y los commits locales aparecen en este repositorio con las mismas fechas, ver [CHANGELOG.md](CHANGELOG.md).

## 2. Respuestas del mantenedor y desenlace

| Fecha (UTC) | Hecho |
|---|---|
| 2026-10-21 17:43:13 | K0NL comenta en #27 (copia: `issue27_comentarios.json`). Pide traducir los informes al inglés, dice que el HDR no está implementado y que no ha adjuntado el zip de informe; avisa de que cerrará las incidencias que no estén traducidas |
| 2026-10-21 17:45:14 | K0NL comenta de nuevo: pide el zip de informe y el texto en inglés, y dice que no hará nada sin el zip |
| 2026-10-21 17:50:49 – 17:51:00 | La propietaria de la cuenta renombra a «Withdrawn» y cierra #27 (17:50:54, *not planned*) y #26 (17:50:52), y borra la rama del fork (17:51:00). Edita antes el texto con un mensaje de retirada |
| 2026-10-21 17:52:03 | K0NL cierra el issue [#25](https://github.com/K0NL/B2recomp/issues/25) («No sound»), al que el PR hacía referencia |
| 2026-10-21 18:22:41 | En #25, el usuario `therealTRASH` escribe que, tras reinstalar, sigue sin audio |
| 2026-10-21 18:33:05 | `therealTRASH` pregunta en #27 cómo se solucionaron los problemas de audio |

**[Hecho]** No hay comentarios ni revisiones de código del mantenedor sobre el contenido técnico del PR: no hay comentarios en #26 (`pr26_comentarios.json` vacío) ni revisiones.

**[Hecho]** El texto del PR y de la incidencia fue editado el 21-oct-2026 antes del cierre: se retiró una línea final de pie de página y, a las 17:50, se sustituyó todo por el mensaje de retirada. Se conservan copias de los textos enviados en `texto_pr26_publicado.md`, `texto_issue27_publicado.md` y `texto_retirada_pr26_y_issue27.md`. No se ha comprobado si GitHub conserva el historial de ediciones visible para otros.

## 3. Cambios posteriores en el repositorio original

| Fecha (UTC) | Hecho |
|---|---|
| 2026-10-21 17:54:47 – 17:55:33 | Commits de la v0.2.9 en `main`: temporizadores a 120 fps, «Vertex buffers kept on the GPU», informes de registro, mundo congelado tras cámara lenta |
| 2026-10-21 17:57:58 | Commit `61be483`: el README pide que las incidencias estén en inglés y con el informe adjunto |
| 2026-10-21 18:39:58 | Publicada la v0.2.9 |
| 2026-10-21 18:45:44 | Publicada la prueba de Linux 7 |
| 2026-10-21 18:58:47 | Commit `3558192` (v0.2.10): actualizar descomprimiendo sobre la carpeta antigua |
| 2026-10-21 19:50:29 | En el issue #22 (congelación a 120 fps con el overlay de NVIDIA), runoam1 confirma que con la versión nueva ya no se congela; K0NL responde a las 19:55:00 |
| 2026-10-21 19:57:52 | Publicada la v0.2.10 |

Referencias: `versiones_publicadas.json` y el historial del repositorio original.

## 4. Comparación de código

El script [`comprobar_cambios_originales.ps1`](docs/autoría-y-contribuciones/comprobar_cambios_originales.ps1) se puede repetir en cualquier momento. Resultado del 2026-10-21 a las 20:20 UTC, sobre `origin/main` en `3558192` y `origin/linux` en `0f4d0c1` (copia: `comprobacion_cambios_originales_2026-10-21.txt`):

- **[Hecho]** Ninguno de los identificadores propios de este trabajo (`swapchain_stale`, `aout_of`, `AOut`, `menu_vk`, `recomps_cheat`, `cheats_patch`, `menu_captures_input`) aparece en el original, ni en `main` ni en `linux`.
- **[Hecho]** `runtime/nnaudio.c` es **idéntico** entre la v0.2.8 y `main`: conserva la regla de volumen por nombre de ejecutable y un único estado de audio.
- **[Hecho]** El tratamiento de `VK_SUBOPTIMAL_KHR` en `nvn_vk.c` y `nvn_vk_draw.c` es el mismo que antes del PR.
- **[Hecho]** De los commits posteriores a la v0.2.8 solo dos tocan alguno de los archivos que modificamos: `f180a00` (`display.c`, ritmo de imágenes a 120 fps) y `3558192` (`setup.py`, actualización por descompresión). Ninguno añade nada relacionado con audio, swapchain, menú ni trucos.

## 5. Valoración

- **[Hecho]** En la fecha de la comprobación **no hay indicios en el código** de que el repositorio original haya incorporado cambios equivalentes a los de este trabajo.
- **[Coincidencia]** Los primeros commits de la v0.2.9 (17:54:47) son unos 4 minutos posteriores al cierre del PR (17:50:52) y 11 minutos posteriores al primer comentario de K0NL. Tratan de otros temas (120 fps, memoria de vértices, registros). En su comentario de las 17:43, K0NL dice que esa actualización llevaba «2 days» de trabajo, lo que sitúa su desarrollo antes de que se abriera el PR; además, la v0.2.8 (`e95cbeb`, 2026-10-20 19:24 UTC) y su comentario en el issue #22 del 20-oct a las 23:33 UTC («there is an update I'm about to push») son anteriores al PR.
- **[Hecho]** El issue #22, que el PR citaba como relacionado, **se dio por resuelto con la v0.2.9 del propio proyecto**, cuyo arreglo trata de otro mecanismo (el mundo del juego se congelaba al final de ciertas cámaras lentas a 120 fps). Por tanto, la hipótesis de este trabajo sobre la causa de #22 (recreación de la swapchain por `SUBOPTIMAL`) **probablemente no era la causa real de ese fallo**. Ver [REPARACIONES_REALIZADAS.md](REPARACIONES_REALIZADAS.md), apartado 3.
- **[Coincidencia]** K0NL cerró el issue #25 a las 17:52:03, 9 minutos después de su comentario y 1 minuto después de la retirada. No se sabe por qué.
- **[Interpretación, no demostrada]** Que el trabajo pudiera haberse aprovechado sin reconocimiento. **Los datos disponibles no lo respaldan** y no se afirma. La comparación puede repetirse con el script cuando se publiquen nuevas versiones; si aparecieran coincidencias, deberá hacerse una comparación línea a línea antes de concluir nada.

## 6. Qué no se ha comprobado

- No se ha comparado el contenido de versiones posteriores a la v0.2.10.
- No se sabe si GitHub conserva y muestra el historial de ediciones de #26 y #27.
- No se han comprobado los repositorios de otros usuarios que hayan copiado el fork.
