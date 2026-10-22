"""Regression test for linker output paths containing spaces and commas."""
import os
from pathlib import Path
import shutil
import subprocess
import tempfile


ROOT = Path(__file__).resolve().parents[1]
CLANG = Path(os.environ.get("B2RECOMP_TEST_CLANG", ROOT / "deps" / "llvm-mingw" / "bin" / "x86_64-w64-mingw32-clang.exe"))


def run(*args):
    return subprocess.run([str(CLANG), *map(str, args)], capture_output=True, text=True)


def link_outputs(folder, linker_args):
    folder.mkdir(parents=True)
    dll = folder / "sample.dll"
    implib = folder / "sample.dll.a"
    exe = folder / "sample.exe"
    pdb = folder / "sample.pdb"
    dll_result = run("-shared", folder.parent / "dll.c", "-o", dll, *linker_args("--out-implib=", implib))
    exe_result = run(folder.parent / "main.c", "-g", "-o", exe, *linker_args("--pdb=", pdb))
    return (dll_result, exe_result), (dll, implib, exe, pdb)


def old_wl(option, path):
    return ["-Wl," + option + str(path)]


def fixed_xlinker(option, path):
    return ["-Xlinker", option + str(path)]


def main():
    if not CLANG.is_file():
        raise SystemExit(f"clang not found: {CLANG}")

    temp = Path(tempfile.mkdtemp(prefix="B2recomp issue 34, comma path "))
    try:
        (temp / "dll.c").write_text("__declspec(dllexport) int answer(void) { return 42; }\n", encoding="ascii")
        (temp / "main.c").write_text("int main(void) { return 0; }\n", encoding="ascii")

        old_results, _ = link_outputs(temp / "old, broken", old_wl)
        assert all(result.returncode != 0 for result in old_results), "the original -Wl, syntax unexpectedly accepted comma paths"
        assert any("could not open" in (result.stdout + result.stderr) for result in old_results), "the original failure was not reproduced"

        for name in ("fixed, spaces and commas", "fixed-simple"):
            results, outputs = link_outputs(temp / name, fixed_xlinker)
            for result in results:
                assert result.returncode == 0, result.stdout + result.stderr
            for output in outputs:
                assert output.is_file(), f"missing output: {output}"

        build_script = (ROOT / "scripts" / "build_runtime.py").read_text(encoding="utf-8")
        assert "['-Xlinker', '--out-implib=' + IMPLIB(DLL)]" in build_script
        assert "'-Xlinker', '--pdb=' + EXE[:-4] + '.pdb'" in build_script
        print("PASS: -Xlinker preserved comma paths for DLL/import library and EXE/PDB; simple paths also passed")
    finally:
        shutil.rmtree(temp)


if __name__ == "__main__":
    main()
