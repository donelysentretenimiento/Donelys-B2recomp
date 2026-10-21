# Instalación

## Requisitos

- Windows 10 u 11 de 64 bits.
- GPU y controlador con Vulkan 1.3.
- Bayonetta 2 para Nintendo Switch, versión 1.2 con actualización v131072, procedente de tu propia copia.
- Espacio para el volcado (`romfs` ocupa alrededor de 12,6 GB), el código generado y la compilación.

El constructor comprueba el Build ID esperado (`F19B2E01E1794F933A9720C1404BB6F6`) y el contenido principal de RomFS. No se ha probado otra versión.

## Archivos de tu copia

Selecciona una carpeta con esta estructura:

```text
volcado/
├─ exefs/
│  ├─ main
│  ├─ sdk
│  ├─ subsdk0
│  ├─ rtld
│  └─ main.npdm
├─ romfs/
│  ├─ data000.cpk ... data006.cpk
│  ├─ movie/
│  └─ sound/
├─ sysdata/fonts/                  opcional
└─ control/icon_AmericanEnglish.dat  opcional
```

Los NSO deben estar descifrados y sin contenedor. No publiques estos archivos ni claves en una incidencia.

## Método recomendado: paquete DONELYS Builder

1. Abre [Releases](https://github.com/donelysentretenimiento/Donelys-B2recomp/releases) y comprueba si el paquete es STABLE o PREVIEW.
2. Descarga el archivo cuyo nombre contiene `builder`; si existe `SHA256SUMS.txt`, verifícalo como explica [actualizaciones](actualizaciones.md#verificar-una-descarga).
3. Descomprime todo en una ruta corta, por ejemplo `D:\Juegos\DONELYS-B2recomp`. No ejecutes directamente desde el ZIP.
4. Abre `B2recomp.exe`. El paquete incluye Python, llvm-mingw, SDL3, Vulkan-Headers, el compilador de shaders y sus runtimes.
5. En la pantalla Build selecciona la carpeta `volcado` anterior y pulsa **Build**.
6. El proceso verifica, convierte, aplica tus mods de código, genera tablas, traduce, compila, enlaza y realiza una comprobación final. No cierres la ventana durante la compilación.
7. Al terminar, configura gráficos y mando y pulsa **Play**.

El constructor enlaza `targets\romfs` con el RomFS seleccionado en vez de copiar sus 12,6 GB. Si mueves o borras el volcado, vuelve a seleccionar su nueva ubicación y reconstruye desde `convert`.

## Si falla

Conserva el texto del error y genera el informe mediante `deps\python\python.exe scripts\report.py`. Revisa [solución de problemas](problemas.md) antes de abrir una incidencia.
