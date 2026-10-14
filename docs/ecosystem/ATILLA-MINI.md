# ATILLA Mini (CYD)

**Role:** independent compact ESP32 touchscreen endpoint.

**Source:** `experimental/atilla-mini/atilla-mini-1.5.7-idle-display.yaml` on `dev/cyd-mini-bringup`; ESPHome 2026.9.1 used for documented acceptance and Dell builds.

**Registered hardware:** ESP32-D0WD-V3 rev 3.1 with 4MB flash and CH340 USB serial. Hardware MAC and original complete flash backup are retained privately on the development server; do not publish them.

**Hardware acceptance:** [Mini 1.5.7 report](../CYD-MINI-1.5.7-HARDWARE-ACCEPTANCE.md). It confirms background weather, 60-second dimming, 120-second backlight-off and continued Wi-Fi/uptime operation. First-touch wake behavior, later weather poll and inserted-card SD CRC were not established by the cited evidence.

**Build and deployment:** validation, compilation and qualification succeeded through ATILLA-LAB. Development secrets labeled `BUILD_ONLY_DO_NOT_FLASH` prohibit deployment. Any future write needs allowlisted identity, a matching build and single-use authorization, plus a verified recovery path and hardware acceptance.

Mini changes must never implicitly change Waveshare firmware.
