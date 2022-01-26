#include "utils.h"

/**
 * In this example we are going to demonstrate how to load a PCG from a hardcoded array into memory.
 * We first load a palette to be able to see the PCG in the emulator. That logic is taken into
 * the library as it isn't the purpose of this tutorial.
 *
 * We need to set a video mode compatible with sprite managements otherwise we won't load the PCG
 *
 * The data is stored in the same order as X68k will store it in memory.
 * We will first load the graphic of a ship that takes a tile of 16 x 16 or
 * put in another way 2 tiles of 8 x 8 for 2 tiles of 8 x 8 like in the scheme:
 *              1, 3
 *              2, 4
 * Note that the sub tiles of the tile are stored in columns instead of in rows.
 * Because the file that contains the ship is ready we don't need to deal with reordering them.
 *
 * To load the PCG into memory we make use of the function:
 *
 * int8_t _iocs_sp_defcg (int_ code, int_ size, const void *addr);
 *
 * I'll start explaining the second param called size.
 * Size refers to whether the PCG is 16 x 16 or just 8 x 8.
 * If 1, the PCG is going to be 16 x 16 and the fist param, code is the number of 16 x 16 tile.
 * X68k has 128 tiles of 16 x 16
 *
 * If size is 0 then the PCG is going to be an 8 x 8 tile there fore the param code is the number of 8 x 8 tile
 * which is a position of a forth of a 16 x 16 tile. This means that if we set the code 6 the PCG will go
 * in the third 8 x 8 of the second 16 x 16 tile. So if we don't handle this carefully we may end up overriding
 * a 16 x 16 tile previously loaded
 *
 * To demonstrate the case above this example loads the ship from the file in the 16 x 16 tiles 0 and 1
 * and a hardcoded 8 x 8 bullet that will land on top of the second instance of the ship.
 */

#define SP_DEFCG_8X8_TILE 0
#define SP_DEFCG_16X16_TILE 1

int main(void)
{
    int32_t status;

    int8_t last_mode; //in this var we will store the video that was before we ran the program
    last_mode = _iocs_crtmod(-1); //we capture the current video mode

    status = _iocs_crtmod(8); //this mode is 512 x 512 256 colours

    //if any problem...
    if(status < 0){
        _dos_c_print("Please set modes 0-19\r\n");
        _dos_exit2(status);
    }


    //we load the palette
    status = loadPalette();

    //if any problem...
    if(status < 0){
        _iocs_crtmod(last_mode); //we restore the video mode
        _dos_exit2(status);
    }

    /**
     * We are loading the PCG of the ship
     */
    {
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
            0,                      // postition in 16 x 16 tiles
            SP_DEFCG_16X16_TILE,    // 16 x 16 tile = 1
            &ship_pcg               // pointer to the data
        );

        //if any problem loading the PCG...
        if(status < 0){
            _iocs_crtmod(last_mode); //we restore the video mode
            _dos_c_print("Can't load the PCG, Illegal screen mode.\r\n");
            _dos_exit2(status);
        }

        //we load a second instance of the ship in position 1
        status = _iocs_sp_defcg(
            1,                      // postition in 16 x 16 tiles
            SP_DEFCG_16X16_TILE,    // 16 x 16 tile = 1
            &ship_pcg               // pointer to the data
        );
    }

    /**
     * We are loading the PCG of the bullet
     */
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
            6,                  // position in 8 x 8 tiles
            SP_DEFCG_8X8_TILE,  // 8 x 8 tile = 0
            &bullet             // pointer to the data
        );

        //if any problem loading the PCG...
        if(status < 0){
            _iocs_crtmod(last_mode); //we restore the video mode
            _dos_c_print("Can't load the PCG of the bullet, Illegal screen mode.\r\n");
            _dos_exit2(status);
        }

        //we load a second instance of the hardcoded bullet in a free position
        status = _iocs_sp_defcg(
            8,                  // position in 8 x 8 tiles
            SP_DEFCG_8X8_TILE,  // 8 x 8 tile = 0
            &bullet             // pointer to the data
        );

        //if any problem loading the PCG...
        if(status < 0){
            _iocs_crtmod(last_mode); //we restore the video mode
            _dos_c_print("Can't load the PCG of the bullet, Illegal screen mode.\r\n");
            _dos_exit2(status);
        }
    }

    _dos_c_print("Look into the PCG window of your emulator and press a key.\r\n");

    //waiting for a keystroke.
    _dos_getchar();

    //we restore the video mode
    _iocs_crtmod(last_mode);
    _dos_exit();
}
