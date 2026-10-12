# ATILLA Mini NEO 1.3.0 — Performance Lab

Experimental standalone CYD sandbox built from 1.2.0 Pixel Forge. Private and credential-free full source files distributed as separate downloads; **this documentation commit does not include the firmware source**.

## New
- 11 touchscreen pages, including **OSC** (rolling ESP32 free-heap samples every two seconds) and **FX** (procedural animated hyperspace effect, tap to cycle four speeds).
- Active display interval reduced from 1s to 500ms. This may increase CPU/SPI overhead; not a measured performance improvement.
- Existing Birthday Blitz, Pixel Forge 16x7 paint canvas, sandbox room toggles, simulated weather, animated radar concept, dimmer, ambient ADC, and HA time retained.
- Bottom tab targeting resized to eleven pages.
- Home Assistant device-control actions remain disabled; no SD card is used.
- Existing ESP32 Arduino framework and known-working pins retained. ESP-IDF chip-revision/SRAM1 settings intentionally not forced onto Arduino; evaluate separately after verified IDF compilation.

## Testing status
Python YAML parsing passed; ESPHome compilation and device testing are **not verified**. The heap graph plots bytes in an approximate fixed range and may saturate outside that range; adjust scales after observing real memory. Keep last working firmware as rollback.

## Security
Public repository notes do not contain Wi-Fi credentials or the API key. Never commit the private YAML. Waveshare 7-inch firmware unchanged.
