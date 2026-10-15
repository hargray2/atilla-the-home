"""Offline tests for the repository Markdown link checker."""
from __future__ import annotations

import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "tools"))
from check_repository_docs import check_links


class MarkdownLinkAuditTests(unittest.TestCase):
    def test_valid_local_and_external_links(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            (root / "docs").mkdir()
            (root / "images").mkdir()
            (root / "docs" / "page.md").write_text("# Example\n")
            (root / "images" / "photo.svg").write_text("<svg/>")
            (root / "README.md").write_text(
                "[Page](docs/page.md) ![Picture](images/photo.svg) "
                "[Folder](docs/) [Web](https://example.com) [Section](#example)\n"
            )
            self.assertEqual(check_links(root), [])

    def test_missing_link(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            (root / "README.md").write_text("[Missing](docs/absent.md)\n")
            self.assertEqual(len(check_links(root)), 1)
            self.assertIn("missing:", check_links(root)[0])

    def test_parent_traversal(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp) / "repository"
            root.mkdir()
            (root / "README.md").write_text("[Escape](../outside.md)\n")
            self.assertEqual(len(check_links(root)), 1)
            self.assertIn("leaves repository:", check_links(root)[0])

    def test_percent_encoded_link(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            (root / "a b.md").write_text("# Encoded\n")
            (root / "README.md").write_text("[Local](a%20b.md)\n")
            self.assertEqual(check_links(root), [])


if __name__ == "__main__":
    unittest.main()
