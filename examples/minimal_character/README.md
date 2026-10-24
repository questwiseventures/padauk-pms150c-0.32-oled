# Minimal character example

Open PMS150C_OLED_MINIMAL.PRJ in IDE 1.04C1. It initializes the accepted board/OLED, selects APP_ROTATION, clears once and draws a single static **0** at logical (0,0), then idles. Everything else blank. No number conversion, string buffer, image loader/index, application counter, delay or animation. DRAWCHAR is the only public request; only FONT_30 is enabled. Existing library helpers/cursor/renderer state still use RAM.

Change APP_ROTATION in app_config.h to 0/1/2/3, rebuilding/programming each time. Check complete zero and position: physical cells (0,0), (6,0), (6,3), (0,3) in the 7x4 drawable grid. Default rotation 0. Update feature/glyph selections before adding application code. Library/PRE/extern and board setup/init/timing match the accepted template.

**New example IDE build, warning count, checksum, ROM/RAM/stack and hardware validation are pending.** Earlier numeric case 0 used drawNumber, so its allocation/validation does not apply here. Record the complete compiler report and hardware observations in BUILD_RESULTS.txt. Host preservation/source checks are not MCU compilation. See the [examples index](../README.md).
