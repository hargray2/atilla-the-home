# SD-backed dashboard assets: combined release gate

**State:** design and host-side contract only. No new panel firmware or SD writes. The 4.7.9y combined read-only worker is hardware-validated; PR #4 is now merged into `main`.

## One larger milestone, not one flash per test

A future opt-in worker test should exercise, in one FatFs mount and one firmware upload:

1. Verify known 12-byte read, enumerate the bounded `/ATILLA` directory and read a small known 4 KiB file with CRC.
2. Open and validate a **host-provisioned** `/ATILLA/manifest.json` under strict length, JSON complexity and entry-count limits. The present empty manifest sample is only a host-side example, *not* on the card.
3. Resolve only relative assets under `/ATILLA`; reject traversal, absolute paths, symlinks or unsupported extensions. Limit 32 entries and 1,048,576 bytes/file. Crucially, the current reusable basename-only reader **does not support nested paths**, so firmware must add safe segment-by-segment validation before nested `icons/` entries.
4. Read each test asset in bounded chunks and compare declared size and CRC32. Capture a stable pass/fail count; reject missing or corrupted files without attempting to render them.
5. Close all file and directory handles, unmount, report stack high-water and heap/PSRAM after the scan. No writes or formatting.

## Prerequisites before deployment

- Use host tooling to create actual manifest and matching harmless test assets **on the card**; do not imply they already exist.
- Maintain a single storage worker ownership/locking model. Avoid simultaneous FatFs and SPI operations from multiple threads.
- Validate short reads, missing files, unexpected sizes, malformed JSON, invalid manifest entries, extra directory entries and card removal in unit tests. Cooperative elapsed-time checks do **not** interrupt blocked FatFs calls.
- Keep firmware secrets and full `waveshare.yaml` private; add only public-safe component code, synthetic fixtures and documentation.
- Delay LVGL rendering integration until the read-only manifest-to-buffer step is proven and its ownership/lifetime is explicit.

## Acceptance

One compiled complete private firmware; one installation; one startup log showing combined SD PASS, manifest file/CRC PASS, no writes, weather current/forecast PASS, six CARTO tiles and six radar tiles, and no reboot. Longer soak and touchscreen interactions remain independent advisories.

**Rollback:** 4.7.9y is the latest hardware-tested checkpoint. Do not alter the deployed component pin until a subsequent candidate is fully compiled.

## Host-side validation hardening

The draft manifest validator now rejects duplicate JSON keys, manifests larger than 16 KiB, case-insensitive FAT name collisions and extension/MIME mismatches, in addition to prior bounded paths, counts, sizes and CRC syntax. `tests/test_asset_manifest.py` exercises these constraints in CI. **This is not a firmware manifest parser** and does not imply the manifest or image files exist on the SD card. Parser complexity, on-card CRC verification and user-provisioned fixture preparation are still required before the next flash.

## Synthetic host fixture (added after delayed milestone)

Run `python tools/build_sd_test_fixture.py /tmp/atilla-fixture` on a computer to create `/tmp/atilla-fixture/ATILLA/manifest.json` and a deterministic 1x1 PNG at `/tmp/atilla-fixture/ATILLA/icons/atilla-test.png`. The manifest's declared size and CRC32 are calculated from the actual PNG bytes; `tests/test_sd_fixture.py` checks them. **This tool writes only to the supplied computer directory, never to a connected SD card.** It refuses to overwrite existing fixture files. The fixture is for the later manifest reader's acceptance and is not required to be copied until that reader is ready. The firmware-side parser and nested path support remain unimplemented.
