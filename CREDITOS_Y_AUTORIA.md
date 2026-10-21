# Créditos y autoría

Este proyecto es una versión modificada de otros proyectos libres. Aquí se dice qué parte viene de dónde y qué parte se ha escrito en esta edición. El aviso original de los autores anteriores está en [NOTICE.md](NOTICE.md) y **no se ha modificado**.

## Cadena de origen

1. **[ssbu-recomp](https://github.com/Denkishi/ssbu-recomp)**, de **Denkishi** (GPL-3.0). Recompilación estática de Super Smash Bros. Ultimate. De ahí vienen `runtime/` (salvo los archivos propios de B2recomp) y las herramientas del `lifter/`.
2. **[suyu](https://suyu.dev/)** y otros proyectos: parte de ese código describe comportamientos tomados de sus fuentes (GPL-3.0), como indica `NOTICE.md`.
3. **[B2recomp](https://github.com/K0NL/B2recomp)**, de **K0NL** (GPL-3.0), versión 0.2.7, commit `b3a6dc6`. Adapta lo anterior a Bayonetta 2 y añade el launcher, los scripts de compilación, los parches de fotogramas por segundo y los marcadores `[Recomps#1]` en el código copiado. Su historial de commits hasta esa versión está intacto en este repositorio, con su autoría original.
4. **Esta edición**, de **donelysentretenimiento** (GPL-3.0-or-later), sobre la versión 0.2.7 anterior.

Las herramientas de ingeniería inversa y de compilación que usa el proyecto (Ghidra, Ghidra-Switch-Loader, REA, llvm-mingw, SDL3, Vulkan-Headers, glslang) pertenecen a sus propios autores y licencias, que se detallan en `NOTICE.md` y `THIRD-PARTY-NOTICES.md`.

## Qué se ha escrito en esta edición

**Archivos nuevos** (`GPL-3.0-or-later`, copyright de donelysentretenimiento 2026):

| Archivo | Qué es |
|---|---|
| `runtime/menu_vk.c` | Menú dentro del juego y trucos de datos |
| `scripts/cheats_patch.py` | Inserta los interruptores de trucos en el C traducido |
| `tools/mods/biblioteca.py` | Biblioteca de mods, fichas, fusiones y perfiles |
| `config/perfiles/*.ini` | Los cinco perfiles de mods |
| `docs/edicion-mejorada/inventario_mods.csv` | Inventario de archivos de mods (rutas y SHA-256, sin contenido) |
| Documentación de la raíz y de `docs/autoría-y-contribuciones/` | Documentos de esta edición |

**Archivos del proyecto base modificados** (la autoría anterior se mantiene; las líneas añadidas son de esta edición):

| Archivo | Cambio |
|---|---|
| `runtime/nnaudio.c` | Un estado por `AudioOut`; volumen sin depender del nombre del exe |
| `runtime/nvn_vk.c`, `runtime/nvn_vk_draw.c` | `swapchain_stale()`; enganche del menú |
| `runtime/nnhid.c`, `runtime/display.c` | Entrada y ratón cuando el menú está abierto |
| `runtime/game_shim.c` | Exporta `recomps_cheat[]` |
| `launcher/launcher.c` | Fichas de mod y selector de perfil en la ventana Mods... |
| `scripts/pchtxt.py` | Fichas, versión, categoría y conflictos en `tree` |
| `scripts/setup.py` | Llama a `cheats_patch.py` en el paso `lift` |
| `.gitignore` | Líneas que permiten subir los archivos nuevos |
| `README.md` | Sustituido; el original está en `README.upstream.md` |

Los cambios por commit están en [CHANGELOG.md](CHANGELOG.md).

## Origen de los trucos

Los trucos no son código copiado: son comportamientos de códigos publicados para la versión 1.2 (build `F19B2E01E1794F93`), reimplementados como interruptores nativos. Direcciones y valores proceden de:

- **Gamerjin** (GBAtemp): código «invincible/time control», para Witch Time.
- **xtatu** (GBAtemp): códigos «inf health+protection» e «1p invincible», de la versión 1.0, cuyas direcciones se verificaron en la 1.2.
- **pearl23**, a partir de la tabla de **DniweTamp**: «Misc Bayonetta 2 Cheats» 1.1 (GameBanana), para personajes y armas de la ranura A.

## Mods

Los mods de GameBanana que usa la biblioteca (autores como DniweTamp, pearl23, YouWhenMe, AlexanderN, Fool_Arcana, Hias, strawed, oothiee, Timo654) son obras de sus autores bajo licencias que **no permiten redistribuirlos**. Este repositorio no los contiene: solo guarda nombres, enlaces a sus fichas y huellas SHA-256.

## Lo que no está en este repositorio

Archivos del juego, código C generado a partir del juego, partidas, claves, ejecutables ni binarios.
