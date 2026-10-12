# ATILLA Mini NEO 1.4.0 — Two-bank navigation

- Two navigation banks, four 80px-wide bottom buttons each.
- Bank 1: HOME, WX, ROOM, GAME. Bank 2: SYS, ART, FX, CTRL.
- XPT2046 on_touch/on_update/on_release recognize horizontal displacement >60px; horizontal swipe chooses bank. Bottom 40px selects a page. Touch calibration must be confirmed on hardware.
- Radar removed. One weather screen shows live temperature, humidity, wind and 3 daily high/low slots.
- Default live weather entity: `weather.forecast_home` (user-configurable); actual entity ID is not verified. Six optional `sensor.atilla_mini_day{1,2,3}_{high,low}` sensors provide forecast values when available. No fabricated forecast is shown.
- Existing simulated room, birthday arcade, Pixel Forge, Hyperspace FX, system diagnostics and brightness retained.
- Credential-free and private copies of 1.4.0 created as separate downloadable YAML files. Full sources not yet committed to GitHub.
- YAML structure parsed; full ESPHome compilation and hardware tests not performed. No Waveshare changes, no real device actions.
