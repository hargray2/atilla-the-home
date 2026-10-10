# ⚔️ ATILLA THE HOME

**ESPHome / LVGL smart-home touchscreen project for Waveshare ESP32-S3-Touch-LCD-7**

ATILLA is an evolving 800×480 touchscreen home dashboard with a dark weather radar map, real-time tile downloads, touch controls, and experimental removable SD storage. The project is developed on an iPhone through Home Assistant's ESPHome Builder: the goal is **one complete `waveshare.yaml`**, while additional custom driver code is automatically downloaded from this repository.

> **Project status (October 2026):** ATILLA **4.5 is the known-good hardware-tested baseline**. ATILLA **4.6 is experimental**. Its GitHub storage component has not yet been confirmed to compile successfully after the latest namespace fix; it does not currently mount FAT32 or write files. Keep the working firmware as a rollback option.

## Documentation

| Start here | Description |
|---|---|
| [Hardware and pinout](docs/HARDWARE.md) | Exact Waveshare revision, SPI microSD wiring, CH422G chip select, RAM constraints |
| [Firmware architecture](docs/ARCHITECTURE.md) | LVGL, CARTO maps, RainViewer overlays, tile preloading and caching limits |
| [Read-only SD test evidence](docs/SD-VALIDATION.md) | Real SD commands, FAT32 layout, directory traversal, file CRC |
| [Build and troubleshooting](docs/BUILD-AND-TROUBLESHOOTING.md) | iPhone workflow, GitHub refresh cache, compiler errors and fixes |
| [Development roadmap](docs/ROADMAP.md) | Compile gate → sector reads → FATFS mount → guarded writes → asset caching |
| [Release notes (v4+)](docs/RELEASE-NOTES.md) | Verified history and experimental development milestones |
| [Security and contributions](docs/SECURITY-AND-CONTRIBUTING.md) | Public-repository safety, secret handling and change discipline |
| [4.6 storage design notes](docs/STORAGE-4.6.md) | Component-specific development plan and constraints |
| [Storage component source](components/atilla_storage/) | ESPHome Python codegen and experimental C++ transport |

## Quick technical summary

- **Board:** Waveshare ESP32-S3-Touch-LCD-7 Rev 1.2, 800×480 touchscreen.
- **Display:** LVGL, GT911 touch, hardware clock/display/backlight handled in private firmware.
- **Weather:** six CARTO dark map tiles plus six RainViewer overlay tiles, rendered from RAM via ESPHome `online_image`.
- **Known stable transfer pacing:** approximately 1.5 seconds between map requests, 4 seconds between radar requests.
- **microSD:** Samsung EVO 64GB FAT32, CLK GPIO12, MOSI GPIO11, MISO GPIO13, CS via **CH422G EXIO4**.
- **SD hardware test:** read-only CMD17 and FAT32 structural parsing succeeded, including CRC32 `630105D1` for one 12-byte file.
- **Not yet delivered:** mounted FATFS, safe persistent writes, radar caching across reboot.

## Installing the experimental external component

The production YAML is deliberately **not published** because it may contain private keys and Wi-Fi credentials. A properly configured one-file ESPHome installation can fetch this component automatically:

```yaml
external_components:
  - source: github://hargray2/atilla-the-home@main
    refresh: 0s
    components: [atilla_storage]
```

The component also needs `atilla_storage:` configuration referencing two existing `spi_device` IDs; see the [component README](components/atilla_storage/README.md). **Do not install the experimental component expecting SD file persistence.** Its current `setup()` performs no SD operations.

## Principles

**Preserve the working dashboard. Never auto-format the SD card. Keep secrets off GitHub. Publish test outcomes accurately.**

This is an actively developed personal project, not a finished product.
