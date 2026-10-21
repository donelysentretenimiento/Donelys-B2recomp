"""Repository checks that do not require proprietary game files."""

from __future__ import annotations

import re
import subprocess
import sys
from pathlib import Path

LINK = re.compile(r"(?<!!)\[[^\]]+\]\(([^)]+)\)")
FORBIDDEN_NAMES = {"game.dll", "game_native.exe", "main", "sdk", "subsdk0", "rtld", "prod.keys"}
FORBIDDEN_EXTENSIONS = {
    ".dat", ".elf", ".nca", ".npdm", ".nso", ".nsp", ".xci", ".keys", ".cpk", ".pchtxt", ".ips"
}
DOC_PAIRS = {
    "README.md": "README.md",
    "instalacion.md": "installation.md",
    "compilacion.md": "building.md",
    "configuracion.md": "configuration.md",
    "hdr.md": "hdr.md",
    "mods.md": "mods.md",
    "mandos.md": "controllers.md",
    "actualizaciones.md": "updates.md",
    "problemas.md": "troubleshooting.md",
    "preguntas-frecuentes.md": "faq.md",
    "contribuir.md": "contributing.md",
}


def markdown_files(root: Path):
    for path in root.rglob("*.md"):
        if not any(part in {".git", "build", "cache", "deps", "mods", "targets"} for part in path.parts):
            yield path


def check_local_links(root: Path) -> list[str]:
    errors = []
    for source in markdown_files(root):
        text = source.read_text(encoding="utf-8", errors="replace")
        for match in LINK.finditer(text):
            raw = match.group(1).strip().split(maxsplit=1)[0].strip("<>")
            if not raw or raw.startswith(("#", "http://", "https://", "mailto:")):
                continue
            target = raw.split("#", 1)[0].replace("%20", " ")
            if target and not (source.parent / target).resolve().exists():
                errors.append(f"{source.relative_to(root).as_posix()}: {raw}")
    return sorted(set(errors))


def check_bilingual_pairs(root: Path) -> list[str]:
    es, en = root / "docs" / "es", root / "docs" / "en"
    errors = []
    if not es.exists() or not en.exists():
        return ["docs/es and docs/en must both exist"]
    mapped_es = set(DOC_PAIRS)
    mapped_en = set(DOC_PAIRS.values())
    for name, counterpart in DOC_PAIRS.items():
        if (es / name).exists() and not (en / counterpart).exists():
            errors.append(f"docs/es/{name}: missing English counterpart")
        if (en / counterpart).exists() and not (es / name).exists():
            errors.append(f"docs/en/{counterpart}: missing Spanish counterpart")
    for path in es.glob("*.md"):
        if path.name not in mapped_es and not (en / path.name).exists():
            errors.append(f"docs/es/{path.name}: missing English counterpart")
    for path in en.glob("*.md"):
        if path.name not in mapped_en and not (es / path.name).exists():
            errors.append(f"docs/en/{path.name}: missing Spanish counterpart")
    return sorted(errors)


def forbidden_paths(paths: list[str]) -> list[str]:
    bad = []
    for raw in paths:
        path = Path(raw.replace("\\", "/"))
        lower = path.name.lower()
        if lower in FORBIDDEN_NAMES or path.suffix.lower() in FORBIDDEN_EXTENSIONS:
            bad.append(raw)
    return bad


def repository_paths(root: Path) -> list[str]:
    result = subprocess.run(
        ["git", "ls-files", "--cached", "--others", "--exclude-standard"],
        cwd=root,
        check=True,
        capture_output=True,
        text=True,
    )
    return [line for line in result.stdout.splitlines() if line]


def main() -> int:
    root = Path(__file__).resolve().parents[1]
    errors = []
    errors.extend(check_local_links(root))
    errors.extend(check_bilingual_pairs(root))
    errors.extend(f"forbidden public path: {path}" for path in forbidden_paths(repository_paths(root)))
    if errors:
        print("Public repository checks failed:")
        print("\n".join(f"- {error}" for error in errors))
        return 1
    print("Public repository checks passed: local links, bilingual coverage, and forbidden paths.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
