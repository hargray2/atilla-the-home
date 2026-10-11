# Proposed /ATILLA asset manifest (host-side only)

This is a **contract prototype**, not a firmware feature. No SD card is formatted, written to, or mounted by the validator. The hardware-verified 4.7.9t read-only probe can open one known 12-byte file, not arbitrary image assets.

## Initial card organization

```text
/ATILLA/
  manifest.json
  icons/
    <user-provisioned PNG/JPEG files>
```

`manifest.json` contains **exactly** `schema_version: 1` and an `assets` array. Each entry has `path` (relative to `/ATILLA`), `kind` (`image/png` or `image/jpeg`), `bytes` (1..1,048,576) and an eight-digit hexadecimal `crc32`. Maximum 32 entries. No absolute paths, `..` segments or redundant path separators. Manifest validation is necessary but not sufficient: the eventual firmware reader must still perform its own bounds checks and verify the actual file's length/checksum before handing an image to LVGL.

`assets/manifest.example.json` intentionally declares **zero assets**; it is a valid empty sample and makes no false claim about files installed on the user's card.

Run `python tools/validate_asset_manifest.py assets/manifest.example.json` locally, or rely on the lightweight GitHub Action for static input validation.

**Before device implementation:** define FatFs ownership, bounded chunk API, file size limits, CRC streaming, card removal behavior, queue handoff, memory lifetime, and error handling. Test read-only on hardware before considering any write capability.
