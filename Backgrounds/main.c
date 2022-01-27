#include "utils.h"
#include <signal.h>
#define SP_DEFCG_8X8_TILE 0
#define SP_DEFCG_16X16_TILE 1

volatile uint16_t x1 = 0;
volatile uint16_t y1 = 0;

volatile uint16_t x2 = 0;
volatile uint16_t y2 = 0;

volatile int8_t last_mode;


void interrupt vsync_disp()
{
    x1+=10;
    y1+=10;

    x2 = (x2 & 0x3F) - 1;
    y2 = (y2 & 0x3F) - 1;

    _iocs_bgscrlst(
        (1 << 31) | 1, //(1 << 31) | 0, // and Background specification (0/1)
        x1/10, //x
        y1/10  //y
    );

    _iocs_bgscrlst(
        (1 << 31) | 0, //(1 << 31) | 0, // and Background specification (0/1)
        x2/10, //x
        y2/10  //y
    );
}

void terminate()			/* Processing routine when pressed CTRL-C */
{
	_iocs_vdispst(  /* Un interrupt */
        (void *)0,
        0,//0: vertical blanking interval 1: vertical display period
        0
    );

    //we deactivate the sprites
    _iocs_sp_off();

    //we activate the console cursor
    _iocs_b_curon();


    //we restore the video mode
    _iocs_crtmod(last_mode);
    _dos_exit();
}

int main(void)
{
    void interrupt vsync_disp();

    int32_t status;

     //in this var we will store the video that was before we ran the program
    last_mode = _iocs_crtmod(-1); //we capture the current video mode

    //status = _iocs_crtmod(2); //this mode is 512 x 512 16 colours

    //status = _iocs_crtmod(6); //this mode is 256 x 256 16 colours

    //status = _iocs_crtmod(8); //this mode is 512 x 512 256 colours, it doesn't work here

    status = _iocs_crtmod(10); //this mode is 256 x 256 256 colours

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

    //we load the palette
    status = loadPalette();

    //if any problem...
    if(status < 0){
        _iocs_crtmod(last_mode); //we restore the video mode
        _dos_exit2(status);
    }

    //we load the palette
    status = loadPCGs();

    //if any problem...
    if(status < 0){
        _iocs_crtmod(last_mode); //we restore the video mode
        _dos_exit2(status);
    }

    //we activate the sprites
    _iocs_sp_on();

    /*
     * here I'm setting the scroll 1 to work with the tilemap 0 and to
     * make it shown.
     * In this example I demonstrate that the tilemap can be either
     */
    status = _iocs_bgctrlst (
        1,  //Background specification (0/1)
        0,  //Specifying a text page (0/1)
        1   //Show / Hide specification (0: Hide 1: Show)
    );

    //if any problem...
    if(status < 0){
        _dos_c_print("Illegal screen mode\r\n");
        _iocs_crtmod(last_mode);
        _dos_exit2(status);
    }

    /*
     * here I'm setting the scroll 0 to work with the tilemap 1 and to
     * make it shown.
     * In this example I demonstrate that the tilemap can be either.
     */
    status = _iocs_bgctrlst (
        0,  //Background specification (0/1)
        1,  //Specifying a text page (0/1)
        1   //Show / Hide specification (0: Hide 1: Show)
    );

    //if any problem...
    if(status < 0){
        _dos_c_print("Illegal screen mode\r\n");
        _iocs_crtmod(last_mode);
        _dos_exit2(status);
    }

    {
        union {
            struct {
                int8_t vf:1;
                int8_t hf:1;
                int8_t :2;           //padding
                int8_t palette:4;
                int8_t pcg;
            } flags;
            uint16_t code;
        } sp_register;

        sp_register.flags.vf = 0;         // VF ON
        sp_register.flags.hf = 0;         // HF OFF
        sp_register.flags.palette = 1;    // palette number
        sp_register.flags.pcg = 0;        // pcg number

        status = _iocs_bgtextcl (
           0,   //Background specification (0/1)
           sp_register.code
        );

        //if any problem...
        if(status < 0){
            _dos_c_print("Illegal screen mode\r\n");
            _iocs_crtmod(last_mode);
            _dos_exit2(status);
        }

        sp_register.flags.vf = 0;         // VF ON
        sp_register.flags.hf = 0;         // HF OFF
        sp_register.flags.palette = 2;    // palette number
        sp_register.flags.pcg = 1;        // pcg number

        status = _iocs_bgtextcl (
           1,   //Background specification (0/1)
           sp_register.code
        );

        //if any problem...
        if(status < 0){
            _dos_c_print("Illegal screen mode\r\n");
            _iocs_crtmod(last_mode);
            _dos_exit2(status);
        }
    }

    signal(SIGINT, terminate);	/* Processing routine settings when is pressed CTRL-C */

    _iocs_vdispst(
        &vsync_disp,
        1,//0: vertical blanking interval 1: vertical display period
        1
    );

    _dos_c_print("Press a key.\r\n");

    //waiting for a keystroke.
    _dos_getchar();

    terminate();
}
