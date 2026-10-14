# ATILLA-LAB development server

**Role:** general-purpose local development/test/deployment controller, not a third ESP display and not restricted to ATILLA firmware.

**Host:** Dell Latitude E6420, Debian 13/Xfce, hostname `atilla-lab`; SSH access, persistent `atilla-lab.service`; local-only dashboard at `127.0.0.1:8765` reached through an SSH tunnel. ESPHome, PlatformIO, esptool, camera and registered physical ESP test boards are part of the lab.

**Installed acceptance:** Mini validation/build/qualification through dashboard succeeded. The Home Assistant v11 workspace was subsequently installed, found over 800 actual entities using read-only discovery, and produced a downloaded Lovelace JSON dashboard with real IDs.

**Developed but not necessarily deployed:** subsequent consolidated previews, appliance grouping and hardware-transaction components were offline-tested. Do not describe them as installed without a Dell acceptance run.

**Default safety policy:** $20 monthly local AI allowance, eight AI calls per job, two correction retries; flash/erase denied unless specifically approved for matching device and job; erase needs its own authorization. Local budget accounting does not guarantee a provider-side hard billing cutoff. Hardware writes, especially for mains-powered Tuya appliances, require independent evidence and recovery readiness.

## Home Assistant setup experience

The intended user workflow is **web-first** at `/ha`: enter Home Assistant LAN IP or full URL, paste a long-lived token into a password field, click **Connect & Discover**, view discovery progress/counts and entities, then generate local dashboard JSON. The token must be stored locally in a `0600` private file, never echoed to the browser after submission, placed in logs, sent to AI prompts or embedded in generated dashboards. HA discovery must use a strict allowlist of read operations; live HA configuration and appliance control must remain disabled. A reconnect/change-token flow and safe failure messages are required. This easy-setup interface is a requested improvement and must not be represented as shipped until installed and tested.

**Current manual fallback:** `esp_agent.ha_workspace_cli configure` and `sync` with a private token file.

**Long-term scope:** independent project and device registry for arbitrary ESPHome firmware, Tuya MCU bridges, retrofitted smart appliances, and future toolchain adapters; locally executed builds and diagnostics; camera/serial physical acceptance; authenticated remote management.
