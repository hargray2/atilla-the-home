# ATILLA Mini NEO 1.5.3 — Root-sector preview + LCD tuning

The user's 1.5.2 logs prove FAT32 geometry and real Open-Meteo forecast retrieval on the CYD. This change derives from the known-good 1.5.2 firmware, preserving ESP-IDF 5.5/ESPHome 2026.9.1, the ST7789V, XPT2046, full UI, weather feeds and SD software SPI GPIO18/23/19/5.

## Proposed 1.5.3 integration

- Set the MIPI SPI display rate explicitly to **20MHz** (was default 10MHz). May shorten display transfers; actual LCD integrity/timing must be checked on the hardware. Roll back to 10MHz if visual artifacts appear.
- Set the full-screen refresh cadence to **1s** (was 500ms). This reduces total time spent rendering, at the cost of slower animations.
- On successful FAT32 BPB validation, compute first FAT32 root-directory sector from reserved sectors, FAT size/count and BPB_RootClus, enforce volume bounds and read **exactly one** additional 512-byte sector with CMD17. Skip deleted/LFN/volume entries; count visible 8.3 entries in that first sector and expose count plus first entry on the System page and serial log. A zero count is not proof that the directory or the card is empty. This is **not** a FATFS mount, full FAT directory enumeration, generic file opening or content verification.
- Weather logs now include HTTP request `duration_ms` to quantify main-loop blocking.

## Status

Two downloadable artifacts created in the associated conversation:
`atilla-mini-neo-1.5.3-asset-vault-preview-private.yaml` (not for public upload), and `atilla-mini-neo-1.5.3-asset-vault-preview-public.yaml` (!secret references). Structural YAML checks passed. The credential-free full 1.5.3 source is now committed to `experimental/atilla-mini/atilla-mini-1.5.3-asset-vault-preview.yaml`. The known-working 1.5.2 public source is separately checkpointed at `experimental/atilla-mini/atilla-mini-1.5.2-hardware-pass.yaml`. Both use `!secret` references. Private configured files remain downloadable from chat only.

**Untested:** ESPHome full compile, SD first-root-sector read, higher SPI display clock, memory/use under HTTPS. ESPHome HTTP requests run on the main loop, so their several-second stall is **not fixed** by the display changes. Keep working 1.5.2 as rollback. No writes, formatting, or files created on Samsung 64GB card. Waveshare 7-inch project untouched.
