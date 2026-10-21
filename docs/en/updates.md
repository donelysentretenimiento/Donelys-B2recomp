# Updates and integrity

Recoverable automatic updates are not yet validated. Keep the previous installation until the new one passes your checks.

1. Close the game and launcher.
2. Back up `config\`, `mods\`, profiles, and installation user data.
3. Extract the new version to another folder.
4. Check its channel, version, and checksum.
5. Select the same dump and build.
6. Merge only your configuration and mods; do not blindly overwrite new files.
7. Test startup, saves, controller, and mods before removing the previous folder.

To verify a published checksum:

```powershell
Get-FileHash .\PACKAGE-NAME.zip -Algorithm SHA256
```

The value must exactly match `SHA256SUMS.txt`. A checksum detects changes but is not a signature. Artifact attestations are available only when a release and workflow actually publish them.
