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

## Read-only FATFS optional hardware gate

The earlier automated compilation failed because the ESPHome 2026.9.1 native ESP-IDF build had excluded the `fatfs` component. Calling `esp32.include_builtin_idf_component("fatfs")` and `esp32.require_fatfs()` in the external component fixed this dependency. [GitHub Actions run 38019567042](https://github.com/hargray2/atilla-the-home/actions/runs/38019567042) **passed**.

The development branch now exposes `fatfs_mount_test: true` under `atilla_storage:` (default: false). When enabled, the existing delayed SD sector/CRC diagnostics run first, then a temporary read-only FATFS mount attempts to open the 12-byte known file; the volume is unmounted before returning. All disk write callbacks return `RES_WRPRT`, status reports `STA_PROTECT`, and there is no `f_mkfs` call. This specific mount pathway is **NOT hardware-verified**. The smoke test YAML deliberately uses fake native GPIO CS pins for compile testing only and must never be installed on the actual Waveshare board. Do not merge to main or install on ATILLA until the updated opt-in build has compiled successfully and the user explicitly chooses a test release.

## 2026-10-09 — opt-in FATFS compilation PASSED

Automated [GitHub Actions run 38019770335](https://github.com/hargray2/atilla-the-home/actions/runs/38019770335) **succeeded**. This run compiled the opt-in `fatfs_mount_test: true` code path with ESPHome 2026.9.1 on ESP32-S3/ESP-IDF (using fake GPIO CS pins strictly in the compile fixture). The actual Waveshare's CH422G CS is used only by the user's private full firmware.

The next step is a deliberate on-device **read-only mount** trial from a full private YAML pointing to `dev/fatfs-readonly`, `refresh: 0s`, and `fatfs_mount_test: true`. The test must never be mistaken for a file-write capability. Runtime logs should distinguish `FATFS READ-ONLY MOUNT PASS`, `FATFS FILE READ`, `FATFS MOUNT FAILED`, or any boot/OTA regressions.

## Hardware incident: mount PASS followed by API disconnect (2026-10-09)

The user ran the opt-in FATFS 4.6.1 test on the actual Waveshare and reported:

```text
[23:30:16.196][I][atilla_storage]: SDHC/FAT32 read-only init: PASS
[23:30:16.244][I][atilla_storage]: READ ONLY OK partition_start=32768 sectors=125009920 bytes_per_sector=512 sectors_per_cluster=64
[23:30:16.377][I][atilla_storage]: FILE CONTENT READ PASSED: WPSETT~1.DAT bytes=12 CRC32=630105D1
[23:30:16.382][I][atilla_storage]: Opt-in FATFS read-only mount starting (NO WRITES)
[23:30:16.514][I][atilla_storage]: FATFS READ-ONLY MOUNT PASS
INFO Processing unexpected disconnect from ESPHome API
[23:30:46.566][E][esp32.crash]: Reason: Fault - Cache error (cause 7)
[23:30:46.566][E][esp32.crash]: Crashed core: 0
[23:30:46.566][E][esp32.crash]: Captured by a different firmware build
```

**What is verified:** real FATFS mount returned success on hardware. **Not verified:** FATFS file-open/read, file CRC via FATFS or safe subsequent unmount. The later crash record is from a different firmware build, so the precise crash reason cannot be conclusively attributed from that record.

**Risk/mitigation:** No further 4.6.1 installation or repeated reboot is recommended. Source changed so FATFS file-open/read is disabled unless `fatfs_file_read_test: true` is explicitly provided (default false); mounting remains disabled by default unless `fatfs_mount_test: true` is requested. The current main branch is unchanged. New source must first pass CI then undergo a separately approved mount-only hardware test if safe. Consider stack use, file system object sizing, stale filesystem pointers, asynchronous I/O/concurrency and complete crash traces before enabling writes. No SD writes or formatting permitted.

## Next diagnostic (no new hardware install yet)

After the last on-device mount pass and possible reboot, instrumented the optional `read_known_file()` path with a log before `f_open`, result after `f_open`, a log before `f_read`, results after `f_read`, and a result after `f_close`. Reduced `f_read` request from 64 bytes to exactly the expected 12 bytes. The `fatfs_file_read_test` flag remains **false by default**. This merely enables finer failure isolation in a future deliberate hardware test; GitHub CI cannot exercise real SD hardware. Wait for compilation and runtime stability confirmation before another OTA install.

## 2026-10-10 — stronger guardrails before on-device FatFs file-open trial

The user-confirmed hardware milestone is **FATFS READ-ONLY MOUNT PASS**, followed by **FATFS unmounted**, while `fatfs_file_read_test` is deliberately false. The existing raw-sector file validation independently reports 12 bytes and CRC32 `630105D1`. This verifies mount/unmount and raw-sector reads, **not yet** FatFs file API reads.

On `dev/fatfs-readonly` we now explicitly require `fatfs_mount_test: true` if `fatfs_file_read_test: true` is requested, preventing a silently skipped experimental test; otherwise both default false. The `read_known_file()` function checks `f_size() == 12` before reading, requests exactly 12 bytes, logs the individual `f_open`, `f_read`, and `f_close` results, and reports success only if the CRC equals the known-good `630105D1`. No write functions are added: the driver remains `STA_PROTECT` / `RES_WRPRT`, and no formatting is performed. GitHub's ESPHome CI compiles the read-enabled code path against fake test GPIOs; **this is not hardware proof** and fake-CS fixture must never be flashed to ATILLA.

**Next device gate (only after CI green and deliberate full private YAML preparation):** keep 14 MHz display, six radar tiles and all user functions unchanged; use GitHub external component source `github://hargray2/atilla-the-home@dev/fatfs-readonly` with `refresh: 0s`, `fatfs_mount_test: true`, `fatfs_file_read_test: true`. Observe log phases and device uptime. If unexpected disconnect, crash or freeze occurs, return to mount-only setting without enabling writes; obtain a matching-build ESP-IDF backtrace before debugging further. Do not commit credentials, private YAML or location into the public repository.

## Next controlled hardware gate: FatFs file API read (2026-10-10)

**Pending CI result for commit `4ca22ca`** (mount/open/read/close/unmount timing instrumentation). Do not proceed with a device update until the compile job succeeds. This commit leaves both FatFs flags disabled by default. Current field firmware remains at proven read-only mount/unmount, without FatFs file API access.

Preconditions for a future complete **private** `waveshare.yaml` test: ESPHome validates/compiles with the real Waveshare CH422G SPI CS implementation; 14 MHz RGB clock remains unchanged; current radar scheduling and the deferred 40 px Wi-Fi indicator adjustment are retained when bundling a substantial release; SD retains read-only transport and `disk_write_` must return `RES_WRPRT`.

The experimental branch source must be `github://hargray2/atilla-the-home@dev/fatfs-readonly`, `refresh: 0s`, with `fatfs_mount_test: true` and `fatfs_file_read_test: true` to run the single one-shot test after its built-in 30-second delay. No `f_mkfs`, write, rename, delete, directory creation or file creation is permitted. Stop after a single on-device attempt until the logs and uptime have been reviewed.

Success markers: SD init PASS; raw CRC32 `630105D1`; FATFS MOUNT PASS; `FATFS f_open result=0`; `FATFS f_read result=0 bytes=12`; `FATFS f_close result=0`; `FATFS file bytes verified against known-good raw-sector CRC`; `FATFS unmounted` and continued uptime without unexpected API reconnect. The new timing messages quantify stalls, not necessarily eliminate them. Failure or crash: revert to `fatfs_file_read_test: false`, capture logs and matching-build ELF/backtrace. This trial is not evidence that SD write access is safe.

**Do not commit the user's full private YAML or its map key, Wi-Fi credentials, location or API secret to public GitHub.** Do not pass off a CI smoke fixture (fake GPIO CS pins) as hardware-ready firmware. When making a full YAML artifact, use the actual latest user-provided private YAML and validate the exact file; do not reconstruct a multi-thousand-line configuration from docs.

## 2026-10-10 — hardware FatFs file-read trial reproduced disconnect / reboot

ATILLA 4.6.7 on real Waveshare logged SDHC/FAT32 init PASS, raw WPSETT~1.DAT 12-byte CRC `630105D1` PASS, `FATFS f_mount result=0 elapsed=127 ms`, and `FATFS READ-ONLY MOUNT PASS`. At `[01:21:11.075]` it logged `EXPERIMENTAL FATFS f_open/f_read requested`, then ESPHome API unexpectedly disconnected. There is **NO** `FATFS opening existing file (read only)` log, **NO** `f_open` result, and **NO** unmount in that boot's supplied excerpt. After reconnection a crash record reports `Reason: Abort`, core 0, but explicitly `Captured by a different firmware build`; the PC and BT addresses cannot be confidently attributed to the running build. Post-reboot memory returns to ~7200 kB PSRAM; this looks consistent with restart, not merely network instability, though the user has not yet shared a matching-build backtrace. The earlier hardware opt-in file-read also disconnected soon after mount, so this is a reproducible *test-path* stability problem, not proof that FatFs cannot open files.

**Immediate safety decision:** DO NOT run another device build with `fatfs_file_read_test: true` on this code. Roll back to full 4.6.6 or change the flag to `false` in the exact private full YAML (not in public GitHub); retain `fatfs_mount_test: true` only if stable. Do not enable SD writes. Debug entry into `run_fatfs_test_` and `read_known_file()` first, paying particular attention to stack-frame allocation, nested FatFs structures, storage lifetime, logging, and previous-build crash attribution. Prefer staged diagnostics before invoking any FatFs API; isolate crash with a no-op file-read entry gate, a stack watermark and matching-build symbols. Do not claim it reached `f_open`; the last verified step is the log announcing the attempt.

**User clarification:** The screen visibly rebooted at the API disconnect during the 4.6.7 FATFS file-read attempt. Treat this as a **confirmed device restart** rather than merely an API network interruption. No log showed entry into `read_known_file()` or execution of `f_open`. The exact abort cause still needs current-build symbols/backtrace or staged crash isolation. Keep `fatfs_file_read_test: false` for subsequent stable installs.

## Crash-isolation compile stage: entry probe (2026-10-10)

A new **opt-in safe no-op** configuration `fatfs_entry_probe: true` has been added on `dev/fatfs-readonly`, while `fatfs_file_read_test` remains **false**. This enters the existing read-only FatFs mount/unmount sequence but deliberately does **not** call `read_known_file()`, allocate `FIL`, or invoke `f_open`/`f_read`. It logs `FATFS ENTRY PROBE PASS` before unmount. The schema requires `fatfs_mount_test: true` and prohibits enabling entry probe and file read together. CI's fake-CS compile fixture now tests the probe configuration. This **cannot diagnose the failing actual file-open path by itself**; a successful no-op result would reconfirm the already-proven mount/unmount stability. Keep file reading off, don't enable writes, and do not ask user to flash until a controlled investigation justifies a test.

## 2026-10-10 — file-read crash mitigation (compile stage)

The device's previous full `fatfs_file_read_test: true` attempt rebooted before the first line within `read_known_file`. A plausible (unproven) explanation is task-stack pressure from constructing the FatFs `FIL` object along with the existing probe stack frames. The experimental implementation now logs **function entry before doing anything else**, allocates `FIL` with `new (std::nothrow)` on the heap rather than as a stack local, logs `sizeof(FIL)` after allocation, and uses an RAII pointer for cleanup. If allocation fails, the file is not opened. The raw file read, expected 12-byte size and CRC remain unchanged. **This is an unverified mitigation, not proof of the crash cause or a hardware fix.** The opt-in flag `fatfs_file_read_test` is still **false by default**. No writes are enabled. The existing mount-only device build should remain unchanged. A future controlled hardware test can distinguish a crash before function entry from a crash in `f_open`, but it must not be shipped as an unattended firmware update.
