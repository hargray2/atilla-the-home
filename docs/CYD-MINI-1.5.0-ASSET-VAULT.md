# ATILLA Mini 1.5.0 — Asset Vault (experimental)

This build retains the existing 8-page CYD Mini sandbox UI, pill navigation, HA clock, weather API, arcade, scope and controls. On the **System** page it displays SD status and detected FAT32 volume size. Tapping the System card retries the SD probe.

## SPI wiring assumptions (not hardware-verified)

- SD SCK GPIO18, MOSI GPIO23, MISO GPIO19, CS GPIO5.
- LCD and XPT2046 continue using their existing separate hardware SPI buses.
- Card uses `spi: interface: software` (third slow bus), `spi_device` mode0 200kHz, CS manually controlled via a GPIO output to permit idle CS-high clock cycles.

## Read-only probe

After boot and a 7-second delay, test CMD0 idle, CMD8 voltage/check pattern, CMD55 + ACMD41 initialization, CMD58 OCR and high-capacity addressing, and read sector 0 with CMD17. If MBR partition 0 type is FAT32 (0x0B/0x0C), read its VBR and check sector size, sectors/cluster, FAT32 geometry and 0x55AA signature. Display `SD: FAT32 READ OK` and the calculated volume size only when these checks pass. Failures report `SD: NOT READY` with descriptive serial logs. No writes, formatting, filesystem mounting, directory traversal, or file access yet; partition type/geometry checking does not constitute a FATFS mount.

**Unverified:** CYD board wiring, compile against installed ESPHome, physical card response and LCD/touch responsiveness during the synchronous probe. This may briefly block the main loop; if latency is substantial the next revision should use a background task. The firmware must not be treated as fully hardware-tested. Keep 1.4.6 as rollback.

Full credential-free and locally configured 1.5.0 YAML artifacts are provided in the chat. GitHub documentation only; private credentials are never committed. The 7-inch Waveshare ATILLA code was not changed.
