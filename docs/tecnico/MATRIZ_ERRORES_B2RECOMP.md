# Matriz técnica de errores de B2recomp

Actualizado: 2026-10-22. Leyenda: **corregido** = cambio implementado y prueba automática; **parcial** = evidencia técnica disponible, falta una ejecución integrada; **pendiente** = aún no modificado.

| Prioridad | Área | Síntoma / riesgo | Causa comprobada | Estado y evidencia |
|---:|---|---|---|---|
| P0 | HDR, launcher | Un pico válido de 329 nits era rechazado | Límite inferior fijo de 400 nits en launcher y parser | **Corregido**. Prueba literal de 329 nits y compilación del launcher |
| P0 | HDR, runtime | Un pico manual menor de 400 terminaba usando monitor o 1000 nits | `hdr_peak()` exigía `>= 400` | **Corregido**. Resolución única de niveles, 41 pruebas de color/política |
| P0 | Metadatos HDR10 | MaxCLL y MaxFALL contenían el blanco SDR | Se confundían capacidades de pantalla y datos del contenido | **Corregido**. Luminancia de pantalla separada; contenido desconocido se envía como 0 |
| P0 | Cambio de monitor | Sólo el booleano HDR provocaba recreación | No se comparaban identidad, modo, color o profundidad | **Corregido**. 6 pruebas de clasificación; luminancia sola no recrea |
| P0 | Vulkan | Una superficie RGBA podía recibir una vista BGRA incompatible | Vista intermedia fijada a `B8G8R8A8_UNORM` | **Corregido**. La vista conserva el formato elegido por la superficie; `nvn_vk.c` compila |
| P0 | Imagen oscura/apagada | Issue K0NL/B2recomp #15: Vulkan oscuro y desaturado, OpenGL correcto | El issue sigue abierto; no ofrece mediciones. En esta rama había metadatos incorrectos y límites inconsistentes, ambos capaces de alterar la presentación HDR | **Parcial**. Corregidos dos defectos objetivos; falta ejecución HDR activa y captura numérica de la salida |
| P1 | Compilación aislada | El worktree limpio no contiene `runtime/gen` ni dependencias ignoradas | Son artefactos locales deliberadamente no versionados | **Parcial**. Launcher compila; 28/45 unidades host compilan y las 17 restantes fallan sólo por generados ausentes; unidad Vulkan modificada compila con los generados del checkout local |
| P1 | Actualización upstream | v0.2.9 mejora 120 FPS; v0.2.10 añade actualización incremental | La rama HDR parte de un estado anterior y no debe fusionar todo sin auditoría | **Pendiente selectivo**. Inventariar commits y portar sólo cambios comprobados |
| P1 | Audio y cinemáticas | Posibles desfases, velocidad o estados duplicados | Requiere reproducir con registros y rutas deterministas | **Pendiente** |
| P1 | AMD / Intel Vulkan | Diferencias de extensiones, formatos y sincronización | Cobertura actual principalmente NVIDIA | **Pendiente**; preparar pruebas de capacidades y rutas de reserva |
| P2 | Mandos / entrada | Combinaciones y hotplug sin matriz completa | Cobertura insuficiente | **Pendiente** |
| P2 | Reubicación / actualización | Rutas absolutas o actualización pueden perder estado | Debe verificarse sobre copia temporal | **Pendiente** |
| P2 | Ultrawide / 120 FPS | Temporización y relación de aspecto experimentales | v0.2.9 contiene cambios relevantes, aún no portados | **Pendiente selectivo** |

## Fuentes de comparación

- Issue upstream [#15, “Low Brightness and Muted Colors on HDR Display”](https://github.com/K0NL/B2recomp/issues/15), abierto el 2026-10-19 y todavía sin diagnóstico técnico concluyente al revisar el 2026-10-22.
- Releases upstream [v0.2.9](https://github.com/K0NL/B2recomp/releases/tag/v0.2.9) y [v0.2.10](https://github.com/K0NL/B2recomp/releases/tag/v0.2.10). La primera cambia temporización/renderizado a más de 60 FPS; la segunda cambia el flujo de actualización y declara el código de juego igual al de v0.2.9.
