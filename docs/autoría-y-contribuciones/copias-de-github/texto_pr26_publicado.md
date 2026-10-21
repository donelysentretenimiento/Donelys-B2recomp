## Resumen

Tres correcciones del runtime, encontradas y comprobadas jugando la versión 1.2 (build `F19B2E01E1794F93`) en Windows 11 con una RTX 5090:

1. **Los vídeos CRI (`.usm`) se reproducían unas 6 veces más lentos y después se saltaban medio vídeo.**
2. **El juego quedaba en silencio si el ejecutable tenía otro nombre** (relacionado con #25).
3. **Congelones y parpadeo con el overlay de NVIDIA, RTX HDR y otros** (relacionado con #22 y #15).

---

## 1. Vídeos CRI lentos: una salida de audio por cada `AudioOut`

### Qué pasaba
Si dejas el juego quieto en la pantalla de título, arranca el vídeo de demostración `rom:/movie/advertise.usm`: 1280×720, 2794 fotogramas a 30 fps, unos 93 s. Comparando capturas reales del juego (`SSBU_SHOT_EVERY`) con los fotogramas del propio archivo:

| Momento del vídeo | En el archivo | En B2recomp, antes |
|---|---|---|
| Luna con el logo de PlatinumGames | 0–9 s | **~56 s** en pantalla |
| Del ángel enmascarado a la explosión | 10 s → 70 s | **~16 s** (salto hacia delante) |
| Final | 93 s | a su hora |

La CPU no era el problema: `CriManaDecodeThread` usaba unos 0,6 ms por fotograma.

### Causa
Al empezar un vídeo, Bayonetta 2 abre una **segunda** `AudioOut` (`OpenDefaultAudioOut` dos veces) para el sonido de la película, mientras la del mezclador sigue abierta, y la cierra al acabar el vídeo. En el registro se ve:

```
nn::audio: OpenDefaultAudioOut ... (mezclador)
nn::audio: OpenDefaultAudioOut ... (película)
nn::audio: StopAudioOut / CloseAudioOut (película)
```

`nnaudio.c` tenía **un solo estado global**: la segunda apertura reiniciaba la cola de la primera, y los dos productores compartían una cola y un único `waveOut`. El sonido de la película se consumía a una fracción del ritmo real. CRI Mana marca el ritmo de la imagen con su sonido, así que el vídeo iba a cámara lenta y después saltaba para acabar a la vez que el audio.

### Corrección
- Todo el estado de `AudioOut` (cola, liberadas, evento, `waveOut`, reloj, frecuencia y canales) pasa a una estructura `AOut`. Hay hasta 4 instancias, y cada una se busca por la dirección del `AudioOut` del juego (`aout_of`).
- Cada `AudioOut` abre **su propio** dispositivo `waveOut`, y Windows mezcla los dispositivos.
- El hilo de audio recoge los búferes terminados de todas las salidas abiertas y señala el evento de cada una.
- Para tocar lo mínimo, el código existente sigue usando los mismos nombres (`g_wave`, `g_head`, …) mediante `#define`s hacia `A_->campo`.

### Comprobado
Con la corrección, la luna dura unos 7 s, como en el archivo, y el tráiler avanza seguido hasta el final. Afecta a todos los vídeos del juego, incluidos los créditos (`Sw_StaffRoll.usm`).

---

## 2. Sin sonido al renombrar el ejecutable (#25)

`OpenDefaultAudioOut` ponía el volumen al 0 % salvo que el exe se llamara `ssbu_native.exe`, `ssbu_user.exe` o `game_native.exe`. Cualquier copia renombrada (un acceso de Steam, un wrapper) sonaba muda sin avisar.

**Corrección:** el sonido va siempre al 100 %. Solo se silencian las ejecuciones de prueba con guion (`SSBU_SEQ` / `SSBU_SCRIPT`), y `SSBU_VOLUME` sigue mandando.

---

## 3. `VK_SUBOPTIMAL_KHR` en cada fotograma (#22, #15)

Cada vez que `vkAcquireNextImageKHR` o `vkQueuePresentKHR` devolvía `VK_SUBOPTIMAL_KHR`, se destruía la swapchain con `vkDeviceWaitIdle`, en los tres sitios de `nvn_vk.c` y `nvn_vk_draw.c`. Las capas de overlay y la composición HDR (overlay de NVIDIA / RTX HDR, Steam, gamescope) pueden devolver `SUBOPTIMAL` en **todos** los fotogramas sin que nada cambie: la GPU se paraba en cada uno, con congelones y parpadeo. Encaja con #22, donde el juego se congela a 120 fps con el overlay de NVIDIA y vuelve al pulsar Alt+Z.

**Corrección:** nueva función `swapchain_stale()` en `nvn_vk.c`:
- `VK_ERROR_OUT_OF_DATE_KHR` → se recrea siempre, como antes.
- `VK_SUBOPTIMAL_KHR` → solo si el `currentExtent` de la superficie es distinto del tamaño de la swapchain. Si no, se conserva y se escribe una línea en el registro una sola vez.

---

## 4. Nuevo: menú dentro del juego (Insert) con trucos nativos

Un menú propio del runtime que se abre con **Insert** (configurable en `config\menu.ini`, que se crea solo) o con **L3+R3** en el mando.
- **Dibujo:** el panel se pinta con GDI (Segoe UI) en un DIB solo cuando cambia algo y se compone sobre el fotograma con un pipeline Vulkan mínimo (dynamic rendering, alfa premultiplicado, fondo oscurecido y fundido de 120 ms). Los shaders se compilan con `ssbu_glslang.dll`. No añade dependencias.
- **Entrada:** mientras está abierto, el juego no recibe teclado, ratón ni mando (`nnhid.c` consulta `menu_captures_input`), el cursor queda libre y los clics y la rueda van al menú (`display.c`).
- **Páginas:** Personaje, Trucos, Armas, Cámara, Gráficos, Mods y Diagnóstico (fps, tiempo de fotograma, CPU/GPU, mods activos y conflictos por ruta). Lo que no está implementado aparece como "No disponible", con el motivo.
- **Desactivación:** apagado en las ejecuciones con guion (`SSBU_SEQ` / `SSBU_SCRIPT`) y con `SSBU_MENU=0`.

### Trucos (solo para la versión 1.2, build `F19B2E01E1794F93`)
`game.dll` exporta `recomps_cheat[16]` (`game_shim.c`). El nuevo `scripts/cheats_patch.py`, llamado desde el paso `lift` de `setup.py` después de `fps_patch.py`, inserta en el C traducido una comprobación de ese vector en la dirección de cada código. Con todos los valores a 0, el juego se comporta como sin el parche. El script es idempotente: quita una versión anterior antes de volver a aplicarse.

| Truco | Origen | Dónde | Tipo |
|---|---|---|---|
| Witch Time: invencible / permanente detenido / permanente lento | Gamerjin (GBAtemp), código para `F19B2E01…` | `0x1c974` (`f_710001c820`), la comprobación `ldr w8,[x8]` | interruptor en tiempo real (+ tecla F5) |
| Salud infinita | xtatu (v1.0), NOP en `0x1d5cc` | se salta la llamada | interruptor |
| Invencibilidad 1P | xtatu (v1.0), RET en `0x20c14` | la función vuelve al instante | interruptor |
| Personaje (Balder, Rodin, King Zero, Rosa) y arma de la ranura A | pearl23, "Misc Bayonetta 2 Cheats" 1.1, para `F19B2E01…` | bytes en `main + 0x194c3cc / 0x19598c4 / 0x34e5898 / 0x34e5790…` | escritura cada fotograma, con el valor original restaurado al volver a Normal; bloqueado durante los combates |

Los dos códigos de la v1.0 se portaron tras comprobar que esa zona es idéntica en la 1.2: `0x312f4` empieza en ambas con el mismo `sub sp, sp, #0x150` que restaura el código de salto de la v1.0, `0x20c14` es inicio de función, y el código de Gamerjin para la 1.2 engancha en `0x1c974`, igual que en la v1.0.

---

## Qué se ha probado y qué no

**Probado en partida real** (prólogo, primer combate, manejando el juego con el teclado y con capturas del propio juego):
- ✅ **Vídeo de la intro**: velocidad correcta tras la corrección.
- ✅ **Menú Insert**: abre, cierra y navega con el teclado, y bloquea la entrada del juego mientras está abierto.
- ✅ **Salud infinita**: con el truco, la vida sigue llena tras 25 s recibiendo golpes. Sin él, cae a un cuarto en 30 s.
- ✅ **El jugador confirmó** que los trucos funcionaron en su partida y que la intro se ve bien.

**Probado solo en parte:**
- ⚠️ **Witch Time permanente (F5)**: el modo cambia y queda registrado, pero en mi captura no se vio el efecto. Lo he dejado listo para revisar con contadores de paso.

**Sin probar todavía en partida:**
- ❌ Invencibilidad 1P, personajes (Balder, Rodin, King Zero, Rosa) y armas descartadas.

## Cómo se ha probado
- Compilado con la cadena del builder (llvm-mingw, `scripts\build_runtime.py`), sin errores.
- Vídeo de la intro: capturas cada 60 fotogramas antes y después de la corrección, comparadas con los fotogramas extraídos de `advertise.usm`.
- Arranques de 40–200 s: 60,0 fps estables en menús y juego, y 30 fps en los vídeos, como es debido.
- Partida real hasta el primer combate del prólogo, manejando el juego con el teclado.

Los cambios son solo de `runtime/`, sin cambios en el lifter ni en los datos.
