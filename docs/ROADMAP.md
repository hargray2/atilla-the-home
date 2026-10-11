# ATILLA development roadmap — from hardware-validated 4.7.9t

## Verified so far (device logs 2026-10-11)

- Background TLS weather current conditions and forecast both pass and update dashboard sensors.
- Six CARTO basemap tiles and six RainViewer tiles download and display.
- SDHC/FAT32 initialization, FatFs read-only mount/open/read/directory traversal/unmount pass.
- Dedicated SD worker has a 24 KB stack, with >17 KB stack remaining in the recorded run.
- Idle heap and PSRAM return to stable observed levels after initial tile loading.

See [acceptance record](VALIDATED-BASELINE-4.7.9t.md). Do not equate read-only FatFs diagnostics with production-ready asset streaming or durable writes.

## Next milestones in order

1. **Repository consolidation:** both external components, production-matched isolated compilation, unit tests, and secure release/source manifest.
2. **Private release discipline:** preserve full working firmware, pin external component revisions, record reproducible Compile/OTA/hardware acceptance results.
3. **microSD read-only asset discovery:** bounded `/ATILLA` scan, path checks, file sizes and explicit failure codes from storage worker.
4. **Chunked SD file reading:** bounded memory, checksum verification, and unplug/corruption behavior.
5. **PSRAM/LVGL handoff:** fully validated image decode and main-loop publish; preserve prior assets on failure.
6. **Manifest and offline assets:** version, type, size, checksums, cache freshness and fallbacks.
7. **Optional write-capable cache:** only after explicit durability and power-loss tests, no automatic formatting.
8. **Long-term reliability:** soak, OTA rollback, Wi-Fi outage, SD removal, stale cache and corrupted file handling.

## Radar operational goals

- Poll metadata on a 15-minute cadence without forcing downloads of unchanged frames.
- Preserve prior displayed radar if a new frame or individual tile fails.
- Ensure six-tile coherent updates, reasonable request pacing and retry/backoff.
- Track last successful frame separately from last poll, and permit manual refresh.
- Save a full known-good frame to SD only after separately validated write and recovery semantics.

Some metadata scheduling already runs on workers; the entire list is **not** claimed implemented in the 4.7.9t acceptance window.

## Settings — Location and Time

A future touchscreen Settings page should permit safe runtime selection of IANA time zone and home coordinates without recompiling. It requires auditing runtime tile URL calculations, weather requests, DST handling, persisted preferences, UI validation and private-location handling. No implementation is claimed yet.
