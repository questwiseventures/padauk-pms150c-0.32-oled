# Validation evidence and boundaries

User builds: IDE 1.04C1 / PMS150C, tested on the user's hardware with controller identified as SSD1315. Codex did not compile/program/test the MCU. The application template and header-separated text/image configurations have reported 0 errors / 0 warnings and all-four-orientation confirmation. The release files are copied exactly; the user subsequently validated the extracted default 37% template with 0 errors / 0 warnings, correct output in all four orientations and matching reference ROM/RAM. See [release-copy report](../validation/RELEASE_BUILD_RESULTS.txt).

## Revision-scoped memory table

Free ROM is words; RAM/stack are bytes. These are exact reported configurations, not additive per-function costs. One reported checksum per configuration does not assign it to every separately built angle.

```text
Configuration            Scope  Checksum  Free ROM PMS/ICE  RAM  Stack
All requests off         S0     0x6C1040       679 / 797      40     8
Level-only static        S1     0x6A0739       423 / 541       8    12
Percentage-only static   S0     0x141D61       422 / 540       8    12
Both bars static         S0     0x0F0BF2       318 / 436       7    12
Clear-only selective     S2     0x2514D5       492 / 610      16    12
Fill-only                S2     0x0C0BF9       522 / 640      18    12
Invert + bitmap          S2     0x111FA4       469 / 587      20    10
Frame-only               S2     0x220A16       427 / 545       8    12
Fill + clear             S2     0x0C1FE9       490 / 608      16    12
Fill + frame             S3     0xB90F28       387 / 505       6    12
One generated pizza      S3     0x0A0700       325 / 443      17    12
DrawLine-only diagonal   S3     0xBC03D9       381 / 499      13    10
DrawRectangle-only       S3     0x441A86       449 / 567      14    10
FillRectangle-only       S3     0x631810       459 / 577      14    10
Three shapes composed    S3     0x4C0BDA       279 / 397      13    10
37% number + char only   S3     0x0106DC       332 / 450      14    12
37% text, header split   S6     0x0106DC       332 / 450      14    12
Pizza, header split      S6     0x0A0700       325 / 443      17    12
37% text + percent bar   S3     0xDE0F0B       197 / 315       2    12
37% drawString only      S3     0x31161F       366 / 484      13    12
String case 0 zero len   S4     0x3912A3       364 / 482      13    12
String case 1 exact fit  S4     0xD101BC       361 / 479      13    12
String case 2 overflow   S4     0x540D13       361 / 479      13    12
String case 3 invalid    S4     0x49112E       363 / 481      13    12
Number case 0 value 0    S5     0x611B93       335 / 453      14    12
Number case 1 value 9    S5     0xD10A64       334 / 452      14    12
Number case 2 value 10   S5     0xA1083D       334 / 452      14    12
Number case 3 value 99   S5     0xAA0FC0       334 / 452      14    12
Number case 4 value 100  S5     0xEC085B       334 / 452      14    12
Number case 5 value 255  S5     0x931C2C       334 / 452      14    12
Number case 6 exact fit  S5     0x891038       331 / 449      14    12
Number case 7 overflow   S5     0xB51F1F       331 / 449      14    12
Number case 8 invalid    S5     0xD11669       333 / 451      14    12
Separate minimal text    EXT    0x031F48       204 / 322       2    12

Application template     APP    0x0106DC       332 / 450      14    12
```

Scope: S0 initial dependency library; S1 later level-only regression before image integration; S2 cell/frame integration/demo revisions before image integration; S3 after image integration but before header separation; S4 string boundary cases; S5 number cases; S6 post-header-split text and pizza; EXT separate minimal-text-plus-bar project; APP the accepted application template default. All rows are user-reported warning-free builds with four-orientation observations for their stated configurations. Only S6 and APP establish those later revisions' own results. Earlier cases are not silently carried forward as new validation.

Full logs: [application](../validation/APPLICATION_BUILD_RESULTS.txt), [integration/library](../validation/LIBRARY_BUILD_RESULTS.txt), [separate minimal text](../validation/MINIMAL_TEXT_BUILD_RESULTS.txt). Initial statuses inside chronological logs describe their dates and can be superseded by later entries. Failed-build memory lines are not valid allocation results.

The header-separated text and image reports match earlier accepted figures; the user reports ROM/RAM matching the reference snapshot. The template also matches text. No reported overhead in those specific configurations; not a binary identity claim. Peak stack/electrical timing are unmeasured. Two free RAM bytes in a text-plus-bar configuration are not a general budget for adding features.

## Known limits and unresolved historical reports

- PMS150C ROM is 1008 words. Arbitrary API/font combinations may not fit. Larger-MCU portability is unverified.
- No framebuffer, OLED pixel readback, transparent overlay, arbitrary pixel graphics, generalized positioned image API or runtime image selection is introduced. Shared workspace/scratch is sequential/nonreentrant.
- Direct drawChar needs valid cell coordinates. String/number clipping stops at the row edge without wrapping. Unknown/excluded glyphs are blank. Shorter or moved readings need explicit erasure.
- Frame edge tiles are opaque, while whole interior cells are skipped. The specific string/number invalid-column cases do not test all invalid rows/values. Blank-screen cases do not visibly demonstrate rotation geometry or prove absence of RAM reads.
- Historical tile_ops_template clear+invert supplied the same checksum/memory as clear-only; fill+invert supplied the same as fill-only despite differing hardware observations. Configuration-specific allocation/savings remain unresolved; see [original tile-op reports](../validation/HISTORICAL_TILE_OPS_REPORTS.txt).
- Earlier tile_level_bar_percent percentage-disabled static supplied the same 0x441566/memory as enabled static despite different observations. Investigation was explicitly skipped; see [original reports](../validation/HISTORICAL_PERCENT_BAR_REPORTS.txt). Later independent bar switches have their own reports; they do not resolve the earlier discrepancy.
- Adding a selective-clear observation demo changed reported free RAM despite no new explicit runtime variable; no specific compiler cause was established. Different checksums with equal memory are not proof of binary equality, caching or a compiler defect.
- Optional shape/text/cell/frame combinations have historical records; exhaustive combinations and all changed application behaviors are unverified. Host source models are not Mini-C simulation.

No unresolved historical report has been relabeled as a clean measurement by packaging. Raw histories are preserved byte-for-byte. The full historical experiment tree/reference snapshots remain outside this small release bundle in the original workspace.

## Extracted release-copy result, 2026-10-23

Default text: checksum 0x0106DC; PMS150C last code 0x2A4 / free 0x14C (332 words); ICE last code 0x22E / free 0x1C2 (450 words); free RAM 14 bytes, reserved stack 0x34..0x3F (12 bytes). User confirms warning-free compilation and correct 37% in all four orientations. Matches the validated template and header-separated text figures: no reported memory overhead or saving. One active report supplied, not four separately supplied rotation reports. Peak stack and binary identity are not established. Other release-copy feature configurations remain untested by this report.

## v1.0.1 examples

Minimal-character example is new and pending IDE/memory/hardware validation. Cube-loading MCU files are byte-identical to the accepted separate application. User reported 0 errors / 0 warnings, checksum 0x3B1A0E, free ROM PMS150C 19 / ICE 100 words, RAM 4, stack 12 and correct cube/bar/number with blank-row spacing. Latest hardware confirmation did not specify orientations; no all-four-orientation claim or explicit reset/cycle-count test is inferred. Full source report is [here](../examples/cube_loading/BUILD_RESULTS.txt). The copied example has no new IDE/hardware run. Neither example alters v1.0.0 API or default template validation.
