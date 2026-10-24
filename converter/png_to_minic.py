"""Convert PNG to 32x32 logo-experiment Mini-C assignments. Python 3.10+, Pillow."""
import argparse
import json
import re
from pathlib import Path
from PIL import Image, ImageOps


def prepare_image(image, foreground="white", crop=None, threshold=None):
    if foreground not in ("white", "black"):
        raise ValueError("foreground must be white or black")
    image = image.convert("RGBA")
    if crop is not None:
        left, top, width, height = crop
        if (left < 0 or top < 0 or width <= 0 or height <= 0
                or left + width > image.width or top + height > image.height):
            raise ValueError("crop must lie entirely inside input")
        image = image.crop((left, top, left + width, top + height))
    if image.width > 32 or image.height > 32:
        raise ValueError("image must fit 32x32; resize externally or explicitly --crop")
    if threshold is None:
        rgba = image.tobytes()
        for offset in range(0, len(rgba), 4):
            red, green, blue, alpha = rgba[offset:offset + 4]
            if alpha == 0:
                continue
            if alpha != 255 or (red, green, blue) not in ((0, 0, 0), (255, 255, 255)):
                raise ValueError("input is not monochrome; supply --threshold explicitly")
    elif not 0 <= threshold <= 255:
        raise ValueError("threshold must be 0..255")
    background = "black" if foreground == "white" else "white"
    gray = ImageOps.grayscale(Image.alpha_composite(Image.new("RGBA", image.size, background), image))
    cutoff = 128 if threshold is None else threshold
    if foreground == "white":
        mask = gray.point(lambda value: 255 if value >= cutoff else 0)
    else:
        mask = gray.point(lambda value: 255 if value < cutoff else 0)
    mask.paste(0, mask=image.getchannel("A").point(lambda alpha: 255 if alpha == 0 else 0))
    canvas = Image.new("L", (32, 32), 0)
    canvas.paste(mask, ((32 - mask.width) // 2, (32 - mask.height) // 2))
    return canvas


def encode_tiles(image):
    if image.size != (32, 32):
        raise ValueError("tile encoder requires a 32x32 canvas")
    pixels = image.load()
    return [[sum((1 << y) if pixels[tx * 8 + x, ty * 8 + y] else 0
                 for y in range(8)) for x in range(8)]
            for ty in range(4) for tx in range(4)]


def render_loader(tiles):
    if len(tiles) != 16 or any(len(tile) != 8 for tile in tiles):
        raise ValueError("expected sixteen eight-byte tiles")
    lines = [
        "// Logo pixels are encoded as Mini-C immediate assignments, like the font.",
        "// Only one eight-byte tile is present in RAM. LOGO_TILE is row-major 0..15.",
        "BYTE LOGO_TILE;", "static void Logo_LoadTile (void)", "{",
    ]
    lines += [f"\tCELL_DATA[{i}] = 0x00;" for i in range(8)]
    lines += ["\tswitch (LOGO_TILE)", "\t{"]
    for index, tile in enumerate(tiles):
        lines.append(f"\t\tcase {index} :")
        for column, value in enumerate(tile):
            if value:
                lines.append(f"\t\t\tCELL_DATA[{column}] = 0x{value:02X};")
        lines.append("\t\t\tbreak;")
    lines += ["\t}", "}", ""]
    return "\r\n".join(lines)



def render_header(tiles):
    return ("// Mini-C image header: include once after CELL_DATA is declared.\r\n"
            "// Contains LOGO_TILE and Logo_LoadTile; do not also paste a fragment.\r\n"
            + render_loader(tiles))


def validate_loader_name(name):
    reserved = {"a", "byte", "word", "dword", "eword", "bit", "void", "static",
                "const", "near", "far", "if", "else", "switch", "case", "default",
                "while", "do", "for", "break", "continue", "return", "enum",
                "cell_data", "image_tile", "image_id", "drawimage", "drawbitmap",
                "oled_init", "oled_writecell", "fppa0", "setrotation", "setcursor"}
    if not isinstance(name, str) or not re.fullmatch(r"[A-Za-z_][A-Za-z0-9_]*", name):
        raise ValueError("loader name must be an ASCII identifier, e.g. Pizza_LoadTile")
    if name.lower() in reserved:
        raise ValueError("loader name conflicts with a keyword or template symbol")
    return name


def render_template_header(tiles, loader_name):
    validate_loader_name(loader_name)
    return (render_loader(tiles)
            .replace("BYTE LOGO_TILE;\r\n", "")
            .replace("LOGO_TILE", "IMAGE_TILE")
            .replace("static void Logo_LoadTile (void)", f"static void {loader_name} (void)"))

def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("input", type=Path)
    parser.add_argument("--output", required=True, type=Path, help="new Mini-C fragment or local header")
    parser.add_argument("--format", choices=("fragment", "header", "template-header"), default="fragment",
                        help="fragment/header retain single-image names; template-header shares IMAGE_TILE")
    parser.add_argument("--loader-name", help="required for template-header, e.g. Pizza_LoadTile")
    parser.add_argument("--foreground", choices=("white", "black"), default="white")
    parser.add_argument("--crop", nargs=4, type=int, metavar=("LEFT", "TOP", "WIDTH", "HEIGHT"))
    parser.add_argument("--threshold", type=int, help="explicit grayscale/color/alpha cutoff 0..255")
    parser.add_argument("--metadata", type=Path, help="optional tile JSON output")
    parser.add_argument("--preview", type=Path, help="optional 32x32 white-on-black PNG")
    args = parser.parse_args(argv)
    if args.format == "template-header":
        try:
            validate_loader_name(args.loader_name)
        except ValueError as error:
            parser.error(str(error))
    elif args.loader_name is not None:
        parser.error("--loader-name requires --format template-header")
    outputs = [path.resolve() for path in (args.output, args.metadata, args.preview) if path]
    if len(set(outputs)) != len(outputs) or args.input.resolve() in outputs:
        parser.error("input and output paths must be distinct")
    for path in outputs:
        if path.exists():
            parser.error(f"refusing to overwrite existing file: {path}")
    try:
        with Image.open(args.input) as image:
            if image.format != "PNG":
                raise ValueError("input must be a PNG")
            if getattr(image, "n_frames", 1) != 1:
                raise ValueError("animated PNG is unsupported")
            canvas = prepare_image(image, args.foreground, args.crop, args.threshold)
        tiles = encode_tiles(canvas)
        if args.format == "template-header":
            rendered = render_template_header(tiles, args.loader_name)
        elif args.format == "header":
            rendered = render_header(tiles)
        else:
            rendered = render_loader(tiles)
        for path in outputs:
            path.parent.mkdir(parents=True, exist_ok=True)
        with args.output.open("x", encoding="ascii", newline="") as output:
            output.write(rendered)
        if args.metadata:
            with args.metadata.open("x", encoding="utf-8") as output:
                json.dump({"source": args.input.name, "canvas": [32, 32],
                           "format": args.format, "foreground": args.foreground, "crop": args.crop,
                           "threshold": args.threshold,
                           **({"loader_name": args.loader_name, "tile_index": "IMAGE_TILE",
                               "declares_tile_index": False} if args.format == "template-header" else {}),
                           "order": "row-major 4x4 tiles; 8 columns per tile, bit 0 top",
                           "tiles": tiles}, output, indent=2)
                output.write("\n")
        if args.preview:
            with args.preview.open("xb") as output:
                canvas.save(output, format="PNG")
    except (OSError, ValueError) as error:
        parser.exit(2, f"error: {error}\n")
    print(f"Wrote {args.output}: 16 tiles, 128 pixel bytes encoded as Mini-C assignments.")
    tile_index = "IMAGE_TILE" if args.format == "template-header" else "LOGO_TILE"
    print(f"Existing CELL_DATA[8] and {tile_index} reused; actual ROM/RAM requires an IDE build.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
