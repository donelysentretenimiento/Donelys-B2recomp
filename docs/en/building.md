# Building

This guide covers this edition without requiring another repository.

The DONELYS Builder package includes 64-bit Python with `lz4` and Pillow, llvm-mingw, SDL3, Vulkan-Headers, `ssbu_glslang.dll`, and required runtimes. `B2recomp.exe` runs the same flow as:

```powershell
deps\python\python.exe scripts\setup.py --dump "D:\path\to\dump"
```

The actual stages are `deps`, `verify`, `convert`, `mods`, `tables`, `lift`, `compile`, `icon`, `link`, and `check`. Inspect state with `deps\python\python.exe scripts\setup.py --status`.

## Source checkout

On Windows, clone this repository and provide dependencies under `deps\` or with `LLVM_MINGW`, `SDL3_ROOT`, `VULKAN_HEADERS`, and `GLSLANG_DLL`. Then run:

```powershell
python scripts\setup.py --dump "D:\path\to\dump" --jobs 16
```

Python 3.10+ needs `lz4` and Pillow. The default toolchain is llvm-mingw; `--toolchain msvc` additionally requires LLVM 9 and the MSVC tools shown by `scripts/setup.py --help`.

Outputs include `build\game.dll`, `build\game_native.exe`, the launcher, `build\setup_state.json`, and generated content under `targets\`. Useful incremental commands:

```powershell
python scripts\build_runtime.py
python scripts\build_launcher.py --out=B2recomp.exe
python scripts\compile_main.py --force
python scripts\setup.py --from mods
```

`scripts/cheats_patch.py` runs during `lift`; `scripts/pchtxt.py` applies code mods before table generation. Never publish generated game code.

Create a builder with `deps\python\python.exe scripts\make_package.py --out=build\package`. The packager rejects known game, generated, and local binary files. Review its output and licenses before publication; it does not create a release.
