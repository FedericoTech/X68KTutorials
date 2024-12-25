
#ifdef __MARIKO_CC__
    #include <doslib.h>
    #include <iocslib.h>
#else
    #include <dos.h>
    #include <iocs.h>
    #include <string.h>

    #define _filbuf dos_filbuf

    #define _iocs_sp_regst(spno, x, y, code, prw) \
        _iocs_sp_regst( \
           spno & VERTICAL_BLANKING_DETECTION, \
           spno & ~VERTICAL_BLANKING_DETECTION, \
           x, \
           y, \
           code, \
           prw \
       )

    #define interrupt __attribute__ ((interrupt_handler))

    #ifdef __FIX__
        #include "newlib_fixes.h"
        #define _dos_files _fix_dos_files
        #define _dos_nfiles _fix_dos_nfiles
    #endif

#endif

#include "utils.h"


#define DMA_DIR_A_TO_B 0
#define DMA_DIR_B_TO_A 1

#define DMA_FIXED 0
#define DMA_PLUS_PLUS 1
#define DMA_MINUS_3 2

#define DMA_MODE(direction, MAC, DAC) (direction << 7 | (MAC << 2) | DAC)

#define DMA_STATUS_IDLE 0x00
#define DMA_STATUS_IN_DMAMOVE_OP 0x8A
#define DMA_STATUS_IN_DMAMOV_A_OP 0x8B
#define DMA_STATUS_IN_DMAMOV_L_OP 0x8C


#define PCG_START 0xEB8000 // Start of PCG memory
#define S_PALETTE_START 0xe82220 //Start of Palette

//sprite palette utils
#define VERTICAL_BLANKING_DETECTION 0x80000000     //0b10000000000000000000000000000000
#define VERTICAL_BLANKING_NO_DETECT 0

#define FALSE 0
#define TRUE 1

int8_t last_mode;

uint8_t *s_walk[6];
volatile int frame_s_walk = 0;

int x = 10, y= 26;

volatile int time = 0;
volatile int draw = FALSE;

volatile union {
    struct {
        int8_t vf:1;
        int8_t hf:1;
        int8_t :2;           //padding
        int8_t palette:4;
        int8_t pcg;
    } flags;
    uint16_t code;
} sp_register;

int status;

void interrupt vsync_disp();

void interrupt crtcras10(); //raster interrupt
void interrupt crtcras127(); //raster interrupt

void interrupt hsyncst(); //horizontal sync interrupt

void init();

void terminate();

int main(void)
{

    int super;
    union FileConf fconf;

    init();

    //load palette
    fconf.config = 0;
    fconf.flags.access_dictionary = ACCESS_NORMAL;
    fconf.flags.sharing = SHARING_COMPATIBILITY_MODE;
    fconf.flags.mode = MODE_R;

    loadData((char*)S_PALETTE_START, "/sonic.pal", fconf.config);

    {
        struct _filbuf sf;

        int cont = 0;

        status = _dos_files (&sf, "/s_walk_?.spt", 48);
        while(status == 0){
            char filename[23] = "/";
            strcat(filename, sf.name);
            s_walk[cont++] = loadData(NULL, filename, fconf.config);
            status= _dos_nfiles(&sf);
        }
    }

    loadData((char*)PCG_START, "/s_walk_0.spt", fconf.config);

    {
        int i, j, cont = 0;

        sp_register.flags.vf = 0;         // VF ON
        sp_register.flags.hf = 0;         // HF OFF
        sp_register.flags.pcg = 0;        // pcg number

        sp_register.flags.palette = 1;    // palette number

        for(i = 0; i < 48; i += 16){
            for(j = 0; j < 48; j += 16){

                sp_register.flags.pcg = cont;

                status = _iocs_sp_regst(
                    cont | VERTICAL_BLANKING_NO_DETECT,     //int_ spno sprite number (0-127) //int_ mode bit 31 0: Vertical blanking interval detection post-setting 1: Not detected
                    16 + j + x,                             //int_ x X coordinates (0-1023 16 displayed on the far left
                    16 + i + y,                             //int_ y Y " (" " Top ")
                    sp_register.code,                       //code pattern code
                    3                                       //int_ prw priority
                );

                ++cont;
            }
        }
    }



/*
    _iocs_vdispst(
        &vsync_disp,
        0,  //0: vertical blanking interval 1: vertical display period
        1   //Counter (when 0, treat as 256)
    );

    _iocs_hsyncst(hsyncst); //set up horizontal interrupt
*/


    _iocs_vdispst(
        &vsync_disp,
        0,  //0: vertical blanking interval 1: vertical display period
        1   //Counter (when 0, treat as 256)
    );



    _iocs_crtcras(
        crtcras10,
        10
    );

    _dos_c_print("Press a key.\r\n");

    while(_dos_inpout(0xFF) == 0){

        if(draw){
            //DMA with _iocs_dmamove
            _iocs_dmamove(
                s_walk[frame_s_walk],   //buffer A, the source
                (uint16_t *)PCG_START,  //buffer B, the destination
                DMA_MODE(
                     DMA_DIR_A_TO_B,    //from A to B
                     DMA_PLUS_PLUS,     //move the pointer forward as it reads
                     DMA_PLUS_PLUS      //move the pointer forward as it writes
                ),
                1152                    //size of the memory block we are moving
            );

            if(++frame_s_walk > 5){ frame_s_walk = 0; }

            draw = FALSE;
        }
    }



    //waiting for a keystroke.
    //_dos_getchar();

    terminate();

}

