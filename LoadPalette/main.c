#include <dos.h>
#include <stdint.h>
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

#define VERTICAL_BLANKING_DETECTION 0b10000000000000000000000000000000
#define VERTICAL_BLANKING_NO_DETECT 0

#define SET_VBD_V(vbd, v) (vbd | v)

int main(void)
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
        _dos_c_print("Can't open the file\r\n");
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
                _dos_exit2(status);
            }
        }
        palette_num ++;
    }

    //now we close the file
    status = _dos_close(file_handler);

    //if any error...
    if(status < 0){
        _dos_c_print("Can't close the file\r\n");
        _dos_exit2(status);
    }

    _dos_c_print("Look into the palette window of your emulator and press a key.\r\n");

    //waiting for a keystroke.
    _dos_getchar();

    _dos_exit();
}
