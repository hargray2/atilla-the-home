# ATILLA release notes (v4 onward)

> Historic observations transcribed from project development. Experimental labels do not imply successful installation.

| Version | Historical outcome |
|---|---|
| 4.0.3 | Experimental large root directory scan; OTA rollback observed. |
| 4.0.4 | Conservative one-sector delayed SD test succeeded. |
| 4.0.5–4.2 | Read-only FAT32 diagnostics improved; directory and file CRC checks became repeatable. |
| 4.2.1 | SD diagnostic delay reduced to around ten seconds and worked in testing. |
| 4.3 | Paced map/radar downloads to improve TLS reliability. |
| 4.4 | Added heap/PSRAM memory diagnostics. |
| 4.5 | Added post-radar memory guard; retained stable, RAM-preloaded 12-tile map/radar and read-only SD test. |
| 4.6 (in development) | GitHub ESPHome external component, SPI bindings and first source fixes. A successful full build and a filesystem mount are **not yet confirmed**. |

Pre-v4 releases are intentionally excluded. This is not a complete chronological changelog; it records milestones for which development evidence exists.

## Hardware-verified October 2026 milestones

| Revision | Observed on ATILLA hardware |
|---|---|
| 4.6.x | SDHC/FAT32 read-only init, known-file raw-sector CRC32, and FatFs mount/unmount passed. |
| 4.7.0 | Heap-backed 4,136-byte FatFs file object allocated; stack watermark around 894 bytes; no file opened in this stage. |
| 4.7.1a | FatFs file open, 12-byte read, CRC32 `630105D1`, directory scan and clean unmount passed. |
| 4.7.2 | SD card CSD capacity verified; `ATILLA.TST` created without overwrite, 28 bytes written/synced, re-read and verified, then deleted and deletion verified. |

These results are supported by user-supplied real-device logs. Production persistence, filesystem endurance, recovery from power interruption and larger-file operation remain unverified. The full device YAML is intentionally private.
