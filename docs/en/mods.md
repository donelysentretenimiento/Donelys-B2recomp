# Mods

DONELYS supports RomFS mods and `.pchtxt`/`.ips` code patches obtained from legitimate sources. It does not redistribute third-party mods.

- A RomFS mod contains `romfs\`, loads at startup, and normally needs no rebuild.
- A code mod contains `exefs\*.pchtxt` or a Build-ID-matched `.ips` and requires rebuilding from `mods`.
- `config\mods.ini` stores switches; the **Mods…** window reports path conflicts and profiles.

Put each mod in its own folder under `mods\`. Close the game before applying a profile. Inspect patches without building using `python scripts\pchtxt.py list mods targets\exefs\main`. Back up `config\mods.ini`; do not delete the whole project as a recovery method.
