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

int8_t loadPCGs(void)
{
    int16_t status;

    //we will collect the palettes pallete by pallete in this array.
    uint32_t ship_pcg[8 * 4] = {
        //tile 0
        0x00000000,
        0x0000000d,
        0x0000000d,
        0x000000cb,
        0x000000cb,
        0x000000cb,
        0x003000cd,
        0x003000cd,
        //tile 1
        0x00300bcd,
        0x02320bcd,
        0x0232bbcd,
        0x0232bbcd,
        0x0232bbcd,
        0x023200cd,
        0x055500cd,
        0x00000055,
        //tile 2
        0x00000000,
        0xd0000000,
        0xd0000000,
        0xbc000000,
        0xbc000000,
        0xbc000000,
        0xdc000300,
        0xdc000300,
        //tile 3
        0xdcb00300,
        0xdcb02300,
        0xdcb02300,
        0xdcbb2300,
        0xdcbb2300,
        0xdc002300,
        0xdc005550,
        0x55000000,
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

    //we load a second instance of the ship in position 1
    status = _iocs_sp_defcg(
        1,                      // position in 16 x 16 tiles
        SP_DEFCG_16X16_TILE,    // 16 x 16 tile = 1
        &ship_pcg               // pointer to the data
    );

    //if any problem loading the PCG...
    if(status < 0){
        _dos_c_print("Can't load the PCG of the second ship, Illegal screen mode.\r\n");
        _dos_getchar(); //wait for keystroke
        return status;
    }

    {
        //PCG of bullet hardcoded
        uint32_t bullet[] = {
            //tile 0
            0x00000000,
            0x00055000,
            0x005bb500,
            0x005bb500,
            0x00055000,
            0x00055000,
            0x00055000,
            0x00000000,
        };

        //we load the hardcoded bullet on top of the second instance of the ship
        status = _iocs_sp_defcg(
            6,                  // postition in 8 x 8 tiles
            SP_DEFCG_8X8_TILE,  // 8 x 8 tile = 0
            &bullet             // pointer to the data
        );

        //if any problem loading the PCG...
        if(status < 0){
            _dos_c_print("Can't load the PCG of the bullet, Illegal screen mode.\r\n");
            _dos_getchar(); //wait for keystroke
            return status;
        }

        //we load a second instance of the hardcoded bullet in a free position
        status = _iocs_sp_defcg(
            8,                  // postition in 8 x 8 tiles
            SP_DEFCG_8X8_TILE,  // 8 x 8 tile = 0
            &bullet             // pointer to the data
        );

        //if any problem loading the PCG...
        if(status < 0){
            _dos_c_print("Can't load the PCG of the bullet, Illegal screen mode.\r\n");
            _dos_getchar(); //wait for keystroke
            return status;
        }
    }

    return 0;
}
