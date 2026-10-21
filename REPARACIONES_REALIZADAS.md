# Reparaciones realizadas

Cada reparación indica el problema, la causa, la solución, los archivos y **hasta dónde está comprobada**. Estados usados: **Implementado**, **Probado** (con evidencia), **Experimental** y **Pendiente**.

## 1. Vídeos del juego a cámara lenta — Implementado y probado

- **Problema:** el vídeo de demostración `advertise.usm` (1280×720, 2794 fotogramas) mostraba el logo de PlatinumGames unos 56 s, cuando en el archivo dura 9 s, y después se saltaba gran parte del tráiler.
- **Causa:** al reproducir una película, el juego abre una **segunda** `AudioOut` (`OpenDefaultAudioOut` dos veces) mientras la del mezclador sigue abierta. `nnaudio.c` tenía un único estado global: la segunda apertura reiniciaba la cola de la primera y ambas compartían cola y dispositivo. El sonido de la película se consumía a una fracción del ritmo real, y CRI Mana sincroniza la imagen con su sonido.
- **Solución:** estructura `AOut` (cola, liberadas, evento, `waveOut`, reloj, canales) con hasta 4 instancias, buscadas por la dirección de la `AudioOut` del juego (`aout_of`). Cada una abre su propio `waveOut` y Windows los mezcla. El hilo de audio atiende todas las salidas.
- **Archivos:** `runtime/nnaudio.c`.
- **Comprobación:** capturas reales de la ventana del juego cada 60 fotogramas, antes y después, comparadas con los fotogramas extraídos del propio `.usm`. Después de la corrección, la luna dura unos 7 s y el tráiler avanza seguido. El usuario confirmó que la intro se veía bien.
- **Límite:** probado con `advertise.usm`; los demás `.usm` usan el mismo mecanismo pero no se han reproducido uno a uno.

## 2. Sonido silenciado al renombrar el ejecutable — Implementado

