# ATILLA ecosystem — project guide

**ATILLA is three connected efforts:** a full-size ESP32-S3 home dashboard, a compact CYD touchscreen, and a dedicated Debian lab that develops and tests ESP firmware. They share ideas and engineering tools, but not interchangeable firmware or automatic hardware-write authorization.

| Project | Hardware / environment | Focus | Documentation |
|---|---|---|---|
| **ATILLA Waveshare 7-inch** | Waveshare ESP32-S3-Touch-LCD-7 Rev 1.2, 800 × 480, GT911 touch | LVGL home UI, weather, map, radar and guarded storage development | [Project](WAVESHARE-7-INCH.md) |
| **ATILLA Mini** | ESP32-2432S028 CYD touchscreen | Small weather and Home Assistant interface, touch, idle screen protection and microSD investigation | [Project](ATILLA-MINI.md) |
| **ATILLA-LAB** | Debian-based Dell Latitude E6420 | Local firmware builds, registered board jobs, camera diagnostics and gated deployment | [Project](ATILLA-LAB.md) |

## Repository boundaries

`main` publishes Waveshare external ESPHome components, smoke tests, and dated engineering evidence. The independent [Mini firmware branch](https://github.com/hargray2/atilla-the-home/tree/dev/cyd-mini-bringup/experimental/atilla-mini) holds versioned CYD configurations. The installed ATILLA-LAB server is a separate Dell checkout, not public source here. Private complete Waveshare YAML, tokens and hardware backups do not belong in this repository.

See [integration and release rules](INTEGRATION.md), [hardware and pinout](../HARDWARE.md), [release notes](../RELEASE-NOTES.md), [reproducibility checklist](../REPOSITORY-CHECKLIST.md), and [security policy](../SECURITY-AND-CONTRIBUTING.md).

## What the tests mean

An ESPHome config check means the YAML was accepted. A compile means a firmware artifact was built. An uploaded firmware needs separately authorized hardware access. Only logs, captures, and observations from the relevant physical board support claims about touch, screen content, weather/radar rendering, card behavior or stability.

Never assume a green CI component fixture certifies the full private Waveshare panel firmware or any physical board.
