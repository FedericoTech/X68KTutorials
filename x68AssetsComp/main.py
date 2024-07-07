#!/usr/bin/env python3

import argparse
import os
from x68assetscomp import convert_tmx, convert_image, convert_audio


def main():
    parser = argparse.ArgumentParser(description="CLI Tool for converting assets to X68000 formats")
    subparsers = parser.add_subparsers(dest='command')

    # TMX conversion
    tmx_parser = subparsers.add_parser('convert-tmx', help='Convert TMX to tilemap, tileset, and palette')
    tmx_parser.add_argument('--input', '-i', required=True, help='Path to the TMX file')
    tmx_parser.add_argument('--output', '-o', required=True, help='Path to the output directory')

    # Image conversion
    img_parser = subparsers.add_parser('convert-image', help='Convert image to custom color format')
    img_parser.add_argument('--input', '-i', required=True, help='Path to the image file')
    img_parser.add_argument('--output', '-o', required=True, help='Path to the output file')
    img_parser.add_argument('--format', '-f', required=True, choices=['4bits', '8bits', '16bits'],
                            help='Color format to convert to')

    # Audio conversion
    audio_parser = subparsers.add_parser('convert-audio', help='Convert audio to custom PCM format')
    audio_parser.add_argument('--input', '-i', required=True, help='Path to the audio file')
    audio_parser.add_argument('--output', '-o', required=True, help='Path to the output file')
    audio_parser.add_argument('--format', '-f', required=True, help='PCM format to convert to')

    args = parser.parse_args()

    match args.command:
        case 'convert-tmx':
            convert_tmx(args.input, os.path.expandvars(args.output))
        case 'convert-image':
            convert_image(args.input, os.path.expandvars(args.output), args.format)
        case 'convert-audio':
            convert_audio(args.input, os.path.expandvars(args.output))
        case _:
            return "This is the default case"


if __name__ == "__main__":
    main()

'''
https://andremichelle.github.io/platforms/tools/html/tileset-extractor/tileset-extractor.html
'''