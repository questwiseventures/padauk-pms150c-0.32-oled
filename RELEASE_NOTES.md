# v1.0.1 — 2026-10-23

Added self-contained minimal drawChar and cube/loading application examples. Existing 19 APIs, library header, initialization, timing and default validated application template are unchanged. No existing projects or v1.0.0 archive modified. Minimal example has host checks only, pending IDE fit/hardware. Cube example is copied from the accepted application (0x3B1A0E, 19 free PMS ROM words, RAM 4); packaged-copy and orientation scope remain explicit. This is an examples-only patch package, not a new driver/API revision.

# v1.0.0 — 2026-10-23

First packaged source release of the existing integrated Mini-C library.

- Freeze the 19 public APIs and their existing input/state contracts.
- Ship the accepted header-separated static 37% application template, without demo-only cases.
- Include the unchanged PNG converter and selected user validation logs.
- Consolidate portable package documentation with revision-scoped results and known limits.
- Preserve routine bodies, eight-byte workspace, software rotations, board clock/pins, initialization command order, original `.delay 400000` and transfer timing byte-for-byte in MCU files.

No new MCU compilation, programming, hardware test, peak stack measurement or calibrated timing measurement was performed during packaging. At preparation time release-copy checks were pending; subsequent user validation on 2026-10-23 confirms the extracted default 37% template with 0 errors / 0 warnings, all four orientations and matching reference ROM/RAM. Other feature combinations remain outside this release-copy check. Header-separated text/image and the application template have prior user validation; earlier bar/shape/cell/frame/boundary reports describe their earlier revisions. Future feature additions belong after this scope freeze and require explicit versioning and measurement.

Documentation refresh 2026-10-23: added the extracted-package validation report; MCU ZIP payload is unchanged.
