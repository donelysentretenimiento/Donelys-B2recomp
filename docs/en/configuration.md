# Configuration

The launcher stores preferences under `config\`; saves, caches, and generated files remain separate.

- `config\graphics.ini`: Vulkan, display, HDR, and graphics.
- `config\controllers.ini`: controller mapping.
- `config\menu.ini`: Insert menu and Witch Time hotkeys.
- `config\mods.ini`: enabled mods.
- `config\perfiles\`: bundled profiles.
- `cache\vk\`: shader and pipeline cache; it is regenerable, but deleting it causes initial recompilation and stutter.

Open the in-game menu with **Insert** or L3+R3. Back up `config\`, `mods\`, and user data before testing a preview. Do not overwrite INI files during an update without comparing your changes.
