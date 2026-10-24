# Cube with repeating loading bar and number

Open PMS150C_OLED_PERCENT_CUBE.PRJ in IDE 1.04C1. This is an exact source copy of the accepted separate application, not a new library feature.

The full 32x32 cube is drawn once. A 3-cell bar progresses 0..100 in 5-point steps, pauses per APP_HOLD_CYCLES=200000, resets and repeats. A right-aligned three-cell numeric label beneath it overwrites old digits with spaces. There is no percent symbol; original digit font retained. One full blank row separates bar and number.

Landscape: cube cells (0,0)..(3,3), bar (4,1)..(6,1), number (4,3)..(6,3). Portrait: cube (0,0)..(3,3), bar (0,4)..(2,4), number (0,6)..(2,6). Full-size cube leaves no space to its right in portrait. APP_ROTATION=0/1/2/3 selects the four layouts; APP_PORTRAIT is derived, do not edit it. Rebuild after selection. Delays are cycle directives plus render work, not calibrated seconds.

The original application user report: **0 errors / 0 warnings**, checksum **0x3B1A0E**, free ROM PMS150C **19 words** / ICE **100 words**, free RAM **4 bytes**, reserved stack **12 bytes**. User confirmed cube/bar/number and blank-row spacing on hardware; the latest confirmation did not identify orientations or explicit reset-erasure/cycle-count checks. Do not infer all-four-orientation validation for this application. This packaged example copy has no new IDE/hardware run. Complete raw revision/build history is local in BUILD_RESULTS.txt.

The code uses CELL_DATA[8] plus IMAGE_TILE and existing bar state. To fit ROM, after bar rendering it temporarily reuses BAR_BITS/BAR_STEP/BAR_RIGHT for fixed-width formatting; drawChar does not use them. This is an application-specific dependency on frozen v1.0.0 internals, not a public scratch API. Sequential use only; do not interleave concurrent bar calls or transplant this assumption into changed drivers. BAR_PERCENT is retained as progress. The next drawPercentBar recomputes scratch.

Library/init/PRE/extern/timing unchanged from the accepted template. No space for unmeasured additions is implied by the tiny remaining ROM/RAM. Build with this local PRJ; compare full report and check repeated full/empty states, old-digit clearing, logo preservation and desired rotations. Do not infer successful fit from failed-build historical output. See the [examples index](../README.md).
