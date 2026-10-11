# ATILLA Mini NEO 0.9.0 — Birthday Challenge

0.9 is a private ESPHome YAML firmware generated from the 0.8 Birthday Blitz build. Local file: `atilla-mini-neo-0.9.0-birthday-challenge-private.yaml` (not committed because it contains Wi-Fi and API credentials).

Changes:
- 30-second timed target challenge, on-device hit/miss counters and shrinking time bar.
- Tap the game screen to begin a new round; then hit moving targets. Game clock advances with ESPHome interval rather than depending on display lambdas.
- Persistent best score using ESPHome restored global (flash preference); updated only when a round finishes.
- Corrected Home Assistant Next Page button wrap from 7 to 8 pages.
- Existing weather, Home Assistant clock, controls, ambient light, SD placeholder and confirmed display/touch GPIO retained.

Limitations: YAML parsed but **not ESPHome compiled or hardware tested**. Gameplay starts on game screen, no SD needed. No live radar, no working SD card I/O, and the weather entity needs to match the homeowner's HA entity. Display and touch calibration are board-specific.

Security: Never publish the private YAML or credentials. The Waveshare 7-inch firmware remains unchanged.
