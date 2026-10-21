# Guía de compilación

La compilación de B2recomp está explicada con detalle en [BUILDING.md](BUILDING.md), del proyecto base. Aquí solo se indica lo que cambia en esta edición. **No se repite lo que ya dice ese documento.**

## Qué necesitas

- Windows 11 (probado) con una GPU con Vulkan 1.3. Probado con una RTX 5090.
- Tu propia copia del juego, **versión 1.2, actualización 131072** (Title ID `01007960049A0000`, Build ID `F19B2E01E1794F93`).
- Las herramientas de compilación: llvm-mingw, SDL3, Vulkan-Headers y la DLL de glslang. El *paquete Builder* de B2recomp las trae en su carpeta `deps\`; consulta `BUILDING.md`.

Este repositorio no incluye el juego, ni las herramientas, ni el código generado a partir del juego.

## Pasos

1. Prepara el proyecto como indica `BUILDING.md` (paquete Builder o carpeta con `deps\`) y copia estas fuentes encima.
2. Ejecuta `Build B2recomp.bat` o `python scripts\setup.py --dump <carpeta de tu volcado>`.
   - El paso **lift** ejecuta `scripts/fps_patch.py` y, después, **`scripts/cheats_patch.py`** (esta edición), que inserta los interruptores de los trucos en el C traducido. Con todos los valores a cero, el juego se comporta como sin el parche.
   - El paso **link** crea `build\game.dll` y `build\game_native.exe`, que ya incluyen el menú.
3. Para recompilar solo el runtime tras tocar un `.c` de `runtime\`: `python scripts\build_runtime.py`.
4. Para el launcher: `python scripts\build_launcher.py --out=B2recomp.exe`.

## Biblioteca de mods

1. Descarga los mods por tu cuenta y descomprímelos en `mods_extraidos\<código>\` (la lista de nombres y enlaces está en `tools/mods/biblioteca.py`).
2. Ejecuta `python -I tools\mods\biblioteca.py` desde la carpeta del proyecto. Crea `mods\`, las fichas `mod.ini`, las fusiones de `.csv` y `docs/edicion-mejorada/inventario_mods.csv`.
3. En el launcher, **Mods...** > elige un perfil > **Cargar perfil** > **Apply**. Los mods de archivos se leen al arrancar el juego; los de código necesitan recompilar.

Los mods están bajo licencias que no permiten redistribuirlos: no los subas a ningún sitio.

## Menú dentro del juego

- Abrir y cerrar: **Insert** o **L3+R3**.
- Configuración: `config/menu.ini` (se crea solo la primera vez): `tecla` y `tecla_witch_time`.
- Desactivar: variable de entorno `SSBU_MENU=0`. Está apagado en las ejecuciones con guion.
- Prueba sin pulsar nada: `SSBU_MENU_TEST=<página 0-6>` lo abre al arrancar.

## Comprobaciones recomendadas tras compilar

- `python scripts\bench.py` (ruta estándar de pruebas del proyecto base; necesita una partida inicial en `build\saves\after-prologue`).
- Arranque corto y registro: `scripts\run_boot.ps1 prueba 40 -NoFront`. El registro debe mostrar `menu: listo` y «trucos de codigo disponibles».
- La ventana **Mods...** debe mostrar fichas y el selector de perfil.
