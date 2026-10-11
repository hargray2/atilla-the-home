# ATILLA Mini NEO 1.1.0 — Sandbox Edition

This release is a **simulated standalone application** for the ESP32-2432S028 Cheap Yellow Display. The 7-inch Waveshare firmware is unchanged.

- Touch launcher with four large targets (ROOM / STORM / ARCADE / SYSTEM)
- Room light and alarm switches simulated in local ESP32 globals; **no Home Assistant service calls**
- Weather simulator (temperature, humidity, particle rain)
- Three simulated scenes (CYBER / CHILL / PARTY)
- Animated radar concept clearly marked **SIMULATION**, not actual weather
- 30-second target arcade and stored best score
- Real device information: Wi-Fi RSSI, free heap, ambient ADC voltage
- Backlight adjustment via GPIO21; SD intentionally not mounted
- Retains HA time only, Wi-Fi/API, ST7789V via mipi_spi, XPT2046 touch

Full `atilla-mini-neo-1.1.0-sandbox-public.yaml` and private source are supplied as conversation downloads. **This documentation commit is not a firmware source commit.** Local private source contains user connection credentials and must not be checked in. YAML parsed structurally, no verified ESPHome compilation or hardware test yet.

Next repository task: add the credentials-free firmware source once verified, and repair any findings from compilation.
