# ATILLA 4.6 storage design — archived engineering stage

This file is retained as a **historical design reference**, not current installation guidance.

The original 4.6 goal was to move from a YAML-only raw FAT32 test to a reusable ESPHome external component using two existing SPI devices and CH422G EXIO4 card selection. That milestone was subsequently compiled and tested.

Later milestones (see [SD validation](SD-VALIDATION.md) and [release notes](RELEASE-NOTES.md)) demonstrated FatFs mount/unmount, controlled file I/O and a persisted record re-read across reboot on experimental branches. The public `main` component **remains read-only**; development implementations are not automatically merged.

Current priorities are radar worker-to-main-loop decoding and safe tile display, followed by sustained SD asset caching only after filesystem fault recovery is tested.

Never commit full-device YAML, API/OTA keys or network credentials; never auto-format a user's card.
