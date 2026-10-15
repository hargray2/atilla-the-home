# ATILLA-LAB — autonomous ESP engineering station

**Role:** a general-purpose local development and test controller for multiple ESPHome/PlatformIO targets. ATILLA-LAB is **not** another ESP display, and its installed server source is **not** contained in this public firmware repository.

**Host:** Dell Latitude E6420 running Debian 13 with Xfce, hostname `atilla-lab`. The lab has a persistent local control service, authenticated management dashboard, ChatGPT remote connector, ESPHome 2026.9.1, PlatformIO/esptool and V4L2 camera support. Camera capture is configured at **1920 × 1080 MJPEG**.

## What has actually been tested

| Capability | Evidence / current boundary |
|---|---|
| Remote connector | Authenticated, time-limited access; status, job history, development requests and qualified staged deployment |
| Server-only updates | Exact source patches qualified, then independently authorized for deployment; service restart and health confirmation were observed |
| ESPHome validation | A registered ATILLA-Mini target was successfully validated through a native local job on 2026-10-15 |
| ESPHome compilation | A registered ATILLA-Mini target returned successful native build results and source digest on 2026-10-15; exact YAML filename was **not** included in the remote result |
| Camera | V4L2 device registered and 1920 × 1080 MJPEG configured; this is not proof of a complete automated visual-acceptance pipeline |
| Hardware safety | Flashing, erasing and hardware writes remain disabled through the remote connector |
| Build-result reporting | Returns job status and limited summary; bounded redacted compiler logs and exact config identity are still needed |

The existing ChatGPT connector deliberately exposes a limited set of actions; it is **not a general remote shell**. Remote validation/build is currently routed through two exact safe-job requests handled by the existing development endpoint. Server status showing a project and config present should never replace validation of the exact source file chosen for compilation.

## Registered targets

- **ATILLA-Mini (CYD):** ESPHome target, local source project and physically registered serial identity. Use its own firmware configuration and hardware acceptance history.
- **ESP12 test board:** PlatformIO placeholder. The checkout and serial identity were not configured at the time of this audit. Do not schedule a physical flash against it.
- **Future devices:** independent ESPHome builds, Tuya MCU bridge diagnostics and appliance retrofits will need their own identities and authorization.

The **Waveshare 7-inch** has private complete firmware. Do not treat it as equivalent to a public component smoke-test YAML or to the CYD target.

## Budget, authorization and safe operations

The lab has a **$20/month internal AI reservation ceiling** (not a provider-side hard billing cap), bounded AI calls and audit records. Reservations may diverge from posted OpenAI API charges; reconciliation requires independently verified monthly costs. Ordinary validation and compilation jobs run locally rather than invoking the AI development worker.

The remote connector uses a timed authorization window, and firmware writes must remain gated by matching device identity, separate explicit approval, backup/recovery readiness and post-write physical verification. No build success, camera image, local source edit, Home Assistant discovery or staged patch automatically permits flashing or erasing.

## Home Assistant and dashboard boundary

The lab has a separate Home Assistant discovery/dashboard-export workspace. Read-only entity discovery and local Lovelace JSON export have previously been demonstrated; they do not imply live Home Assistant configuration edits or appliance control. A requested web-first **Connect & Discover** interface (HA URL/IP, secret token handling, entity preview and JSON export) must be labeled as planned until demonstrated on the installed service. Sensitive tokens must remain in local private storage, not model prompts, logs, public Git history or dashboard JSON.

The installed server lives in a separate Dell checkout at `~/atilla_control_server_v0_3`. An eventual dedicated lab repository should have its own release rules, tests, permissions and secrets policy. Do not copy that checkout into this public firmware-components repository as a shortcut.

**Related:** [ecosystem index](README.md) · [integration rules](INTEGRATION.md) · [Mini firmware](ATILLA-MINI.md) · [repository audit 2026-10-15](../REPOSITORY-AUDIT-2026-10-15.md).
