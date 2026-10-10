# ATILLA release notes — v4 onward

Only on-device observations are treated as verified; candidate labels and parse/compile passes do not imply hardware success.

| Revision | Status / observed result |
|---|---|
| 4.0.3 | Experimental directory scan followed by OTA rollback; root cause not established. |
| 4.0.4–4.2.1 | Bounded FAT32 read-only checks, directory traversal and known-file CRC refined. |
| 4.3–4.5 | Radar pacing, memory diagnostics and 12-tile map/radar baseline. |
| 4.6.x | External SD component and FAT32 read-only checks hardware-tested. |
| 4.7.0–4.7.1a | FatFs object allocation, file open/read, CRC and mount/unmount tested. |
| 4.7.2 (storage stage) | Controlled test file create/write/sync/read/delete, plus later cross-boot settings record validation on experimental branch. |
| 4.7.3 | Existing radar pipeline functional; multi-second dispatch blocking persisted. |
| 4.7.4 | Timing confirmed six radar URL dispatches individually blocked ~3.1–3.5 s. |
| 4.7.5c | Independent FreeRTOS HTTPS worker returned HTTP 200 in ~3.9 s; main-loop multi-second wait avoided for that probe. |
| 4.7.6 | Full PNG downloaded to temporary bounded PSRAM buffer and validated at file-boundary level. |
| 4.7.7 | Queue transferred worker PNG ownership to main loop; 256×256 PNG handoff/cleanup validated. |
| 4.7.8a | Decoder experiment: compilation failed due to lambda-local PNGLE declarations. |
| 4.7.8b | Compiled/ran; PNGLE decoder allocation unavailable. |
| 4.7.8c | Allocation isolated: `pngle_new()` failed, frame allocation intentionally skipped; no decode success. |
| 4.7.8d | Candidate PSRAM PNGLE decoder allocation; hardware **not yet verified**. |

**Version policy (effective from 4.7.8):** alphabetic suffixes mark experiments, fixes and diagnostics. Do not advance numeric revision until the relevant function has been verified on hardware and accepted.

The private device YAML and secrets are not stored in this public repository. None of the worker experiments yet replaced the original radar display data path; runtime blocking in `online_image` persists.
