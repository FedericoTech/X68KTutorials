from . import Image
from . import os
from . import struct
from .utils import rgb_to_grb


def convert_image(filename, output, output_name, format):

    encoded_color = []

    image = Image.open(filename)

    match format:
        case '16bits':
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
            else:
                if len(image.getpalette()) // 3 <= 16:
                    image = image.convert('RGB').convert('P', palette=Image.ADAPTIVE, colors=256)

            # we capture the palette
            palette = image.getpalette()

            # we traverse the palette
            for i in range(0, len(palette), 3):
                # and encode the colours
                encoded_color.append(rgb_to_grb(palette[i], palette[i + 1], palette[i + 2]))

            # we save the palette
            with open(os.path.join(output, f"{output_name}.pal"), 'wb') as binary_file:
                for word in encoded_color:
                    binary_file.write(struct.pack('>H', word))

            encoded_color = []

            aux = list(image.getdata())

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

            # we save the palette
            with open(os.path.join(output, f"{output_name}.pal"), 'wb') as binary_file:
                for word in encoded_color:
                    binary_file.write(struct.pack('>H', word))

            encoded_color = []

            aux = list(image.getdata())

            # we traverse the data
            for i in range(0, len(aux) - 1, 4):
                # we pack four 4bit indices (pixels) in one 16bit word
                encoded_color.append((aux[i] << 12) | (aux[i + 1] << 8) | (aux[i + 2] << 4) | aux[i + 3])

    # we save the image
    with open(os.path.join(output, f"{output_name}.pic"), 'wb') as binary_file:
        for word in encoded_color:
            binary_file.write(struct.pack('>H', word))
