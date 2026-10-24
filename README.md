# Padauk PMS150C 0.32″ OLED Library

The Padauk PMS150C is a tiny microcontroller: a small programmable chip that runs instructions to control an electronic device. Its limited program memory and RAM make it useful to keep display code small and enable only the features an application needs.

This library is made for the SSD1315 0.32-inch monochrome OLED. See the [0.32-inch OLED product page at Questwise Ventures](https://www.questwiseventures.com/products/worlds-smallest-0-32-oled-60x32-display-screen).

This library gives the chip ready-made routines for drawing text, numbers, images, simple shapes and level bars. For example, an application can show a reading, a loading indicator or a small logo. It supports four screen orientations and builds graphics in one reusable eight-byte tile workspace, rather than keeping a copy of the whole screen in RAM. An 8×8 tile is simply a square of 64 pixels.

Start with the included application template, choose the features and characters you need, and replace the example drawing commands with your own. The code is written for **Padauk Mini-C and IDE 1.04C1**; it is not standard C. Each configuration must fit the chip's memory, so the guide includes build figures and the hardware checks reported by the user.

Examples-only package update prepared 2026-10-23 for **PMS150C, Padauk Mini-C and IDE 1.04C1**, using the user's working SSD1315 OLED setup. This is the frozen existing API, not standard C or a general framebuffer graphics library.

## Start an application

1. Extract/copy the complete `application_template` folder to your application location.
2. Open `PMS150C_OLED_APP.PRJ` in IDE 1.04C1. No Python is needed for this prepared project.
3. Select public requests in `feature_config.h` and glyphs in `font_config.h`. Leave derived requirements unchanged. The default enables drawNumber and drawChar only.
4. Set `APP_ROTATION => 0;` (or 1/2/3) in `app_config.h`.
5. Edit the marked **BEGIN APPLICATION DRAWING** section in the main `.C`; put subsequent application-controlled updates in its final loop. Preserve board setup/init/timing. Include the library header once, not as another compiled source.
6. Build, record full memory/warning output, program and check your application. Repeat every intended rotation. New features/images/glyphs require a fresh fit check.

Default: static **37%** at logical (0,0), occupying three cells, everything else blank. No demo case controls, timer counter, string buffer or observation pause.

| User-validated reference | PMS150C free ROM words | ICE free ROM words | Free RAM bytes | Reserved stack bytes | Checksum |
|---|---:|---:|---:|---:|---|
| Application template, static 37% | 332 | 450 | 14 | 12 | `0x0106DC` |
| Header-separated library, 37% text | 332 | 450 | 14 | 12 | `0x0106DC` |
| Header-separated library, one pizza image | 325 | 443 | 17 | 12 | `0x0A0700` |

All three source configurations have user-reported 0 errors / 0 warnings and hardware confirmation in all four orientations. The packaged application code/config/project files are byte-identical to the accepted template. **The user subsequently built the extracted v1.0.0 template with 0 errors / 0 warnings and confirmed correct 37% in all four orientations, with ROM/RAM matching the validated template.** See the [release-copy build report](validation/RELEASE_BUILD_RESULTS.txt). Codex performed packaging and host checks only. Stack reservation is not peak runtime use; one report is not four angle-specific checksums. Other feature combinations retain their documented historical scope.

## Package contents

- `application_template/`: ready-to-copy local Mini-C project, unchanged library/config/board timing.
- [API guide](docs/API_GUIDE.md): frozen public inputs, dependencies, cursor/scratch contracts and limits.
- [Feature configuration](docs/CONFIGURATION.md): recipes, default glyphs and optional image usage.
- [Validation and limitations](docs/VALIDATION.md): report scope, unresolved historical discrepancies and memory references.
- [Converter usage](converter/README.md): optional Python/Pillow PNG conversion; not needed to compile the template.
- `validation/`: selected verbatim user build histories. Absolute paths inside original logs are historical evidence, not package dependencies.
- `assets/`: accepted pizza input, tile metadata and expected-output previews, not hardware screenshots.
- `tests/`: portable host checks; these do not compile Mini-C.
- `PROVENANCE.json`, `MANIFEST.sha256`: source hashes and package-file integrity.

## Scope freeze

v1.0.0 contains the existing 19 public functions listed in the API guide and current feature switches, using one eight-byte workspace and four rotations. Internal helpers, declaration order and shared scratch are not a stable independent interface. No new drawing function, MCU port, memory optimization or timing change is included. See [release notes](RELEASE_NOTES.md).

Validation covers the configurations and hardware checks documented here; it does not imply exhaustive testing. Keep accepted working projects and reference snapshots for comparison. No license terms have been invented; this package does not add or change source/asset permissions.

## Optional host verification

From this extracted release root with Python/Pillow available:

```powershell
py -3 -B -m unittest discover -s tests -p test_release.py -v
```

Checks cover copied-file provenance, default project/API presence, exact converter regeneration of the accepted pizza header, refusing overwrite, portable documentation links and exclusion of IDE outputs. They do not build or emulate the MCU. `MANIFEST.sha256` covers all package files except itself; the adjacent ZIP checksum identifies the archive.

## Extracted-package validation — 2026-10-23

User report: checksum `0x0106DC`, PMS150C free ROM **332 words**, ICE **450 words**, free RAM **14 bytes**, reserved stack **12 bytes**. Text is correct in all four orientations. This validates the default text configuration; no new feature combinations or peak stack/timing measurements are implied. One supplied checksum is not four angle-specific checksums. The ZIP source/configuration payload remains the frozen rotation-0 template. Local extracted copies can contain changed rotation settings and IDE outputs; those are excluded from the distribution archive. This documentation refresh adds no code change.

## v1.0.1 application examples

Added [minimal-character and cube-loading examples](examples/README.md). No library API/body/timing or default application-template code changed. v1.0.0 archive remains untouched. The minimal single-glyph example is a new configuration pending IDE/memory/hardware results. Cube-loading source is an exact copy of the accepted application; its report is not a new packaged-copy test and does not specify tested orientations. Build and measure either example locally before adapting it.

## Repository preparation

For repository settings and publication instructions, see [GITHUB_SETUP.md](GITHUB_SETUP.md). Suggested description is in [REPOSITORY_DESCRIPTION.txt](REPOSITORY_DESCRIPTION.txt); the [draft release notes](.github/RELEASE_DRAFT.md) retain the distinction between user-validated configurations and pending minimal-example validation.
