
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

int8_t loadPCGs(void)
{
    int16_t file_handler, status;

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
        _dos_c_print("Can't open the file\r\n");
        return file_handler;
    }

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
        _dos_c_print("Can't load the PCG, Illegal screen mode.\r\n");
        return status;
    }

    //we load a second instance of the ship in position 1
    status = _iocs_sp_defcg(
        1,          // postition in 16 x 16 tiles
        1,          // 16 x 16 tile
        &ship_pcg   // pointer to the data
    );

    //if any problem loading the PCG...
    if(status < 0){
        _dos_c_print("Can't load the PCG of the second ship, Illegal screen mode.\r\n");
        return status;
    }

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
        _dos_c_print("Can't load the PCG of the bullet, Illegal screen mode.\r\n");
        return status;
    }

    //we load a second instance of the hardcoded bullet in a free position
    status = _iocs_sp_defcg(
        8,          // postition in 8 x 8 tiles
        0,          // 8 x 8 tile
        &bullet     // pointer to the data
    );

    //if any problem loading the PCG...
    if(status < 0){
        _dos_c_print("Can't load the PCG of the bullet, Illegal screen mode.\r\n");
        return status;
    }

    //now we close the file
    status = _dos_close(file_handler);

    //if any error...
    if(status < 0){
        _dos_c_print("Can't close the file\r\n");
        return status;
    }

    return 0;
}
