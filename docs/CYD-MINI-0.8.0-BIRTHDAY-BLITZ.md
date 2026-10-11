# ATILLA Mini NEO 0.8.0 — Birthday Blitz

A self-contained touch arcade page added to the seven-page CYD instrument console as page 8 (GAME), with a glowing hit target, score counter, miss counter, animated reticle and a particle celebration every five successful hits.

Local ESPHome firmware: `atilla-mini-neo-0.8.0-birthday-private.yaml` — distributed privately because it contains user-provided credentials. A credential-free equivalent is staged separately as `atilla-mini-neo-0.8.0-public.yaml`.

Preserves:
- The working ESP32-2432S028 display and XPT2046 touch GPIO mapping
- MIPI SPI ST7789V and Home Assistant API/clock
- Home Assistant weather entity, seven prior pages, and brightness controls
- SD card optional, no SD access at this stage

Navigation: bottom screen 40px-wide segments select one of eight pages; select GAME and tap the glowing target. Touch outside target increments miss. All interactions stay local to the ESP32; no internet assets required.

**Validation:** Source has passed YAML parsing only. No ESPHome compile or hardware proof. Restore the last working source if graphics or touch fail.

**Security:** No Wi-Fi password/API token in this commit. Do not commit local private YAML.
