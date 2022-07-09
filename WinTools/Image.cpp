#include "Image.h"

Image::saveAs16BitsImage(Bmp bmp, const char *fileDest)
{
    puts(__FUNCTION__);

    std::ofstream fImage(fileDest, std::ofstream::binary);

    int32_t num_of_pixels = bmp.getNumPixels();

    uint16_t * img = (uint16_t *)bmp.getImage();

    uint16_t * buffer = (uint16_t *) malloc(sizeof(uint16_t) * num_of_pixels);

    //reverse the bytes for the bigendien
    for(int cont = 0; cont < num_of_pixels; cont++){
        buffer[cont] = REVERSE_BYTES_16(img[cont]);
    }

    fImage.write((char*)buffer, sizeof(uint16_t) *  num_of_pixels);

    fImage.close();

    free(buffer);
    //free(img);

    return 0;
} //saveAs16BitsImage

Image::saveAs8BitsImage(Bmp bmp, const char *fileDest)
{
    return 0;
} //saveAs8BitsImage

Image::saveAs4BitsImage(Bmp bmp, const char *fileDest)
{
    return 0;
} //saveAs4BitsImage
