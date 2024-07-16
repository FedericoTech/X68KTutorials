#!/usr/bin/env python3

import argparse
import os
from x68assetscomp import convert_tmx, convert_image, convert_audio


def main():
    parser = argparse.ArgumentParser(description="CLI Tool for converting assets to X68000 formats")
    subparsers = parser.add_subparsers(dest='command')

    # TMX conversion
    tmx_parser = subparsers.add_parser('convert-tmx',   help='Convert TMX to tilemap, tileset, and palette')
    tmx_parser.add_argument('--input', '-i',        required=True, help='Path to the TMX file')
    tmx_parser.add_argument('--output-dir', '-o',   required=True, help='Path to the output directory')
    tmx_parser.add_argument('--output-name', '-n',  help='Custom name for the output file')
    tmx_parser.add_argument('--no-warn', '-w',      action='store_true', help='Disable warnings')
    tmx_parser.add_argument('--magic-pink', type=str, default='255,0,255',
                            help='Magic pink color in R,G,B format (default is 255,0,255).')

    # Image conversion
    img_parser = subparsers.add_parser('convert-image', help='Convert image to custom color format')
    img_parser.add_argument('--input', '-i',        required=True, help='Path to the image file')
    img_parser.add_argument('--output-dir', '-o',   required=True, help='Path to the output file')
    img_parser.add_argument('--format', '-f',       required=True, choices=['4bits', '8bits', '16bits'],
                            help='Color format to convert to')
    img_parser.add_argument('--output-name', '-n',  help='Custom name for the output file')
    img_parser.add_argument('--no-warn', '-w',      action='store_true', help='Disable warnings')

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
        output_name, ext = os.path.splitext(os.path.basename(args.input))
    else:
        output_name = args.output_name

    output_dir = os.path.expandvars(args.output_dir)

    if len(output_name) > 8 and not args.no_warn:
        print("\033[93mWarning: \033[0mThe output name is more than 8 characters long.")

    match args.command:
        case 'convert-tmx':

            transparency_color = tuple(map(int, args.magic_pink.split(',')))
            '''
            transparency_int = (
                (transparency_color[0] << 16)
                + (transparency_color[1] << 8)
                + transparency_color[2]
            )
            '''
            convert_tmx(args.input, output_dir, output_name, transparency_color)
        case 'convert-image':
            convert_image(args.input, output_dir, output_name, args.format)
        case 'convert-audio':
            convert_audio(args.input, output_dir, output_name, args.sample)
        case _:
            return "This is the default case"


if __name__ == "__main__":
    main()

'''
https://andremichelle.github.io/platforms/tools/html/tileset-extractor/tileset-extractor.html
'''