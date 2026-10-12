# SD graphics cache — first opt-in WRITE pilot (candidate 4.7.9aa)

**Status: firmware-side code in development. NOT deployed or hardware tested.**
**4.7.9z remains the latest hardware-validated checkpoint.**
This milestone allows one controlled PNG write and readback. **HTTPS
downloads and display rendering are not included yet.**

## Why a pilot first

ATILLA 4.7.9z verified read-only FAT32, a manifest and one tiny PNG.
The existing SD transport has independently gated historical write
diagnostics, but a graphics cache needs different protections. FAT32
has no transaction journal; power removal during file creation,
directory creation or rename can still damage a card even when the
application uses a .TMP file. This code does not promise atomicity.

## Opt-in behavior

- `cache_write_pilot: false` by default; mutually exclusive with every
  other FatFs test mode.
- One 24,576-byte one-shot SD worker after startup. The SPI transport,
  FatFs mount, legacy read and write probes stay on the worker.
- Verify CSD capacity against the MBR partition end before allowing
  sector writes; existing two-layer write gates are restored to
  write-protected mode after the transaction.
- Source is a fixed **70-byte 1x1 PNG**, CRC32 `F60D2E27`, embedded
  in the test component. Network data is not connected yet.
- Only fixed slot `C0001` inside `/ATILLA/CACHE` is ever targeted.
  The worker creates the CACHE directory if absent, refuses to overwrite
  existing files or a stale `.TMP` from an interrupted run, writes
  `C0001.TMP` using `FA_CREATE_NEW`, syncs/closes, reopens and checks
  exact byte count and CRC, then renames to `C0001.PNG` and re-verifies.
- On later boots, a matching committed PNG is **read and verified only**.
  A conflicting committed file or orphaned stage fails closed. No
  deleting/reformatting/mass cleanup and no modifying `manifest.json`.
- File writes occur ONLY in opt-in mode. Failed intermediate stages
  remain on the card for operator review; automatic boot retries
  and endless writes are prohibited.
- No `f_getfree` full FAT scan: some large 64-GB FAT32 cards can spend a
  long time enumerating free clusters if FSInfo is invalid. Short
  writes and capacity errors instead abort and preserve the stage.

## First hardware gate (after CI)

1. Back up the entire card before any firmware write experiment. Ensure
   stable power and **do not remove the microSD during the trial**.
2. Use one complete private firmware build with the PR commit pinned and
   `fatfs_mount_test: true`, `cache_write_pilot: true`. Disable
   `manifest_readonly_probe` while that write pilot is enabled.
3. Expect `CACHE WRITE PILOT PASS slot=1 bytes=70 crc32=F60D2E27
   writes_disabled=YES` and `CACHE WRITE PILOT OVERALL PASS`, then
   unmount and a healthy weather, CARTO, radar and memory startup.
4. Inspect card on a PC after power down: `/ATILLA/CACHE/C0001.PNG`
   should be 70 bytes. Existing `manifest.json`, `VERIFY.BIN`,
   `SETT01.TST` and `icons/` should be unchanged.
5. One repeat boot should verify the existing cache file **without
   additional writes**. The user should retain 4.7.9z firmware for
   rollback, but FAT media itself may still need restoration from backup
   if the write trial is interrupted.

## Subsequent milestone: actual online graphic downloads

A separately tested HTTPS download worker should fetch from explicit
trusted HTTPS origins, enforce content-type and Content-Length plus an
absolute byte cap, prevent redirects to untrusted domains, obtain a
known catalog SHA-256 or CRC32/size, and hand off a fully verified
bounded PSRAM buffer to the exclusive SD worker. Only after the cache
write and on-card reread pass should the asset be made visible to LVGL.
An immutable generation catalog or manifest must preserve the previous
working generation until the new one is complete; never overwrite the
active asset in place. Handle TLS failure, out-of-space, reset,
corrupted payload, partial download, old cached version and card removal.
No network-led, unattended, repeating writes are enabled in this pilot.

## Further quality constraints

- The PNG signature check is shallow and **not a PNG decoder**; decoding
  untrusted graphics requires additional limits and validation.
- CRC32 provides accidental-corruption detection, **not authenticity**.
- Real PSD/vector art/large backgrounds, typography and LVGL rendering
  require a new memory-budgeted pipeline independent of the write pilot.
- The successful 4.7.9z log still includes main-loop interval warnings
  and a 1.4-second API operation; any cache/network worker must not add
  main-loop stalls.
- No private `waveshare.yaml`, Wi-Fi details, credentials or location
  are committed to this repository.
