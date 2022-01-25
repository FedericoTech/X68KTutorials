#include "utils.h"

int32_t loadPalette()
{
    int16_t file_handler;
    int32_t status;

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
        return file_handler;
    }

    {
        //we will collect the palettes pallete by pallete in this array.
        uint16_t colours[16];

        uint8_t palette_num = 1; //the sprite pallete count starts with 1. 0 belongs to text.

        //whereas there are palettes...
        while(_dos_read(file_handler, (char*)&colours, sizeof(colours))) {

            uint8_t colour_in_palette;
            //now we go through the current pallete
            for(colour_in_palette = 0; colour_in_palette < 16; colour_in_palette++){
                //we set the color
                status = _iocs_spalet( //returns 32bit
                    SET_VBD_V(
                        VERTICAL_BLANKING_DETECTION,
                        colour_in_palette   //0 - 15 if higher it only takes from 0-15
                    ),
                    palette_num,            //1-15 or 0
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
            palette_num ++;
        }
    }

    //now we close the file
    status = _dos_close(file_handler);

    //if any error...
    if(status < 0){
        _dos_c_print("Can't close the palette file\r\n");
        return status;
    }

    return 0;
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
        _dos_c_print(getErrorMessage(file_handler));
        _dos_getchar(); //wait for keystroke
        return file_handler;
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
            0,                      // postition in 16 x 16 tiles
            SP_DEFCG_16X16_TILE,    // 16 x 16 tile = 1
            &ship_pcg               // pointer to the data
        );

        //if any problem loading the PCG...
        if(status < 0){
            _dos_c_print("Can't load the PCG, Illegal screen mode.\r\n");
            return status;
        }

        //we load a second instance of the ship in position 1
        status = _iocs_sp_defcg(
            1,                      // postition in 16 x 16 tiles
            SP_DEFCG_16X16_TILE,    // 16 x 16 tile = 1
            &ship_pcg               // pointer to the data
        );

        //if any problem loading the PCG...
        if(status < 0){
            _dos_c_print("Can't load the PCG of the second ship, Illegal screen mode.\r\n");
            return status;
        }
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
            return status;
        }
    }

    //now we close the file
    status = _dos_close(file_handler);

    //if any error...
    if(status < 0){
        _dos_c_print("Can't close the file\r\n");
        _dos_c_print(getErrorMessage(status));
        _dos_getchar(); //wait for keystroke
        return status;
    }

    return 0;
}

const char *getErrorMessage(int8_t code)
{
    char *message;

    switch(code)
    {
        case -1: message = "Executed invalid function code\r\n"; break;
        case -2: message = "Specified file not found\r\n"; break;
        case -3: message = "Specified directory not found\r\n"; break;
        case -4: message = "Too many open files\r\n"; break;
        case -5: message = "Cannot access directory or volume label\r\n"; break;
        case -6: message = "Specified handle is not open\r\n"; break;
        case -7: message = "Memory manager region was destroyed\r\n"; break;
        case -8: message = "Not enough memory to execute\r\n"; break;
        case -9: message = "Invalid memory manager pointer specified\r\n"; break;
        case -10: message = "Illegal environment specified\r\n"; break;
        case -11: message = "Abnormal executable file format\r\n"; break;
        case -12: message = "Abnormal open access mode\r\n"; break;
        case -13: message = "Error in selecting a filename\r\n"; break;
        case -14: message = "Called with invalid parameter\r\n"; break;
        case -15: message = "Error in selecting a drive\r\n"; break;
        case -16: message = "Cannot remove current directory\r\n"; break;
        case -17: message = "Cannot ioctrl device\r\n"; break;
        case -18: message = "No more files found"; break;
        case -19: message = "Cannot write to specified file\r\n"; break;
        case -20: message = "Specified directory already registered\r\n"; break;
        case -21: message = "Cannot delete because file exists\r\n"; break;
        case -22: message = "Cannot name because file exists\r\n"; break;
        case -23: message = "Cannot create file because disk is full\r\n"; break;
        case -24: message = "Cannot create file because directory is full\r\n"; break;
        case -25: message = "Cannot seek to specified location\r\n"; break;
        case -26: message = "Specified supervisor mode with supervisor status on\r\n"; break;
        case -27: message = "Thread with same name exists\r\n"; break;
        case -28: message = "Interprocess communication buffer is write-protected\r\n"; break;
        case -29: message = "Cannot start any more background processes\r\n"; break;
        case -32: message = "Not enough lock regions\r\n"; break;
        case -33: message = "Locked; cannot access\r\n"; break;
        case -34: message = "Handler for specified drive is opened\r\n"; break;
        case -35: message = "Symbolic link nest exceeded 16 steps (lndrv)\r\n"; break;
        case -80: message = "File exists\r\n"; break;
        default: message = "No error\r\n"; break;
    }

    return message;
}