- **Problema:** `OpenDefaultAudioOut` ponía el volumen al 0 % salvo que el exe se llamara `ssbu_native.exe`, `ssbu_user.exe` o `game_native.exe`.
- **Solución:** el volumen es 100 % por defecto; solo las ejecuciones de prueba con guion (`SSBU_SEQ`, `SSBU_SCRIPT`) quedan en silencio. `SSBU_VOLUME` sigue mandando.
- **Archivos:** `runtime/nnaudio.c`.
- **Comprobación:** compilado y ejecutado; el registro muestra el volumen aplicado. **No se ha probado con un exe renombrado y a oído.**
- **Aviso:** esta corrección cubre solo el caso del exe renombrado. La causa de otros casos de «sin sonido» (por ejemplo, el issue #25 del proyecto base, con un usuario que seguía sin audio tras reinstalar) **no está confirmada** y no se afirma que quede resuelta.

## 3. Recreación de la swapchain por `VK_SUBOPTIMAL_KHR` — Implementado, sin verificar con overlay

- **Problema (hipótesis):** cada respuesta `VK_SUBOPTIMAL_KHR` de `vkAcquireNextImageKHR` o `vkQueuePresentKHR` destruía la swapchain con `vkDeviceWaitIdle`. Las capas de overlay y de composición HDR podrían devolverla en todos los fotogramas, lo que explicaría congelones y parpadeo (relacionado con los issues #22 y #15 del proyecto base).
- **Solución:** `swapchain_stale()` en `nvn_vk.c`: `VK_ERROR_OUT_OF_DATE_KHR` recrea siempre, como antes; `VK_SUBOPTIMAL_KHR` solo si el `currentExtent` de la superficie difiere del de la swapchain. Se registra una línea una sola vez si se conserva.
- **Archivos:** `runtime/nvn_vk.c`, `runtime/nvn_vk_draw.c`.
- **Comprobación:** compila y el juego mantiene 60 fps sin regresiones. **No se ha reproducido el fallo original con el overlay de NVIDIA ni con RTX HDR**, por lo que no se puede afirmar que lo resuelva.
- **Aviso:** el issue #22 se dio por resuelto el 2026-10-21 con la v0.2.9 del proyecto base, cuyo arreglo trata de otro mecanismo (el mundo del juego se congelaba al final de ciertas cámaras lentas a 120 fps). Es probable que la hipótesis de esta reparación no fuera la causa de ese fallo. Se mantiene el cambio porque evita una parada completa de la GPU que no aporta nada cuando el tamaño no cambia, pero no debe presentarse como la solución de #22.

## 4. Menú dentro del juego — Implementado; teclado probado

- **Qué es:** panel dibujado con GDI y compuesto sobre el fotograma con un pipeline Vulkan mínimo (dynamic rendering, alfa premultiplicado, fundido). Sin dependencias nuevas.
- **Entrada:** Insert (configurable en `config/menu.ini`) o L3+R3. Mientras está abierto, el juego no recibe teclado, ratón ni mando (`menu_captures_input` en `nnhid.c`), el cursor queda libre y los clics y la rueda van al menú (`display.c`).
- **Páginas:** Personaje, Trucos, Armas, Cámara, Gráficos, Mods y Diagnóstico. Lo no implementado aparece como «No disponible» con el motivo.
- **Archivos:** `runtime/menu_vk.c`, `runtime/nnhid.c`, `runtime/display.c`, `runtime/nvn_vk.c`, `runtime/nvn_vk_draw.c`.
- **Comprobación:** abrir, cerrar y navegar con el teclado en una partida real (registro y capturas). **Ratón y mando: implementados, sin probar.** Cambio de ventana y pérdida de foco: sin probar.

## 5. Trucos nativos para la versión 1.2 — Parcialmente probados

| Truco | Estado | Comprobación |
|---|---|---|
| Salud infinita (`0x1d5cc`) | **Probado** | Con el truco, la vida sigue llena tras 25 s de golpes; sin él, baja a un cuarto en 30 s (capturas de la barra) |
| Witch Time permanente (`0x1c974`, tecla F5) | **Experimental** | El modo cambia y queda registrado; el efecto no se vio con claridad en las capturas |
| Invencibilidad 1P (`0x20c14`) | **Sin probar** en partida | — |
| Personajes (Balder, Rodin, King Zero, Rosa) y arma de la ranura A | **Sin probar** en partida | Se escriben cada fotograma y se restauran al volver a Normal; bloqueados en combate |

El usuario indicó que «los trucos funcionaron», sin detallar cuáles. Magia infinita, cámara libre y daño multiplicado **no están implementados**: el menú los muestra como no disponibles.

- **Archivos:** `scripts/cheats_patch.py`, `runtime/game_shim.c`, `runtime/menu_vk.c`, `scripts/setup.py`.

## 6. Biblioteca de mods, fichas, conflictos y perfiles — Implementado y probado

- **Qué hace:** monta `mods\` desde las descargas sin tocarlas; crea fichas con autor, versión, enlace y requisitos; detecta conflictos por ruta en las pantallas del launcher; fusiona de forma segura los `.csv` de combos cuando un mod solo cambia valores `ATK_`; ofrece cinco perfiles.
- **Archivos:** `tools/mods/biblioteca.py`, `scripts/pchtxt.py`, `launcher/launcher.c`, `config/perfiles/`.
- **Comprobación:** la ventana **Mods...** muestra fichas y perfiles (captura de la ventana); con el perfil «Bayonetta 3 Experience» el juego carga los seis mods del perfil y los iconos (el registro indica 7 activos); los iconos de Xbox se ven en la partida.
- **Límite:** el aspecto de los modelos y armas modificados **no se ha visto en pantalla**; los archivos se cargan, pero no se ha comprobado visualmente cada mod. Los mods de movimientos y las fusiones de `.csv` no se han probado en combate.
