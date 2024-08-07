#!/usr/bin/env python3

import argparse
import os
from utils import my_warning
from x68assetscomp import convert_tmx, convert_image, convert_audio
from x68assetscomp.image_converter import merge_images, quad_merge
from x68assetscomp.image_tile import tile_image


def main():
    parser = argparse.ArgumentParser(description="CLI Tool for converting assets to X68000 formats")
    subparsers = parser.add_subparsers(dest='command')

    # TMX conversion
    tmx_parser = subparsers.add_parser('convert-tmx',   help='Convert TMX to tilemap, tileset, and palette')
    tmx_parser.add_argument('--input', '-i',            required=True, help='Path to the TMX file')
    tmx_parser.add_argument('--output-dir', '-o',       required=True, help='Path to the output directory')
    tmx_parser.add_argument('--output-name', '-n',      help='Custom name for the output file')
    tmx_parser.add_argument('--no-warn', '-w',          action='store_true', help='Disable warnings')
    tmx_parser.add_argument('--magic-pink',             type=str, default=False,
                            help='Magic pink color in 0xRRGGBB format.')

    # Image conversion
    img_parser = subparsers.add_parser('convert-image', help='Convert image to custom color format')
    img_parser.add_argument('--input', '-i',            required=True, help='Path to the image file')
    img_parser.add_argument('--output-dir', '-o',       required=True, help='Path to the output file')
    img_parser.add_argument('--format', '-f',           required=True, choices=['4bits', '8bits', '16bits'],
                            help='Color format to convert to')
    img_parser.add_argument('--output-name', '-n',      help='Custom name for the output file')
    img_parser.add_argument('--no-warn', '-w',          action='store_true', help='Disable warnings')
    img_parser.add_argument('--magic-pink',             type=str, default=False,
                            help='Magic pink color in 0xRRGGBB format.')

    # Image merge
    merge_parser = subparsers.add_parser('merge-images',        help='Convert image to custom color format')
    merge_parser.add_argument('--input',                required=True, nargs='+', help="Input image files to merge.")
    merge_parser.add_argument('--format',               required=True, choices=['4bits', '8bits'],
                            help="Bit depth of the images.")
    merge_parser.add_argument('--output-dir',           required=True, help="Output file name for the merged image.")
    merge_parser.add_argument('--output-name', '-n',    help='Custom name for the output file')
    merge_parser.add_argument('--no-warn', '-w',        action='store_true', help='Disable warnings')
    merge_parser.add_argument('--magic-pink',           type=str, default=False,
                            help='Magic pink color in 0xRRGGBB format.')

    # Image merge
    quad_parser = subparsers.add_parser('quad-merge', help='Big image into planes or for big image display')
    quad_parser.add_argument('--input', '-i',           required=True, help='Path to the image file')
    quad_parser.add_argument('--format',                required=True, choices=['4bits', '8bits'],
                              help="Bit depth of the images.")
    quad_parser.add_argument('--output-dir',            required=True, help="Output file name for the merged image.")
    quad_parser.add_argument('--output-name', '-n',     help='Custom name for the output file')
    quad_parser.add_argument('--same-palette', '-s',    action='store_true', help='Whether all the pieces share palette')
    quad_parser.add_argument('--no-warn', '-w',         action='store_true', help='Disable warnings')
    quad_parser.add_argument('--magic-pink',            type=str, default=False,
                              help='Magic pink color in 0xRRGGBB format.')

    # Tile conversion
    tiler_parser = subparsers.add_parser('tile-image',  help='Convert image to custom color format')
    tiler_parser.add_argument('--input', '-i',          required=True, help='Path to the image file')
    tiler_parser.add_argument('--output-dir', '-o',     required=True, help='Path to the output file')
    tiler_parser.add_argument('--format', '-f',         required=True, choices=['1', '2'],
                            help='tile sizes, 1 = 8x8, 2 = 16x16 pixels')
    tiler_parser.add_argument('--output-name', '-n',    help='Custom name for the output file')
    tiler_parser.add_argument('--no-warn', '-w',        action='store_true', help='Disable warnings')
    tiler_parser.add_argument('--magic-pink',           type=str, default=False,
                            help='Magic pink color in 0xRRGGBB format.')

    # Audio conversion
    audio_parser = subparsers.add_parser('convert-audio',   help='Convert audio to custom PCM format')
    audio_parser.add_argument('--input', '-i',          required=True, help='Path to the audio file')
    audio_parser.add_argument('--output-dir', '-d',     required=True, help='Path to the output file')
    audio_parser.add_argument('--sample', '-s',         required=True, choices=['1', '2', '3', '4', '5'],
                            help='1 = 3.9Khz, 2 = 5.2Khz, 3 = 7.8Khz, 4 = 10.4Khz, 5 = 15.6Khz')
    audio_parser.add_argument('--output-name', '-n',    help='Custom name for the output file')
    audio_parser.add_argument('--no-warn', '-w',        action='store_true', help='Disable warnings')

    args = parser.parse_args()

    if not args.output_name:
        output_name, ext = os.path.splitext(os.path.basename(args.input if args.command != 'merge-images' else args.input[0]))
    else:
        output_name = args.output_name

    output_dir = os.path.expandvars(args.output_dir)

    if len(output_name) > 8 and not args.no_warn:
        my_warning("The output name is more than 8 characters long.")

    match args.command:
        case 'convert-tmx':
            convert_tmx(args.input, output_dir, output_name, args.magic_pink)
        case 'convert-image':
            convert_image(args.input, output_dir, output_name, args.format, args.magic_pink)
        case 'merge-images':
            merge_images(args.input, output_dir, output_name, args.format, args.magic_pink)
        case 'quad-merge':
            quad_merge(args.input, output_dir, output_name, args.format, args.same_palette, args.magic_pink)
        case 'tile-image':
            tile_image(args.input, output_dir, output_name, args.format, args.magic_pink)
        case 'convert-audio':
            convert_audio(args.input, output_dir, output_name, args.sample)
        case _:
            return "This is the default case"


if __name__ == "__main__":
    main()

'''
https://andremichelle.github.io/platforms/tools/html/tileset-extractor/tileset-extractor.html
'''