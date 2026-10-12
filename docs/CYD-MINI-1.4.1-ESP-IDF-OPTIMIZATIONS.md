# ATILLA Mini NEO 1.4.1 — Performance-optimized candidate

On the user's ESP32-2432S028, ESPHome reports a **classic ESP32 revision 3.1** and a bootloader compatible with SRAM1-as-IRAM. Compared with 1.4.0, this experimental update changes `esp32.framework.type` from `arduino` to `esp-idf`, sets `advanced.minimum_chip_revision: "3.1"` and `advanced.sram1_as_iram: true`, and replaces `ESP.getFreeHeap()` with ESP-IDF `esp_get_free_heap_size()`.

The user has separately downloaded complete private and credential-free YAMLs. They have passed YAML parsing but **have not been fully compiled or hardware tested**. Changing frameworks can expose compatibility issues. Preserve the known-good firmware and use USB recovery if needed. `sram1_as_iram` requires an ESP-IDF v5.1+ bootloader; OTA does not update the bootloader. This does not add 40 KB of application heap or framebuffer memory.

Source: https://esphome.io/components/esp32/

No Waveshare files or private credentials changed in the repository.