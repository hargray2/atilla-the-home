# ATILLA external storage component — public main branch

**Scope:** conservative read-only SPI/FAT32 diagnostic. This is not a writable storage service.

## Installation

```yaml
external_components:
  - source: github://hargray2/atilla-the-home@main
    refresh: 0s
    components: [atilla_storage]

atilla_storage:
  id: atilla_storage_probe
  card_device: atilla_sd_probe_device
  idle_device: atilla_sd_idle_clocks
```

The `card_device` and `idle_device` SPI IDs must match components in the private configuration. The one-file private firmware remains outside GitHub.

This component runs a one-shot delayed probe around 30 seconds after boot: SDHC init, FAT32 partition/boot-sector validation, known directory and 12-byte file read with CRC32. It issues **no writes**, does not mount FatFs in this `main` implementation, and cannot provide persistent asset storage.

**Hardware status:** read-only partition and file-content CRC checks have been verified. Experimental FATFS and settings-persistence features were tested using a **different development branch**; see [SD validation](../../docs/SD-VALIDATION.md). Do not assume these features are available on `main`.

Updating GitHub source has no effect on the already installed firmware until ESPHome rebuild/OTA. Preserve a known-good rollback and avoid logging full URLs or secrets.
