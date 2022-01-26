#include "utils.h"

int32_t loadPalette()
{
    int32_t status;

    uint16_t colours[2][16] = {
        {0x0000, 0x5294, 0x0020, 0x003e, 0x0400, 0x07c0, 0x0420, 0x07fe, 0x8000, 0xf800, 0x8020, 0xf83e, 0x8400, 0xffc0, 0xad6a, 0xfffe},
        {0x0000, 0x5294, 0x0020, 0x003e, 0x0400, 0x003e, 0x0420, 0x07fe, 0x8000, 0xf800, 0x8020, 0xf83e, 0x8400, 0xffc0, 0xad6a, 0xfffe}
    };



    uint8_t colour_in_palette, palette_num;

    for(palette_num = 0; palette_num < 2; palette_num++){
        //now we go through the current palette
        for(colour_in_palette = 0; colour_in_palette < 16; colour_in_palette++){
            //we set the color
            status = _iocs_spalet( //returns 32 bit integer
                SET_VBD_V(
                    VERTICAL_BLANKING_DETECTION,
                    colour_in_palette   //0 - 15 if higher it only takes from 0-15
                ),
                palette_num + 1,
                colours[palette_num][colour_in_palette]
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
                _dos_getchar(); //wait for keystroke
                return status;
            }
        }
    }

    return 0;
}

int8_t loadPCGs(void)
{
    int16_t status;

    //we will collect the palettes pallete by pallete in this array.
    uint32_t ship_pcg[8 * 4] = {
        //tile 0
        0x00000005,
        0x00000055,
        0x00000555,
        0x00005555,
        0x00055555,
        0x00555555,
        0x05555555,
        0x55555555,
        //tile 1
        0x55555555,
        0x05555555,
        0x00555555,
        0x00055555,
        0x00005555,
        0x00000555,
        0x00000055,
        0x00000005,
        //tile 2
        0x50000000,
        0x55000000,
        0x55500000,
        0x55550000,
        0x55555000,
        0x55555500,
        0x55555550,
        0x55555555,
        //tile 3
        0x55555555,
        0x55555550,
        0x55555500,
        0x55555000,
        0x55550000,
        0x55500000,
        0x55000000,
        0x50000000,
    };

    //we load the ship as a 16 x 16 tile in the position 0
    status = _iocs_sp_defcg(
        0,                      // position in 16 x 16 tiles
        SP_DEFCG_16X16_TILE,    // 16 x 16 tile = 1
        &ship_pcg               // pointer to the data
    );

    //if any problem loading the PCG...
    if(status < 0){
        _dos_c_print("Can't load the PCG, Illegal screen mode.\r\n");
        _dos_getchar(); //wait for keystroke
        return status;
    }
    return 0;
}
