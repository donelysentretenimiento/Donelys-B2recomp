# Actualizaciones e integridad

Las actualizaciones automáticas recuperables aún no están validadas. Conserva la versión anterior hasta comprobar la nueva.

1. Cierra juego y launcher.
2. Copia `config\`, `mods\`, tus perfiles y la carpeta de datos de usuario de la instalación.
3. Descomprime la nueva versión en otra carpeta.
4. Comprueba su canal, versión y checksum.
5. Selecciona el mismo volcado y construye.
6. Copia o fusiona solamente tus configuraciones y mods; no reemplaces a ciegas archivos nuevos.
7. Comprueba arranque, partidas, mando y mods antes de borrar la instalación anterior.

## Verificar una descarga

Si la release publica `SHA256SUMS.txt`, abre PowerShell en la carpeta descargada y ejecuta:

```powershell
Get-FileHash .\NOMBRE-DEL-PAQUETE.zip -Algorithm SHA256
```

El valor debe coincidir exactamente. Un checksum detecta alteraciones, pero no equivale a una firma. Las attestations sólo se considerarán disponibles cuando el workflow y la release las publiquen realmente.

Si una actualización falla, vuelve a la carpeta anterior y conserva la nueva para diagnóstico. No borres partidas, mods ni el volcado.
