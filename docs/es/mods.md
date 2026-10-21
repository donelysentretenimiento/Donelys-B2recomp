# Mods

DONELYS admite mods RomFS y parches de código `.pchtxt`/`.ips` procedentes de fuentes legítimas. No redistribuye mods de terceros.

- Un mod RomFS contiene una carpeta `romfs\`; se carga al iniciar y normalmente no requiere recompilar.
- Un mod de código contiene `exefs\*.pchtxt` o un `.ips` cuyo Build ID coincide con la versión compatible; requiere reconstruir desde `mods`.
- `config\mods.ini` guarda los interruptores. La ventana **Mods…** muestra conflictos por ruta y perfiles.

Instala cada mod en una carpeta propia bajo `mods\`. Revisa conflictos, cierra el juego, aplica el perfil y reconstruye cuando el launcher lo indique. Para inspeccionar sin compilar:

```powershell
python scripts\pchtxt.py list mods targets\exefs\main
```

Haz una copia de `config\mods.ini` antes de cambios grandes. Quitar un parche y reconstruir restaura el ejecutable generado desde el volcado original; no borres todo el proyecto como método de recuperación.
