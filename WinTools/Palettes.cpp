#include "palettes.h"

int Palettes::reformatPalette(const char *fileSource, const char *fileDest)
{
    std::fstream paleteFileIn;
    std::fstream paleteFileOut;
    paleteFileIn.open(fileSource, std::ios::in | std::ios::binary);

    //if any error...
    if(!paleteFileIn.is_open()){
        std::cout << "Can't open the file" << std::endl;
        return 1;
    }

    paleteFileOut.open(fileDest, std::ios::out | std::ios::binary);

    //if any error...
    if(!paleteFileOut.is_open()){
        std::cout << "Can't create the file" << std::endl;
        return 1;
    }

    uint32_t buffer;

    while(paleteFileIn.read((char*)&buffer, sizeof(buffer))){

        buffer = REVERSE_BYTES_32(buffer);
        uint16_t buffer16 = buffer;
        buffer16 = REVERSE_BYTES_16(buffer16);

        paleteFileOut.write((char*)&buffer16, sizeof(uint16_t));
    }

    paleteFileOut.close();
    paleteFileIn.close();

    return 0;
} //reformatPalette

void Palettes::savePalette(Bmp bmp, const char *fileDest)
{
    std::ofstream fPalette(fileDest, std::ofstream::binary);

    RGBQUAD *buffer = bmp.getPalette();

    for(int cont = 0; cont < bmp.getNumOfColours(); cont++){

        uint16_t colour = REVERSE_BYTES_16(
            RGB888_2GRB(
                buffer[cont].rgbRed,
                buffer[cont].rgbGreen,
                buffer[cont].rgbBlue,
                (
                    (
                        (
                            buffer[cont].rgbRed
                            + buffer[cont].rgbGreen
                            + buffer[cont].rgbBlue
                        ) / 3
                     ) > 127
                )
            )
        );

        fPalette.write((char*)&colour, sizeof(colour));
    }

    fPalette.close();
} //savePalette
