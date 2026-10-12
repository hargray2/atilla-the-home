"""Deterministic synthetic SD fixture builder tests (host-side only)."""
import json
import sys
import tempfile
import unittest
import zlib
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "tools"))
from build_sd_test_fixture import create_fixture
from validate_asset_manifest import validate

class TestFixture(unittest.TestCase):
    def test_manifest_matches_actual_png(self):
        with tempfile.TemporaryDirectory() as temp:
            root = Path(temp)
            result = create_fixture(root)
            self.assertEqual(validate(result), 1)
            declared = json.loads((root / "ATILLA/manifest.json").read_text())
            entry = declared["assets"][0]
            data = (root / "ATILLA" / entry["path"]).read_bytes()
            self.assertTrue(data.startswith(b"\x89PNG\r\n\x1a\n"))
            self.assertEqual(entry["bytes"], len(data))
            self.assertEqual(entry["crc32"], f"{zlib.crc32(data):08X}")
            with self.assertRaises(FileExistsError):
                create_fixture(root)

if __name__ == "__main__":
    unittest.main()
