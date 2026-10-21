# Compilación

Esta guía describe esta edición y no requiere consultar otro repositorio.

## Constructor incluido

El paquete DONELYS Builder incluye Python 64 bits con `lz4` y Pillow, llvm-mingw, SDL3, Vulkan-Headers, `ssbu_glslang.dll` y los runtimes necesarios. `B2recomp.exe` ejecuta el mismo flujo que:

```powershell
deps\python\python.exe scripts\setup.py --dump "D:\ruta\al\volcado"
```

Las etapas reales son `deps`, `verify`, `convert`, `mods`, `tables`, `lift`, `compile`, `icon`, `link` y `check`. Consulta el estado sin construir con:

```powershell
deps\python\python.exe scripts\setup.py --status
```

## Desde el código fuente

En Windows, clona este repositorio y proporciona las dependencias mediante `deps\` o las variables `LLVM_MINGW`, `SDL3_ROOT`, `VULKAN_HEADERS` y `GLSLANG_DLL`. Después ejecuta:

```powershell
python scripts\setup.py --dump "D:\ruta\al\volcado" --jobs 16
```

Python 3.10 o posterior necesita `lz4` y Pillow; el autotest de `check` utiliza Unicorn cuando está disponible. El flujo predeterminado usa llvm-mingw; `--toolchain msvc` requiere además LLVM 9 y las herramientas de MSVC descritas por `scripts/setup.py --help`.

## Salidas y recompilación incremental

- `build\game.dll`: código traducido del volcado del usuario.
- `build\game_native.exe`: runtime y cargador.
- `B2recomp.exe` o `build\launcher.exe`: launcher.
- `build\setup_state.json`: herramientas y etapas ya completadas.
- `targets\exefs` y `targets\romfs`: archivos convertidos y enlaces al volcado.

Comandos útiles:

```powershell
python scripts\build_runtime.py
python scripts\build_launcher.py --out=B2recomp.exe
python scripts\compile_main.py --force
python scripts\setup.py --from mods
```

`scripts/cheats_patch.py` se aplica durante `lift`. `scripts/pchtxt.py` procesa mods de código antes de generar las tablas. No edites ni publiques los archivos generados desde el juego.

## Crear el paquete constructor

```powershell
deps\python\python.exe scripts\make_package.py --out=build\package
```

El empaquetador rechaza extensiones y nombres asociados al juego, archivos generados y binarios locales. Revisa el resultado y sus licencias antes de publicarlo. No crea una release automáticamente.
