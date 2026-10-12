# ATILLA SD-to-LVGL preview pilot — private full-panel integration required

**Status: DRAFT, OFF by default, CI-only until reviewed. DO NOT FLASH the GPIO smoke fixture.** This branch is based on PR #10 (`dev/sd-cache-verified-reader-handoff`), itself dependent on draft PR #9.

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

## Safety constraints and next stages

1. Review fresh head CI and the complete **private** panel integration before any OTA. Preserve backup of entire SD and private 4.7.9aa/4.7.9z rollback files; never publish private YAML.
2. Ensure LVGL image descriptor/data are retained past callback return; never make LVGL hold a pointer to the temporary compressed buffer or a stack pixel array.
3. Keep existing weather, all six CARTO, and all six radar paths unchanged. Monitor ESPHome main-loop latency and Wi-Fi processing; a scaled 1×1 diagnostic must not become a blocking general PNG decoder.
4. After the exact 1×1 preview succeeds, add **a separate** pinned, fixed, recognizable modest image and a bounded off-main-loop decoder (stronger format checks, explicit pixel/PSRAM budget), followed by a stable LVGL owner. No automatic downloads, overwrites, deletions, mutable generations, or production cache scheduling.

The prior 4.7.9ab logs showed both HTTPS slot-2 creation and independent reboot slot-2 NO WRITES PASS on 2026-10-12. PR #9 remains draft/unmerged pending review.
