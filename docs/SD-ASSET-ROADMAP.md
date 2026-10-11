# microSD assets — planned incremental delivery

The 4.7.9t SD worker proves safe read-only FAT32 mount and short-file operations. It does **not** yet provide general file-based LVGL assets.

## Stage 1: bounded discovery (read only)

Enumerate `/ATILLA` with a bounded entry count, path sanitization and supported extensions. Report filenames, lengths and failures to logs or an internal status model. Never follow arbitrary paths; no automatic writes.

## Stage 2: bounded file reader

Read a known, user-provisioned test asset in chunks through the dedicated worker. Validate size limits, EOF, CRC where known, card removal, timeouts and open/close/unmount. Avoid large stack buffers and avoid accessing the SPI card concurrently from the main loop.

## Stage 3: LVGL asset handoff

Decode selected supported images from validated bytes into PSRAM buffers; ensure LVGL image descriptor lifetimes outlive displayed assets. Publish the new asset from main loop only when a complete replacement is ready, and retain prior content on failure.

## Stage 4: asset manifest and offline fallback

Add a versioned `/ATILLA/manifest.json` (or a simpler validated format if memory requires). Include file sizes, type, checksum and freshness policy. Prefer deterministic card-provisioning outside the microcontroller before enabling any writes.

## Stage 5: optional write-capable cache

Only after separate durability and power-loss tests: temporary file, fsync and atomic rename where supported, card-removal detection, quota and eviction policy, wear/endurance budgeting and recovery. Never auto-format.

## Acceptance criteria

Each stage must prove no UI main-loop stalls from SD I/O, no worker stack regression, bounded memory, correct card removal behavior, and preservation of radar and weather functionality.
