# Troubleshooting

| Message or symptom | Action |
|---|---|
| Missing `exefs` or `romfs` | Select their parent folder. |
| `main` is v0, another version, or not a plain NSO | Extract your copy again with update v131072 applied and decrypted files. |
| Missing RomFS files | Repeat the complete extraction. |
| Junction creation fails | Use NTFS, avoid protected paths, and ensure `targets\romfs` is not a real folder containing data. |
| Tool error | Run `deps\python\python.exe scripts\setup.py --only deps`. |
| Black screen or Vulkan failure | Update the driver, disable HDR and overlays, and create a report. |
| Dump moved | Run `setup.py --from convert` with the new `--dump` path. |

Create a diagnostic report with `deps\python\python.exe scripts\report.py`. Review it before posting; never attach keys, game files, private saves, or unnecessary personal paths.
