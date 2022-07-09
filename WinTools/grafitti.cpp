
#include "stdafx.h"

#include "grafitti.h"



int Grafitti::doGrafitti(const char *fileSource, const char *fileGraff, const char *fileDest)
{

    typedef struct {
        BmpHeader header;
        uint8_t *data;
    } Bmp;

    std::fstream imageFileIn, grafittiFileIn;
    std::fstream imageFileOut;


    imageFileIn.open(fileSource, std::ios::in | std::ios::binary);

    //if any error...
    if(!imageFileIn.is_open()){
        std::cout << "Can't open the file" << std::endl;
        return 1;
    }

    grafittiFileIn.open(fileGraff, std::ios::in | std::ios::binary);

    //if any error...
    if(!grafittiFileIn.is_open()){
        std::cout << "Can't open the file" << std::endl;
        return 1;
    }

    imageFileOut.open(fileDest, ios::out | ios::binary);

    //if any error...
    if(!imageFileOut.is_open()){
        std::cout << "Can't create the file" << std::endl;
        return 1;
    }

    Bmp bmp;
    imageFileIn.read((char *)&bmp, sizeof(bmp));

    char sys[3];
    memcpy(&sys, &bmp.header.sys, 2);

    std::cout << "header.sys " << sys << "\0"
        << std::endl << "header.file_siz " << bmp.header.file_size
        << std::endl << "header.reserved1 " << bmp.header.reserved1
        << std::endl << "header.reserved2 " << bmp.header.reserved2
        << std::endl << "header.starting " << bmp.header.starting
        << std::endl << "header.header_size " << bmp.header.header_size
        << std::endl << "header.witdth " << bmp.header.witdth
        << std::endl << "header.height " << bmp.header.height
        << std::endl << "header.color_planes " << bmp.header.color_planes
        << std::endl << "header.bits_per_pixel " << bmp.header.bits_per_pixel
        << std::endl << "header.compression_method " << bmp.header.compression_method
        << std::endl << "header.image_raw_size " << bmp.header.image_raw_size
        << std::endl << "header.horizontal_resolution " << bmp.header.horizontal_resolution
        << std::endl << "header.colours_per_pallete " << bmp.header.colours_per_pallete
        << std::endl << "header.num_important_colours " << bmp.header.num_important_colours
        << std::endl;

    imageFileIn.seekg(bmp.header.starting, ios::beg);

    Bmp bmp_grafitti;
    grafittiFileIn.read((char *)&bmp_grafitti, sizeof(bmp_grafitti));

    std::cout << "header.sys " << sys << "\0"
        << std::endl << "header.file_siz " << bmp_grafitti.header.file_size
        << std::endl << "header.reserved1 " << bmp_grafitti.header.reserved1
        << std::endl << "header.reserved2 " << bmp_grafitti.header.reserved2
        << std::endl << "header.starting " << bmp_grafitti.header.starting
        << std::endl << "header.header_size " << bmp_grafitti.header.header_size
        << std::endl << "header.witdth " << bmp_grafitti.header.witdth
        << std::endl << "header.height " << bmp_grafitti.header.height
        << std::endl << "header.color_planes " << bmp_grafitti.header.color_planes
        << std::endl << "header.bits_per_pixel " << bmp_grafitti.header.bits_per_pixel
        << std::endl << "header.compression_method " << bmp_grafitti.header.compression_method
        << std::endl << "header.image_raw_size " << bmp_grafitti.header.image_raw_size
        << std::endl << "header.horizontal_resolution " << bmp_grafitti.header.horizontal_resolution
        << std::endl << "header.colours_per_pallete " << bmp_grafitti.header.colours_per_pallete
        << std::endl << "header.num_important_colours " << bmp_grafitti.header.num_important_colours
        << std::endl;

    #define BUFFER_SIZE 262144

    uint16_t *buffer_out, *buffer_out_aux;

    buffer_out = (uint16_t*) malloc (sizeof(uint16_t) * BUFFER_SIZE);

    buffer_out_aux = buffer_out;

    uint8_t buffer[3];

    while(imageFileIn.read((char*)&buffer, sizeof(buffer))){

        *(buffer_out_aux++) = REVERSE_BYTES_16(
            RGB888_2GRB(
                buffer[2],
                buffer[1],
                buffer[0],
                1
            )
        );
    }


    std::cout << "before reversing the buffer" << std::endl;

    uint8_t bit_buffer, bit_cont = 0;
    grafittiFileIn.seekg(bmp_grafitti.header.starting, ios::beg);
    grafittiFileIn.read((char*)&bit_buffer, sizeof(bit_buffer));

    uint16_t aux[BUFFER_SIZE];

    int k= 0;
    for(int i = BUFFER_SIZE; i > 0; i-= 512) {

        for(int j = 0; j < 512; j++) {

            if(bit_cont > 7){
                grafittiFileIn.read((char*)&bit_buffer, sizeof(bit_buffer));
                bit_cont = 0;
            }

            aux[k] = buffer_out[i + j] | (((bit_buffer >> bit_cont) & 0x01)<<13);
            bit_cont++;
            k++;
        }
    }

    imageFileOut.write((char*)aux, sizeof(aux));

    imageFileOut.close();
    grafittiFileIn.close();
    imageFileIn.close();

    return 0;
}
