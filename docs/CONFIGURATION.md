# Configuration and application use

The complete Mini-C project is local to `application_template/`. Public switches use `=> 0;` or `=> 1;`. A zero request does not exclude a helper needed by another enabled API. Never edit derived flags. Start every recipe with all requests and OLED_KEEP_CURSOR=0, then enable only those listed.

| Use | Public requests to enable | Additional application work |
|---|---|---|
| Default 37% | DRAWNUMBER, DRAWCHAR | Existing marked code draws number then `%` |
| RAM string | DRAWSTRING | Supply a separate valid RAM byte buffer/count; glyph renderer automatic |
| Raw tile | DRAWBITMAP | Set BITMAP_POINT to eight valid RAM column bytes |
| Clear/fill cell | CLEARCELL and/or FILLCELL | Set active cursor; required bitmap helpers automatic |
| Workspace inversion | INVERTTILE | RAM-only; request DRAWBITMAP separately to display it |
| Local shapes | DRAWLINE, DRAWRECTANGLE and/or FILLRECTANGLE | clearTile, compose, then drawBitmap; helper dependencies automatic |
| Bar | LEVELBAR and/or PERCENTBAR | Set length/value and active start |
| Multi-cell frame | DRAWFRAME | Set width/height and active start |
| One generated image | DRAWIMAGE | Call drawImage; replaces whole scene |

Names abbreviate OLED_USE_* except OLED_KEEP_CURSOR. See [full API contracts](API_GUIDE.md). The template has only APP_ROTATION, no DEMO_* settings. Library routines do not create application update/timing policy. Do not enable unused APIs without calling them if you want to avoid compiler bypass warnings.

The default font enables digits, space and percent. Per-glyph literals can override groups. Use BYTE/WORD, accumulator A, `sl`/`sr`, `=>`, `#IF` and `.delay` according to the existing Mini-C code; do not replace these with assumed standard C constructs.

Logical grids: 7x4 at rotations 0/2, 4x7 at 1/3. Coordinates are cells, local shape coordinates pixels. Text APIs stop at row edge; direct drawChar requires a valid position and does not advance. The default 37% example fits both grids. Shorter/moved content needs explicit old-footprint clearing.

## One-image application

Disable text/other requests, enable OLED_USE_DRAWIMAGE only, and replace the marked application drawing code with:

```c
drawImage();
```

Keep board/init/rotation/initial clear unchanged. The prepared local bitmap_image.h defines Image_LoadTile and shares IMAGE_TILE/CELL_DATA. drawImage clears the panel, renders the fixed 32x32 image, then resets both cursor forms to (0,0). No runtime image selector, transparent overlay or second buffer. See [converter](../converter/README.md) to replace the header. The header-separated validation project's image has a prior report; this particular modified application-template image configuration requires its own IDE/memory/hardware check.

## Validation workflow

Use identical settings/rotation when comparing source revisions. Capture errors/warnings, checksum, free ROM/RAM and reserved stack, then program and check every intended orientation. Default reference: 0x0106DC, free ROM PMS150C 332 / ICE 450 words, RAM 14, stack 12. Images/feature combinations change costs. ROM capacity PMS150C 1008 words; full feature/font selection is not guaranteed to fit. One workspace does not mean scratch/stack are free. No framebuffer, pixel readback or reentrant/concurrent display use.
