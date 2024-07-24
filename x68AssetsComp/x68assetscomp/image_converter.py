from . import Image
from . import os
from . import struct
from .utils import rgb_to_grb
from .utils import my_warning
from .utils import parse_magic_pink


def process_image(image, format, magic_pink):
    if magic_pink:
        magic_pink = parse_magic_pink(magic_pink)

    encoded_pixels = []
    encoded_palette = []

    match format:
        case '16bits':

            # if a magic color was set in this mode...
            if magic_pink:
                my_warning("Magic pink option is ignored at 16bits.")

            image = image.convert('RGB')
            image_data = list(image.getdata())

            # we traverse the tile data
            for (red, green, blue) in image_data:
                encoded_pixels.append(rgb_to_grb(red, green, blue))
        case '8bits':
            # if the image doesn't have a palette...
            if image.mode != 'P':
                # we turn the image into 8 bits
                image = image.convert('P', palette=Image.ADAPTIVE, colors=256)
            # if the image has a palette...
            else:
                # if the image is of 4bits
                if len(image.getpalette()) // 3 <= 16:
                    # turn into 8 bits
                    image = image.convert('RGB').convert('P', palette=Image.ADAPTIVE, colors=256)

            # we capture the palette
            palette = image.getpalette()

            # we traverse the palette
            for i in range(0, len(palette), 3):
                # and encode the colours
                encoded_palette.append(rgb_to_grb(palette[i], palette[i + 1], palette[i + 2]))

            aux = list(image.getdata())

            # if a magic pink is set...
            if magic_pink:
                # Check if the transparency color is in the palette
                if magic_pink in encoded_palette:

                    # we capture de index of the colour
                    values_index = encoded_palette.index(magic_pink)

                    # we traverse de pixels
                    for i in range(0, len(aux)):
                        # if we find the color we change the index to the first colour
                        if aux[i] == values_index:
                            aux[i] = 0
                        # if we find the first colour we set the index of the magic pink
                        elif aux[i] == 0:
                            aux[i] = values_index

                    # Remove the transparency color from its original position
                    encoded_palette[values_index] = encoded_palette[0]

                    # Insert the transparency color at the beginning of the list
                    encoded_palette[0] = magic_pink
                else:
                    my_warning("The magic pink color is not present in the palette.")

            # we re-use this array to store the pixels, 2 pixels per word

            # we traverse the data
            for i in range(0, len(aux), 2):
                # we pack two 8bit indices (pixels) in one 16bit word
                encoded_pixels.append((aux[i] << 8) | aux[i + 1])

        case '4bits':
            # if the image doesn't have a palette...
            if image.mode != 'P':
                # we turn the image into 4 bits
                image = image.convert('P', palette=Image.ADAPTIVE, colors=16)
            else:
                if len(image.getpalette()) // 3 > 16:
                    # we reprocess the image to make it have 16 colours
                    image = image.convert('RGB').convert('P', palette=Image.ADAPTIVE, colors=16)

            # we capture the palette
            palette = image.getpalette()

            # we traverse the palette
            for i in range(0, len(palette), 3):
                # and encode the colours
                encoded_palette.append(rgb_to_grb(palette[i], palette[i + 1], palette[i + 2]))

            aux = list(image.getdata())

            # if a magic pink is set...
            if magic_pink:
                # Check if the transparency color is in the palette
                if magic_pink in encoded_palette:

                    # we capture de index of the colour
                    values_index = encoded_palette.index(magic_pink)

                    # we traverse de pixels
                    for i in range(0, len(aux) - 1, 4):

                        for j in range(0, 4):
                            # print(f"[{i} + {j}], {i + j} = {aux[i + j]} == {values_index}")

                            # if we find the color we change the index to the first colour
                            if aux[i + j] == values_index:
                                aux[i + j] = 0
                            # if we find the first colour we set the index of the magic pink
                            elif aux[i + j] == 0:
                                aux[i + j] = values_index

                    # Remove the transparency color from its original position
                    encoded_palette[values_index] = encoded_palette[0]

                    # Insert the transparency color at the beginning of the list
                    encoded_palette[0] = magic_pink
                else:
                    my_warning("The magic pink color is not present in the palette.")

            # we traverse the data
            for i in range(0, len(aux) - 1, 4):
                # we pack four 4bit indices (pixels) in one 16bit word
                encoded_pixels.append((aux[i] << 12) | (aux[i + 1] << 8) | (aux[i + 2] << 4) | aux[i + 3])

    return encoded_pixels, encoded_palette


