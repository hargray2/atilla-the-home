# ATILLA Mini NEO 1.0.0 — Room Control

- Complete credential-free source is distributed as downloadable `atilla-mini-neo-1.0.0-public.yaml` (source not yet committed to this repository).
- Private working-copy YAML: `atilla-mini-neo-1.0.0-private.yaml`; never commit local Wi-Fi/API credentials.
- New ROOM page replaces repetitive DATA page, keeps eight-screen layout and Birthday Challenge.
- Configurable `room_light_entity` defaults to **placeholder** `light.living_room` — change to an existing Home Assistant light entity before flashing.
- Live HA binary state displayed. Tapping the room card invokes `homeassistant.action: light.toggle`; HA exposes a corresponding software button `Mini Toggle Room Light`.
- **Requires Home Assistant authorization:** Settings > Devices & services > ESPHome > configure ATILLA Mini > "Allow the device to perform Home Assistant actions".
- No external SD required. Existing hardware and Waveshare 7-inch firmware unchanged.
- YAML structure parsed with Python PyYAML; ESPHome build and hardware operation **unverified**.

Reference: https://esphome.io/components/api/#homeassistant-action-action
