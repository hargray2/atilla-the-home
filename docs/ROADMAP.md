# ATILLA development roadmap

Updated 2026-10-10. Distinguish **on-device validated** behavior from work that merely compiled or was proposed.

## Validated milestones

- SDHC/FAT32 read-only read path and known 12-byte file CRC.
- FatFs mount/unmount, bounded read and controlled write-read-delete probe.
- Persisted test record verified across reboots without rewriting.
- Six map/radar tiles operational in the existing ESPHome `online_image` pipeline.
- One independent worker HTTPS request (4.7.5c).
- Full PNG download into bounded PSRAM buffer (4.7.6).
- PNG ownership queue handoff to main loop, dimensions check and cleanup (4.7.7).

## Current experimental gate: 4.7.8

4.7.8a–c exposed decoder declaration and allocator problems; 4.7.8c identified `pngle_new()` failure. **4.7.8d**, with decoder state in PSRAM, requires compile and on-device evidence. Keep letter suffixes for tests and diagnostics. Advance number only when functionality is hardware validated.

## Next engineering work (dependency order)

1. Validate PNGLE decoder allocation and complete 256×256 decode on main loop with bounded memory and proper cleanup; measure scheduler blocking.
2. Compare decoded pixels/color/alpha handling against original radar overlays; verify rain palette filtering.
3. Render **one worker-fetched tile** through a safe main-thread LVGL image-buffer path. Do not refetch its URL using `online_image`.
4. Migrate remaining radar tiles sequentially with a reusable worker, bounded queue, cancellation and graceful fallback.
5. Address PNG decode stalls with incremental main-loop budgeting or tested off-main-loop decode; retain thread-safe display operations.
6. Stage a coherent six-tile radar frame; refresh ~15 minutes, retain prior complete frame on partial failures, display last successful frame time, and implement retry/backoff.
7. Build SD-backed persistent asset cache after safe, atomic file persistence is validated; test removal, corrupted files and power loss.
8. Implement persistent touchscreen Location & Time settings (IANA time zone, validated lat/long, DST, tile coordinate update), after auditing runtime configuration dependencies.
9. Reliability: OTA rollback, poor Wi-Fi, large transfers, heap fragmentation, repeated refresh, filesystem stress and recovery.

## Guardrails

Never auto-format SD, publish secrets, overwrite verified settings records during a diagnostic, call LVGL from a worker, or describe compile-only changes as hardware-validated. Preserve stable fallback and all user-facing dashboard functions.
