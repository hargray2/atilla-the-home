# ATILLA storage external component

**Development scaffold only.**

ESPHome can retrieve this component through `external_components` from the GitHub repository. Currently this component registers no SD storage operations, does not mount FAT32, and does not enable writing. Do not add it to the known-good ATILLA 4.5 firmware expecting SD writes. Hardware adapter prototype is stored under `experimental/` pending compiler verification.

Planned: bind existing `spi_device` instances with CH422G EXIO4 CS to a read-only FATFS disk I/O adapter. **Automatic formatting is forbidden**. A future release must verify compile results before claiming usable mounting.
