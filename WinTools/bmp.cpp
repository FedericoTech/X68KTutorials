
#include "bmp.h"

std::ostream &operator<<  (std::ostream &o, const BmpHeader &header)
{
    char sys[3] = {0};
    std::memcpy(&sys, header.sys, 2);

	o   <<              "header.sys "                   << sys
        << std::endl << "header.file_siz "              << header.file_size
        << std::endl << "header.reserved1 "             << header.reserved1
        << std::endl << "header.reserved2 "             << header.reserved2
        << std::endl << "header.starting "              << header.starting
        << std::endl << "header.header_size "           << header.header_size
        << std::endl << "header.witdth "                << header.witdth
        << std::endl << "header.height "                << header.height
        << std::endl << "header.color_planes "          << header.color_planes
        << std::endl << "header.bits_per_pixel "        << header.bits_per_pixel
        << std::endl << "header.compression_method "    << header.compression_method
        << std::endl << "header.image_raw_size "        << header.image_raw_size
        << std::endl << "header.horizontal_resolution " << header.horizontal_resolution
        << std::endl << "header.colours_per_pallete "   << header.colours_per_pallete
        << std::endl << "header.num_important_colours " << header.num_important_colours
        << std::endl;

	return o;
}

Bmp Bmp::fromBmpFile(const char *fileName) {

    std::fstream imageFileIn;

    //we open the image file
    imageFileIn.open(fileName, std::ios::in | std::ios::binary);

    //if any error...
    if(!imageFileIn.is_open()){
        throw "Can't open the file";
    }

    Bmp bmp;

    BmpHeader header;

    //we retrieve the header
    imageFileIn.read(reinterpret_cast<char*>(&header), sizeof(header));

    std::cout << header;

    bmp.has_palette = header.colours_per_pallete > 0;
    bmp.width = header.witdth;
    bmp.height = header.height;
    bmp.colours_per_pallete = header.colours_per_pallete;
    bmp.image_raw_size = header.image_raw_size;
    bmp.bits_per_pixel = static_cast<ColorModes>(header.bits_per_pixel);

    //if there is a palette...
    if(bmp.has_palette){

        int num_of_colours = bmp.getNumOfColours();

        //int size_palette = sizeof(RGBQUAD) * num_of_colours;
        try{
            bmp.palette = new RGBQUAD[num_of_colours];
        } catch(const std::bad_alloc& e){
            std::cout << "cannot allocate" << std::endl;
            exit(1);
        }

        std::cout << "num of colours " << num_of_colours << std::endl;

        //we initialize the array
        //std::fill_n(bmp.palette, num_of_colours, RGBQUAD{{0, 0, 0}, 0});


        for(int cont = 0; cont < num_of_colours; cont++){
            bmp.palette[cont].rgbBlue = 0;
            bmp.palette[cont].rgbGreen = 0;
            bmp.palette[cont].rgbRed = 0;
            bmp.palette[cont].rgbReserved = 0;
        }


        std::puts("before initializing the palette\n");

        int size_this_palette = sizeof(RGBQUAD) * bmp.colours_per_pallete;

        imageFileIn.seekg(header.starting - size_this_palette, imageFileIn.beg);

        std::puts("before reading");

        imageFileIn.read(reinterpret_cast<char*>(bmp.palette), size_this_palette);
/*
        for(int cont = 0; cont < num_of_colours; cont++){
            printf(
                "%d, %d, %d\n",
                bmp.palette[cont].rgbRed,
                bmp.palette[cont].rgbGreen,
                bmp.palette[cont].rgbBlue
            );
        }
*/
        std::puts("after reading the palette\n");
    }

    imageFileIn.seekg(header.starting, std::ios::beg);

    std::puts("after seekg the buffer\n");

    //uint8_t *buffer = (uint8_t*)malloc(bmp.image_raw_size);
    uint8_t *buffer = new (std::nothrow) uint8_t[bmp.image_raw_size];

    if(buffer == nullptr){
        std::puts("cannot allocate\n");
        exit(1);
    }

    std::puts("after creating the buffer\n");

    imageFileIn.read(reinterpret_cast<char*>(buffer), bmp.image_raw_size);

    switch(header.bits_per_pixel)
    {
        case 32:
            bmp.convertFrom32Bits2Native16(reinterpret_cast<RGB32 *>(buffer));
            bmp.bits_per_pixel = CM_16BIT;
            break;
        case 24:
            bmp.convertFrom24Bits2Native16(reinterpret_cast<RGB24 *>(buffer));
            bmp.bits_per_pixel = CM_16BIT;
            break;
        case 16:
            bmp.convertFrom16Bits2Native16(reinterpret_cast<uint16_t *>(buffer), false);
            bmp.bits_per_pixel = CM_16BIT;
            break;
        case 8:
            //bmp.convertFrom8Bits2Native8(buffer);
            bmp.image = buffer;
            bmp.bits_per_pixel = CM_8BIT;
            break;
        case 4:
            bmp.convertFrom4Bits2Native4(reinterpret_cast<RGB4 *>(buffer));

            //bmp.image = (uint8_t *) buffer;
            bmp.bits_per_pixel = CM_4BIT;
            break;
    }

    std::puts("before turning the image\n");

    bmp.turnImage();

    imageFileIn.close();

    return bmp;
} //fromFile

