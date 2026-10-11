# Opt-in /ATILLA read-only discovery — experimental milestone

This branch adds `asset_discovery_probe: true` to the storage component. It must **replace**, not accompany, `fatfs_batch_read_probe: true`: the schema allows only one active FatFs diagnostic. Keep `fatfs_mount_test: true`.

## What it does

The existing one-shot 24,576-byte SD worker mounts the same FatFs read-only adapter after the prior FAT32/known-file checks. When opted in, it opens `/ATILLA`, enumerates **up to 32 immediate entries**, logs filenames (capped to 64 printed bytes), directory/file type and byte length, closes the handle and unmounts. It never traverses subdirectories, reads asset content, formats, creates, renames, deletes or writes to files. A missing `/ATILLA` produces a clean diagnostic FAIL without modifying the card. A full 32-entry result is explicitly marked `complete=NO`; it is not evidence there are exactly 32 entries.

Expected logs: `ASSET DISCOVERY entry=...`, `ASSET DISCOVERY PASS entries=... files=... complete=... NO WRITES`, then `ASSET DISCOVERY READ-ONLY PASS` and `FATFS unmounted`.

## Safety and acceptance

This remains an **experimental branch**. Its two compile-only CI fixtures use generic ESP32-S3 GPIO chip selects and must NEVER be flashed to the Waveshare. Before any hardware test, prepare a complete private firmware revision from 4.7.9u with *only* the storage source ref and the mutually exclusive probe flag changed; compile, OTA and verify no Wi-Fi/weather/radar/LVGL regression. Full asset file reading, checksums, manifest consumption, persistent caching, automatic SD formatting and writes are not implemented.

The `/ATILLA` directory itself was visible on the card in 4.7.9u's root directory scan, but enumeration of its children has **not yet been tested on real hardware**.
