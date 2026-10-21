# Continuidad de Codex

Actualizado: 2026-10-22 (Europe/Madrid).

## Estado de partida

- Rama de trabajo: `donelys-identidad-bilingue`.
- HEAD de partida: `f5d03e537a8bc08ea386d2cdfb165b57d69389bb` (`origin/hdr-nativo`).
- Rama pública estable observada: `origin/main` en `a3745a8d9a3a`.
- Remoto: `origin = https://github.com/donelysentretenimiento/Donelys-B2recomp.git`.
- Cambio local preexistente y propiedad del usuario: `config/graphics.ini` (`hdr = auto`, blanco SDR 340 nits, expansión 100 %, monitor 1). No incluir en commits ni sobrescribir.
- No se han borrado ni movido partidas, mods, cachés, copias de seguridad o archivos del juego.

## Inventario comprobado

- El checkout conserva audio, Vulkan, menú Insert, trucos, mods y HDR de la rama `hdr-nativo`.
- HDR existente: `runtime/hdr_vk.c`, `runtime/hdr_win.c`, `runtime/hdr_win.h`, `runtime/hdr_color.h`, `runtime/hdr_selftest.inc`.
- Launcher Win32 existente: `launcher/launcher.c`; se mantendrá sin introducir otro framework.
- Toolchain empaquetado: Python, llvm-mingw 23.1.2, SDL3, Vulkan-Headers y `ssbu_glslang.dll` en `deps/`.
- Herramientas no encontradas en PATH: RenderDoc, PIX, Nsight, glslc, glslangValidator y la capa de validación Khronos.

## Línea base verificada por Codex

- `scripts/setup.py --only link`: correcto; generó `game_native.exe`, `game.dll` y `launcher.exe`.
- Autotest Vulkan SDR, 120 fotogramas: 59,99 FPS; mediana 16,67 ms; p99 16,81 ms; máximo 17,05 ms; 0 fotogramas sobre 25 ms; 0 fallos Vulkan; 0 recreaciones de swapchain.
- GPU usada: NVIDIA GeForce RTX 5090. Monitor de la ventana: `DISPLAY2`, 59,987 Hz.
- Esta comprobación se ejecutó antes de recibir el encargo de identidad; no valida visualmente HDR.

## Fases

- [x] A — Inventario inicial y rama independiente.
- [x] B — Presentación GitHub y portada bilingüe en rama revisable.
- [x] C — Documentación autónoma español/inglés y navegación preparada para Pages.
- [x] D — Autoría, política de marca, citación y archivos de comunidad preparados.
- [x] E — Protecciones y autenticidad documentadas; checks locales implementados. Los rulesets y Pages todavía no están activados.
- [ ] F — Publicar rama revisable y abrir Pull Request; decidir el destino documental de `main` sin fusionar código HDR.
- [ ] G — Constructor y actualizaciones.
- [ ] H — Revisión HDR posterior.
- [ ] I — Verificación final.

## Entrega GitHub y documentación

- `main` remoto comprobado en `a3745a8d9a3afaea9565d7f35b2e5c2a12b6ae51`.
- `hdr-nativo` remoto comprobado en `f5d03e537a8bc08ea386d2cdfb165b57d69389bb`: 9 commits por delante, 0 por detrás y 21 archivos distintos.
- GitHub público: rama predeterminada `main`; ramas `main` y `hdr-nativo`; una release `v0.2.7-donelys-hdr-preview1` marcada como pre-release; Pages desactivada; sin workflows ni rulesets visibles al inicio.
- Rama documental publicada: `donelys-github-main`, basada directamente en `main` y sin los 9 commits HDR.
- Pull Request de revisión: `#1`, abierta como borrador contra `main`.
- Descripción y topics del repositorio actualizados y verificados (`bayonetta-2`, `static-recompilation`, `vulkan`, `hdr10`, `scrgb`, `windows`, `game-modding`, `mods`, `donelys`).
- Commit de documentación: `4c34e77` (`docs: add autonomous bilingual project guides`).
- Añadidos README bilingüe, `docs/es`, `docs/en`, portada de Pages, instalación/compilación propia y estado STABLE/PREVIEW/EXPERIMENTAL.
- Preparados `TRADEMARKS.md`, `CITATION.cff`, `CONTRIBUTING.md`, `SECURITY.md`, `CODEOWNERS`, plantillas de incidencias/PR y propuesta de rulesets.
- Preparado workflow `Repository checks` con permisos `contents: read`; no se publicó porque el token HTTPS carece del scope `workflow`. No se considera CI activa.
- Pruebas locales: `python -m unittest tests.test_check_public_repo -v` = 3/3; `python scripts/check_public_repo.py` = correcto.

## Cambios locales preservados fuera de los commits documentales

- `config/graphics.ini`: configuración personal del usuario; no incluir.
- `scripts/build_launcher.py`, `launcher/i18n.c`, `launcher/i18n.h`, `scripts/test_launcher_i18n.bat`, `tests/launcher_i18n_test.c`: localización iniciada antes del cambio de prioridad; conservar para una fase posterior y confirmar por separado.

## Próximo paso exacto

Revisar la Pull Request `#1`. Antes de marcarla lista para fusionar, validar los formularios de Issues en la interfaz y decidir si se autoriza el scope `workflow` para añadir `Repository checks`; después aplicar rulesets sólo cuando el nombre del check esté confirmado. No fusionar automáticamente los 9 commits de `hdr-nativo`.

## Notas para Claude Code

- Leer también `CONTINUIDAD_HDR.md`; no sustituirlo.
- No publicar ni mezclar automáticamente esta rama en `main`.
- No tocar el cambio local de `config/graphics.ini`.
- No crear imágenes, vídeos, logotipos ni capturas en esta fase.
