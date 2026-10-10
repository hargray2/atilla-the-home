# SD-card observations and validation evidence

This records successful historical **read-only** testing of the actual FAT32 microSD installed in ATILLA. It does not indicate a filesystem mount or any successful writes.

## Card initialization

| Operation | Observed result |
|---|---|
| CMD0 | 0x01 (idle) |
| CMD8 | 0x01, echo 000001AA |
| ACMD41 | ready YES, final 0x00 |
| CMD58 | 0x00, OCR C0FF8000 |
| Addressing | Block-addressed SDHC/SDXC |

## MBR and FAT32

| Property | Observed |
|---|---|
| MBR partition type | 0x0C (FAT32 LBA) |
| Partition start LBA | 32768 |
| Sector size | 512 B |
| Sectors/cluster | 64 |
| FAT copies | 2 |
| Reserved sectors | 32 |
| Reported volume | 59.61 GiB |
| Root directory cluster | 2 |
| Root directory LBA | 63314 |

The directory entry `SYSTEM~1` was encountered at root. In its subdirectory, `WPSETT~1.DAT` was found as a 12-byte file at cluster 4 / LBA 63442. Its 12-byte contents were read repeatedly and the measured CRC32 was **630105D1**.

## What is verified

- The electrical SPI path, CS expander behavior and CMD17 path have successfully read multiple physical sectors.
- A FAT32 volume layout and directory data were parsed without mounting.

## What is not verified

- ESP-IDF FATFS registration and mount
- File opens through a mounted filesystem
- Block writes (CMD24) or read-back from an ATILLA-created file
- FAT metadata consistency under power loss
- Wi-Fi persistent asset storage

## Safety gates

1. Compile external component against the actual installed ESPHome version.
2. Confirm read-only mount; compare directory listing and file CRC to evidence above.
3. Keep formatting disabled; no `f_mkfs`.
4. Resolve SPI contention, timeout, scheduling and stack/RAM concerns.
5. Only then implement bounded writes and a controlled test-file create/read-back.
