# ATILLA Mini NEO 1.5.4 — Bounded reference-file CRC32

1.5.3 is the latest user-confirmed working hardware firmware: FAT32 geometry PASS, first root sector had two entries and first name `SYSTEM~1`, Open-Meteo 3-day data updated. This development version adds a **read-only known-file checksum test** without touching proven GPIO, SPI, LCD refresh, touch, weather or boot logic.

## Read path

- Use FAT32 root-sector entry `SYSTEM~1` to resolve a subdirectory's first cluster and first sector.
- Read one subdirectory sector, search for the known FAT short entry `WPSETT~1.DAT` (11-byte directory short name `WPSETT~1DAT`).
- Accept only its expected length of **12 bytes**; read the first 512-byte data sector and compute CRC32 (polynomial 0xEDB88320, reflected, init/final 0xFFFFFFFF).
- Compare against the previously verified Waveshare card reading: **630105D1**. A mismatch or absent file is informational and does not invalidate FAT32 initialization.
- Reject out-of-range cluster references, never traverse unknown FAT chains; no writes, formatting, file creation, or filesystem mounting. Maximum 5 sector reads (MBR/VBR/root/subdir/file).
- Display a small `12-BYTE FILE CRC32 MATCH` / `REFERENCE FILE CRC CHANGED` / `REFERENCE FILE UNAVAILABLE` result at bottom of System page, without replacing any existing navigation buttons.

## Expected firmware log

```text
[I][mini_sd]: SD probe: FAT32 READ-ONLY PASS ...
[I][mini_sd]: ROOT FIRST SECTOR READ OK: 2 entries, first=SYSTEM~1
[I][mini_sd]: KNOWN FILE CONTENT: SYSTEM~1/WPSETT~1.DAT bytes=12 CRC32=630105D1 reference=630105D1 MATCH
```

These are **expected**, not observed, for 1.5.4. The last known **hardware-tested** release is 1.5.3.

Credential-free full source:
`experimental/atilla-mini/atilla-mini-1.5.4-known-file-crc.yaml`.
Private configured YAML remains chat-only. A separate local public YAML artifact was generated from the same 1.5.3 base and contains the same feature changes.

## Limits

- The new read is not a general FAT filesystem or asset loader. The first root-sector count is not a complete directory listing.
- User's 1.5.3 weather request still takes ~2.36 s in latest log and blocks the ESPHome main loop. 1.5.4 does not change HTTP behavior; move it off the main loop in a separate validated iteration.
- Last logged display operation was ~276 ms with 20MHz SPI / 1s update, still above the warning threshold.
- YAML syntax parsed; complete ESPHome compilation and CYD test pending.
