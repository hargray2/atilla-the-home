# ATILLA THE HOME — 4.6 Storage Engine

**Status: experimental source; do not flash yet.**

## Confirmed hardware and baseline

- Waveshare ESP32-S3-Touch-LCD-7, ESPHome 2026.9.1, ESP-IDF.
- SD SPI: SCK GPIO12, MOSI GPIO11, MISO GPIO13, chip select **CH422G EXIO4**.
- Working firmware: ATILLA 4.5, with radar background preload and 10-second read-only SD diagnostic.
- Verified microSD access: initialization CMD0/CMD8/ACMD41/CMD58, MBR, FAT32 BPB, root/subdirectory reads, 12-byte file read (`CRC32 630105D1`).

## Engineering requirements

1. ESPHome `external_components` retrieval, so iPhone users only paste one complete `waveshare.yaml`.
2. Read-only FATFS disk I/O adapter and controlled mount. Do not enable writes or formatting.
3. Verify component compilation on ESPHome 2026.9.1 and mount on Waveshare hardware.
4. After successful mount, add transaction limits, mutual exclusion, error handling, bounded busy waits, proper sector count, FATFS read-only settings and UI responsiveness.
5. Only after those checks, add guarded file writes with metadata consistency and read-back verification.

**Security:** Never commit the private ATILLA YAML, Wi-Fi password, API encryption key, or map service tokens. This public repository is for reusable driver code only.

## Current code

`components/atilla_storage/atilla_storage.h` contains the currently committed experimental read-only SPI transport and compile-only ESPHome wrapper. The previous historical `atilla_storage_41.h` concept was never published to this repository. No ESP-IDF FATFS adapter or mount has been implemented here; compilation and on-device tests remain pending.
