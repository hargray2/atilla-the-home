# Experimental FATFS read-only adapter

Development branch: `dev/fatfs-readonly`. **Not included or activated in the working ATILLA firmware.**

`atilla_fatfs_readonly.h` is a first-pass read-only `ff_diskio_register()` implementation, intended to connect the confirmed physical-sector SPI reader to ESP-IDF FatFs `f_mount`, `f_open`, and `f_read`.

Safety: no format call, all block writes return `RES_WRPRT`, no production YAML modifications, and no component activation on `main`.

**Known unverified details:** exact ESP-IDF `diskio_impl_t` typedef/callback signatures and available FatFs symbols under ESPHome 2026.9.1; linking/build setup; volume selection behavior; mounting an MBR-partitioned 64GB card; concurrency with existing YAML probe; stack usage and main-loop blocking. The known 12-byte file path and CRC are test targets, not a successful mount report.

Before enabling the adapter, validate the source using the user's exact ESPHome and ESP-IDF versions, resolve errors, avoid simultaneous raw SPI users, and ensure a bounded/nonblocking operational path. The production component continues to use its proven read-only directory/CRC verification.

## 2026-10-09 adapter hardening

- Added a guarded physical-LBA read path (the existing application read interface was limited to the partition and would not allow an MBR read at LBA 0).
- Corrected status reporting to include `STA_PROTECT`; read callbacks now test specifically for `STA_NOINIT` rather than treating write protection as an I/O failure.
- Ensured the static disk-I/O callback dispatch table has sufficient lifetime; improved cleanup and error handling.
- No FatFs mount has been compiled or tested. The implementation is still an isolated design prototype and may require adjustments to callback types, ESP-IDF component linkage, volume configuration and stack/memory budgets before activation.

**Next gate:** compile an opt-in version against the same ESP-IDF shipped with the user's ESPHome installation, without activating disk access; only then attempt a read-only filesystem mount on device. Continue to avoid every write and formatting function.

## API review (2026-10-09)

Reviewed ESP-IDF disk-I/O declarations: `ff_diskio_impl_t` expects `init/status/read/write/ioctl` callbacks and `ff_diskio_get_drive(BYTE*)` selects an unused logical drive. `ff_diskio_register` copies the callback table. The prototype's callback types match the current documented interface in principle, but compile behavior in the **actual** ESPHome-linked ESP-IDF build remains unverified.

Corrected a malformed escaped-newline block in `unmount()`. The prototype reports `STA_PROTECT` to FatFs and rejects all write callbacks with `RES_WRPRT` (read callbacks must therefore test the `STA_NOINIT` bit rather than treating any nonzero status as failure).

**Open engineering risks:** This code is not included in firmware. Before testing a mount, confirm CMake/IDF fatfs component dependency, real `FF_VOLUMES` mapping and FAT32 partition autodetection, lifetime after mount failure, concurrency with existing raw SD code, bounded 512-byte buffers, and watchdog-friendly scheduling. Never call `f_mkfs`.

## Compile-only integration checkpoint

A copy of the adapter now lives in `components/atilla_storage/fatfs_readonly.h` on this **development branch**, and the branch's `atilla_storage.h` includes it. This forces the compiler to parse the adapter without constructing it or calling its mount functions. The normal `main` branch and installed firmware have **not changed**.

The callback typedefs were cross-checked against Espressif's `components/fatfs/diskio/diskio_impl.h` in the ESP-IDF v5.5.1 source, including `ff_diskio_register` and `ff_diskio_get_drive`. This is an upstream reference only; the exact ESP-IDF version bundled in the ATILLA compile output still needs verification.

**Next check:** build this branch as an opt-in experiment. It may report a missing FatFs header, unavailable FatFs dependency, a compiler-level API mismatch, or no error. Do not report the FatFs mount as functional unless the adapter has also been activated and tested on hardware. No SD writes are implemented.

## GitHub Actions compile result — confirmed failure

Workflow run [38019233276](https://github.com/hargray2/atilla-the-home/actions/runs/38019233276) completed. ESPHome 2026.9.1 validation **passed**, but native ESP-IDF compilation **failed** when compiling `main.cpp`:

```text
fatal error: ff.h: No such file or directory
Compilation failed because fatfs_readonly.h (in "src" component) includes ff.h, provided by fatfs component(s).
However, fatfs component(s) is not in the requirements list of "src".
```

This is not a missing file in the ESP-IDF SDK: the generated ESPHome `src` CMake component does not currently declare the native ESP-IDF `fatfs` requirement. A correct integration needs to add that dependency through supported ESPHome build generation rather than simply copying headers or modifying private YAML. **Do not merge to `main` or ask for an OTA install until this build gate passes.**