RGBQUAD *Bmp::getPalette()
{
    return palette;
} //getPalette

uint8_t * Bmp::getImage()
{
    return image;
} //getImage

uint8_t Bmp::getBitsPerPixel() const
{
    return bits_per_pixel;
} //getBitsPerPixel

uint16_t Bmp::getNumOfColours() const
{
    return pow(2, (uint16_t) bits_per_pixel);
} //getNumOfColours

uint8_t Bmp::getPaletteSize() const
{
    return sizeof(RGBQUAD) * getNumOfColours();
} //getPaletteSize

int Bmp::hasPalette() const
{
    return has_palette;
} //hasPalette

int32_t Bmp::getWidth() const
{
    return width;
} //getWidth

int32_t Bmp::getHeight() const
{
    return height;
} //getHeight

int32_t Bmp::getNumPixels() const
{
    return width * height;
} //getHeight

void Bmp::convertFrom32Bits2Native16(RGB32 *buffer)
{
    std::puts(__FUNCTION__);

    int32_t num_of_pixels = getNumPixels();

    uint16_t *img = reinterpret_cast<uint16_t *>(malloc(sizeof(uint16_t) * num_of_pixels));

    if(img == nullptr){
        std::puts("cannot allocate\n");
        exit(1);
    }

    for(int cont = 0; cont < num_of_pixels; cont++){
        img[cont] = RGB888_2GRB(
            buffer[cont].rgbRed,
            buffer[cont].rgbGreen,
            buffer[cont].rgbBlue,
            buffer[cont].workoutShadeBit()
        );
    }

    image = reinterpret_cast<uint8_t *>(img);
} //convertFrom32Bits

void Bmp::convertFrom24Bits2Native16(RGB24 *buffer)
{
    std::puts(__FUNCTION__);

    int32_t num_of_pixels = getNumPixels();

    uint16_t * img = nullptr;

    //image = (uint8_t *) malloc(sizeof(uint16_t) * num_of_pixels);

    try{
        img = new uint16_t[num_of_pixels];
    } catch(std::exception& e) {
        std::cout << e.what() << std::endl;
    }


    std::puts("before loop");

    for(int32_t cont = 0; cont < num_of_pixels; cont++){
        img[cont] = RGB888_2GRB(
            buffer[cont].rgbRed,
            buffer[cont].rgbGreen,
            buffer[cont].rgbBlue,
            buffer[cont].workoutShadeBit()
        );
    }

    image = reinterpret_cast<uint8_t *>(img);

    std::puts("after loop");

} //convertFrom24Bits



