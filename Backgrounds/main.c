
#ifdef __MARIKO_CC__
    #include <doslib.h>
    #include <iocslib.h>
#else
    #include <dos.h>
    #include <iocs.h>
    #define interrupt __attribute__ ((interrupt_handler))
#endif

#include <stdint.h>
#include <signal.h>

//sprite palette utils
#define VERTICAL_BLANKING_DETECTION 0x80000000     //0b10000000000000000000000000000000
#define VERTICAL_BLANKING_NO_DETECT 0

#define SET_VBD_V(vbd, v) (vbd | v)

#define S_PALETTE_START 0xe82220

#define SP_DEFCG_8X8_TILE 0
#define SP_DEFCG_16X16_TILE 1

uint16_t colours[2][16] = {
    {0x0000, 0x5294, 0x0020, 0x003e, 0x0400, 0x07c0, 0x0420, 0x07fe, 0x8000, 0xf800, 0x8020, 0xf83e, 0x8400, 0xffc0, 0xad6a, 0xfffe},
    {0x0000, 0x5294, 0x0020, 0x003e, 0x0400, 0x003e, 0x0420, 0x07fe, 0x8000, 0xf800, 0x8020, 0xf83e, 0x8400, 0xffc0, 0xad6a, 0xfffe}
};

 uint8_t tileset[] = {
    //tile 0
    0x00, 0x00, 0x00, 0x05,
    0x00, 0x00, 0x00, 0x55,
    0x00, 0x00, 0x05, 0x55,
    0x00, 0x00, 0x55, 0x55,
    0x00, 0x05, 0x55, 0x55,
    0x00, 0x55, 0x55, 0x55,
    0x05, 0x55, 0x55, 0x55,
    0x55, 0x55, 0x55, 0x55,
    //tile 1
    0x55, 0x55, 0x55, 0x55,
    0x05, 0x55, 0x55, 0x55,
    0x00, 0x55, 0x55, 0x55,
    0x00, 0x05, 0x55, 0x55,
    0x00, 0x00, 0x55, 0x55,
    0x00, 0x00, 0x05, 0x55,
    0x00, 0x00, 0x00, 0x55,
    0x00, 0x00, 0x00, 0x05,
    //tile 2
    0x50, 0x00, 0x00, 0x00,
    0x55, 0x00, 0x00, 0x00,
    0x55, 0x50, 0x00, 0x00,
    0x55, 0x55, 0x00, 0x00,
    0x55, 0x55, 0x50, 0x00,
    0x55, 0x55, 0x55, 0x00,
    0x55, 0x55, 0x55, 0x50,
    0x55, 0x55, 0x55, 0x55,
    //tile 3
    0x55, 0x55, 0x55, 0x55,
    0x55, 0x55, 0x55, 0x50,
    0x55, 0x55, 0x55, 0x00,
    0x55, 0x55, 0x50, 0x00,
    0x55, 0x55, 0x00, 0x00,
    0x55, 0x50, 0x00, 0x00,
    0x55, 0x00, 0x00, 0x00,
    0x50, 0x00, 0x00, 0x00,

    //tile 4
    0x00, 0x00, 0x00, 0x05,
    0x00, 0x00, 0x00, 0x55,
    0x00, 0x00, 0x05, 0x55,
    0x00, 0x00, 0x55, 0x55,
    0x00, 0x05, 0x55, 0x55,
    0x00, 0x55, 0x55, 0x55,
    0x05, 0x55, 0x55, 0x55,
    0x55, 0x55, 0x55, 0x55,
};


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

    //                              Actual  Viewport    Colours     Pages   Frequency   Bg Planes       BG text res
    //status = _iocs_crtmod(0); //   1024   512 x 512   16          1       31 KkHz     * bg 1 only        16x16
    //status = _iocs_crtmod(1); //   1024   512 x 512   16          1       15 KkHz     * bg 1 only        16x16

    //status = _iocs_crtmod(2); //   1024   256 x 256   16          1       31 KkHz     * bg 1 & 2         8x8
    //status = _iocs_crtmod(3); //   1024   256 x 256   16          1       15 KkHz     * bg 1 & 2         8x8

    status = _iocs_crtmod(4); //   512    512 x 512   16          4       31 KkHz     * bg 1 only        16x16
    //status = _iocs_crtmod(5); //   512    512 x 512   16          4       15 KkHz     * bg 1 only        16x16

    //status = _iocs_crtmod(6); //   512    256 x 256   16          4       31 KkHz     * bg 1 & 2         8x8
    //status = _iocs_crtmod(7); //   512    256 x 256   16          4       15 KkHz     * bg 1 & 2         8x8

    //status = _iocs_crtmod(8); //   512    512 x 512   256         2       31 KkHz     * bg 1 only        16x16
    //status = _iocs_crtmod(9); //   512    512 x 512   256         2       15 KkHz     * bg 1 only        16x16

    //status = _iocs_crtmod(10); //  512    256 x 256   256         2       31 KkHz     * bg 1 & 2         8x8
    //status = _iocs_crtmod(11); //  512    256 x 256   256         2       15 KkHz     * bg 1 & 2         8x8

    //status = _iocs_crtmod(12); //  512    512 x 512   65536       1       31 KkHz     * bg 1 only        16x16
    //status = _iocs_crtmod(13); //  512    512 x 512   65536       1       15 KkHz     * bg 1 only        16x16

    //status = _iocs_crtmod(14); //  512    256 x 256   65536       1       31 KkHz     * bg 1 & 2         8x8
    //status = _iocs_crtmod(15); //  512    256 x 256   65536       1       15 KkHz     * bg 1 & 2         8x8

    //status = _iocs_crtmod(16); //  1024   768 x 512   16          1       31 KkHz     * can't load pcg
    //status = _iocs_crtmod(17); //  1024   1024 x 424  16          1       24 KkHz     * can't load pcg
    //status = _iocs_crtmod(18); //  1024   1024 x 848  16          1       24 KkHz     * can't load pcg
    //status = _iocs_crtmod(19); //  1024   640 x 480   16          1       24 KkHz     * can't load pcg


    //status = _iocs_crtmod(20); //  1024   768 x 512   256         2       31 KkHz     * can't load pcg
    //status = _iocs_crtmod(21); //  1024   1024 x 424  256         2       24 KkHz     * can't load pcg
    //status = _iocs_crtmod(22); //  1024   1024 x 848  256         2       24 KkHz     * can't load pcg
    //status = _iocs_crtmod(23); //  1024   640 x 480   256         2       24 KkHz     * can't load pcg

    //status = _iocs_crtmod(24); //  1024   768 x 512   65536       1       31 KkHz     * can't load pcg
    //status = _iocs_crtmod(25); //  1024   1024 x 424  65536       1       24 KkHz     * can't load pcg
    //status = _iocs_crtmod(26); //  1024   1024 x 848  65536       1       24 KkHz     * can't load pcg
    //status = _iocs_crtmod(27); //  1024   640 x 480   65536       1       24 KkHz     * can't load pcg


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

    //for the second palette we will access the memory directly
    _iocs_b_memstr((void *) colours[0], (void *) S_PALETTE_START, 32);

    //here we load the sprite palette by using the _iocs_spalet function colour by colour.
    {
        int colour_in_palette;
        //now we go through the current palette
        for(colour_in_palette = 0; colour_in_palette < 16; colour_in_palette++){
            //we set the color
            status = _iocs_spalet( //returns 32 bit integer
                SET_VBD_V(
                    VERTICAL_BLANKING_DETECTION,
                    colour_in_palette   //0 - 15 if higher it only takes from 0-15
                ),
                1,  //the first palette for tiles is 1. 0 is the palette for text
                colours[1][colour_in_palette]
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

                _iocs_crtmod(last_mode); //we restore the video mode
                _dos_exit2(status);
            }
        }
    }


    //we load the ship as a 16 x 16 tile in the position 0
    status = _iocs_sp_defcg(
        0,                      // position in 16 x 16 tiles
        SP_DEFCG_16X16_TILE,    // 16 x 16 tile = 1
        tileset                 // pointer to the data
    );

    //we load the ship as a 16 x 16 tile in the position 0
    status = _iocs_sp_defcg(
        4,                      // position in 8 x 8 tiles
        SP_DEFCG_8X8_TILE,      // 8 x 8 tile = 1
        tileset + 128           // pointer to the data
    );

    //if any problem loading the PCG...
    if(status < 0){
        _dos_c_print("Can't load the PCG, Illegal screen mode.\r\n");
        _dos_getchar(); //wait for keystroke
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
        0,  //Background specification (0/1)
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
        1,  //Background specification (0/1)
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
        //this union is to ease the process to config a pattern
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

        //we fill the buffer with this PGC
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

        //we fill the buffer with this PGC
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
