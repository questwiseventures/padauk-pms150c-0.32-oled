# Version 1.0.0 API guide

Frozen existing public API, 2026-10-23. Source: `application_template/oled_library.h`. Build scope is in [VALIDATION.md](VALIDATION.md); verbatim reports are in `validation/`. The packaged MCU files are byte-identical to the accepted application template; no IDE/hardware testing was performed by Codex. Subsequent user testing confirms the extracted default 37% application in all four orientations with matching reference ROM/RAM; see [release validation](../validation/RELEASE_BUILD_RESULTS.txt).

Public functions: `OLED_Init()`, `setRotation()`, `ClearScreen()`, `setCursor()`, `drawChar()`, `drawString()`, `drawNumber()`, `drawBitmap()`, `clearTile()`, `drawLine()`, `drawRectangle()`, `fillRectangle()`, `drawLevelBar()`, `drawPercentBar()`, `clearCell()`, `fillCell()`, `invertTile()`, `drawFrame()`, `drawImage()`.

## Feature selection

In `feature_config.h`, use literal Mini-C `=> 0;` / `=> 1;` for public requests. Do not edit derived requirements. Public zero means no independent request; dependent features may still include a helper.

| Public request | API / automatic requirements |
|---|---|
| `OLED_USE_DRAWCHAR` | `drawChar`; font, cursor, cell renderer and workspace |
| `OLED_USE_DRAWSTRING` | `drawString` plus `drawChar` and its dependencies |
| `OLED_USE_DRAWNUMBER` | `drawNumber` plus `drawChar` and its dependencies |
| `OLED_USE_DRAWBITMAP` | `drawBitmap`, cursor, renderer and workspace |
| `OLED_USE_CLEARTILE` | `clearTile` and workspace; no renderer/cursor by itself |
| `OLED_USE_DRAWLINE` | `drawLine`, shape scratch, `clearTile`, `drawBitmap`, cursor/renderer/workspace |
| `OLED_USE_DRAWRECTANGLE` | `drawRectangle`, same shape dependencies |
| `OLED_USE_FILLRECTANGLE` | `fillRectangle`, same shape dependencies |
| `OLED_USE_LEVELBAR` | `drawLevelBar`, bar scratch, `clearTile`, `drawBitmap`, cursor/renderer/workspace |
| `OLED_USE_PERCENTBAR` | `drawPercentBar`, same bar dependencies |
| `OLED_USE_CLEARCELL` | `clearCell`, bitmap/cursor/renderer/workspace |
| `OLED_USE_FILLCELL` | `fillCell`, bitmap/cursor/renderer/workspace |
| `OLED_USE_INVERTTILE` | `invertTile`, workspace only; display output needs a separate bitmap requirement |
| `OLED_USE_DRAWFRAME` | `drawFrame`, bitmap/cursor/renderer/workspace, optional frame dimensions |
| `OLED_USE_DRAWIMAGE` | `drawImage`, guarded generated header, one BYTE IMAGE_TILE, bitmap/cursor/renderer/workspace |
| `OLED_KEEP_CURSOR` | Keep `setCursor`/cursor inputs even without a drawing request |

`OLED_Init`, `setRotation` and `ClearScreen` remain available independently. Text selection does not automatically enable glyphs: select required characters in `font_config.h`. The parent font has digits, space and percent; the minimal-text demo excludes space. Excluded or unknown characters render as blank cells. Full feature/font selections may exceed memory.

## Coordinates and cursor state

The drawable grid is 7 columns by 4 rows at rotations 0/2 and 4 by 7 at rotations 1/3. Coordinates are zero-based **8x8 cells**, not pixels. Physical drawing spans columns 34..89 and pages 0..3; `ClearScreen` retains the wider original clear window, columns 34..93.

`CURSOR_COLUMN/PAGE` are public staging inputs to `setCursor`. `CHAR_COLUMN/PAGE` are the active drawing position. Advancing the active column does not change the staging inputs. Calling `setCursor` again reloads the staged position. `setCursor` performs no validation. Direct `drawChar` also performs no bounds check: ensure the active cell is valid before calling it.

## API contracts

All callable APIs use Mini-C shared globals or accumulator `A`, not standard C parameters. Treat `A`, flags, transport/address state and internal rendering scratch as volatile across display calls. None of the drawing routines wraps to a new row.