void Bmp::convertFrom16Bits2Native16(uint16_t * buffer, bool format)
{
    std::puts(__FUNCTION__);

    image = reinterpret_cast<uint8_t *>(buffer);
} //convertFrom16Bits

void Bmp::convertFrom8Bits2Native8(uint8_t * buffer)
{
    std::puts(__FUNCTION__);
/*
    int32_t num_of_pixels = getNumPixels();

    uint16_t *img = (uint16_t *)malloc(sizeof(uint16_t) * num_of_pixels);

    for(int cont = 0; cont < num_of_pixels; cont++){

        img[cont] = buffer[cont];
    }

    image = (uint8_t *) img;
    */
    image = buffer;
} //convertFrom8Bits2Native8

void Bmp::convertFrom4Bits2Native4(RGB4 * buffer)
{
    std::puts(__FUNCTION__);

    uint32_t num_of_pixels = getNumPixels();

    //RGB4 *img = (RGB4 *) malloc(num_of_pixels/2);

    RGB4 *img = new (std::nothrow) RGB4[num_of_pixels/2];

    if(img == nullptr){
        std::puts("cannot allocate\n");
        exit(1);
    }

    uint32_t real_width = image_raw_size / height;

    printf(
       "rw: %d, hw: %d, w: %d\n",
       real_width,
       width/2,
       width
    );

    int cont2 = 0;
    for(uint32_t j = 0; j < height; j++){

        for(uint32_t i = 0; i < real_width ; i++){

            if(i < (width / 2)) {
                img[cont2++] = buffer[(j * real_width) + i];
            }
        }
    }

    //delete[] image;
    image = reinterpret_cast<uint8_t *>(img);
}



void Bmp::convertFrom8To16Bits()
{
    std::puts(__FUNCTION__);

    int32_t num_of_pixels = getNumPixels();

    delete[] image;
    image = nullptr;

    //uint16_t * buffer = (uint16_t *) malloc(sizeof(uint16_t) * num_of_pixels);

    uint16_t * buffer = new (std::nothrow) uint16_t[num_of_pixels];

    if(buffer == nullptr){
        std::puts("cannot allocate\n");
        exit(1);
    }

    for(int cont = 0; cont < num_of_pixels; cont++){

        printf("%d \n", image[cont]);

        buffer[cont] = RGB888_2GRB(
            palette[image[cont]].rgbRed,
            palette[image[cont]].rgbGreen,
            palette[image[cont]].rgbBlue,
            palette[image[cont]].workoutShadeBit()
        );
    }

    image = reinterpret_cast<uint8_t *>(buffer);
} //convertFrom8Bits

void Bmp::convertFrom4To16Bits()
{
    std::puts(__FUNCTION__);

    int32_t num_of_pixels = getNumPixels() / 2;

    uint16_t * buffer = nullptr;

    //uint16_t * buffer = (uint16_t *) malloc(sizeof(uint16_t) * num_of_pixels * 2);
    try{
        buffer = new uint16_t[num_of_pixels * 2];
    } catch(const std::bad_alloc& e){
        std::cout << e.what() << std::endl;
        exit(1);
    }

    RGB4 * img = reinterpret_cast<RGB4 *>(image);

    int cont2 = 0;
    for(int cont = 0; cont < num_of_pixels; cont++){

        //printf("%d %d %d %d \n", img[cont].pixel0, img[cont].pixel1, cont, cont2);

        buffer[cont2++] = RGB888_2GRB(
            palette[img[cont].pixel1].rgbRed,
            palette[img[cont].pixel1].rgbGreen,
            palette[img[cont].pixel1].rgbBlue,
            palette[img[cont].pixel1].workoutShadeBit()
        );

        if((width % 2 == 0) ||(cont2 % width != 0)){
            buffer[cont2++] = RGB888_2GRB(
                palette[img[cont].pixel0].rgbRed,
                palette[img[cont].pixel0].rgbGreen,
                palette[img[cont].pixel0].rgbBlue,
                palette[img[cont].pixel0].workoutShadeBit()
            );
        }
    }

    delete[] image;
    image = reinterpret_cast<uint8_t *>(buffer);

} //convertFrom4Bits


