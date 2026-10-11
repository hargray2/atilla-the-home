#!/usr/bin/env python3
"""Validate a small ATILLA SD asset inventory without touching any SD card.

This is a host-side contract checker only. It does not prove actual file contents
or implement a firmware JSON parser.
"""
import json
import re
import sys
from pathlib import Path

ALLOWED_KINDS = {"image/png", "image/jpeg"}
MAX_ASSETS = 32
MAX_BYTES_PER_ASSET = 1024 * 1024
MAX_MANIFEST_BYTES = 16 * 1024
RELATIVE_PATH = re.compile(r"^[A-Za-z0-9][A-Za-z0-9_./-]{0,95}$")
CRC32 = re.compile(r"^[0-9A-Fa-f]{8}$")

def validate(data):
    if not isinstance(data, dict) or set(data) != {"schema_version", "assets"}:
        raise ValueError("expected exactly schema_version and assets")
    if type(data["schema_version"]) is not int or data["schema_version"] != 1:
        raise ValueError("unsupported manifest version")
    assets = data["assets"]
    if not isinstance(assets, list) or len(assets) > MAX_ASSETS:
        raise ValueError("assets must be a list of at most 32 entries")
    names = set()
    for i, obj in enumerate(assets):
        if not isinstance(obj, dict) or set(obj) != {"path", "kind", "bytes", "crc32"}:
            raise ValueError(f"asset {i}: invalid fields")
        p = obj["path"]
        if not isinstance(p, str) or not RELATIVE_PATH.fullmatch(p):
            raise ValueError(f"asset {i}: unsafe path")
        parts = p.split("/")
        if any(part in {"", ".", ".."} for part in parts) or len(parts) > 4:
            raise ValueError(f"asset {i}: path traversal or excessive nesting")
        canonical = p.casefold()  # FAT filesystems are case-insensitive
        if canonical in names:
            raise ValueError(f"asset {i}: repeated FAT path")
        names.add(canonical)
        if (obj["kind"] == "image/png" and not p.lower().endswith(".png")) or (obj["kind"] == "image/jpeg" and not p.lower().endswith((".jpg", ".jpeg"))):
            raise ValueError(f"asset {i}: extension and MIME mismatch")
        if not isinstance(obj["kind"], str) or obj["kind"] not in ALLOWED_KINDS:
            raise ValueError(f"asset {i}: unsupported kind")
        size = obj["bytes"]
        if type(size) is not int or not 1 <= size <= MAX_BYTES_PER_ASSET:
            raise ValueError(f"asset {i}: invalid asset byte size")
        if not isinstance(obj["crc32"], str) or not CRC32.fullmatch(obj["crc32"]):
            raise ValueError(f"asset {i}: invalid CRC32")
    return len(assets)

def reject_duplicate_keys(pairs):
    result = {}
    for key, value in pairs:
        if key in result:
            raise ValueError(f"repeated JSON key: {key}")
        result[key] = value
    return result

def main(argv):
    if len(argv) != 2:
        print("Usage: python tools/validate_asset_manifest.py assets/manifest.example.json", file=sys.stderr)
        return 2
    try:
        path = Path(argv[1])
        if path.stat().st_size > MAX_MANIFEST_BYTES:
            raise ValueError("manifest exceeds 16 KiB host-side cap")
        count = validate(json.loads(path.read_text(encoding="utf-8"), object_pairs_hook=reject_duplicate_keys,
                                    parse_constant=lambda value: (_ for _ in ()).throw(ValueError(f"invalid JSON number: {value}"))))
    except (OSError, UnicodeError, ValueError, json.JSONDecodeError) as exc:
        print(f"ATILLA SD manifest FAIL: {exc}", file=sys.stderr)
        return 1
    print(f"ATILLA SD manifest PASS: {count} entries checked (metadata only)")
    return 0

if __name__ == "__main__":
    raise SystemExit(main(sys.argv))
