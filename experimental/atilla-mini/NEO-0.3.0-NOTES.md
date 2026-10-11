# ATILLA Mini NEO 0.3.0 — feature build checkpoint

- Device: ESP32-2432S028 dual-USB CYD.
- Hardware known working: ST7789V on MIPI SPI with GPIO14/13/15/2/21; XPT2046 on GPIO25/32/39/33/36; ESPHome HA clock/API online.
- 0.3.0 **private** owner-local YAML prepared for device testing. It is deliberately not stored in this GitHub repository because it contains Wi-Fi and API credentials.
- Four-screen tap-cycling UI: HOME, WX, SYS, LAB; 1-second redraw, dark grid/HUD styling, live HA clock and weather attribute reads, Wi-Fi and uptime/heap status.
- The previous version has shown correct touch callbacks, but coordinate accuracy is not yet demonstrated; tap anywhere is a temporary navigation model.
- Temperature and humidity still depend on the configurable `weather_entity`. Matching Waveshare readings requires the actual Waveshare source entity(s) or a shared HA feed; the existing private Waveshare firmware uses independently fetched Open-Meteo data.
- Initial 0.3.0 build **has not been compiled or validated on device**. Restore known-good 0.1.0 if needed.
- Future work: touch calibration, swipe gestures, light/entity controls, forecast, and memory-bounded cached radar. No map/radar downloads are in this version.
- Waveshare firmware remains unchanged.
