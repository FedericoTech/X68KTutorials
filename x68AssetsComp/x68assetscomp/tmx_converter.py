from collections import namedtuple
from . import os
from . import struct
from . import Image
from .utils import rgb_to_grb
from .utils import parse_magic_pink

import xml.etree.ElementTree as ET

Palettes = namedtuple('Palettes', ['palettes', 'indices'])


def extract_palete(tiles, output_dir, output_name, magic_pink):
    max = 15 if magic_pink else 16

    palettes = []
    indices = []

    # for tile_index, tile in enumerate(sorted(tiles, key=lambda tile: (len(tile), sorted(tile)))):
    for tile_index, tile in enumerate(tiles):
        # print(len(tile), tile)
        # we capture the current matrix
        new_palette = set(tile)
        # new_palette.add(0)  # we add the transparent colour
        new_palette = set(sorted(list(new_palette)))

        # of groups is empty
        if len(palettes) == 0:
            # we just insert the matrix
            palettes.append(new_palette)
            indices.append(0)
            continue

        # otherwise we check whether this matrix is in any group
        for palette_index, palette in enumerate(palettes):

            # we try to merge the current matrix with the current group
            aux = palette.union(new_palette)

            # if the matrix fits in the current group fits...
            if len(aux) <= max:
                # we update the group with the current matrix
                palette.update(new_palette)
                # we keep the group number for this matrix
                indices.append(palette_index)
                break

            # if the current group is the last in the array...
            if palette_index == len(palettes) - 1:
                # we add the current_matrix as a new group
                palettes.append(new_palette)
                # we keep the group number for this matrix
                indices.append(palette_index + 1)
                break
            # if the current group isn't the last in the array...
            else:
                # we move to the next group
                continue

    with open(os.path.join(output_dir, f"{output_name}.pal"), 'wb') as palette_out:
        # Write the encoded palette to the palette file
        for palette in palettes:
            palette = sorted(palette)

            # if a magic pink is set...
            if magic_pink:
                # Check if the transparency color is in the palette
                if magic_pink in palette:
                    # Remove the transparency color from its original position
                    palette.remove(magic_pink)
                    # Insert the transparency color at the beginning of the list

                palette.insert(0, magic_pink)

            while len(palette) < max + 1 if magic_pink else 0:
                palette.append(0)

            for colour in palette:
                # print(f"Types - colour: {type(colour)}")
                # print(f"Values - colour: {colour}")
                palette_out.write(struct.pack('>H', colour))  # Pack as big-endian 16-bit value

    return Palettes(palettes, indices)


def save_tiles(tiles, palettes, output_dir, output_name, magic_pink):

    with (open(os.path.join(output_dir, f"{output_name}.ts"), 'wb') as binary_file):
        # Write the encoded palette to the palette file

        for index, tile in enumerate(tiles):
            palette = sorted(palettes.palettes[palettes.indices[index]])

            # if a magic pink is set...
            if magic_pink:
                # Check if the transparency color is in the palette
                if magic_pink in palette:
                    # Remove the transparency color from its original position
                    palette.remove(magic_pink)
                    # Insert the transparency color at the beginning of the list

                palette.insert(0, magic_pink)

            # print(palette)
            for i in range(0, len(tile), 2):
                positions = {palette: ind for ind, palette in enumerate(palette)}
                byte = 0xff & (
                    (positions.get(tile[i]) << 4)
                    | (0xf & positions.get(tile[i + 1]))
                )

                binary_file.write(struct.pack('B', byte))


def process_image(filename, tile_size):
    image = Image.open(filename).convert('RGB')

    width, height = image.size

    # Ensure the image dimensions are a multiple of the tile size
    assert width % tile_size == 0, "Image width is not a multiple of tile size"
    assert height % tile_size == 0, "Image height is not a multiple of tile size"

    tile_post = []
    # Loop through the image rows
    for y in range(0, height, tile_size):
        # Loop through the image cols
        for x in range(0, width, tile_size):

            # if tile size is 8x8...
            if tile_size == 8:
                # Extract the tile
                tile = image.crop((x, y, x + tile_size, y + tile_size))
                # Get the tile data as a list of indices
                tile_data = list(tile.getdata())

                encoded_color = []
                # we traverse the tile data
                for (red, green, blue) in tile_data:
                    encoded_color.append(rgb_to_grb(red, green, blue))

            # if tile size is 16x16...
            else:
                tile16x16 = image.crop((x, y, x + tile_size, y + tile_size))

                encoded_color = []

                # inside the 16 x 16 block we go through 2 columns of 8 x 16 pixels
                for j in range(0, 16, 8):
                    # we crop the 8 x 16 column
                    tile = tile16x16.crop((j, 0, j + 8, 16))
                    # Get the tile data as a list of indices
                    tile_data = list(tile.getdata())

                    # we traverse the tile data
                    for (red, green, blue) in tile_data:
                        encoded_color.append(rgb_to_grb(red, green, blue))

            tile_post.append(encoded_color)

    return tile_post


# tiled functions
def convert_tmx(file_path, output_directory, output_name, magic_pink):

    if magic_pink:
        magic_pink = parse_magic_pink(magic_pink)

    # Define the namespace URI
    namespace = {'xhtml': 'http://www.w3.org/1999/xhtml'}

    FLIP_HORIZONTAL_FLAG = 0x80000000
    FLIP_VERTICAL_FLAG = 0x40000000
    # FLIP_DIAGONAL_FLAG = 0x20000000

    # Parse the XML file
    tree = ET.parse(file_path)

    # map element
    root = tree.getroot()

    # we get the tileset filename
    tileset_element = root.find('xhtml:tileset', namespace)

    tile_width = int(tileset_element.get('tilewidth'))
    tile_height = int(tileset_element.get('tileheight'))

    assert tile_width == tile_height, "Tiles are to be square"
    assert tile_width % 8 == 0 or tile_width % 16 == 0, "Tiles are to be either 8x8 or 16x16 pixels"

    image_element = tileset_element.find('xhtml:image', namespace)
    source_filename = image_element.get('source')

    tileset_file = os.path.join(os.path.dirname(file_path), source_filename)

    tile_post = process_image(
        tileset_file,
        tile_width
    )

    palette_indices = extract_palete(tile_post, output_directory, output_name, magic_pink)

    save_tiles(tile_post, palette_indices, output_directory, output_name, magic_pink)

    # Use namespace prefix in findall and find methods
    for layer in root.findall('xhtml:layer', namespace):
        layer_data = []

        with open(os.path.join(output_directory, f"{output_name}.tm"), 'wb') as binary_file:
            # we traverse the
            for tile in layer.find('xhtml:data', namespace).findall('xhtml:tile', namespace):
                gid = int(tile.get('gid'))

                tile_id = 0xFF & int(gid) - 1

                palette_id = palette_indices.indices[tile_id] + 1  # avoid text palette

                vertical_flip = (gid & FLIP_VERTICAL_FLAG) != 0
                horizontal_flip = (gid & FLIP_HORIZONTAL_FLAG) != 0

                gid = (vertical_flip << 15) | (horizontal_flip << 14) | (palette_id << 8) | tile_id

                binary_file.write(struct.pack('>H', gid))
