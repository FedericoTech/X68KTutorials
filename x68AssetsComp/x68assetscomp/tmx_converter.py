from collections import namedtuple
from . import os
from . import struct
from . import Image
from .utils import rgb_to_grb
import xml.etree.ElementTree as ET

Palettes = namedtuple('Palettes', ['palettes', 'indices'])


def extract_palete(tiles, output_directory, filename):
    max = 15

    palettes = []
    indices = []

    # for tile_index, tile in enumerate(sorted(tiles, key=lambda tile: (len(tile), sorted(tile)))):
    for tile_index, tile in enumerate(tiles):

        # we capture the current matrix
        new_palette = set(tile)
        new_palette.add(0)  # we add the black colour
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

    # Split the filename into base and extension
    base, ext = os.path.splitext(os.path.basename(filename))

    lolo = os.path.join(output_directory, f"{base}.pal")


    with open(lolo, 'wb') as palette_out:
        # Write the encoded palette to the palette file
        for palette in palettes:
            palette = sorted(palette)
            while len(palette) < max + 1:
                palette.append(0)

            for colour in palette:
                palette_out.write(struct.pack('>H', colour))  # Pack as big-endian 16-bit value

    return Palettes(palettes, indices)


def save_tiles(tiles, palettes, output_directory, filename):
    # Split the filename into base and extension

    base, ext = os.path.splitext(os.path.basename(filename))
    lolo = os.path.join(output_directory, f"{base}.ts")

    with open(lolo, 'wb') as binary_file:
        # Write the encoded palette to the palette file

        for index, tile in enumerate(tiles):
            palette = sorted(palettes.palettes[palettes.indices[index]])
            # print(palette)
            for i in range(0, len(tile), 2):
                positions = {palette: ind for ind, palette in enumerate(palette)}
                byte = 0xff & ((positions.get(tile[i]) << 4)
                               | (0xf & positions.get(tile[i + 1])))

                binary_file.write(struct.pack('B', byte))


def process_image(filename, tilewidth, tileheight, output_directory):
    image = Image.open(filename).convert('RGB')

    width, height = image.size

    # Ensure the image dimensions are a multiple of the tile size
    assert width % tilewidth == 0, "Image width is not a multiple of tile size"
    assert height % tileheight == 0, "Image height is not a multiple of tile size"

    tile_post = []
    # Loop through the image and extract 8x8 tiles
    for y in range(0, height, tileheight):
        for x in range(0, width, tilewidth):
            # Extract the tile
            tile = image.crop((x, y, x + tilewidth, y + tileheight))
            # Get the tile data as a list of indices
            tile_data = list(tile.getdata())

            # print(tile_data)

            encoded_color = []
            # we traverse the tile data
            for (red, green, blue) in tile_data:
                encoded_color.append(rgb_to_grb(red, green, blue))

            tile_post.append(encoded_color)

    palettes = extract_palete(tile_post, output_directory, filename)

    save_tiles(tile_post, palettes, output_directory, filename)

    return palettes


# tiled functions
def convert_tmx(file_path, output_directory):
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
    tileset_element = root.find('tileset')
    tilewidth = int(tileset_element.get('tilewidth'))
    tileheight = int(tileset_element.get('tileheight'))

    image_element = tileset_element.find('image')
    filename = image_element.get('source')

    print(os.path.join(os.path.dirname(file_path), filename))

    palette_indices = process_image(os.path.join(os.path.dirname(file_path), filename), tilewidth, tileheight, output_directory)

    # Use namespace prefix in findall and find methods
    for layer in root.findall('layer'):
        layer_data = []

        base, ext = os.path.splitext(os.path.basename(filename))
        lolo = os.path.join(output_directory, f"{base}.tm")

        with open(lolo, 'wb') as binary_file:
            # we traverse the
            for tile in layer.find('data').findall('tile'):
                gid = int(tile.get('gid'))

                tile_id = 0xFF & int(gid) - 1

                palette_id = palette_indices.indices[tile_id] + 1  # avoid text palette

                vertical_flip = (gid & FLIP_VERTICAL_FLAG) != 0
                horizontal_flip = (gid & FLIP_HORIZONTAL_FLAG) != 0

                gid = (vertical_flip << 15) | (horizontal_flip << 14) | (palette_id << 8) | tile_id

                binary_file.write(struct.pack('>H', gid))
