# ATILLA Mini NEO 1.5.7 — Weather cadence and idle display protection

## Released changes

- Removes the WX UPDATED / WX SYNCING line from Home. Uses that space for today's actual forecast high/low and live wind, with humidity and temperature unchanged. Weather screen retains 3-day Open-Meteo data; the HOME SYNC pill remains a manual refresh.
- Weather background worker fetch interval now **10 minutes** (previous 45); 30-second retries remain until first successful fetch; no automatic screen wake from weather results.
- **60 seconds** after last touch, enters low-backlight (**8%**) image-retention saver with sparse `ATILLA` text moving position over solid black every 15 seconds.
- **120 seconds** after last touch, switches the backlight fully OFF and updates one black frame. No repeated repaint while sleeping. Network, HA API, weather, telemetry and SD logic remain active.
- First finger contact during saver/sleep wakes to user's existing brightness. Its subsequent release is *consumed* and does not activate a hidden button.
- Retains established ESP32 rev3.1 optimizations, 25ms touch polling, nonblocking Open-Meteo HTTPS worker, read-only SD FAT32 probe/checksum, 20 MHz LCD SPI, 50% display buffer and 8-page centered-pill UI.

## Files

- Full credential-free source committed to `experimental/atilla-mini/atilla-mini-1.5.7-idle-display.yaml` on `dev/cyd-mini-bringup`.
- Configured private and credential-free YAML downloads supplied separately in the chat. **No real connection credentials committed.** The separate Waveshare 7-inch firmware was not changed.

## Validation and next log check

YAML syntax parsed and expected logic verified statically. **Not ESPHome compiled or flashed; idle timing, backlight behavior, and memory load require testing on device.** Version 1.5.5 has user-confirmed async forecast update; 1.5.6's 50% buffer and touch timing have not yet been hardware-confirmed. Expect `mini_idle` messages after 60 and 120 seconds. Wake by one tap, then tap again to activate any button. Retain a confirmed working firmware as rollback.
