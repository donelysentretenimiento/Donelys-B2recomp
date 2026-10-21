# Propuesta de protección del repositorio

Estado comprobado el 2026-10-22: repositorio público, `main` predeterminada, ramas remotas `main` y `hdr-nativo`, sin rulesets visibles, sin workflows y GitHub Pages desactivada. Este documento prepara cambios; no afirma que estén aplicados.

## Ruleset `stable-main`

Destino: rama `main`. Enforcement: **Active** después de validar el primer workflow.

- Bloquear borrado y force-push.
- Exigir Pull Request antes de fusionar, con 1 aprobación.
- Invalidar aprobaciones cuando cambie el contenido.
- Exigir resolución de conversaciones.
- Exigir el check `repository-checks` cuando haya completado al menos una ejecución en la rama.
- Permitir bypass al propietario del repositorio para recuperación, siempre registrando el motivo.

## Ruleset `preview-hdr`

Destino: rama `hdr-nativo`.

- Bloquear borrado y force-push.
- Exigir Pull Request y resolución de conversaciones.
- Exigir `repository-checks` después de comprobar su nombre real.
- Mantener bypass del propietario para el flujo local de Codex/Claude, sin permitir force-push habitual.

## Ruleset `release-tags`

Destino: etiquetas `v*`.

- Bloquear actualización y borrado de tags existentes.
- Restringir la creación de tags de release al propietario.

## Configuración adicional

1. **Settings → Actions → General → Workflow permissions:** `Read repository contents permission`; permitir escritura sólo en jobs concretos de una futura release revisada.
2. **Settings → Code security and analysis:** activar Dependabot alerts y secret scanning si GitHub los ofrece para este repositorio.
3. **Settings → General → Pull Requests:** conservar al menos merge o squash; no activar auto-merge hasta tener checks obligatorios fiables.
4. **Pages:** publicar desde `docs/` de `main` o desde un workflow revisado sólo tras fusionar la documentación. Hasta entonces `has_pages` seguirá en `false`.
5. **Releases:** exigir checksum, commit de construcción, canal y resultados de pruebas. No automatizar una release STABLE.

Aplicar primero en modo **Evaluate** si la interfaz lo ofrece. Confirmar que una Pull Request de prueba puede ejecutarse y que el propietario conserva recuperación antes de activar enforcement.