def save_palette(encoded_palette, output_dir, output_name):
    # we save the palette
    with open(os.path.join(output_dir, f"{output_name}.pal"), 'wb') as binary_file:
        for word in encoded_palette:
            binary_file.write(struct.pack('>H', word))


def save_image(encoded_pixels, output_dir, output_name):
    # we save the image
    with open(os.path.join(output_dir, f"{output_name}.pic"), 'wb') as binary_file:
        for word in encoded_pixels:
            binary_file.write(struct.pack('>H', word))


def convert_image(file_path, output_dir, output_name, format, magic_pink):
    image = Image.open(file_path)

    encoded_pixels, encoded_palette = process_image(image, format, magic_pink)

    if len(encoded_palette) > 0:
        save_palette(encoded_palette, output_dir, output_name)

    save_image(encoded_pixels, output_dir, output_name)



def compact_images(images, output_dir, output_name, format, magic_pink):

    images_pixels = []

    for index, image in enumerate(images):
        encoded_pixels, encoded_palette = process_image(image, format, magic_pink)

        print(image.filename)
        print([f"{num:04x}" for num in encoded_palette])

        save_palette(encoded_palette, output_dir, f"{output_name}_{index + 1}")

        images_pixels.append(encoded_pixels)

    ello = []

    # if the image is 8bits...
    if format == '8bits':
        for index in range(0, len(images_pixels[0])):
            # odd pixel of both images
            ello.append(
                (
                    (images_pixels[0][index] & 0xff00)
                    | images_pixels[1][index] >> 8
                ) & 0xFFFF
            )
            # even pixel of both images
            ello.append(
                (
                    images_pixels[0][index] << 8
                    | (images_pixels[1][index] & 0x00ff)
                ) & 0xFFFF
            )
    # if the image is 4bits...
    else:
        for index in range(0, len(images_pixels[0])):
            # pixel 0 of the 4 images
            ello.append(
                (images_pixels[0][index] & 0xf000)
                | (images_pixels[1][index] & 0xf000) >> 4
                | (images_pixels[2][index] & 0xf000) >> 8
                | (images_pixels[3][index] & 0xf000) >> 12
            )

            # pixel 1 of the 4 images
            ello.append(
                (
                    (images_pixels[0][index] & 0xf00) << 4
                    | (images_pixels[1][index] & 0xf00)
                    | (images_pixels[2][index] & 0xf00) >> 4
                    | (images_pixels[3][index] & 0xf00) >> 8
                ) & 0xFFFF
            )

            # pixel 2 of the 4 images
            ello.append(
                (
                    (images_pixels[0][index] & 0xf0) << 8
                    | (images_pixels[1][index] & 0xf0) << 4
                    | (images_pixels[2][index] & 0xf0)
                    | (images_pixels[3][index] & 0xf0) >> 4
                ) & 0xFFFF
            )

            # pixel 3 of the 4 images
            ello.append(
                (
                    (images_pixels[0][index] & 0xf) << 12
                    | (images_pixels[1][index] & 0xf) << 8
                    | (images_pixels[2][index] & 0xf) << 4
                    | (images_pixels[3][index] & 0xf)
                ) & 0xFFFF
            )

    save_image(ello, output_dir, output_name)

def merge_images(file_paths, output_dir, output_name, format, magic_pink):
    assert format == '4bits' or len(file_paths) == 2, "at 8bits there has to be 2 images."
    assert format == '8bits' or len(file_paths) == 4, "at 4bits there has to be 4 images max."

    images = []

    # Open the first image to get the reference dimensions
    img = Image.open(file_paths[0])
    img.filename = file_paths[0]
    reference_size = img.size
    images.append(img)

    # Check each image
    for path in file_paths[1:]:
        img = Image.open(path)

        img.filename = path

        assert img.size == reference_size, "images need to have the same dimensions"
        images.append(img)

    compact_images(images, output_dir, output_name, format, magic_pink)