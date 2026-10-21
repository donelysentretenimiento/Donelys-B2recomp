# DONELYS

## BAYONETTA 2 RECOMP

### Enhanced Edition

[Español](#español) · [English](#english) · [Descargas / Downloads](https://github.com/donelysentretenimiento/Donelys-B2recomp/releases) · [Documentación / Documentation](docs/README.md)

[![Licencia](https://img.shields.io/badge/licencia-GPL--3.0--or--later-blue)](LICENSE)
[![Plataforma](https://img.shields.io/badge/plataforma-Windows%2010%2F11-0078d4)](docs/es/instalacion.md)
[![Gráficos](https://img.shields.io/badge/gráficos-Vulkan%201.3-a41e22)](docs/es/configuracion.md)
[![Estado HDR](https://img.shields.io/badge/HDR-preview-orange)](docs/es/hdr.md)

> Este repositorio contiene código fuente y herramientas de construcción, pero no contiene *Bayonetta 2*, archivos extraídos del juego, claves, código generado desde el juego ni mods de terceros. Necesitas tu propia copia compatible.

## Español

DONELYS — BAYONETTA 2 RECOMP es una edición independiente para Windows basada en B2recomp. Añade reparaciones de audio y Vulkan, launcher y menú Insert, perfiles de mods, opciones de personalización y una vista previa de salida HDR10/scRGB.

### Estado de las versiones

| Canal | Rama | Uso |
|---|---|---|
| **STABLE** | [`main`](https://github.com/donelysentretenimiento/Donelys-B2recomp/tree/main) | Base pública. No incluye automáticamente el trabajo experimental de HDR. |
| **PREVIEW** | [`hdr-nativo`](https://github.com/donelysentretenimiento/Donelys-B2recomp/tree/hdr-nativo) | HDR10/scRGB y mejoras posteriores. Compila y tiene pruebas técnicas; la calidad visual aún requiere más pantallas. |
| **EXPERIMENTAL** | ramas de desarrollo | Cambios en revisión. No deben presentarse como versiones estables. |

La release `v0.2.7-donelys-hdr-preview1` es una **pre-release**. Su HDR convierte la imagen SDR final a una presentación HDR10 o scRGB; no es iluminación HDR interna del motor.

### Funciones principales

- Constructor local para la versión 1.2 de Bayonetta 2 para Nintendo Switch (actualización v131072).
- Reparaciones de audio, presentación Vulkan y caché.
- Launcher DONELYS y menú Insert con gráficos, diagnóstico, trucos y mods.
- Mods RomFS y de código (`.pchtxt`/`.ips`), conflictos y perfiles.
- Vista previa HDR10/scRGB con retorno seguro a SDR.
- Informes de diagnóstico y scripts de pruebas sin archivos del juego.

### Inicio rápido

1. Descarga un paquete **DONELYS Builder** desde nuestras [Releases](https://github.com/donelysentretenimiento/Donelys-B2recomp/releases); comprueba que el canal sea el que deseas.
2. Descomprime el paquete en una carpeta con espacio libre.
3. Abre `B2recomp.exe`, selecciona el volcado compatible de tu propia copia y pulsa **Build**.
4. Cuando termine, configura el launcher y pulsa **Play**.

Consulta antes la [guía de instalación](docs/es/instalacion.md). Para desarrollo: [compilación desde el código fuente](docs/es/compilacion.md).

### Navegación

[Requisitos e instalación](docs/es/instalacion.md) · [Compilación](docs/es/compilacion.md) · [Configuración](docs/es/configuracion.md) · [HDR](docs/es/hdr.md) · [Mods](docs/es/mods.md) · [Mandos](docs/es/mandos.md) · [Actualizaciones](docs/es/actualizaciones.md) · [Solución de problemas](docs/es/problemas.md) · [Preguntas frecuentes](docs/es/preguntas-frecuentes.md)

Abre una [incidencia](https://github.com/donelysentretenimiento/Donelys-B2recomp/issues/new/choose) en español o inglés. No adjuntes claves ni archivos del juego.

## English

DONELYS — BAYONETTA 2 RECOMP is an independent Windows edition based on B2recomp. It adds audio and Vulkan fixes, a launcher and Insert menu, mod profiles, customization options, and a preview HDR10/scRGB output path.

### Release status

| Channel | Branch | Intended use |
|---|---|---|
| **STABLE** | [`main`](https://github.com/donelysentretenimiento/Donelys-B2recomp/tree/main) | Public baseline. It does not automatically include experimental HDR work. |
| **PREVIEW** | [`hdr-nativo`](https://github.com/donelysentretenimiento/Donelys-B2recomp/tree/hdr-nativo) | HDR10/scRGB and later improvements. It builds and has technical tests; visual quality still needs validation on more displays. |
| **EXPERIMENTAL** | development branches | Changes under review. They must not be presented as stable releases. |

Release `v0.2.7-donelys-hdr-preview1` is a **pre-release**. Its HDR path converts the final SDR image to an HDR10 or scRGB presentation; it is not native engine lighting.

### Main features

- Local builder for Bayonetta 2 for Nintendo Switch version 1.2 (update v131072).
- Audio, Vulkan presentation, and cache fixes.
- DONELYS launcher and Insert menu with graphics, diagnostics, cheats, and mods.
- RomFS and code mods (`.pchtxt`/`.ips`), conflict reporting, and profiles.
- HDR10/scRGB preview with a safe SDR fallback.
- Diagnostic reports and game-independent test scripts.

### Quick start

1. Download a **DONELYS Builder** package from our [Releases](https://github.com/donelysentretenimiento/Donelys-B2recomp/releases); check its channel first.
2. Extract it to a folder with sufficient free space.
3. Open `B2recomp.exe`, select a compatible dump from your own copy, and choose **Build**.
4. When the build finishes, configure the launcher and choose **Play**.

Read the [installation guide](docs/en/installation.md) first. Developers can use the [source build guide](docs/en/building.md).

### Documentation

[Requirements and installation](docs/en/installation.md) · [Building](docs/en/building.md) · [Configuration](docs/en/configuration.md) · [HDR](docs/en/hdr.md) · [Mods](docs/en/mods.md) · [Controllers](docs/en/controllers.md) · [Updates](docs/en/updates.md) · [Troubleshooting](docs/en/troubleshooting.md) · [FAQ](docs/en/faq.md)

Open an [issue](https://github.com/donelysentretenimiento/Donelys-B2recomp/issues/new/choose) in Spanish or English. Never attach keys or game files.

## Licencia, autoría y marcas / License, authorship, and trademarks

El código se distribuye bajo **GPL-3.0-or-later**; consulta [LICENSE](LICENSE), [NOTICE.md](NOTICE.md), [CREDITOS_Y_AUTORIA.md](CREDITOS_Y_AUTORIA.md) y [TRADEMARKS.md](TRADEMARKS.md). DONELYS no está afiliado a Nintendo, PlatinumGames o SEGA. La política de marca no modifica los derechos concedidos por la GPL.

The code is distributed under **GPL-3.0-or-later**; see [LICENSE](LICENSE), [NOTICE.md](NOTICE.md), [CREDITOS_Y_AUTORIA.md](CREDITOS_Y_AUTORIA.md), and [TRADEMARKS.md](TRADEMARKS.md). DONELYS is not affiliated with Nintendo, PlatinumGames, or SEGA. The trademark policy does not alter the rights granted by the GPL.

Upstream lineage: [ssbu-recomp](https://github.com/Denkishi/ssbu-recomp) → [B2recomp](https://github.com/K0NL/B2recomp) → DONELYS Enhanced Edition. The preserved upstream README is [README.upstream.md](README.upstream.md).
