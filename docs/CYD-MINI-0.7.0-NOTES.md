# ATILLA Mini NEO 0.7.0 — direct navigation and controls

Private firmware artifact: `atilla-mini-neo-0.7.0-private.yaml` (delivered outside Git because it contains local credentials).

Features:
- Upgrade from 0.6.0 and keep confirmed ST7789V MIPI SPI, XPT2046, Arduino ESP32, HA time and weather wiring.
- Touch the bottom 7 tabs to select HOME/WX/DATA/MAP/SYS/CTRL/SD directly; tapping elsewhere advances one page. Navigation depends on touch coordinate calibration, with tap-to-cycle as fallback.
- CONTROL card cycles 100%, 70%, 40%, 15% backlight through GPIO21.
- Two device-side Home Assistant buttons: `Mini Next Page`, `Mini Brightness Step`.
- SYSTEM page has a new ambient ADC voltage graph, along with Wi-Fi and heap metrics.
- SD remains disabled; radar remains a non-live visual.
- Private file YAML-parsed successfully; **not ESPHome compiled or hardware verified**.

Never commit private credentials to this repository. Do not copy the Waveshare YAML into this branch.
