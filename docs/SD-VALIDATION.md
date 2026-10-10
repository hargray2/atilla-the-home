# SD validation — hardware evidence and scope

Updated 2026-10-10. Historical detailed tests remain in Git history; this document is the consolidated current status.

## Tested hardware and filesystem

- Waveshare ESP32-S3-Touch-LCD-7 Rev 1.2, SD SPI SCK GPIO12, MOSI GPIO11, MISO GPIO13, CH422G EXIO4 chip select.
- FAT32 partition LBA 32768, 512-byte sectors, 64 sectors/cluster; card CSD capacity check passed.
- Known 12-byte `WPSETT~1.DAT` content was read with CRC32 `630105D1`, repeatedly and without modification.
- Opt-in FatFs `f_mount` and unmount returned success on-device.
- An experimental bounded file creation/write/sync/read/delete round trip succeeded.
- Persistent settings probe: existing `ATILLA/SETT01.TST` test record was located and validated across boots **read-only**.

## Example on-device evidence (4.7.8c boot)

```text
SDHC/FAT32 read-only init: PASS
FILE CONTENT READ PASSED: WPSETT~1.DAT bytes=12 CRC32=630105D1
FATFS f_mount result=0
FATFS READ-ONLY MOUNT PASS
SD CSD verified ... status=PASS
SETTINGS PERSISTENCE PASS (existing record across boot; read-only)
ATILLA SETTINGS PERSISTENCE PASS
FATFS f_unmount result=0
```

One-shot SD worker stack minimum-free remained about 9 KiB on the logged boot. A successful test-record read after reboot is a real persistence result, **not yet** evidence of a general-purpose fault-tolerant settings system.

## Boundaries and safety

The public `main` component is still a conservative read-only driver. Advanced FatFs/persistence operations reside on experimental branches; do not infer that updating documentation promotes them to `main`. Full firmware remains private.

Not yet validated: endurance, atomic replacement under sudden power loss, corruption recovery, FAT exhaustion, card-removal handling, many simultaneous files, full radar asset caching, automatic formatting (forbidden). Preserve test records and never overwrite/create them merely to rerun an existing-record verification.