| API | Inputs | Cursor and input behavior | Workspace / output / limits |
|---|---|---|---|
| `OLED_Init()` | No application input; inherited board clock/pins must already be configured | Does not initialize a logical cursor or replace `setRotation` | Sends working initialization sequence and original `.delay 400000`; does not clear display RAM |
| `setRotation()` | `A=0,1,2,3` for clockwise 0/90/180/270 degrees | Invalid BYTE value falls back to 0; sets grid dimensions and resets active `CHAR_COLUMN/PAGE` to 0; staging cursor unchanged | Software mapping only; does not erase/redraw existing content |
| `ClearScreen()` | None | Active and staging cursors unchanged | Clears original physical window independent of rotation; changes transport/address scratch, not `CELL_DATA` |
| `setCursor()` | `CURSOR_COLUMN`, `CURSOR_PAGE` | Copies to active position; staging inputs retained; no bounds check | No OLED write or workspace change |
| `drawChar()` | `A=single-byte character`, valid active cell | Active/staging cursor unchanged; character in `A` not retained | Rebuilds all eight `CELL_DATA` bytes, sets `CELL_SOURCE`, writes opaque cell; unknown/excluded glyph writes blank; no clipping |
| `drawString()` | `STRING_POINT` WORD RAM-byte address; `STRING_LENGTH` BYTE count | Advances pointer and active column and decrements length for each byte drawn; page/staging cursor unchanged | Stops at row edge before next read; remaining pointer/count describe unread suffix. No terminator processing, wrap or ROM/string-literal pointer support; workspace overwritten per glyph |
| `drawNumber()` | `NUMBER_VALUE` BYTE 0..255 | Value retained; active column advances per digit drawn; page/staging cursor unchanged | Decimal, no leading zeroes; zero draws one digit. Stops at row edge, possibly leaving a numeric prefix. Uses four local BYTE temporaries plus glyph/renderer scratch |
| `drawBitmap()` | `BITMAP_POINT` WORD address of eight valid RAM bytes, active cell | Pointer and both cursor forms retained | Bit 0 is top of each input column before rotation. Invalid active cell skips read/write. Valid call sets `CELL_SOURCE` and sends opaque rotated cell; does not modify source bytes or `CELL_DATA` itself |
| `clearTile()` | None | Both cursors and bitmap/source pointers unchanged | Zeros `CELL_DATA[0..7]` only; does not erase OLED pixels |
| `drawLine()` | `SHAPE_X0/Y0/X1/Y1`, inclusive local pixels 0..7 | Endpoints retained; cursors unchanged | ORs line pixels into workspace, includes endpoints; accepts either direction. Any coordinate outside 0..7 rejects request. No display write |
| `drawRectangle()` | Same shape inputs, ordered endpoints | Inputs/cursors retained | ORs outline into workspace. Invalid coordinates or reversed endpoints do nothing. No display write |
| `fillRectangle()` | Same shape inputs, ordered endpoints | Inputs/cursors retained | ORs inclusive filled area into workspace; invalid/reversed endpoints do nothing. No display write |
| `drawLevelBar()` | Active start cell, `BAR_LENGTH`, `BAR_LEVEL=0..4` | Public inputs retained; active column restored after valid rendering, page/staging cursor retained | Eight-pixel-high opaque horizontal bar across whole cells; generates/sends one tile at a time. Workspace, bitmap/source pointers and bar scratch overwritten |
| `drawPercentBar()` | Active start cell, `BAR_LENGTH`, `BAR_PERCENT=0..100` | Same retention as level bar | Same footprint and scratch use; interior fill is floor((8*length-4)*percent/100), not nearest-pixel rounding |

For bars, length must be nonzero and fit the remaining logical columns; the start page/column must be valid. Invalid values/geometry reject the request without changing display, workspace, bitmap/source pointers or cursor; scratch may change during validation. Length is not clipped. At column zero, lengths 1..4 fit every orientation; 5..7 fit landscape only. Interior width is `8*length-4`, with exact quarter increments for levels 0..4. Outer columns are blank margins; border is at x=1 and x=8*length-2, y=0 and y=7.

## Cell and frame APIs

| API | Inputs and cursor | Shared state and limits |
|---|---|---|
| `clearCell()` | Active cell; no cursor advance | Sets all `CELL_DATA` to 0, `BITMAP_POINT=CELL_DATA`, then `drawBitmap`; opaque erase. Invalid cell still changes workspace/pointer, but skips display write |
| `fillCell()` | Active cell; no cursor advance | Same as clearCell with all bytes 0xFF; invalid-cell behavior identical |
| `invertTile()` | Existing `CELL_DATA[8]`; cursors unchanged | Complements workspace only, preserves pointers, clobbers A/flags; caller explicitly sends result using drawBitmap; no OLED readback |
| `drawFrame()` | Active start, `FRAME_WIDTH/HEIGHT` BYTE nonzero cells; inputs/staging cursor retained, active start restored | Requires whole frame to fit; otherwise no display/workspace/pointer/cursor change. Valid calls overwrite perimeter cells, skip interior cells, leave `BITMAP_POINT=CELL_DATA` and last perimeter tile in workspace; renderer/address scratch clobbered |

Frame edge cells are opaque, including cleared non-border pixels; whole interior and outside cells are untouched. Border is at the extreme outer pixels of the cell footprint. Degenerate 1x1/1xN/Nx1 cases have no dividers. Moving/resizing a frame does not erase old edges. Frame adds two public BYTE inputs and uses six local BYTE temporaries plus a WORD pointer; no new tile buffer. Exact cost depends on the enabled application; see the revision-scoped validation records. clear/fill/invert add no local variables, but their dependencies and call depth can affect the compiler memory report.

## Shared storage and call discipline

