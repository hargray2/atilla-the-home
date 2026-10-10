# ATILLA development roadmap

## Implemented / hardware-tested

- ATILLA 4.5 is the known-good dashboard baseline with LVGL, live online-image map/radar overlays, background radar preload, memory diagnostics, and a custom read-only SD-sector diagnostic.
- Hardware read-only FAT32 structure and known file CRC were checked on the installed card.

## Source committed but not yet confirmed working on device

- 4.6 external-component scaffold and CH422G-aware experimental transport.
- Codegen binding for existing `spi_device` entities.
- Fixes for Python SPI codegen identifier and ambiguous C++ namespaces.
- Current setup is deliberately **compile-only** and does not access the SD card.

## Next milestones, in dependency order

1. **4.6 compile gate:** obtain successful Validate + Compile with real ATILLA `waveshare.yaml`. No card operations from the new component.
2. **4.6 read-only activation:** run bounded SD initialization/sector checks, reconcile card behavior with independent known-good diagnostic.
3. **Read-only FATFS mount:** implement IDF-compatible disk I/O registration and file operations, with automatic formatting explicitly forbidden.
4. **4.7 guarded writes:** validate command and data responses, write protection, lock ownership, timeouts, read-back and safe metadata behavior.
5. **4.8 persistent assets:** download to vetted files, atomic replacement where possible, invalidate caches, render stored images; keep RAM and internal heap budget.
6. **Reliability:** OTA rollback, stale cache, loss of Wi-Fi, SD removal, power interruption, and corrupted images.

Version labels are development goals, not promises of a completed firmware or shipping dates.
