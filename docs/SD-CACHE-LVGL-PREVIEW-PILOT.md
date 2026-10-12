# ATILLA SD-to-LVGL preview pilot — private full-panel integration required

**Status: DRAFT, OFF by default, private 4.7.9ac full-panel hardware preview PASSED on 2026-10-12; pending final review/merge. DO NOT FLASH the generic GPIO smoke fixture.** This branch is based on PR #10 (`dev/sd-cache-verified-reader-handoff`), itself dependent on draft PR #9.

## What is implemented

- A separate `cache_screen_pilot: false` mode (mutually exclusive with `cache_read_pilot`, `https_cache_pilot`, and every other storage probe).
- The SD worker performs a **read-only** exact-length and CRC32 check of already existing `/ATILLA/CACHE/C0002.PNG`: 70 bytes, CRC32 `F60D2E27`. No new downloads or SD writes.
- The worker closes the file and requires a **successful FatFs unmount** before transferring an RAII-owned PSRAM buffer to the ESPHome main loop. An unmount error prevents handoff.
- The main loop revalidates the PNG and checks the **exact pinned 1×1 dimensions/CRC/size**, and only then calls an optionally registered `CachePreviewCallback`. If no callback is registered, the test logs FAIL and frees the compressed buffer.
- The callback is invoked synchronously **on the main loop**, never on the SD worker. Its PNG pointer is **borrowed** and becomes invalid immediately when the callback returns; callbacks MUST independently allocate and retain decoded LVGL pixel bytes for the widget lifetime. Returning `true` is only appropriate after successful LVGL source assignment.
- No LVGL widget, PNGLE decoder, private `waveshare.yaml`, or credentials are compiled into the public component. A private full-panel integration supplies the callback and LVGL image widget. The public non-flashable ESPHome smoke fixture validates component compilation, not rendered pixels.

## Expected first hardware logs (private pilot only)

```text
CACHE PSRAM READ PASS slot=2 bytes=70 crc32=F60D2E27 dimensions=1x1 NO WRITES
CACHE PSRAM LOAD OVERALL PASS slot=2 NO WRITES
FATFS f_unmount result=0 ...
CACHE PSRAM POST-UNMOUNT PASS published-to-main-loop NO WRITES
CACHE SCREEN MAIN-LOOP PASS slot=2 bytes=70 NO WRITES (preview callback=SET)
```

On the preview page, a single RGBA(28,111,164,255) pixel may be displayed as a deliberately enlarged blue RGB565 square. **This is a proof of the complete previous HTTPS download → immutable SD file → verify/reopen → decode → LVGL pathway**, not a production graphics cache, a large image, or a RAM replacement.

## Hardware acceptance — private full-panel 4.7.9ac, 2026-10-12

User reported seeing the expected blue square. A private ESPHome 2026.9.1 log (not committed, because it contains private environment identifiers) includes:

```text
CACHE PSRAM READ PASS slot=2 bytes=70 crc32=F60D2E27 dimensions=1x1 NO WRITES
CACHE PSRAM LOAD OVERALL PASS slot=2 NO WRITES
FATFS f_unmount result=0 elapsed=0 ms
FATFS unmounted
CACHE PSRAM POST-UNMOUNT PASS published-to-main-loop NO WRITES
CACHE SCREEN MAIN-LOOP PASS slot=2 bytes=70 NO WRITES (preview callback=SET)
SD LVGL PREVIEW PASS slot=2 decoded=1x1 expanded=128x128 RGB565 psram_bytes=32768
```

The attached private log also showed two weather worker PASS responses, six CARTO HTTPS/map-display PASS pairs and six radar PNG decode/display PASS results. Free internal heap returned to approximately 100 kB; free PSRAM stabilized around 4.86 MB following all six radar tiles. Intermittent ESPHome interval warnings remain (including 332 ms around one radar handoff), so latency optimization is outstanding.

**Acceptance scope:** The pinned 70-byte image was downloaded to slot 2 and persisted in prior 4.7.9ab testing; 4.7.9ac demonstrates **read-only SD-to-screen** display of that existing asset, not a fresh graphics download during this boot. The software gate requires clean unmount before publication. Worker/main-loop log lines can appear interleaved; do not interpret their print ordering as a guarantee of task synchronization.

No arbitrary image decoding, production cache scheduling or general PSRAM offload was validated by this tiny fixture. PR #9, #10 and #11 remain dependent drafts until code/release review.

## Safety constraints and next stages

1. Review fresh head CI and the complete **private** panel integration before any OTA. Preserve backup of entire SD and private 4.7.9aa/4.7.9z rollback files; never publish private YAML.
2. Ensure LVGL image descriptor/data are retained past callback return; never make LVGL hold a pointer to the temporary compressed buffer or a stack pixel array.
3. Keep existing weather, all six CARTO, and all six radar paths unchanged. Monitor ESPHome main-loop latency and Wi-Fi processing; a scaled 1×1 diagnostic must not become a blocking general PNG decoder.
4. After the exact 1×1 preview succeeds, add **a separate** pinned, fixed, recognizable modest image and a bounded off-main-loop decoder (stronger format checks, explicit pixel/PSRAM budget), followed by a stable LVGL owner. No automatic downloads, overwrites, deletions, mutable generations, or production cache scheduling.

The prior 4.7.9ab logs showed both HTTPS slot-2 creation and independent reboot slot-2 NO WRITES PASS on 2026-10-12. PR #9 remains draft/unmerged pending review.