void Bmp::turnImage()
{
    std::puts(__FUNCTION__);

    int32_t num_of_pixels = getNumPixels();

    switch(bits_per_pixel)
    {
        case CM_16BIT:
        {
            std::puts("turning in 16 bits\n");

            //uint16_t *buffer = (uint16_t *) malloc(sizeof(uint16_t) * num_of_pixels);
            uint16_t *buffer = nullptr;

            try{
                buffer = new uint16_t[num_of_pixels];
            } catch(const std::bad_alloc& e){
                std::cout << e.what() << std::endl;
                exit(1);
            }

            //we take the image as 16 bits per pixel
            uint16_t *img = reinterpret_cast<uint16_t *>(image);

            uint32_t k = 0;
            //this loop goes from bottom to top
            for(uint32_t i = num_of_pixels - width; i > 0; i -= width) {

                //this loop goes from one side to the other
                for(uint32_t j = 0; j < width; j++) {

                    buffer[k] = img[i+j];
                    ++k;
                }
            }

            delete[] image; //we free the previous image
            image = reinterpret_cast<uint8_t *>(buffer); //we set the current image

            break;
        }
        case CM_8BIT:
        {
            std::puts("turning in 8 bits\n");

            //char *buffer = (char *) malloc(num_of_pixels);
            char *buffer = new (std::nothrow) char[num_of_pixels];

            if(buffer == nullptr){
                std::puts("cannot allocate\n");
                exit(1);
            }

            std::puts("before turning the array");
            uint32_t k = 0;
            //this loop goes from bottom to top
            for(int i = num_of_pixels - width; i > -1; i -= width) {

                //this loop goes from one side to the other
                for(uint32_t j = 0; j < width; j++) {

                    buffer[k++] = image[i+j];
                }
            }

            delete[] image; //we free the previous image
            image = reinterpret_cast<uint8_t *>(buffer); //we set the current image

            break;

        }
        case CM_4BIT:
        {
            std::puts("turning in 4 bits\n");

            //we take the image as 4bit per pixel
            RGB4 * img = reinterpret_cast<RGB4 *>(image);

            //char *buffer = (char *) malloc(sizeof(char) * num_of_pixels);
            char *buffer = new (std::nothrow) char[num_of_pixels];

            if(buffer == nullptr){
                std::puts("cannot allocate\n");
                exit(1);
            }

            std::puts("before putting the data in the array");

            int cont2 = 0;
            for(int cont = 0; cont < num_of_pixels/2; cont++){

                buffer[cont2++] = img[cont].pixel1;
                buffer[cont2++] = img[cont].pixel0;
            }

            std::puts("before turning the array");

            //char *buffer2 = (char *) malloc(num_of_pixels);

            char *buffer2 = nullptr;

            try{
                buffer2 = new char[num_of_pixels];
            } catch(const std::bad_alloc& e){
                std::cout << e.what() << std::endl;
                exit(1);
            }

            cont2 = 0;
            //this loop goes from bottom to top
            for(int i = num_of_pixels - width; i > -1; i -= width) {

                //this loop goes from one side to the other
                for(int j = 0; j < (int) width; j++) {

                    buffer2[cont2++] = buffer[i+j];
                }
            }

            cont2 = 0;
            for(int cont = 0; cont < num_of_pixels/2; cont++){

                img[cont].pixel1 = buffer2[cont2++];
                img[cont].pixel0 = buffer2[cont2++];
            }
            delete[] buffer;
            delete[] buffer2;
            buffer = buffer2 = nullptr;
            break;

        }
    }
} //turningImage

int Bmp::convertTo16Bits()
{
    std::puts(__FUNCTION__);

    switch(bits_per_pixel){
    case CM_16BIT:
        break;
    case CM_8BIT:
        convertFrom8To16Bits();
        break;
    case CM_4BIT:
        convertFrom4To16Bits();
        break;
    }

    bits_per_pixel = CM_16BIT;
    has_palette = false;

    delete[] palette;
    palette = nullptr;

    return 0;
}

