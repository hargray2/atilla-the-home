# ATILLA Mini 0.1.0 — CYD first-flash test

Branch: `dev/cyd-mini-bringup` only. **No Waveshare code is changed.**

## Candidate configurations
- `atilla-mini-0.1.0-st7789v.yaml`: first-screen candidate for dual-USB CYD.
- Display model and touch calibration are **not hardware-confirmed**.
- This is an ESPHome YAML **source**, not a precompiled .bin. Requires local secrets and ESPHome compilation; compilation has not been verified in this session.

## Factory backup BEFORE first flash

Install esptool on your PC and identify the CYD serial port. Read its flash size first:

```sh
python -m esptool --port PORT flash_id
```

For a verified **4 MB** flash chip only:

```sh
python -m esptool --port PORT --baud 115200 read_flash 0x0 0x400000 atilla-mini-factory-4mb.bin
```

Do **not** assume 4 MB if `flash_id` reports another size. Disconnect other serial tools and save the backup somewhere safe. A backup does not guarantee restoration of external calibration/settings.

## Secrets

In ESPHome `secrets.yaml` supply these names (do not commit private values):

```yaml
wifi_ssid: "YOUR_WIFI"
wifi_password: "YOUR_WIFI_PASSWORD"
atilla_mini_api_key: "YOUR_32_BYTE_BASE64_API_KEY"
atilla_mini_ota_password: "YOUR_UNIQUE_OTA_PASSWORD"
atilla_mini_fallback_password: "YOUR_UNIQUE_AP_PASSWORD_8_OR_MORE_CHARACTERS"
```

Generate API key with e.g. `openssl rand -base64 32`.

## Build/flash
Run `esphome config experimental/atilla-mini/atilla-mini-0.1.0-st7789v.yaml` followed by `esphome run experimental/atilla-mini/atilla-mini-0.1.0-st7789v.yaml` from the repository root. Use USB/serial for the initial flash. Do not attempt to flash this device with the Waveshare YAML.

## Record results
1. Does backlight turn on? Is the title legible, correctly colored, and landscape?
2. Does touching corners update X/Y? Copy several `atilla_mini_touch` log lines.
3. Does Home Assistant discover `atilla-mini`? Does the clock appear after pairing?
4. Record ESPHome version, boot messages, reboot loops, Wi-Fi signal, and uptime.
5. If panel remains black/white but ESPHome logs and Wi-Fi work, **stop and report**. Try another controller definition after reviewing logs; don't randomly rewire GPIOs.

Note: two independent SPI buses are intentional for this board family. An ESPHome 2026 issue reports touch failures on a shared bus; see https://github.com/esphome/esphome/issues/16879.

## Following gates
0.1.1 panel controller/orientation and touch calibration, 0.2.x responsive UI and HA entities, 0.3.x compact weather/forecast, later bounded memory tests for static map/radar.
