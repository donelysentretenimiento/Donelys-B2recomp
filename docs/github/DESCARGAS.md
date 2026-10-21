# Política de descargas y releases

## Canales

- **STABLE:** cumple compilación, instalación, actualización, conservación de datos, pruebas pertinentes, documentación, licencias y checksum.
- **PREVIEW:** función utilizable con una limitación concreta pendiente de validar; debe marcarse como pre-release.
- **EXPERIMENTAL:** artefacto de desarrollo, no recomendado como instalación habitual.

## Contenido mínimo de una release

- Tag y título con el canal.
- Commit exacto usado para construir.
- Paquete DONELYS Builder; nunca archivos del juego.
- `SHA256SUMS.txt` generado después de crear el paquete.
- Lista de pruebas realmente ejecutadas y hardware requerido que no se haya podido probar.
- Licencia y avisos de terceros incluidos.

Generar checksum en PowerShell:

```powershell
(Get-FileHash .\Donelys-B2recomp-builder-VERSION.zip -Algorithm SHA256).Hash.ToLower() + "  Donelys-B2recomp-builder-VERSION.zip" | Set-Content -Encoding ascii SHA256SUMS.txt
```

Una futura attestación de GitHub debe producirse dentro del workflow que construye el artefacto y verificarse con GitHub CLI. No se debe describir como existente hasta que la release contenga la evidencia real.