- `CELL_DATA[8]` is the only common pixel workspace. It exists only when required. Glyphs overwrite it, shapes modify it, bars rebuild it per tile. Send a composed shape before calling another routine that overwrites it.
- `CELL_SOURCE` is the renderer's shared RAM pointer. `BITMAP_POINT` is the raw tile input pointer; bar calls leave it pointing to `CELL_DATA`. Do not assume a previously selected external bitmap survives a bar call.
- Shapes share `TILE_X/Y` plus helper locals. Bars share six BYTE values (`BAR_RIGHT`, `BAR_FILL`, `BAR_STEP`, `BAR_X`, `BAR_COLUMN`, `BAR_BITS`) and WORD `BAR_POINT`. These are implementation scratch, not application storage. Public bar inputs also share between successive uses; set them before each requested draw.
- Transport uses `I2C_Address`, column/page address globals, accumulator/flags and local scratch. The renderer uses five BYTE locals and a WORD pointer. Local declarations are not a portable peak-RAM calculation; use the actual Mini-C linker report.
- Calls are sequential and nonreentrant. Do not invoke display APIs from an interrupt while another call is active. Do not alias application data onto library scratch.
- `drawString` needs its own valid RAM input buffer, retained for the duration of the call. Do not place a multi-character string in `CELL_DATA`: the first glyph replaces unread bytes. Valid raw tile source RAM must likewise remain stable during output.
- No framebuffer or OLED pixel readback exists. Cell writes are opaque; they replace all pixels in their footprint. Shapes compose only within the RAM tile, not over existing screen pixels.
- Shorter numbers do not erase old trailing digits. Moving/resizing/rotating a drawing does not erase its previous footprint. Explicitly redraw blank cells or clear the required screen area. A blank glyph can only be relied on through direct `drawChar` at a valid cell; include space if your string requires it.

## Mini-C usage examples

These are application snippets, not new wrapper routines or compiler-tested configurations. Enable the referenced APIs and glyphs first. Run inherited board setup, `OLED_Init`, selected rotation and initial `ClearScreen` before drawing.

Append percent after a number (known to fit):

```c
CURSOR_COLUMN = 0;
CURSOR_PAGE = 0;
setCursor();
NUMBER_VALUE = 37;
drawNumber();
A = 0x25;
drawChar();
```

Draw the percentage bar using the same workspace after text:

```c
CURSOR_COLUMN = 0;
CURSOR_PAGE = 2;
setCursor();
BAR_LENGTH = 3;
BAR_PERCENT = 37;
drawPercentBar();
```

Compose a tile outline then send it (new application shape configuration requires its own build):

```c
clearTile();
SHAPE_X0 = 0;
SHAPE_Y0 = 0;
SHAPE_X1 = 7;
SHAPE_Y1 = 7;
drawRectangle();
CURSOR_COLUMN = 0;
CURSOR_PAGE = 1;
setCursor();
BITMAP_POINT = CELL_DATA;
drawBitmap();
```

## Scope and preservation

The existing public functions above are frozen for v1.0.0. Individual cell/frame, combined tile shapes and text/bar configurations have historical integrated user reports; post-header-split regressions covered text and one image. The application template default has its own user validation. Do not treat every historical test as a post-split or release-copy test. Internal transport/address helpers are not stable application entry points. Library timing and API bodies are unchanged in this package.

## Single generated-image API

`OLED_USE_DRAWIMAGE=1` retains `drawImage()`, `BYTE IMAGE_TILE`, and the guarded local `bitmap_image.h` defining `static void Image_LoadTile(void)`. The converter's template-header format uses the existing IMAGE_TILE/CELL_DATA and immediate assignments; no second pixel buffer or full image RAM array. It does not need clearTile. With the request disabled, these image symbols/data are excluded. IMAGE_TILE is internal shared scratch, not an application selector.

`drawImage()` takes no arguments and clears the panel before sending all 16 row-major tiles of a fixed 32x32 image. It starts at logical (1,0) for landscape or (0,1) for portrait and supports the existing four rotations. It resets staging and active cursor to (0,0), leaves IMAGE_TILE=16, BITMAP_POINT/CELL_SOURCE referring to CELL_DATA containing the last tile, and clobbers A/flags/address/renderer scratch. It has no internal observation delay. Call sequentially after initialization/setRotation, when replacing the displayed scene; it erases earlier content and provides no transparent composition or arbitrary positioning.

In the application template, request DRAWIMAGE only and call drawImage in the marked application section. The package has no DEMO_IMAGE_MODE or other demo controls. The prepared header is the accepted pizza. Replacing the header or adding other features requires a fresh IDE fit/hardware check; see [CONFIGURATION.md](CONFIGURATION.md) and [converter usage](../converter/README.md).

## File layout

Main `.C`: feature selection, conditional font include, preserved board/timing definitions, one oled_library.h include, app_config.h and FPPA0 application setup/drawing. The library header contains existing routines/state and the guarded generated-image header. Include once after board/configuration definitions, not as a separate compiled source. There are no demo-case source files in the application template. All routines share the original state/workspace; no wrapper or timing change is introduced by this release.
