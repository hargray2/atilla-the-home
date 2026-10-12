"""Host-side manifest contract regression tests (no SD hardware required)."""
import json
import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "tools"))
from validate_asset_manifest import validate, reject_duplicate_keys, main


def manifest(path="icons/radar.png", kind="image/png", size=512, crc="630105D1"):
    return {"schema_version": 1, "assets": [
        {"path": path, "kind": kind, "bytes": size, "crc32": crc}
    ]}


class TestAssetManifest(unittest.TestCase):
    def test_valid(self):
        self.assertEqual(validate({"schema_version": 1, "assets": []}), 0)
        self.assertEqual(validate(manifest()), 1)
        self.assertEqual(validate(manifest("icons/photo.jpeg", "image/jpeg")), 1)

    def test_reject_invalid_paths(self):
        for path in ("../secret.png", "icons/../secret.png", "/root.png",
                     "a//b.png", "a/./b.png", "a\\b.png", ".hidden.png",
                     "a:b.png", "a" * 97 + ".png"):
            with self.subTest(path=path), self.assertRaises(ValueError):
                validate(manifest(path))

    def test_reject_extension_mismatch(self):
        for path, kind in (("icons/a.jpg", "image/png"),
                           ("icons/a.png", "image/jpeg"),
                           ("icons/a.txt", "image/png")):
            with self.subTest(path=path), self.assertRaises(ValueError):
                validate(manifest(path, kind))

    def test_case_insensitive_duplicate(self):
        data = manifest()
        data["assets"].append({**data["assets"][0], "path": "ICONS/RADAR.PNG"})
        with self.assertRaises(ValueError):
            validate(data)

    def test_bounds(self):
        for bad in (0, -1, 1048577, True, "512"):
            with self.subTest(size=bad), self.assertRaises(ValueError):
                validate(manifest(size=bad))
        data = {"schema_version": 1, "assets": [manifest()["assets"][0]] * 33}
        with self.assertRaises(ValueError):
            validate(data)

    def test_repeated_json_key_rejected(self):
        with self.assertRaises(ValueError):
            json.loads('{"schema_version":1,"assets":[],"assets":[]}',
                       object_pairs_hook=reject_duplicate_keys)

    def test_file_size_cap(self):
        with tempfile.TemporaryDirectory() as folder:
            p = Path(folder) / "manifest.json"
            p.write_text(" " * 16385)
            self.assertEqual(main(["validator", str(p)]), 1)


if __name__ == "__main__":
    unittest.main()
