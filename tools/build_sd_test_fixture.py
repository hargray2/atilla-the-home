#!/usr/bin/env python3
"""Build a synthetic ATILLA read-only SD fixture in a HOST directory.

Never connects to the panel or formats a card. The resulting directory may be
copied manually to an already prepared FAT32 card for a future firmware test.
"""
import argparse
import json
import zlib
from pathlib import Path

from validate_asset_manifest import validate

# Minimal 1x1 RGBA PNG, generated entirely from fixed synthetic pixels.
def png_fixture():
    import struct
    def chunk(tag, body):
        return struct.pack(">I", len(body)) + tag + body + struct.pack(">I", zlib.crc32(tag + body))
    return (b"\x89PNG\r\n\x1a\n" +
            chunk(b"IHDR", struct.pack(">IIBBBBB", 1, 1, 8, 6, 0, 0, 0)) +
            chunk(b"IDAT", zlib.compress(b"\x00\x1c\x6f\xa4\xff")) +
            chunk(b"IEND", b""))

def create_fixture(root: Path):
    base = root / "ATILLA"
    base.mkdir(parents=True, exist_ok=True)
    image_path = base / "icons" / "atilla-test.png"
    image_path.parent.mkdir(parents=True, exist_ok=True)
    if image_path.exists() or (base / "manifest.json").exists():
        raise FileExistsError("refusing to overwrite existing fixture files")
    payload = png_fixture()
    image_path.write_bytes(payload)
    manifest = {"schema_version": 1, "assets": [{
        "path": "icons/atilla-test.png",
        "kind": "image/png",
        "bytes": len(payload),
        "crc32": f"{zlib.crc32(payload):08X}",
    }]}
    validate(manifest)
    (base / "manifest.json").write_text(json.dumps(manifest, indent=2) + "\n", encoding="utf-8")
    return manifest

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("output_directory", type=Path)
    args = parser.parse_args()
    manifest = create_fixture(args.output_directory)
    print(f"ATILLA fixture created: {args.output_directory / 'ATILLA'} ({len(manifest['assets'])} asset)")
if __name__ == "__main__":
    main()