int Bmp::saveImage(const char *fileDest)
{
    std::puts(__FUNCTION__);

    std::ofstream fImage(fileDest, std::ofstream::binary);

    int32_t num_of_pixels = getNumPixels();

    switch(bits_per_pixel)
    {
        case CM_16BIT:
            {
                //we take the image as 16bit per pixel
                uint16_t * img = reinterpret_cast<uint16_t *>(image);

                //uint16_t * buffer = (uint16_t *) malloc(sizeof(uint16_t) * num_of_pixels);
                uint16_t * buffer = new (std::nothrow) uint16_t[num_of_pixels];

                if(buffer == nullptr){
                    std::puts("cannot allocate\n");
                    exit(1);
                }

                //reverse the bytes for the bigendien
                for(int cont = 0; cont < num_of_pixels; cont++){
                    buffer[cont] = REVERSE_BYTES_16(img[cont]);
                }

                fImage.write(reinterpret_cast<char*>(buffer), sizeof(uint16_t) *  num_of_pixels);

                delete[] buffer;
                buffer = nullptr;

                break;
            }
        case CM_8BIT:
            {
                fImage.write(reinterpret_cast<char*>(image), num_of_pixels);
                break;
            }
        case CM_4BIT:
            {
                fImage.write(reinterpret_cast<char*>(image), num_of_pixels / 2);
            }
            break;
    }

    fImage.close();

    return 0;
} //saveAs16BitsImage

int Bmp::saveDump(const char *fileDest)
{
    std::puts(__FUNCTION__);

    std::ofstream fImage(fileDest, std::ofstream::binary);

    int32_t num_of_pixels = getNumPixels();

    switch(bits_per_pixel)
    {
        case CM_16BIT:
            {
                std::puts("dumping in 16 bit\n");
                //we take the image as 16bit per pixel
                uint16_t * img = reinterpret_cast<uint16_t *>(image);

                //uint16_t * buffer = (uint16_t *) malloc(sizeof(uint16_t) * num_of_pixels);

                uint16_t * buffer = nullptr;

                try{
                    buffer = new uint16_t[num_of_pixels];
                } catch(const std::bad_alloc& e){
                    std::cout << e.what() << std::endl;
                    exit(1);
                }

                //reverse the bytes for the bigendien
                for(int cont = 0; cont < num_of_pixels; cont++){
                    buffer[cont] = REVERSE_BYTES_16(img[cont]);
                }

                fImage.write(reinterpret_cast<char*>(buffer), sizeof(uint16_t) *  num_of_pixels);

                delete[] buffer;
                buffer = nullptr;
                break;
            }
        case CM_8BIT:
            {
                std::puts("dumping in 8 bit\n");
                //uint16_t * buffer = (uint16_t *) malloc(sizeof(uint16_t) * num_of_pixels);
                uint16_t * buffer = new (std::nothrow)uint16_t[num_of_pixels];

                if(buffer == nullptr){
                    std::puts("cannot allocate\n");
                    exit(1);
                }

                //reverse the bytes for the bigendien
                for(int cont = 0; cont < num_of_pixels; cont++){
                    buffer[cont] = REVERSE_BYTES_16(image[cont]);
                }

                fImage.write(reinterpret_cast<char*>(buffer), sizeof(uint16_t) *  num_of_pixels);

                delete[] buffer;
                buffer = nullptr;
                break;
            }
        case CM_4BIT:
            {
                std::puts("dumping in 4 bit\n");
                //we take the image as 4bits per pixel
                RGB4 * img = reinterpret_cast<RGB4 *>(image);

                //uint16_t * buffer = (uint16_t *) malloc(sizeof(uint16_t) * num_of_pixels);

                uint16_t * buffer = nullptr;

                try {
                    buffer = new uint16_t[num_of_pixels];
                } catch(const std::bad_alloc& e){
                    std::cout << e.what() << std::endl;
                    exit(1);
                }


                //reverse the bytes for the bigendien
                for(int cont = 0, k = 0; cont < num_of_pixels / 2; cont++){
                    buffer[k++] = REVERSE_BYTES_16(img[cont].pixel1);
                    buffer[k++] = REVERSE_BYTES_16(img[cont].pixel0);
                }
                fImage.write(reinterpret_cast<char*>(buffer), sizeof(uint16_t) *  num_of_pixels);

                delete[] buffer;
                buffer = nullptr;
                break;
            }
            break;
    }

    fImage.close();

    return 0;
} //saveDump

