#include "utils.h"

int32_t loadPalette()
{
    int32_t status;

    uint16_t colours[16] = {0x0000, 0x5294, 0x0020, 0x003e, 0x0400, 0x07c0, 0x0420, 0x07fe, 0x8000, 0xf800, 0x8020, 0xf83e, 0x8400, 0xffc0, 0xad6a, 0xfffe};

    uint8_t colour_in_palette;

    //now we go through the current palette
    for(colour_in_palette = 0; colour_in_palette < 16; colour_in_palette++){
        //we set the color
        status = _iocs_spalet( //returns 32 bit integer
            SET_VBD_V(
                VERTICAL_BLANKING_DETECTION,
                colour_in_palette   //0 - 15 if higher it only takes from 0-15
            ),
            1,  //first sprite palette
            colours[colour_in_palette]
        );

        //if any issue...
        if(status < 0){
            switch(status){
            case -1:
                _dos_c_print("Incorrect screen mode\r\n");
                break;
            case -2:
                _dos_c_print("attempting to set palette in block 0\r\n");
                break;
            }
            return status;
        }
    }

    return 0;
}
