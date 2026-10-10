# ATILLA 4.6 external component — delayed read-only probe

**Experimental.** Following the user's report of a successful compile and OTA installation of the previous compile-only build, the component now performs one read-only SD probe approximately **30 seconds after boot**. The new code has **not yet been validated or installed on device**.

```yaml
external_components:
  - source: github://hargray2/atilla-the-home@main
    refresh: 0s
    components: [atilla_storage]

atilla_storage:
  id: atilla_storage_46
  card_device: atilla_sd_probe_device
  idle_device: atilla_sd_idle_clocks
```

The two `spi_device` IDs must be present in the private YAML. No changes are required to those existing IDs.

At startup the component schedules a single probe. After 30 seconds, it initializes the SDHC card using the CH422G-controlled SPI device, checks the MBR FAT32 partition and boot sector, rereads the FAT32 boot sector, and logs partition geometry. It does **not** mount FATFS, format, create files, run CMD24, or enable persistent caching. It does not retry after a failure.

Expected log if successful: `READ ONLY OK partition_start=... sectors=... bytes_per_sector=512 sectors_per_cluster=64`. Failure is reported explicitly.

**IMPORTANT:** Editing GitHub source doesn't change firmware already installed on ATILLA. A new ESPHome compile and OTA installation is required to test. This is an unverified candidate. Keep ATILLA 4.5 or the current known-working build as rollback. Do not upload passwords or the full private YAML.

## Next candidate: FAT32 directory and known-file verification

Commit `4d26d6a` adds bounded read-only traversal using the FAT32 BPB: read the first sector of the root cluster; look for short-name directory `SYSTEM~1`; read its first cluster; locate `WPSETT~1.DAT`; read the first data sector and compute a 12-byte CRC32. Expected CRC32 is `630105D1` from the previously working legacy YAML diagnostic.

This is a **specific bounded diagnostic**, not FATFS, not a general file-open-by-name API, and not yet compiled/tested on the board. It assumes these first-sector directory entries are still present. If a PC reformats or reorganizes the card, a failure may be expected. It has no write commands. It can extend the synchronous loop time beyond the previous 144 ms; watch the ESPHome watchdog and performance logs.

Expected when verified: `FILE CONTENT READ PASSED: WPSETT~1.DAT bytes=12 CRC32=630105D1`.
