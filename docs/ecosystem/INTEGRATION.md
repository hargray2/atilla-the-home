# ATILLA — integration and release boundaries

ATILLA projects share design principles, but keep their firmware, hardware identities and acceptance evidence separate.

| Ownership | Source of truth | Public repo boundary |
|---|---|---|
| **Waveshare 7-inch** | Private complete `waveshare.yaml`, hardware acceptance logs and rollback image | Public external components and component smoke tests only |
| **ATILLA Mini (CYD)** | Versioned Mini ESPHome YAML and hardware acceptance on `dev/cyd-mini-bringup` | Separate board pinout, touch, display and backup from Waveshare |
| **ATILLA-LAB** | Separate installed Debian server checkout, persistent jobs and private hardware identity registry | Repository includes documentation only, not the installed server/credentials |
| **Home Assistant** | Independent instance and configuration | Read-only discovery / locally generated dashboard JSON do not imply live HA writes |

## A release must identify

1. Exact project and physical board revision (including registered identity for a write).
2. Exact source file and immutable revision, toolchain version and firmware artifact digest.
3. Level of evidence: inspected, validated, compiled, uploaded, booted, physically observed and soak tested.
4. Credential and privacy boundary; secrets must remain outside public source and logs.
5. Recovery image and tested rollback method.
6. Explicit approval before flash/erase or other hardware writes.

A successfully compiled ESPHome fixture does **not** prove full private-firmware performance. A working hardware cache-write experiment does **not** authorize a general-purpose SD file writer. All custom storage writes and appliance controls need separate safety and recovery checks.

For development context, see [ecosystem projects](README.md), [release workflow](../RELEASE-WORKFLOW.md), and [security guidance](../SECURITY-AND-CONTRIBUTING.md).
