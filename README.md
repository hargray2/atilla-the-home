# ⚔️ ATILLA THE HOME

**ESPHome / LVGL smart-home touchscreen project for Waveshare ESP32-S3-Touch-LCD-7**

ATILLA is an evolving 800×480 touchscreen home dashboard with a dark weather radar map, real-time tile downloads, touch controls, and experimental removable SD storage. The project is developed on an iPhone through Home Assistant's ESPHome Builder: the goal is **one complete `waveshare.yaml`**, while additional custom driver code is automatically downloaded from this repository.

> **Project status (10 October 2026):** The private ATILLA 4.7.2 firmware passed hardware tests for FAT32 mounting, file creation, a 28-byte write, read-back verification, deletion and clean unmount. These features remain **experimental**, not a production-ready storage service. The tested firmware is private and is **not** contained in this repository. Keep your working firmware as a rollback option.

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
- **Demonstrated in experimental hardware testing:** FatFs mount, 12-byte file read with CRC32, and 28-byte create/write/read-back/delete round trip.
- **Not yet delivered:** an operational, long-term storage service, endurance/large-file testing, radar caching across reboot.

## Installing the experimental external component

The production YAML is deliberately **not published** because it may contain private keys and Wi-Fi credentials. A properly configured one-file ESPHome installation can fetch this component automatically:

```yaml
external_components:
  - source: github://hargray2/atilla-the-home@main
    refresh: 0s
    components: [atilla_storage]
```

The component also needs `atilla_storage:` configuration referencing two existing `spi_device` IDs; see the [component README](components/atilla_storage/README.md). **The `main` component remains read-only.** Writable filesystem diagnostics exist only on experimental branches and are not an always-on storage service.

## Principles

**Preserve the working dashboard. Never auto-format the SD card. Keep secrets off GitHub. Publish test outcomes accurately.**

This is an actively developed personal project, not a finished product.

## Development branches and test status

- `main`: conservative read-only SD transport and project documentation; no writable storage functionality.
- `dev/fatfs-readonly`: compiled read-only FatFs adapter and hardware-verified file read/directory scan.
- `dev/fatfs-write-test`: hardware-verified 28-byte file creation, sync, read-back and deletion; experimental and not for unattended use.
- `dev/storage-service`: candidate 4 KiB integrity test and `/ATILLA` directory; **not yet confirmed on hardware**.
- `dev/radar-intensity`: radar filtering and ambient brightness development, including the hardware-tested precipitation filter.

**Licensing:** This repository currently has no published software license. Viewing source is not a grant of redistribution or commercial use rights. A noncommercial community license is being considered; do not assume it is in force until published.
