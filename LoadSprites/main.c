#include "utils.h"

/**
 * In this example we are going to demonstrate how to set a sprite
 *
 * The logic for that is in the library as it isn't the topic of this tutorial.
 *
 * Then we set a compatible screen mode. In this example we will use 512 x 512 256 colours.
 *
 * we clear the screen with void _iocs_g_clr_on (void) which also resets the palette.
 * we load the palette, initialize the sprite engine we load the PCGs and
 * set the sprite engine on.
 *
 * Now we set up a sprite with the function:
 *
 * int_ _iocs_sp_regst (int_ spno, int_ mode, int_ x, int_ y, int_ code, int_ prw)
 *
 * The first param, spno is the sprite number. X68k has 128 sprites.
 * The second param, mode is for whether we want Vertical blanking interval detection post-setting or not
 * The third and forth params x and y are the screen coordinates we want to put the sprite on.
 * The fifth param, code is a 16 bit word of flags which also contains the PCG number
 * the bit 15 is the flag for whether we want to make a vertical flip.
 * the bit 12 is the flag for whether we want to make an horizontal flip.
 * the bits 8 to 11 are the number of palette which is between 0 to 15. (palette 0 is for text but it can be used in sprites too)
 * the bits from 0 through 7 is the PCG number which is between 0 to 127.
 * The sixth field, prw is the priority. 0 is its value by default and means hidden.
 * 1 is in the level of BG0, 2 in the level of BG1 and 3 in the sprites level.
 *
 * To change the position of the sprite in the screen or the vertical and horizontal flips, or any other change
 * you just need to keep calling this _iocs_sp_regst function to refresh then.
 */

#define SPRITE_VF_ON    0b1000000000000000
#define SPRITE_VF_OFF   0x0

#define SPRITE_HF_ON    0b0001000000000000
#define SPRITE_HF_OFF   0x0

#define SPRITE_OFF 0
#define SPRITE_PRI_BG0 1
#define SPRITE_PRI_BG1 2
#define SPRITE_PRI_SP 3

#define SETUP_SP(VF,HF, PAL, PT) (VF | HF | (PAL << 8) | PT)

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


    //we clear the screen
    _iocs_g_clr_on();

    //we deactivate the console cursor
    _iocs_b_curoff();

    //we initialise the sprites
    _iocs_sp_init();

    status = loadPalette();

    //if any problem...
    if(status < 0){
        _iocs_crtmod(last_mode); //we restore the video mode
        _dos_exit2(status);
    }

    //we load the PCGs
    status = loadPCGs();

    //if any error...
    if(status < 0){
        _iocs_crtmod(last_mode); //we restore the video mode
        _dos_exit2(status);
    }

    //we activate the sprites
    _iocs_sp_on();

    //we centre the sprite in the middle of the screen.
    int16_t x = 256, y = 256;

    //we set up the sprite.
    status = _iocs_sp_regst(
        0,                              //int_ spno sprite number (0-127)
        VERTICAL_BLANKING_DETECTION,    //int_ mode bit 31 0: Vertical blanking interval detection post-setting 1: Not detected
        x,                              //int_ x X coordinates (0-1023 16 displayed on the far left
        y,                              //int_ y Y " (" " Top ")
        SETUP_SP(
            SPRITE_VF_ON,               //we can flip the sprite vertically
            SPRITE_HF_OFF,              //we can flip the sprite horizontally
            1,                          //palette 1
            0                           //PCG 0
        ),                              //code pattern code
        SPRITE_PRI_SP                   //int_ prw priority
    );

    //if any error...
    if(status < 0){
        _dos_c_print("Couldn't setup the sprite, Illegal screen mode.\r\n");
        _dos_exit2(status);
    }

    _dos_c_print("Press a key.\r\n");

    //waiting for a keystroke.
    _dos_getchar();


    //we deactivate the sprites
    _iocs_sp_off();

    //we activate the console cursor
    _iocs_b_curon();

    //we restore the video mode
    _iocs_crtmod(last_mode);
    _dos_exit();
}
