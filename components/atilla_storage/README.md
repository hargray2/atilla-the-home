# ATILLA 4.6 ESPHome storage component — compile-only

**Experimental:** checks linking of two existing SPI devices. No card reads, mounts or writes are invoked by this external component.

```yaml
external_components:
  - source: github://hargray2/atilla-the-home@main
    components: [atilla_storage]

atilla_storage:
  id: atilla_storage_46
  card_device: atilla_sd_probe_device
  idle_device: atilla_sd_idle_clocks
```

The spi_device IDs must already be defined in the YAML. Expected log, once compiled and installed: `ATILLA storage driver linked (compile-only, SD untouched)`.

**Not yet compiled or hardware-tested.** This is not a FATFS mount or functional file writer. The existing ATILLA 4.5 code remains the independent known-good read-only test.

Do not publish the full private dashboard YAML or secrets.