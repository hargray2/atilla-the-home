# Waveshare 7-inch ATILLA

**Hardware:** Waveshare ESP32-S3-Touch-LCD-7 Rev 1.2, 800×480 RGB display, GT911 touch, microSD. See [hardware wiring](../HARDWARE.md).

**Source of truth:** complete `waveshare.yaml` is private and **not in this public repository**. Public modules include `components/atilla_storage` and `components/atilla_radar_filter`. Never publish private YAML, credentials or location data.

**Documented checkpoint:** ATILLA 4.7.9u (2026-10-11), with hardware-observed weather/forecast HTTPS, CARTO maps, RainViewer radar and read-only FatFs operations. See [4.7.9u acceptance](../VALIDATED-BASELINE-4.7.9u.md), [architecture](../CURRENT-ARCHITECTURE.md), and [release workflow](../RELEASE-WORKFLOW.md).

**Boundaries:** separate firmware and acceptance from the Mini. Keep LVGL updates on the ESPHome main loop; asynchronous workers handle heavy networking/storage. SD writes or formatting are not approved. Preserve hardware-validated rollback prior to new firmware changes. A compile does not prove radar/map imagery appeared on the display.
