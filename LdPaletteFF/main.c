#include "utils.h"

/**
  * In this example we are loading the palettes from the file palette.pal
  * The data is stored in binary format, in 16 bit words sequentially ordered.
  * Every palette is 32 bytes long.
  *
  * Then we make use of the function int _iocs_spalet (uint32_t code, uint8_t block, uint16_t color);
  *
  * The param code is a 32 bit word whose bit 31 is to set the Vertical Blanking Detection if 1
  * or not detection if 0
  * The bits 0 through 3 are the number of colour inside the palette which are 0 to 15
  * The macro SET_VBD_C is to ease
  *
  * The second param is the number of palette which are 1 to 15
  *
  * The third param is the color which is a 16 bit word to pick one of 65,536 colours
  */

#define VERTICAL_BLANKING_DETECTION 0x80000000     //0b10000000000000000000000000000000
#define VERTICAL_BLANKING_NO_DETECT 0

#define SET_VBD_V(vbd, v) (vbd | v)

int main(void)
{
    int8_t last_mode;
    int16_t file_handler;
    int32_t status;

    last_mode = _iocs_crtmod(-1);

    status = _iocs_crtmod(8); //this mode is 512 x 512 256 colours

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
        _dos_c_print("Can't open the file\r\n");
        _dos_c_print(getErrorMessage(file_handler));
        _iocs_crtmod(last_mode);
        _dos_exit2(file_handler);
    }

    {
        //we will collect the palettes palette by palette in this array.
        uint16_t colours[16];

        /**
         * Now are loading the sprite palette
         */

        uint8_t palette_num = 1; //the sprite palette count starts with 1. 0 belongs to text.

        //whereas there are palettes...
        while(_dos_read(file_handler, (char*)&colours, sizeof colours)) {

            uint8_t colour_in_palette;
            //now we go through the current palette
            for(colour_in_palette = 0; colour_in_palette < 16; colour_in_palette++){
                //we set the colour
                status = _iocs_spalet(                  //returns 32 bit with the code of the colour just set or if colour is -1
                    SET_VBD_V(
                        VERTICAL_BLANKING_DETECTION,    //if 0 it waits for VBlank and it's slow
                        colour_in_palette               //0 - 15 if higher it only takes from 0-15
                    ),
                    palette_num,                        //1-15 or 0
                    colours[colour_in_palette]          // colour code, -1 to retrieve the code in that cell
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
                    _dos_exit2(status);
                }
            }
            palette_num ++;
        } //while END

        _dos_c_print("Look into the palette window of your emulator and press a key.\r\n");

        /**
         * Now we load the graphics palette
         */

        //we move the pointer back to the beginning of the file
        status = _dos_seek(file_handler, 0, SEEK_MODE_BEGINNING);

        //if any error...
        if(status < 0){
            _dos_c_print("Can't move the pointer to the beginning\r\n");
            _dos_c_print(getErrorMessage(status));
            _iocs_crtmod(last_mode);
            _dos_exit2(status);
        }

        //we activate the graphic screen and display mode setting
        _iocs_g_clr_on();

        {
            uint16_t g_colour;
            uint8_t colour_in_palette = 0;

            while(status = _dos_read(file_handler, (char*)&g_colour, sizeof g_colour)){

                status = _iocs_gpalet(  //returns 32 bit with the code of the colour just set or if colour is -1
                    colour_in_palette,  //0 - 255
                    g_colour            // colour code, -1 to retrieve the code in that cell
                );

                //if any issue...
                if(status < 0){
                    _dos_c_print("The graphic screen is not initialized \r\n");
                }
                ++colour_in_palette;
            }
        }

        _dos_c_print("Look into the palette window of your emulator and press a key.\r\n");

        _dos_getchar();

        /**
         * Now we are loading the text palette
         */

        //we move the pointer back to the beginning of the file
        status = _dos_seek(file_handler, 0, SEEK_MODE_BEGINNING);

        //if any error...
        if(status < 0){
            _dos_c_print("Can't move the pointer to the beginning\r\n");
            _dos_c_print(getErrorMessage(status));
            _iocs_crtmod(last_mode);
            _dos_exit2(status);
        }

        {
            uint16_t t_colour;

            for(colour_in_palette = 0; colour_in_palette < 16; colour_in_palette++){
            {
                _dos_read(file_handler, (char*)&t_colour, sizeof t_colour);

                status =_iocs_tpalet(
                     colour_in_palette, //0, 1, 2 and 3 are independent colours, 4 -7 and 8 - 15 share the same colours.
                     t_colour
                );

                //if any error...
                if(status < 0){
                    _dos_c_print("Cant set the colour\r\n");
                }
            }
        }
    }

    //now we close the file
    status = _dos_close(file_handler);

    //if any error...
    if(status < 0){
        _dos_c_print("Can't close the file\r\n");
        _dos_c_print(getErrorMessage(status));
        _iocs_crtmod(last_mode);
        _dos_exit2(status);
    }

    _dos_c_print("Look into the palette window of your emulator and press a key.\r\n");

    //waiting for a keystroke.
    _dos_getchar();

    /**
     * Restoring the text palette
     */
    {
         uint8_t colour_in_palette;
        //now we go through the current pallete
        for(colour_in_palette = 0; colour_in_palette < 4; colour_in_palette++){
            status =_iocs_tpalet(
                colour_in_palette, //0, 1, 2 and 3 are independent colours, 4 -7 and 8 - 15 share the same colours.
                -2  //returning the factory color
            );
        }

        status =_iocs_tpalet(
            4, //0, 1, 2 and 3 are independent colours, 4 -7 and 8 - 15 share the same colours.
            -2  //returning the factory color
        );

        status =_iocs_tpalet(
            8, //0, 1, 2 and 3 are independent colours, 4 -7 and 8 - 15 share the same colours.
            -2  //returning the factory color
        );
    }

    status = _iocs_crtmod(last_mode);

    _dos_exit();
}
