# Bounded 4 KiB read-only CRC probe

This experimental probe advances the hardware-validated 4.7.9v directory discovery to **file-content reads**, but does **not** implement a general asset loader.

Set `fatfs_mount_test: true` and **exactly one** mode, `bounded_crc_probe: true`, with `asset_discovery_probe: false` and `fatfs_batch_read_probe: false`. The probe runs on the existing 24,576-byte storage worker.

It opens the existing `/ATILLA/VERIFY.BIN` in `FA_READ` only, requires an **exact 4,096-byte file**, reads **16 × 256-byte chunks per pass**, calculates CRC32, closes the file, then repeats the entire pass. It requires both CRC32 results to match and logs the observed CRC. Each pass has a 10-second elapsed-time guard checked between reads. A blocking FatFs call may itself exceed that duration; this is **not a hard I/O timeout**.

**Important integrity limitation:** CRC agreement across two reads only proves repeatability, **not** authenticity against a trusted expected checksum. The file was observed at 4,096 bytes on 4.7.9v hardware; its expected CRC was not independently established. No file contents are sent to LVGL, no filesystem writes are enabled, and no automatic formatting occurs.

Expected log: `FILE CRC pass=1`, `FILE CRC pass=2`, `FILE CRC READ-ONLY PASS ... repeatable=YES NO WRITES`, `FILE CRC READ-ONLY PROBE PASS`, followed by FatFs unmount.

**Release gate:** CI parse and compile, then full private firmware compile + physical hardware check. Keep 4.7.9v as rollback and don't merge this branch before hardware acceptance.

## Combined acceptance mode (fewer firmware cycles)

Use `combined_readonly_probe: true` with all other FatFs probe modes false and `fatfs_mount_test: true`. The existing worker performs the raw 12-byte known-file check, mounts FatFs **once**, enumerates up to 32 immediate `/ATILLA` entries and performs the two-pass 4 KiB CRC read of `/ATILLA/VERIFY.BIN`. It logs `COMBINED READ-ONLY PASS` only if discovery and CRC both pass, then unmounts. No writes, UI asset loading, or recursive traversal. The automated pipeline validates four probe YAML fixtures but runs one combined C++ compile instead of four separate compiles. This mode is experimental until hardware-tested.
