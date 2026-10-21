# Installation

## Requirements

- 64-bit Windows 10 or 11.
- A GPU and driver supporting Vulkan 1.3.
- Your own Bayonetta 2 for Nintendo Switch version 1.2 dump with update v131072 applied.
- Space for the dump (RomFS is about 12.6 GB), generated code, and build output.

The builder checks the expected Build ID (`F19B2E01E1794F933A9720C1404BB6F6`) and key RomFS content. Other versions have not been validated.

## Files from your copy

Select a folder with this layout:

```text
dump/
├─ exefs/       main, sdk, subsdk0, rtld, main.npdm
├─ romfs/       data000.cpk ... data006.cpk, movie/, sound/
├─ sysdata/fonts/                     optional
└─ control/icon_AmericanEnglish.dat   optional
```

NSO files must be decrypted, extracted files. Never publish these files or keys in an issue.

## Recommended method: DONELYS Builder package

1. Open [Releases](https://github.com/donelysentretenimiento/Donelys-B2recomp/releases) and check whether the package is STABLE or PREVIEW.
2. Download the file containing `builder`; verify `SHA256SUMS.txt` when supplied.
3. Extract everything to a short path such as `D:\Games\DONELYS-B2recomp`. Do not run it inside the ZIP.
4. Open `B2recomp.exe`. The complete package includes Python, llvm-mingw, SDL3, Vulkan-Headers, the shader compiler, and required runtimes.
5. On the Build screen, choose the dump folder above and select **Build**.
6. The process verifies, converts, applies your code mods, generates tables, translates, compiles, links, and performs a final check.
7. Configure graphics and controls, then choose **Play**.

`targets\romfs` is a junction to the selected RomFS rather than a 12.6 GB copy. If the dump moves, select its new location and rebuild from `convert`.

On failure, retain the error text and run `deps\python\python.exe scripts\report.py`. See [troubleshooting](troubleshooting.md).
