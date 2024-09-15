from . import Image
from . import os
from . import struct
from .tmx_converter import extract_palete, save_tiles
from .utils import rgb_to_grb
from .utils import my_warning
from .utils import parse_magic_pink

import numpy as np


def convert_image_to_grb(image):
    image_data = np.array(image)
    rows, cols, _ = image_data.shape

    # we initialize an array with zeroes of rows x columns x uint16
    grb_data = np.zeros((rows, cols), dtype=np.uint16)
    # we traverse the rows
    for row in range(rows):
        # we traverse the columns
        for col in range(cols):
            # we extract the rgb values
            r, g, b = image_data[row, col]
            # we store the grb format
            grb_data[row, col] = rgb_to_grb(int(r), int(g), int(b))
    return grb_data


def rotate_tile(tile, angle):
    return np.rot90(tile, k=angle // 90)


def flip_tile(tile, axis):
    if axis == 'horizontal':
        return np.fliplr(tile)
    elif axis == 'vertical':
        return np.flipud(tile)
    elif axis == 'diagonal':
        return np.fliplr(np.flipud(tile))
    return tile


def tile_variants(tile):
    # We initialize the list of variants with the original tile
    variants = [(tile, 'none')]

    # Generate horizontal and vertical flips
    for axis in ['horizontal', 'vertical', 'diagonal']:
        flipped = flip_tile(tile, axis)
        variants.append((flipped, axis))

    # Including the original and flips, we get 4 versions
    return variants


def tiles_equal(tile1, tile2):
    return np.array_equal(tile1, tile2)


def process_image(filename, tile_size):
    assert tile_size == 8 or tile_size == 16, "tile size can only be either 8 or 16"

    # we open the file
    image = Image.open(filename)

    if image.mode != 'RGB':
        image = image.convert('RGB')

    width, height = image.size

    # Ensure the image dimensions are a multiple of the tile size
    assert width % tile_size == 0, "Image width is not a multiple of tile size"
    assert height % tile_size == 0, "Image height is not a multiple of tile size"

    image_data = convert_image_to_grb(image)

    # array for unique tiles
    unique_tiles = []

    # map for tiles
    tile_map = []

    # we get the width and the height
    rows, cols = image_data.shape

    # we traverse the rows with tile_size as step
    for row in range(0, rows, tile_size):
        # we traverse the columns with tile_size as step
        for col in range(0, cols, tile_size):

            # if tile size is 8x8...
            if tile_size == 8:
                # we get the (row, col) tile
                tile_post = image_data[row:row + tile_size, col:col + tile_size]

            # if tile size is 16x16...
            else:
                tile16x16 = image_data[row:row + tile_size, col:col + tile_size]

                tile_post = [tile16x16[0:16, 0:8]]
                tile_post.append(tile16x16[0:16, 8:16])
                tile_post = np.concatenate(tile_post)

            found = False

            # we traverse the array of unique tiles
            for idx, unique_tile in enumerate(unique_tiles):
                # we traverse all the 4 variants of the unique tile
                for variant, flip_type in tile_variants(unique_tile):
                    # if the tile is the same...
                    if tiles_equal(tile_post, variant):
                        tile_map.append((idx, flip_type))
                        found = True
                        break

                if found:
                    break

            if not found:
                unique_tiles.append(tile_post)
                idx = len(unique_tiles) - 1
                tile_map.append((idx, 'new'))


    # Convert unique tiles from NumPy arrays to Python lists
    unique_tiles = [tile_post.flatten().tolist() for tile_post in unique_tiles]

    return unique_tiles, tile_map


def tile_image(file_path, output_dir, output_name, format, magic_pink):
    if magic_pink:
        magic_pink = parse_magic_pink(magic_pink)

    tile_size = 8
    match format:
        case ['1', _]:  # 8x8 pixels
            tile_size = 8
        case '2':  # 16x16 pixels
            tile_size = 16

    unique_tiles, tile_map = process_image(
        file_path,
        tile_size
    )

    palette_indices = extract_palete(unique_tiles, output_dir, output_name, magic_pink)

    save_tiles(unique_tiles, palette_indices, output_dir, output_name, magic_pink)

    with (open(os.path.join(output_dir, f"{output_name}.tm"), 'wb') as binary_file):
        # we traverse the
        for tile_id, flip in tile_map: # tile_map.values():
            palette_id = palette_indices.indices[tile_id] + 1  # avoid text palette

            vertical_flip = 1 if flip == 'vertical' or flip == 'diagonal' else 0
            horizontal_flip = 1 if flip == 'horizontal' or flip == 'diagonal' else 0

            gid = (vertical_flip << 15) | (horizontal_flip << 14) | (palette_id << 8) | (tile_id + 1)

            binary_file.write(struct.pack('>H', gid))
