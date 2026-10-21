# Solución de problemas

| Mensaje o síntoma | Acción |
|---|---|
| Faltan `exefs` o `romfs` | Selecciona la carpeta padre que contiene ambas. |
| `main` es v0, otra versión o no es NSO plano | Extrae de nuevo tu copia con la actualización v131072 aplicada y los archivos descifrados. |
| Faltan archivos RomFS | Repite la extracción completa; no uses una carpeta parcial. |
| No se puede crear la junction | Usa una unidad NTFS, evita rutas protegidas y comprueba que `targets\romfs` no sea una carpeta real con datos. |
| Error de herramienta | Ejecuta `deps\python\python.exe scripts\setup.py --only deps`. |
| Ventana negra o fallo Vulkan | Actualiza el driver, desactiva HDR y overlays, y genera un informe. |
| Colores HDR incorrectos | Usa SDR, registra modo/monitor/driver y abre una incidencia HDR. |
| El juego no encuentra RomFS tras mover carpetas | Vuelve a ejecutar `setup.py --from convert` con `--dump` en la ruta nueva. |

Informe:

```powershell
deps\python\python.exe scripts\report.py
```

Revisa el informe antes de publicarlo. No adjuntes claves, archivos del juego, partidas privadas ni rutas que revelen datos personales innecesarios.
