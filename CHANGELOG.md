# Registro de cambios

Todos los cambios de esta edición sobre **B2recomp v0.2.7** (commit `b3a6dc6`, de K0NL). Las fechas son las de los commits (hora de Madrid, UTC+2). Formato inspirado en *Keep a Changelog*.

## [Sin versionar] — 2026-10-21

### Añadido
- **Biblioteca de mods** (`fa1ce2c`, 14:31): `tools/mods/biblioteca.py` monta la carpeta `mods\` a partir de las descargas sin tocar los originales, genera fichas `mod.ini`, fusiona los `.csv` de combos de forma segura (solo valores `ATK_`) y escribe un inventario con SHA-256 de cada archivo.
- **Perfiles de mods** (`fa1ce2c`): cinco perfiles en `config/perfiles/` y selector «Cargar perfil» en la ventana **Mods...** del launcher.
- **Fichas y conflictos en la ventana Mods...** (`fa1ce2c`): `scripts/pchtxt.py tree` etiqueta cada mod con categoría y versión y marca en rojo los conflictos por ruta idéntica entre mods activados.
- **Menú dentro del juego** (`2f1bd3d`, 14:48): `runtime/menu_vk.c`. Se abre con Insert (configurable en `config/menu.ini`) o con L3+R3; páginas Personaje, Trucos, Armas, Cámara, Gráficos, Mods y Diagnóstico.
- **Trucos nativos para la versión 1.2** (`2f1bd3d`): `scripts/cheats_patch.py` inserta interruptores en el C traducido (salud infinita, invencibilidad, Witch Time permanente) y el menú escribe los datos de personaje y arma de la ranura A.

### Corregido
- **Vídeos CRI a cámara lenta** (`4c6f156`, 16:14): una estructura `AOut` por cada `AudioOut` y un `waveOut` por cada una. Antes, la segunda `AudioOut` que abre el juego para el sonido de una película reiniciaba la primera.
- **Sonido silenciado según el nombre del ejecutable** (`37d5c35`, 16:17): el volumen ya no vale 0 % si el exe se llama de otra forma.
- **Recreación innecesaria de la swapchain** (`37d5c35`): `swapchain_stale()` solo recrea ante `VK_SUBOPTIMAL_KHR` si el tamaño de la superficie ha cambiado.
- **`cheats_patch.py` reaplicable** y contadores de paso de los trucos para poder verificar que el juego pasa por cada punto (`523d9e0`, 22:15).

### Sin cambios en esta edición
- `config/graphics.ini` y `config/controllers.ini` conservan los valores por defecto del proyecto base.
