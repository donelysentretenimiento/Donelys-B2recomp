# Autoría y contribuciones: evidencias

Esta carpeta conserva la prueba documental de las aportaciones de esta edición al proyecto B2recomp y de lo que ocurrió con ellas. La cronología explicada está en [EVIDENCIAS_Y_CRONOLOGIA.md](../../EVIDENCIAS_Y_CRONOLOGIA.md).

## Contenido

| Archivo | Qué es |
|---|---|
| `copias-de-github/*.json` | Respuestas **en bruto** de la API pública de GitHub, tal como se descargaron: PR #26, incidencias #27 y #25 con comentarios y línea temporal, los dos commits del fork, la lista de versiones publicadas |
| `copias-de-github/pr26_commits.patch` | Los dos commits del PR tal como figuran en GitHub (`e95cbeb..ebb5726`) |
| `copias-de-github/texto_pr26_publicado.md`, `texto_issue27_publicado.md` | Texto enviado en el PR y en la incidencia |
| `copias-de-github/texto_retirada_pr26_y_issue27.md` | Texto con el que se retiraron |
| `comprobar_cambios_originales.ps1` | Script que compara el repositorio original con los identificadores de este trabajo. Se puede repetir |
| `comprobacion_cambios_originales_2026-10-21.txt` | Resultado de ese script el 2026-10-21 |
| `SHA256SUMS.txt` | Huellas de todos los archivos de esta carpeta |

## Reglas de conservación

- Los archivos de `copias-de-github/` **no se editan**. Si se quiere actualizar una evidencia, se añade una copia nueva con su fecha.
- Las URL públicas originales siguen siendo la fuente principal:
  - Commits: <https://github.com/donelysentretenimiento/B2recomp/commit/701c6b373651084aaf5c96f52f20c50452fdd265> y <https://github.com/donelysentretenimiento/B2recomp/commit/ebb5726765536941998660ca758ed4ba9099e34b>
  - PR: <https://github.com/K0NL/B2recomp/pull/26>
  - Incidencia: <https://github.com/K0NL/B2recomp/issues/27>
- Para comprobar la integridad: `Get-FileHash <archivo> -Algorithm SHA256` y comparar con `SHA256SUMS.txt`.
- Las copias contienen solo datos públicos de GitHub.
