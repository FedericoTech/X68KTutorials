from . import Image
from . import os
from . import struct
from .utils import rgb_to_grb
from .utils import my_warning


def convert_image(file_path, output_dir, output_name, format, magic_pink):
    if magic_pink:
        magic_pink = tuple(map(int, magic_pink.split(',')))
        magic_pink = rgb_to_grb(magic_pink[0], magic_pink[1], magic_pink[2])

    encoded_color = []

    image = Image.open(file_path)

    match format:
        case '16bits':

            # if a magic color was set in this mode...
            if magic_pink:
                my_warning("Magic pink option is ignored at 16bits.")

            image = image.convert('RGB')
            image_data = list(image.getdata())

            # we traverse the tile data
            for (red, green, blue) in image_data:
                encoded_color.append(rgb_to_grb(red, green, blue))
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
                encoded_color.append(rgb_to_grb(palette[i], palette[i + 1], palette[i + 2]))

            aux = list(image.getdata())

            # if a magic pink is set...
            if magic_pink:
                # Check if the transparency color is in the palette
                if magic_pink in encoded_color:

                    # we capture de index of the colour
                    values_index = encoded_color.index(magic_pink)

                    # we traverse de pixels
                    for i in range(0, len(aux)):
                        # if we find the color we change the index to the first colour
                        if aux[i] == values_index:
                            aux[i] = 0
                        # if we find the first colour we set the index of the magic pink
                        elif aux[i] == 0:
                            aux[i] = values_index

                    # Remove the transparency color from its original position
                    encoded_color[values_index] = encoded_color[0]

                    # Insert the transparency color at the beginning of the list
                    encoded_color[0] = magic_pink
                else:
                    my_warning("The magic pink color is not present in the palette.")

            # we save the palette
            with open(os.path.join(output_dir, f"{output_name}.pal"), 'wb') as binary_file:
                for word in encoded_color:
                    binary_file.write(struct.pack('>H', word))

            # we re-use this array to store the pixels, 2 pixels per word
            encoded_color = []

            # we traverse the data
            for i in range(0, len(aux), 2):
                # we pack two 8bit indices (pixels) in one 16bit word
                encoded_color.append((aux[i] << 8) | aux[i + 1])

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
                encoded_color.append(rgb_to_grb(palette[i], palette[i + 1], palette[i + 2]))

            aux = list(image.getdata())

            # if a magic pink is set...
            if magic_pink:
                # Check if the transparency color is in the palette
                if magic_pink in encoded_color:

                    # we capture de index of the colour
                    values_index = encoded_color.index(magic_pink)

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
                    encoded_color[values_index] = encoded_color[0]

                    # Insert the transparency color at the beginning of the list
                    encoded_color[0] = magic_pink
                else:
                    my_warning("The magic pink color is not present in the palette.")

            # we save the palette
            with open(os.path.join(output_dir, f"{output_name}.pal"), 'wb') as binary_file:
                for word in encoded_color:
                    binary_file.write(struct.pack('>H', word))

            encoded_color = []

            # we traverse the data
            for i in range(0, len(aux) - 1, 4):
                # we pack four 4bit indices (pixels) in one 16bit word
                encoded_color.append((aux[i] << 12) | (aux[i + 1] << 8) | (aux[i + 2] << 4) | aux[i + 3])

    # we save the image
    with open(os.path.join(output_dir, f"{output_name}.pic"), 'wb') as binary_file:
        for word in encoded_color:
            binary_file.write(struct.pack('>H', word))