int Bmp::saveMultiPlanes(const char *fileDest)
{
    std::puts(__FUNCTION__);

    std::ofstream fImage(fileDest, std::ofstream::binary);

    int32_t num_of_pixels = getNumPixels();

    switch(bits_per_pixel)
    {
        case CM_16BIT:
            {
                std::puts("dumping in 16 bit\n");
                //we take the image as 16bit per pixel
                uint16_t * img = reinterpret_cast<uint16_t *>(image);

                //uint16_t * buffer = (uint16_t *) malloc(sizeof(uint16_t) * num_of_pixels);

                uint16_t * buffer = nullptr;

                try{
                    buffer = new uint16_t[num_of_pixels];
                } catch(const std::bad_alloc& e){
                    std::cout << e.what() << std::endl;
                    exit(1);
                }

                //reverse the bytes for the bigendien
                for(int cont = 0; cont < num_of_pixels; cont++){
                    buffer[cont] = REVERSE_BYTES_16(img[cont]);
                }

                fImage.write(reinterpret_cast<char*>(buffer), sizeof(uint16_t) *  num_of_pixels);

                delete[] buffer;
                buffer = nullptr;
                break;
            }
        case CM_8BIT:
            {
                std::puts("dumping in 8 bit\n");
                //uint16_t * buffer = (uint16_t *) malloc(sizeof(uint16_t) * num_of_pixels);
                uint16_t * buffer = new (std::nothrow)uint16_t[num_of_pixels / 2];

                if(buffer == nullptr){
                    std::puts("cannot allocate\n");
                    exit(1);
                }

                //reverse the bytes for the bigendien
                /*
                for(int cont = 0; cont < num_of_pixels; cont++){
                    buffer[cont] = REVERSE_BYTES_16(image[cont]);
                }*/

                for(uint32_t y = 0, k = 0; y < 512; y++){ //height
                    for(uint32_t x = 0; x < 512; x++){ //width
                        buffer[k++] =  ((uint16_t) image[y * 1024 + x]) << 8                     //a quadrant
                                       | ((uint16_t) image[y * 1024 + x + 512]) << 0;              //b quadrant

                    }
                }


                fImage.write(reinterpret_cast<char*>(buffer), sizeof(uint16_t) *  num_of_pixels / 2);

                delete[] buffer;
                buffer = nullptr;
                break;
            }
        case CM_4BIT:
            {
                std::puts("dumping in 4 bit\n");
                //we take the image as 4bits per pixel
                RGB4 * img = reinterpret_cast<RGB4 *>(image);

                //uint16_t * buffer = (uint16_t *) malloc(sizeof(uint16_t) * num_of_pixels);

                uint16_t * buffer = nullptr;

                try {
                    buffer = new uint16_t[num_of_pixels / 4];
                } catch(const std::bad_alloc& e){
                    std::cout << e.what() << std::endl;
                    exit(1);
                }

                for(uint32_t y = 0, k = 0; y < 512; y++){ //height
                    for(uint32_t x = 0; x < 256; x++){ //width
                        buffer[k++] =     ((uint16_t) img[y * 512 + x].pixel1) << 8                     //a quadrant
                                        | ((uint16_t) img[y * 512 + x + 256].pixel1) << 12              //b quadrant
                                        | ((uint16_t) img[y * 512 + x + 512 * 512].pixel1)  << 0        //c quadrant
                                        | ((uint16_t) img[y * 512 + x + 512 * 512 + 256].pixel1) << 4;  //d quadrant

                        buffer[k++] =     ((uint16_t) img[y * 512 + x].pixel0) << 8                     //a quadrant
                                        | ((uint16_t) img[y * 512 + x + 256].pixel0) << 12              //b quadrant
                                        | ((uint16_t) img[y * 512 + x + 512 * 512].pixel0)  << 0        //c quadrant
                                        | ((uint16_t) img[y * 512 + x + 512 * 512 + 256].pixel0) << 4;  //d quadrant

                    }
                }

                fImage.write(reinterpret_cast<char*>(buffer), sizeof(uint16_t) *  (num_of_pixels / 4));

                delete[] buffer;
                buffer = nullptr;
                break;
            }
            break;
    }

    fImage.close();

    return 0;
} //saveMultiPlanes

