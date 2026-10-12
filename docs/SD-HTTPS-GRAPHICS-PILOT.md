# ATILLA SD graphics — immutable HTTPS download pilot

**Development only. DO NOT FLASH / DO NOT MERGE yet.** This branch builds on PR #8, which itself awaits a second-boot hardware log verifying cached `C0001.PNG` without further SD writes. The newly introduced HTTPS mode is independently OFF by default.

## Implemented code

- A **single public test image** is hosted in this repository at an immutable commit SHA:
  `assets/test/cache-test-1x1.png` (70 bytes, CRC32 `F60D2E27`).
- The test endpoint is fixed in firmware using a full HTTPS URL with a commit hash. Caller-supplied URLs, user credentials and arbitrary hostnames are not supported.
- The ESP-IDF HTTPS client verifies TLS using the certificate bundle, does not follow HTTP redirects, requires status **200** and known unchunked Content-Length **70**, and uses bounded reads.
- A dedicated SD worker downloads into a **PSRAM-owned buffer**, verifies streamed CRC, PNG signature/IHDR dimensions and IEND marker. The independent pin to exactly `F60D2E27` is an accidental-corruption check; a future production authenticity catalog should use SHA-256 and signed metadata where needed.
- Only after the HTTPS client is closed does the existing readback/commit path in PR #8 mount FatFs and write a **new, immutable cache slot 2**: `/ATILLA/CACHE/C0002.PNG`. Existing slot 1 is never overwritten.
- The same storage logic refuses to overwrite a preexisting file or stale `.TMP`, stages with `FA_CREATE_NEW`, syncs, rereads and compares bytes/CRC, commits by rename and verifies again. On a repeated run an existing matching final file is read back only.
- Failure during TLS, download, length, format or CRC validation causes **no SD write attempt**.
- New ESPHome option `https_cache_pilot: false` defaults off and is mutually exclusive with all other storage probes including `cache_write_pilot`.

## Compile-only tests

`tests/test_graphics_download_contract.cpp` checks exact allowed URL, response statuses (including redirect rejection), Content-Length, chunked responses, incremental streaming, truncation, overlength, corruption and PNG structure. `tests/https-graphics-cache-smoke.yaml` has fake GPIO wiring and **MUST NEVER be flashed to the real panel**. CI compiles the network/storage code on ESPHome 2026.9.1.

## Safety and release sequence

1. PR #8 passes **reboot-only** test on 4.7.9aa:
   `CACHE PILOT EXISTING PASS slot=1 NO WRITES`; observe healthy boot. Do not merge until the result is verified.
2. PR #8 is merged after that hardware gate and current-head CI.
3. Review this follow-on PR and its CI, then prepare **one complete private firmware** explicitly enabling `https_cache_pilot: true` (not both modes). Keep 4.7.9aa and 4.7.9z for rollback.
4. **Back up the full SD card.** First online test should show `HTTPS CACHE DOWNLOAD PASS bytes=70 crc32=F60D2E27 tls=VERIFIED redirects=DISABLED`, `CACHE WRITE PILOT PASS slot=2 bytes=70 ...`, `HTTPS CACHE COMBINED PASS`, and clean unmount.
5. Power-cycle without reflash to prove `CACHE PILOT EXISTING PASS slot=2 NO WRITES`.
6. Inspect the card from a PC for `C0002.PNG`, preserve existing files, and then plan rendering of verified graphics in LVGL.

This is a **small end-to-end transport test**, not production downloading. The TLS trust anchor and exact URL must be checked during hardware acceptance. FAT32 still offers no power-loss journaling, and writing remains an explicit one-shot operator test. Normal or automated downloads, refresh schedules, multiple assets, deletions, replacing live cache generations, failure recovery and SD-to-LVGL rendering are **not enabled**.

## Later visual-quality implementation

Create a versioned signed graphic catalog and fixed, allowlisted provider origins; separately verify size, SHA-256, MIME/PNG structure and dimensions before staging. Use thumbnail, theme artwork, weather icon and map/background slots with per-asset memory budgets. Perform HTTPS and filesystem I/O only on workers, hold graphics in PSRAM with explicit LVGL lifetime ownership, and roll back to firmware-embedded fallback art on missing/corrupt data. Cache updates must not block GT911 touch input or mutate the active generation. Treat current ~300–490ms LVGL interval warnings as an optimization target, not a regression baseline to normalize.
