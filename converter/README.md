# PNG to Mini-C converter (included unchanged)

Host Python 3.10+ and Pillow are needed only for conversion. IDE builds of the prepared template need neither. Run commands from the extracted release root:

```powershell
py -3 -m pip install -r converter/requirements.txt
py -3 converter/png_to_minic.py assets/pizza_32x32.png --format template-header --loader-name Image_LoadTile --foreground white --output converted/bitmap_image.h --metadata converted/image.tiles.json --preview converted/image_preview.png
```

For black artwork on white use --foreground black. Inputs must satisfy monochrome/explicit-threshold rules and fit the fixed 32x32 canvas; smaller images are centered, not implicitly resized. Crop and threshold options are explicit; use --help for details. Transparency handling is documented by the tool arguments and errors. Animated PNG is unsupported.

Output paths must be new; the tool refuses to overwrite. Review the preview then replace application_template/bitmap_image.h with the generated header. Use `--format template-header --loader-name Image_LoadTile`: this shares IMAGE_TILE and CELL_DATA[8] instead of declaring another tile/index. Do not paste duplicate loader functions or buffers. Keep ASCII headers without a BOM.

The immediate assignment data occupies generated program code; actual ROM/RAM/stack require an IDE build. No whole-image RAM buffer is emitted. All four rotations are handled by the existing renderer, not separate image variants. A new image changes ROM size and requires fit/hardware checks.

The unchanged converter also supports legacy fragment/header outputs for older experiments; use template-header for this package. No new converter mode or function is introduced. The prepared pizza and header bytes are copied from the accepted template/library. Portable host tests are in ../tests and do not compile Mini-C.
