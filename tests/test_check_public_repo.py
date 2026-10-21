import tempfile
import unittest
from pathlib import Path

from scripts.check_public_repo import check_bilingual_pairs, check_local_links, forbidden_paths


class PublicRepositoryChecksTest(unittest.TestCase):
    def test_local_links_report_missing_target_and_accept_anchor(self):
        with tempfile.TemporaryDirectory() as folder:
            root = Path(folder)
            (root / "README.md").write_text("[ok](guide.md#part) [bad](missing.md)", encoding="utf-8")
            (root / "guide.md").write_text("# Part", encoding="utf-8")
            self.assertEqual(check_local_links(root), ["README.md: missing.md"])

    def test_bilingual_pairs_report_only_missing_counterpart(self):
        with tempfile.TemporaryDirectory() as folder:
            root = Path(folder)
            (root / "docs/es").mkdir(parents=True)
            (root / "docs/en").mkdir(parents=True)
            (root / "docs/es/README.md").write_text("es", encoding="utf-8")
            (root / "docs/en/README.md").write_text("en", encoding="utf-8")
            (root / "docs/es/hdr.md").write_text("es", encoding="utf-8")
            self.assertEqual(check_bilingual_pairs(root), ["docs/es/hdr.md: missing English counterpart"])

    def test_forbidden_paths_detect_game_and_secret_extensions(self):
        paths = ["README.md", "dump/exefs/main.nso", "prod.keys", "build/game.dll"]
        self.assertEqual(forbidden_paths(paths), paths[1:])


if __name__ == "__main__":
    unittest.main()
