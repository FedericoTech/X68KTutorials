#include "utils.h"

void loadPalette()
{
    int16_t file_handler, status;

    //we open the palette file
    file_handler = _dos_open(
        "palette.pal",
        OPENING_MODE(
            ACCESS_NORMAL,
            SHARING_COMPATIBILITY_MODE,
            MODE_R
        )
    );

    //if any error...
    if(file_handler < 0){
        _dos_c_print("Can't open the palette file\r\n");
        _dos_exit2(file_handler);
    }

    //we will collect the palettes pallete by pallete in this array.
    uint16_t colours[16];

    uint8_t palette_num = 1; //the sprite pallete count starts with 1. 0 belongs to text.

    //whereas there are palettes...
    while(_dos_read(file_handler, (char*)&colours, sizeof(colours))) {

        uint8_t colour_in_palette;
        //now we go through the current pallete
        for(colour_in_palette = 0; colour_in_palette < 16; colour_in_palette++){
            //we set the color
            status = _iocs_spalet(
                SET_VBD_V(
                    VERTICAL_BLANKING_DETECTION,
                    colour_in_palette
                ),
                palette_num,
                colours[colour_in_palette]
            );
        }
        palette_num ++;
    }

    //now we close the file
    status = _dos_close(file_handler);

    //if any error...
    if(status < 0){
        _dos_c_print("Can't close the palette file\r\n");
        _dos_exit2(status);
    }
}
