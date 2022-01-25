#include "utils.h"

/**
 * In this example we are going to demonstrate how to load a PCG from a file into memory.
 * We first load a palette to be able to see the PCG in the emulator. That logic is taken into
 * the library as it isn't the purpose of this tutorial.
 *
 * We need to set a video mode compatible with sprite managements otherwise we won't load the PCG
 *
 * We then open the file ship.pcg. The data is stored in the same order as X68k will store it in memory.
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

int main(void)
{
    int16_t file_handler;
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

    //we open the palette file
    file_handler = _dos_open(
        "ship.pcg",
        OPENING_MODE(
            ACCESS_NORMAL,
            SHARING_COMPATIBILITY_MODE,
            MODE_R
        )
    );

    //if any error...
    if(file_handler < 0){
        _iocs_crtmod(last_mode);
        _dos_c_print("Can't open the file\r\n");
        _dos_exit2(file_handler);
    }

    {
        //we will collect the palettes pallete by pallete in this array.
        uint32_t ship_pcg[8 * 4];

        //we read the PCG of the ship from the file
        _dos_read(
            file_handler,       //file habdler
            (char*)&ship_pcg,   //pointer to the buffet
            sizeof(ship_pcg)    //size
        );

        //we load the ship as a 16 x 16 tile in the position 0
        status = _iocs_sp_defcg(
            0,          // postition in 16 x 16 tiles
            1,          // 16 x 16 tile
            &ship_pcg   // pointer to the data
        );

        //if any problem loading the PCG...
        if(status < 0){
            _iocs_crtmod(last_mode); //we restore the video mode
            _dos_c_print("Can't load the PCG, Illegal screen mode.\r\n");
            _dos_exit2(status);
        }

        //we load a second instance of the ship in position 1
        status = _iocs_sp_defcg(
            1,          // postition in 16 x 16 tiles
            1,          // 16 x 16 tile
            &ship_pcg   // pointer to the data
        );
    }

    //if any problem loading the PCG...
    if(status < 0){
        _iocs_crtmod(last_mode); //we restore the video mode
        _dos_c_print("Can't load the PCG of the second ship, Illegal screen mode.\r\n");
        _dos_exit2(status);
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
            6,          // postition in 8 x 8 tiles
            0,          // 8 x 8 tile
            &bullet     // pointer to the data
        );

        //if any problem loading the PCG...
        if(status < 0){
            _iocs_crtmod(last_mode); //we restore the video mode
            _dos_c_print("Can't load the PCG of the bullet, Illegal screen mode.\r\n");
            _dos_exit2(status);
        }

        //we load a second instance of the hardcoded bullet in a free position
        status = _iocs_sp_defcg(
            8,          // position in 8 x 8 tiles
            0,          // 8 x 8 tile
            &bullet     // pointer to the data
        );

        //if any problem loading the PCG...
        if(status < 0){
            _iocs_crtmod(last_mode); //we restore the video mode
            _dos_c_print("Can't load the PCG of the bullet, Illegal screen mode.\r\n");
            _dos_exit2(status);
        }
    }

    //now we close the file
    status = _dos_close(file_handler);

    //if any error...
    if(status < 0){
        _iocs_crtmod(last_mode); //we restore the video mode
        _dos_c_print("Can't close the file\r\n");
        _dos_exit2(status);
    }

    _dos_c_print("Look into the PCG window of your emulator and press a key.\r\n");

    //waiting for a keystroke.
    _dos_getchar();

    //we restore the video mode
    _iocs_crtmod(last_mode);
    _dos_exit();
}
