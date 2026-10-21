## Fallos encontrados (con corrección en #26)

Versión del juego 1.2 (build `F19B2E01E1794F93`), B2recomp v0.2.7/v0.2.8, Windows 11, RTX 5090.

### 1. La intro y los vídeos van a cámara lenta y luego se saltan partes
- **Síntoma:** al quedarse en la pantalla de título, el vídeo de demostración (`advertise.usm`) muestra el logo de PlatinumGames sobre la luna casi parado durante ~56 s, cuando en el archivo dura 9 s. Después se salta medio tráiler y acaba a su hora.
- **Causa:** el juego abre una **segunda** `AudioOut` para el sonido de la película y `nnaudio.c` solo admitía una. El sonido de la película iba lento, y CRI Mana sincroniza la imagen con él.
- Afecta a todos los `.usm` (tráileres, créditos `Sw_StaffRoll.usm`, etc.).

### 2. Sin sonido si el ejecutable se renombra (relacionado con #25)
- El volumen se ponía al 0 % si el exe no se llamaba `game_native.exe`, `ssbu_native.exe` o `ssbu_user.exe`.

### 3. Congelones o parpadeo con el overlay de NVIDIA o RTX HDR (relacionado con #22 y #15)
- Cada `VK_SUBOPTIMAL_KHR` destruía la swapchain esperando a la GPU. Con overlays ocurre en cada fotograma.

### Además: propuesta de menú dentro del juego
En #26 también va un menú nativo (tecla Insert o L3+R3) con información de rendimiento y mods y con trucos para la 1.2 (salud infinita, invencibilidad, Witch Time permanente, personajes y armas). Todo está desactivado por defecto.

La explicación técnica completa, cómo se ha probado y qué queda sin probar está en el pull request #26.
