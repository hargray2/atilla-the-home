# ATILLA THE HOME

ESPHome/LVGL dashboard for **Waveshare ESP32-S3-Touch-LCD-7 Rev 1.2** (800×480).

**Repository scope:** public reusable ESPHome component code, sanitized technical documentation, and hardware-test evidence. The complete working `waveshare.yaml` remains private because it includes device-specific configuration and may contain secrets. A one-file YAML install can fetch custom components from this repository at build time.

## Current status — 2026-10-10

**Hardware-validated in the private firmware:**
- FAT32 SD read-only initialization, known-file CRC, FatFs mount/unmount, and a controlled write/read/delete diagnostic.
- Settings persistence: an existing `ATILLA/SETT01.TST` test record was re-read and verified across reboots **without further writes**.
- Six CARTO basemap and six RainViewer overlay tiles loaded and rendered.
- A separate FreeRTOS HTTPS worker completed a request (4.7.5c), downloaded a complete PNG into PSRAM (4.7.6), and handed the PNG buffer to the ESPHome main loop via a queue (4.7.7).
- Temporary worker buffers were released; memory returned to approximately 4,763 KiB free PSRAM and 99 KiB free internal heap after the test.

**Experimental / not yet validated:**
- Main-loop PNG decoding with a separate PNGLE decoder: 4.7.8b–c compiled and ran, but `pngle_new()` failed allocation. **4.7.8d PSRAM allocation is an unverified candidate.**
- Rendering worker-fetched pixels in an actual radar tile, replacing the synchronous six-tile network path, SD-backed asset caching, and long-term filesystem reliability.

**Known limitation:** Existing `online_image.set_url` radar dispatches block the ESPHome main loop for roughly **3–3.5 s per tile**. Worker experiments reduce blocking for their independent diagnostic requests, **not** the original six-tile display pipeline. PNG decoding can separately stall hundreds of milliseconds.

## Project documentation

- [Hardware](docs/HARDWARE.md)
- [Current architecture](docs/ARCHITECTURE.md)
- [Development roadmap](docs/ROADMAP.md)
- [Release and hardware-test history (v4+)](docs/RELEASE-NOTES.md)
- [SD verification](docs/SD-VALIDATION.md)
- [Build and troubleshooting](docs/BUILD-AND-TROUBLESHOOTING.md)
- [Security and contributions](docs/SECURITY-AND-CONTRIBUTING.md)
- [Historical storage design (4.6)](docs/STORAGE-4.6.md)
- [Published read-only component](components/atilla_storage/README.md)

## Branch safety

- `main` is the **conservative read-only** component, not the advanced storage implementation in the private firmware.
- `dev/persistent-settings` contains the experimental storage/persistence work used by the hardware tests; it is **not automatically promoted to main**.
- Other development branches remain available for traceability and rollback. See branch names in GitHub.

## External component

```yaml
external_components:
  - source: github://hargray2/atilla-the-home@main
    refresh: 0s
    components: [atilla_storage]
```

Choose a tested branch/ref explicitly if using features not on `main`. Changing GitHub files does **not** update installed firmware until the next ESPHome build/OTA.

## Versioning

Use lettered candidates (`4.7.8a`, `4.7.8b`, …) for tests, diagnostics and fixes. Advance the numeric revision only after successful **on-device functional validation**. A compiler pass or YAML parser pass does not qualify.

## Security

Do not commit full-device YAML, local credentials, API encryption keys, map keys, private location information, or unsanitized logs. Do not auto-format the SD card. Preserve the last hardware-tested rollback build.

No software license has been granted in this repository.
