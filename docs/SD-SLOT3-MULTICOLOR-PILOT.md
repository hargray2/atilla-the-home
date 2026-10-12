# ATILLA — immutable slot-3 multicolor SD graphics experiment

**Status: DRAFT / development only. OFF by default. DO NOT FLASH smoke fixtures or enable on the panel until private full-firmware compile, safety review, backups, and explicit pilot approval.**

Depends on PR #11 (slot-2 display hardware PASS, latest-head CI PASS), PR #10 (SD-to-main-loop verified handoff), and PR #9 (HTTPS to slot-2 hardware PASS). All are still draft/unmerged.

## New fixed graphic

- Repo test fixture: `assets/test/sd-color-preview-96x64.png`, a 96×64 RGBA 8-bit multicolor **SD** badge with transparency; **2,314 compressed bytes**, **CRC32 B171217C**, **24,576 bytes** if decoded to ARGB8888 pixels (not counting decoder memory).
- Network source is a single **commit-pinned** HTTPS URL referencing asset commit `dbb2c26e5103b55c44bd5f0e0003523dd035a7e9`.
- Expected path: `/ATILLA/CACHE/C0003.PNG`. Slots 1 and 2 remain untouched. HTTPS is TLS verified with trusted certificate bundle; redirects, unknown/chunked/incorrect length, short responses, wrong CRC, format and dimensions are rejected before writing.
- `color_https_pilot: false` opt-in: network download on the 24KB worker; mount SD only after verified download; reuse existing create-new, sync, readback, CRC and immutable rename transaction. A pre-existing matching slot produces `CACHE PILOT EXISTING PASS slot=3 NO WRITES` instead of an overwrite.
- `color_screen_pilot: false` opt-in: no network request, FAT32 read-only `C0003.PNG`, 2,314-byte length/CRC/PNG validation, successful unmount **before main-loop handoff**. Main loop repeats pinned dimensions/format/CRC checking and invokes a registered private callback; if absent, it fails and drops the bytes. No LVGL widgets or PNGLE decoder are included in public component.
- Modes are mutually exclusive with each other, all prior SD probes, slot-2 HTTPS and slot-2 screen, and require `fatfs_mount_test: true`. Both are OFF without explicit selection. No background refresh, generalized URL, production cache writes, deletion, or mutation.

## Required private-device acceptance (NOT YET PERFORMED)

**Phase A — explicit slot-3 write pilot**, only after a full SD backup and a production Waveshare (CH422G) private firmware compile:
```text
HTTPS COLOR DOWNLOAD PASS slot=3 bytes=2314 crc32=B171217C tls=VERIFIED redirects=DISABLED
CACHE WRITE PILOT PASS slot=3 bytes=2314 crc32=B171217C writes_disabled=YES
COLOR CACHE COMBINED PASS slot=3 download=PASS card=PASS
FATFS f_unmount result=0
```
If a prior run already committed slot 3, expect `CACHE PILOT EXISTING PASS slot=3 NO WRITES` instead of an initial write. A log that says a file was written is **not** a proof of durability until a subsequent power-cycle/reboot check.

**Phase B — separately compiled read-only on-screen pilot**: enable only `color_screen_pilot`, keep download/write flags OFF; main-loop private callback should decode full RGBA with alpha into an independently owned, bounded LVGL pixel buffer. Expect `CACHE PSRAM READ PASS slot=3`, successful unmount, `COLOR SCREEN MAIN-LOOP PASS slot=3`, and a visible multicolor badge. Reboot to establish read-only persistence. Monitor weather, six CARTO tiles, six radar tiles, heap and input latency; **no hardware test or live image has been claimed yet**.

The private callback currently exists only for the previously hardware-tested 1×1 slot-2 square. A separate private 96×64 callback and complete application compile must be completed before Phase B. The existing radar PNGLE decoder takes around 250 ms for a 256×256 tile on the main loop; this new diagnostic size is far smaller but UI timing and memory still need direct measurement. This pilot **does not yet reduce persistent map/radar PSRAM use**; it validates a modest cached source asset stored on SD. Generic off-main-loop decoding and eviction/reload will be separate gated work.

Never publish private full `waveshare.yaml`, Wi-Fi identifiers, keys, sensor coordinates, or personal URLs. Preserve private 4.7.9ac, 4.7.9ab, 4.7.9aa and 4.7.9z recovery builds and full SD backup.