void interrupt crtcras10()
{
    int ello = 36;

    *(short *) 0xEB0002 = ello;

    *(short *) 0xEB000A = *(short *) 0xEB0002;

    *(short *) 0xEB0012 = *(short *) 0xEB0002;


    *(short *) 0xEB001A = 16 + ello;

    *(short *) 0xEB0022 = *(short *) 0xEB001A;

    *(short *) 0xEB002A = *(short *) 0xEB001A;



    *(short *) 0xEB0032 = 32 + ello;

    *(short *) 0xEB003A = *(short *) 0xEB0032;

    *(short *) 0xEB0042 = *(short *) 0xEB0032;

    _iocs_crtcras(
        NULL,
        10
    );

    _iocs_crtcras(
        crtcras127,
        127
    );
}

void interrupt crtcras127()
{
    int ello = 127;

    *(short *) 0xEB0002 = ello;

    *(short *) 0xEB000A = *(short *) 0xEB0002;

    *(short *) 0xEB0012 = *(short *) 0xEB0002;


    *(short *) 0xEB001A = 16 + ello;

    *(short *) 0xEB0022 = *(short *) 0xEB001A;

    *(short *) 0xEB002A = *(short *) 0xEB001A;



    *(short *) 0xEB0032 = 32 + ello;

    *(short *) 0xEB003A = *(short *) 0xEB0032;

    *(short *) 0xEB0042 = *(short *) 0xEB0032;

    _iocs_crtcras(
        NULL,
        127
    );

    _iocs_crtcras(
        crtcras10,
        10
    );
}

void interrupt vsync_disp()
{
    time = 0;
    draw = TRUE;
}

void interrupt hsyncst()
{
    int ello = 26;

    if(time == 127 || time == 0) {


        if(time == 127){
            ello = 154;
        }

        //
        *(short *) 0xEB0002 = ello;

        *(short *) 0xEB000A = *(short *) 0xEB0002;

        *(short *) 0xEB0012 = *(short *) 0xEB0002;


        *(short *) 0xEB001A = 16 + ello;

        *(short *) 0xEB0022 = *(short *) 0xEB001A;

        *(short *) 0xEB002A = *(short *) 0xEB001A;



        *(short *) 0xEB0032 = 32 + ello;

        *(short *) 0xEB003A = *(short *) 0xEB0032;

        *(short *) 0xEB0042 = *(short *) 0xEB0032;
    }

    ++time;
}

void terminate()
{
    /* Un interrupt */
	_iocs_vdispst(
        (void *)NULL,
        0,//0: vertical blanking interval 1: vertical display period
        0
    );

    _iocs_hsyncst ((void *)NULL); //horizontal interrupt

     _iocs_crtcras (
        (void *)NULL,
        0 //int_ luster
    );

    //we activate the console cursor
    _iocs_b_curon();

    //we restore the video mode
    _iocs_crtmod(last_mode);
    _dos_exit();
}

void init()
{
    last_mode = _iocs_crtmod(-1);

    //_iocs_crtmod(4); //this mode is 512 x 512 16 colours 31 Hz
    //_iocs_crtmod(5); //this mode is 512 x 512 16 colours 15 Hz

    //_iocs_crtmod(6); //this mode is 256 x 256 16 colours 31 Hz
    _iocs_crtmod(7); //this mode is 256 x 256 16 colours 15 Hz

    _iocs_b_curoff(); //disable the cursor

    _iocs_g_clr_on(); //start graphic system

    _iocs_sp_init();

    _iocs_sp_on();

    signal(SIGINT, terminate); //for the Control + C
}


