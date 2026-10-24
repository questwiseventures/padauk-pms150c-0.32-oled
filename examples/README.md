# Application examples

Each folder is a complete local Mini-C project with its own frozen library/configuration. Open one PRJ; do not combine sources between example folders. The default application_template remains the recommended starting point.

| Example | Open | Behavior | Validation scope |
|---|---|---|---|
| [Minimal character](minimal_character/README.md) | minimal_character/PMS150C_OLED_MINIMAL.PRJ | One 0 via drawChar, then idle | New configuration: IDE/memory/hardware pending |
| [Cube loading](cube_loading/README.md) | cube_loading/PMS150C_OLED_PERCENT_CUBE.PRJ | Cube, repeating loading bar and spaced numeric value | Exact copy of separately accepted application; 0 warnings, free ROM 19/100, RAM 4, stack 12, checksum 0x3B1A0E. Orientation scope unspecified in latest hardware report; packaged copy not retested |

For first use, build/program unchanged, capture compiler report, and test intended rotations. Set APP_ROTATION=0,1,2,3 in each local app_config.h and rebuild. Preserve clock/pins, init command order, original post-init delay and library transfer timing. No Python needed for IDE builds. Host package checks do not compile Mini-C. Record new results in the respective BUILD_RESULTS.txt. API definitions and default template are unchanged from v1.0.0.
