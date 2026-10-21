# Configuración

El launcher escribe preferencias bajo `config\`; las partidas, cachés y archivos generados se mantienen separados.

- `config\graphics.ini`: Vulkan, pantalla, HDR y gráficos.
- `config\controllers.ini`: asignación de mandos.
- `config\menu.ini`: tecla del menú Insert y atajo de Witch Time.
- `config\mods.ini`: mods activos.
- `config\perfiles\`: perfiles incluidos.
- `cache\vk\`: shaders y caché de pipelines; se puede regenerar, pero borrarla causa recompilaciones y tirones iniciales.

Abre el menú interno con **Insert** o L3+R3. Los apartados muestran gráficos, mods, trucos y diagnóstico. Algunas opciones experimentales se bloquean durante combate para reducir el riesgo de cuelgue.

No sustituyas archivos INI durante una actualización sin comparar tus cambios. Haz una copia de `config\`, `mods\` y de la carpeta de datos de usuario antes de probar una preview.
