# SD graphics — verified cache-to-PSRAM read pilot

**Status: draft firmware-side development only. This mode is OFF by default, not installed and not yet hardware tested.** It builds on the HTTPS-to-SD work in PR #9, which is not hardware accepted or merged. The only physical cache write/persistence verified so far is **4.7.9aa slot 1**.

## Implemented gate

A separate `cache_read_pilot: false` ESPHome test mode is available, mutually exclusive with the write and HTTPS pilots and all other FatFs test modes. When explicitly enabled with `fatfs_mount_test: true`:

1. Uses the existing 24,576-byte SD worker to initialize the SDHC card and mount FAT32.
2. Opens *only* the fixed committed path `/ATILLA/CACHE/C0001.PNG` in read-only mode, with expected 70 bytes and CRC32 `F60D2E27` from the previously hardware-tested cache slot.
3. Requires file length to match, allocates exactly that many bytes in PSRAM (never a large stack array), and reads in at most 256-byte chunks with a cooperative elapsed-time cap.
4. Rejects any short/error reads, missing files, wrong length, CRC mismatch, malformed PNG envelope, zero/oversized dimensions or unavailable PSRAM.
5. Closes the file and returns an **RAII-owned compressed PNG buffer** and descriptive dimensions. All operations are read-only. The generic API caps compressed files at **256 KiB** and dimensions at **800×480**.
6. Unmounts FAT32 and **atomically publishes ownership** of the PSRAM-backed compressed PNG to the ESPHome main loop. The main-loop consumer revalidates the PNG/CRC, logs the result, and then releases the owned buffer. No FatFs handle or raw worker stack pointer escapes. It does **not** yet render the image in LVGL or retain it permanently.

The expected logs in this opt-in future build are:

```text
CACHE PSRAM READ PASS slot=1 bytes=70 crc32=F60D2E27 dimensions=1x1 NO WRITES
CACHE PSRAM LOAD OVERALL PASS NO WRITES
FATFS unmounted
CACHE PSRAM POST-UNMOUNT PASS published-to-main-loop NO WRITES
CACHE MAIN-LOOP HANDOFF PASS bytes=70 dimensions=1x1 NO WRITES (not displayed)
```

## Why the display is not yet enabled

A PNG file is **compressed**, not directly usable as raw LVGL RGB565/ARGB8888 pixels. The existing full-panel YAML uses its own worker-driven map and radar decoders, and touching those while FatFs has an open file risks SPI contention and main-loop delay. To render cached assets we must explicitly define:

- A decoder path (bounded PNG decoding to PSRAM-backed pixel buffer) and a separate owned LVGL image descriptor.
- A **one-way worker-to-main-loop handoff**, avoiding all LVGL calls from the SD or networking workers.
- Lifetime rules: each LVGL widget holds its buffer until a successful source replacement, invalidation, or widget destruction. The storage reader's buffer may be freed only after decode and not while a decoder could still be using it.
- Screen geometry and per-widget bounds; a 256×256 ARGB8888 tile is **262,144 decoded bytes**, independent of its compressed PNG length. A full 800×480 ARGB8888 background is **1,536,000 bytes** and must not be casually multiplied per layer.
- Touch latency and memory regression budgets, keeping ~100 kB free internal heap and substantial PSRAM after six radar tiles; the existing ~300–490 ms interval warnings are not acceptable as a goal.

The read-only API's PNG header/IEND checks are **not a full decoder** and CRC32 is an integrity check, not cryptographic authorization. The future host graphics catalog should carry signed or independently trusted SHA-256 hashes and actual dimensions.

## Current release gates

- **4.7.9aa**: one embedded 70-byte cache file written and verified, then independently reread across a reboot — hardware PASS.
- **PR #9 / 4.7.9ab**: one pinned HTTPS 70-byte PNG to slot 2 — **hardware pending**, not merged.
- **This branch**: read-only verified PSRAM loader for slot 1 — **CI pending, hardware pending**. Do not flash a CI-only smoke fixture: its generic GPIO CS wiring is not the real Waveshare CH422G configuration.

No production SD write service, auto-fetch loop, card cleanup or new LVGL rendering is activated by this draft. Keep a full card backup and private 4.7.9aa / 4.7.9z rollback files.