int Bmp::savePalette(const char *fileDest)
{
    if(has_palette){
        std::puts(__FUNCTION__);

        //uint16_t * buffer = (uint16_t *) malloc(sizeof(uint16_t) * getNumOfColours());

        uint16_t * buffer = nullptr;

        try{
            buffer = new uint16_t[getNumOfColours()];
        } catch(const std::bad_alloc& e){
            std::cout << e.what() << std::endl;
            exit(1);
        }

        for(uint32_t cont = 0; cont < colours_per_pallete; cont++){

            buffer[cont] = REVERSE_BYTES_16(
                RGB888_2GRB(
                    palette[cont].rgbRed,
                    palette[cont].rgbGreen,
                    palette[cont].rgbBlue,
                    palette[cont].workoutShadeBit()
                )
            );
        }


        std::ofstream fPalette(fileDest, std::ofstream::binary);

        fPalette.write(reinterpret_cast<char*>(buffer), sizeof(uint16_t) * getNumOfColours());

        fPalette.close();

        delete[] buffer;
        buffer = nullptr;
        return 0;
    }
    return 1;
}

int Bmp::saveTiles(const char *fileDest)
{
    std::puts(__FUNCTION__);

    if(!has_palette //the image does not have a palete.
       || bits_per_pixel != CM_4BIT //the image is not 4 bits
       || width % 8 != 0            //the width is not multiple of 8
       || height % 8 != 0           //the height is not multiple of 8
    ){
        std::puts("file needs to be multiple of 8 or bpp not 4bit");
        return 1;
    }

    int num_of_pixels = getNumPixels();

    //uint8_t *buffer = (uint8_t *) malloc(sizeof (uint8_t) * num_of_pixels);
    uint8_t *buffer = new (std::nothrow) uint8_t[num_of_pixels];

    if(buffer == nullptr){
        std::puts("cannot allocate\n");
        exit(1);
    }

    //we convert 8 to 4 bits.
    RGB4 *img = reinterpret_cast<RGB4 *>(image);

    int cont2 = 0;
    for(int cont = 0; cont < num_of_pixels/2; cont++){
        buffer[cont2++] = img[cont].pixel1;
        buffer[cont2++] = img[cont].pixel0;
    }

    //uint8_t *buffer2 = (uint8_t *) malloc(sizeof (uint8_t) * num_of_pixels);
    uint8_t *buffer2 = new uint8_t[num_of_pixels];

    int cont7 = 0;

    for(int cont = 0; cont < (int) height; cont += 8){          //image 8 pixel row by image 8 pixel row
        for(int cont2 = 0; cont2 < (int) width; cont2 += 8){    //image 8 pixel column by image 8 pixel column
            for(int cont5 = 0; cont5 < 8; cont5++){             // tile row by tile row
                for(int cont6 = 0; cont6 < 8; cont6++){         // tole column by tile column

                    buffer2[cont7++] = buffer[
                        cont * width
                        + cont2

                        + cont5 * width
                        + cont6
                    ];
                }
            }
        }
    }

    RGB4 *exit_buffer = new (std::nothrow) RGB4[num_of_pixels / 2];

    if(exit_buffer == nullptr){
        std::puts("cannot allocate\n");
        exit(1);
    }

    cont2 = 0;
    for(int cont = 0; cont < (int)num_of_pixels/2; cont++){

        exit_buffer[cont].pixel1 = buffer2[cont2++];
        exit_buffer[cont].pixel0 = buffer2[cont2++];
    }

    std::ofstream fTiles(fileDest, std::ofstream::binary);

    fTiles.write(reinterpret_cast<char*>(exit_buffer), num_of_pixels/2);

    fTiles.close();

    delete[] buffer;
    delete[] buffer2;
    delete[] exit_buffer;
    buffer = buffer2 = nullptr;
    exit_buffer = nullptr;

    return 0;
}

