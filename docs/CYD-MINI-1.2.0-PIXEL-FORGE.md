# ATILLA Mini NEO 1.2.0 — Pixel Forge

Experimental ESPHome Mini release built from 1.1.1 Sandbox. Includes a new ninth page, **ART**, with a 16x7 in-memory pixel painting grid, six-color palette, and CLEAR operation. Uses 112-byte vector palette indices; no SD card required. Tap-to-select navigation uses nine bottom tabs. Birthday Blitz interval refresh is corrected to the actual game page (page 5 instead of 7).

The downloadable private firmware is intentionally **not** included in this public repository because it contains local Wi-Fi and API credentials. A credential-free companion source was also generated separately for review and manual application; that source is not committed by this documentation commit.

No Home Assistant device actions and no real room entities. MIPI SPI and XPT2046 pin configuration remain unchanged. YAML parsing passed, but full ESPHome compilation and hardware tests have not been performed. The separate Waveshare firmware was not changed.
