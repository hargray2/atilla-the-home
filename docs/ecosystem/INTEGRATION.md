# Integration and ownership boundaries

- **Waveshare 7-inch:** private firmware; public components and acceptance reports under `components/` and `docs/`.
- **ATILLA Mini:** source under `experimental/atilla-mini/`; own acceptance and rollback history.
- **ATILLA-LAB:** separate Debian service checkout at `~/atilla_control_server_v0_3`; should eventually move to a dedicated repository when stable.
- **Home Assistant:** independent server; ATILLA-LAB may read sanitized entities, devices, areas and layout summaries; downloaded Lovelace JSON is generated locally and does not imply HA installation.

## Release record checklist

For each component record source revision, exact hardware target, toolchain, test level (offline/validate/build/hardware), firmware artifact digest when relevant, physical verification evidence, unresolved issues and verified rollback. Never equate compilation with runtime success. Never assume a flash authorization for one board carries to another.

## Safety

Do not commit tokens, Wi-Fi passwords, private full firmware YAML, original flash backups or precise private coordinates. Device flash and erase operations must fail closed for unknown targets and missing grants. Home Assistant connection must not grant live appliance-control operations.
