# Experimental FATFS read-only adapter

Development branch: `dev/fatfs-readonly`. **Not included or activated in the working ATILLA firmware.**

`atilla_fatfs_readonly.h` is a first-pass read-only `ff_diskio_register()` implementation, intended to connect the confirmed physical-sector SPI reader to ESP-IDF FatFs `f_mount`, `f_open`, and `f_read`.

Safety: no format call, all block writes return `RES_WRPRT`, no production YAML modifications, and no component activation on `main`.

**Known unverified details:** exact ESP-IDF `diskio_impl_t` typedef/callback signatures and available FatFs symbols under ESPHome 2026.9.1; linking/build setup; volume selection behavior; mounting an MBR-partitioned 64GB card; concurrency with existing YAML probe; stack usage and main-loop blocking. The known 12-byte file path and CRC are test targets, not a successful mount report.

Before enabling the adapter, validate the source using the user's exact ESPHome and ESP-IDF versions, resolve errors, avoid simultaneous raw SPI users, and ensure a bounded/nonblocking operational path. The production component continues to use its proven read-only directory/CRC verification.
