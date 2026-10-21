<div align="center">

# Donelys · B2recomp

### Bayonetta 2 recompilado para Windows — **edición mejorada**

HDR10 y scRGB · menú dentro del juego · trucos · biblioteca de mods · audio y Vulkan reparados

[![Licencia GPL-3.0](https://img.shields.io/badge/licencia-GPL--3.0-blue)](LICENSE)
![Plataforma](https://img.shields.io/badge/plataforma-Windows%2010%2F11-0078d4)
![API](https://img.shields.io/badge/gr%C3%A1ficos-Vulkan%201.3-a41e22)
![HDR](https://img.shields.io/badge/HDR-10%20%C2%B7%20scRGB-f2a900)
![Estado](https://img.shields.io/badge/HDR-pendiente%20de%20validaci%C3%B3n%20visual-lightgrey)

</div>

---

> **Aviso importante.** Este repositorio contiene **solo código fuente y documentación**. No incluye el juego, ni archivos extraídos de él, ni código generado a partir de él, ni mods de terceros. *Bayonetta 2* es propiedad de Nintendo y PlatinumGames. Cada jugador debe compilar el proyecto a partir de su **propia copia legítima**. No se ofrece ni se acepta ayuda para obtener el juego por otras vías.

## Qué es

Una edición independiente de [B2recomp](https://github.com/K0NL/B2recomp) (de K0NL) centrada en tres cosas: que el juego **suene y vaya bien**, que **se vea bien en pantallas HDR** y que sea **cómodo de personalizar** sin salir del juego.

## Novedades

### HDR nativo (Vulkan)

| | |
|---|---|
| **HDR10** | PQ (ST 2084), primarias Rec.2020, 10 bits, metadatos HDR opcionales |
| **scRGB** | Lineal, FP16, 1,0 = 80 nits |
| **Automático** | HDR10 si hay, si no scRGB; solo si Windows tiene HDR activo en el monitor del juego |
| **Blanco SDR** | Sigue el control «Brillo del contenido SDR» de Windows, o un valor fijo en nits |
| **Expansión de altas luces** | Opcional, desactivada por defecto: sin ella la imagen es la original, solo convertida |
| **Cambio en caliente** | Detecta que Windows active o desactive el HDR y rehace la presentación |
| **Seguro** | Si el formato no está disponible o algo falla, vuelve al SDR original sin cerrar el juego |

Cómo funciona: la imagen se compone igual que siempre (incluidos el contador de tiempos y el menú) y **un único pase de pantalla completa en la GPU** la convierte al formato HDR. No usa ReShade ni inyecta DLL. La conversión tiene pruebas con valores conocidos (`tests/hdr_color_test.c`) y un autotest en la GPU real (`SSBU_HDR_SELFTEST=1`).

> **Estado honesto:** es la imagen SDR del juego convertida correctamente, **no** el HDR interno del motor. Presenta sin errores en HDR10 y scRGB y el shader coincide con la matemática de referencia, pero **falta la validación visual** en más pantallas. Cuéntanos qué ves en los [issues](../../issues).

Se configura en el launcher (**HDR y color avanzado…**), en el menú del juego (Gráficos) o en `config/graphics.ini` (`hdr`, `hdr_sdr_white`, `hdr_peak`, `hdr_expand`). Por defecto está **desactivado**.

### Reparaciones del runtime

- **Audio:** un estado y un dispositivo por cada `AudioOut`; los vídeos dejan de ir unas 6 veces más lentos. El sonido ya no depende del nombre del ejecutable.
- **Vulkan:** la swapchain no se recrea por `VK_SUBOPTIMAL_KHR` si el tamaño no ha cambiado.

### Menú dentro del juego

**Insert** (o L3+R3 en mando): personaje, trucos de código de la versión 1.2, armas, gráficos, mods y diagnóstico en tiempo real.

### Biblioteca de mods

Fichas, detección de conflictos por ruta y perfiles, en la ventana **Mods…** del launcher.

El estado exacto de cada función está en [ESTADO_DEL_PROYECTO.md](ESTADO_DEL_PROYECTO.md).

## Compilar

El proyecto genera todo lo del juego en tu PC a partir de **tu propio volcado**. Sigue [GUIA_COMPILACION.md](GUIA_COMPILACION.md) y el [BUILDING.md](BUILDING.md) del proyecto base. La compilación del runtime tarda unos segundos:

```
deps\python\python.exe scripts\build_runtime.py --skip-dll
```

Pruebas de color (no necesitan el juego): `scripts\test_hdr_color.bat`.

## Releases

Los ejecutables de las [Releases](../../releases) son solo el **launcher** y el **runtime** (`game_native.exe`), sin nada del juego. Siguen necesitando tu propio `game.dll`, que genera la compilación a partir de tu volcado.

## Licencia y créditos

- **GPL-3.0-or-later** ([LICENSE](LICENSE)). Esta edición y sus aportaciones se publican bajo la misma licencia, como exige la de los proyectos de origen.
- Cadena de origen: [ssbu-recomp](https://github.com/Denkishi/ssbu-recomp) (Denkishi) → [B2recomp](https://github.com/K0NL/B2recomp) (K0NL) → esta edición (**donelysentretenimiento**). Detalle en [CREDITOS_Y_AUTORIA.md](CREDITOS_Y_AUTORIA.md) y [NOTICE.md](NOTICE.md), que no se modifica.
- El README original del proyecto base se conserva en [README.upstream.md](README.upstream.md).

## Documentación

[CHANGELOG](CHANGELOG.md) · [Créditos y autoría](CREDITOS_Y_AUTORIA.md) · [Reparaciones](REPARACIONES_REALIZADAS.md) · [Cronología](EVIDENCIAS_Y_CRONOLOGIA.md) · [Estado del proyecto](ESTADO_DEL_PROYECTO.md)