int Bmp::saveSprites(const char *fileDest)
{
    std::puts(__FUNCTION__);

    if(!has_palette                     //image has not palete
       || bits_per_pixel != CM_4BIT     //image is not 4 bits
       || width % 16 != 0               //the width is not multiple of 16
       || height % 16 != 0              //the hight is not multiple of 16
    ){
        std::puts("file needs to be multiple of 16 or bpp not 4bits");
        return 1;
    }

    int num_of_pixels = getNumPixels();

    //uint8_t *buffer = (uint8_t *) malloc(sizeof (uint8_t) * num_of_pixels);

    uint8_t *buffer = nullptr;

    try{
        buffer = new uint8_t[num_of_pixels];
    } catch(const std::bad_alloc& e){
        std::cout << e.what() << std::endl;
        exit(1);
    }

    //we convert 4 to 8 bits.
    RGB4 *img = (RGB4 *) image;
    int cont2 = 0;
    for(int cont = 0; cont < num_of_pixels/2; cont++){
        buffer[cont2++] = img[cont].pixel1;
        buffer[cont2++] = img[cont].pixel0;
    }

    printf("h: %d, w: %d\n", height, width);


    //uint8_t *buffer2 = (uint8_t *) malloc(sizeof (uint8_t) * num_of_pixels);
    uint8_t *buffer2 = new (std::nothrow) uint8_t[num_of_pixels];

    if(buffer == nullptr){
        std::puts("cannot allocate\n");
        exit(1);
    }

    int cont7 = 0;

    for(int cont = 0; cont < (int) height; cont += 16){         //image 16 pixel row by image 16 pixel row
        for(int cont2 = 0; cont2 < (int) width; cont2 += 16){   //image 16 pixel column by image 16 pixel column
            for (int cont4 = 0; cont4 < 16; cont4 += 8) {       //sprite 8 pixel column by sprite 8 pixel column
                for(int cont5 = 0; cont5 < 16; cont5++){        //sprite 16 pixel row by 16 pixel row
                    for(int cont6 = 0; cont6 < 8; cont6++){     //pixel by pixel

                        buffer2[cont7++] = buffer[

                            cont * width
                            + cont2

                            + cont4
                            + cont5 * width
                            + cont6
                        ];
                    }
                }
            }
        }
    }

    //RGB4 *exit = (RGB4 *) malloc(sizeof (RGB4) * num_of_pixels / 2);
    RGB4 *exit_buffer = nullptr;
    try{
        exit_buffer = new RGB4[num_of_pixels / 2];
    } catch(const std::bad_alloc& e){
        std::cout << e.what() << std::endl;
        exit(1);
    }

    cont2 = 0;
    for(int cont = 0; cont < (int)num_of_pixels/2; cont++){

        exit_buffer[cont].pixel1 = buffer2[cont2++];
        exit_buffer[cont].pixel0 = buffer2[cont2++];
    }

    std::ofstream fTiles(fileDest, std::ofstream::binary);

    fTiles.write(reinterpret_cast<char*>(exit_buffer), num_of_pixels/2);

    fTiles.close();

    delete[] buffer;
    delete[] buffer2;
    delete[] exit_buffer;

    buffer = buffer2 = nullptr;
    exit_buffer = nullptr;

    return 0;
} //saveTiles
