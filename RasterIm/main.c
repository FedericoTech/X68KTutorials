#ifdef __MARIKO_CC__
	#include <doslib.h>
	#include <iocslib.h>

	#define NULL 0
#else
	#include <dos.h>
	#include <iocs.h>
	#include <string.h>
	#include <stdio.h>
	#define interrupt __attribute__ ((interrupt_handler))
#endif

#define ACCESS_DICTIONARY           0x80    //0b10000000 //1 not for users
#define ACCESS_NORMAL               0x00    //0b00000000 //0

#define SHARING_TOTAL               0x08    //0b00010000  //4 allow others to write and read
#define SHARING_WRITE_ONLY          0x0C    //0b00001100  //3 allow others to write only
#define SHARING_READ_ONLY           0x08    //0b00001000  //2 allow others to read only
#define SHARING_RESTRICTED          0x04    //0b00000100  //1 don't allow others anything
#define SHARING_COMPATIBILITY_MODE  0x00    //0b00000000  //0

#define MODE_RW                     0x02    //0b00000010  //2 open for write and read
#define MODE_W                      0x01    //0b00000001  //1 open only for writing
#define MODE_R                      0x00    //0b00000000  //0 open only for reading



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


#define Y_SCROLL_PAGE_A 0xe8001a
#define Y_SCROLL_PAGE_B 0xe8001e
#define Y_SCROLL_PAGE_C 0xe80022
#define Y_SCROLL_PAGE_D 0xe80026


#define GVRAM_PAGE_0	0xC00000	// Start of graphics vram
#define GVRAM_PAGE_1    0xc80000    // Start of page 1 in 16 and 256 colours

#define G_PALETTE_START 0xe82000    //graphic pallette



volatile short palette0[256];
volatile short palette1[256];

volatile char last_mode;

volatile int y = -1;

short *scroll_a;
short *scroll_b;

void init();

void termin();

void interrupt vsync_disp();

void interrupt crtcras();

void interrupt hsyncst15();

void interrupt hsyncst31();

volatile int page_num;

int main(void)
{
    int status;
    short file_handler;
    short colour;
    unsigned char num_color = 0;

    union {
        struct {
            char access_dictionary:1;
            char :2;      //padding
            char sharing:3;
            char mode:2;
        } flags;
        char config;
    } fconf;

    init();

    fconf.config = 0;

    //we open the palette file
    file_handler = _dos_open(
        "landscap.pal",
        fconf.config
    );

    //if any error...
    if(file_handler < 0){
        _dos_c_print("Can't open the file landscap.pal\r\n");
        _dos_getchar();
        termin();
    }

    //we read the whole palette file
    while(_dos_read(file_handler, (char *) &colour, sizeof(short)) > 0){

        palette0[num_color] = colour;

        status = _iocs_gpalet(      //returns 32 bit with the code of the colour just set or if colour is -1
            num_color,      //0 - 255
            colour                  // colour code, -1 to retrieve the code in that cell
        );
        ++num_color;
    }

    status = _dos_close(file_handler);

    //if any error...
    if(status < 0){
        _dos_c_print("Can't close the file landscap.pal\r\n");
        _dos_getchar();
        termin();
    }


    //we open the palette file
    file_handler = _dos_open(
        "landscap.dmp",
        fconf.config
    );

    //if any error...
    if(file_handler < 0){
        _dos_c_print("Can't open the file landscap.dmp\r\n");
        _dos_getchar();
        termin();
    }

    _dos_read(file_handler, (char*)GVRAM_PAGE_0, sizeof (short) * (512 * 512));

    status = _dos_close(file_handler);

    //if any error...
    if(status < 0){
        _dos_c_print("Can't close the file landscap.dmp\r\n");
        _dos_getchar();
        termin();
    }

    //we open the palette file
    file_handler = _dos_open(
        "colours.pal",
        fconf.config
    );

    //if any error...
    if(file_handler < 0){
        _dos_c_print("Can't open the file colours.pal\r\n");
        _dos_getchar();
        termin();
    }

    num_color = 0;
    //we read the whole palette file
    while(_dos_read(file_handler, (char *) &colour, sizeof(short)) > 0){

        status = _iocs_gpalet(      //returns 32 bit with the code of the colour just set or if colour is -1
            num_color,      //0 - 255
            colour                  // colour code, -1 to retrieve the code in that cell
        );

        palette1[num_color] = colour;
        ++num_color;
    }

    status = _dos_close(file_handler);

    //if any error...
    if(status < 0){
        _dos_c_print("Can't close the file colours.pal\r\n");
        _dos_getchar();
        termin();
    }

    //we open the palette file
    file_handler = _dos_open(
        "colours.dmp",
        fconf.config
    );

    //if any error...
    if(file_handler < 0){
        _dos_c_print("Can't open the file colours.dmp\r\n");
        _dos_getchar();
        termin();
    }

    _dos_read(file_handler, (char*)GVRAM_PAGE_1, sizeof (short) * (512 * 512));

    status = _dos_close(file_handler);

    //if any error...
    if(status < 0){
        _dos_c_print("Can't close the file colours.dmp\r\n");
        _dos_getchar();
        termin();
    }

    _iocs_vdispst(
        &vsync_disp,
        0,  //0: vertical blanking interval 1: vertical display period
        1   //Counter (when 0, treat as 256)
    );

    //if 15 kHz
    if(_iocs_crtmod(-1) % 2){
        _iocs_crtcras(
            &crtcras,
            128 // line
        );

        _iocs_hsyncst(&hsyncst15);
    //if 31 kHz
    } else {
        _iocs_crtcras(
            &crtcras,
            270 // line
        );
        _iocs_hsyncst(&hsyncst31);
    }

    _dos_c_print("Press a key.\r\n");

    //waiting for a keystroke.
    _dos_getchar();

    termin();
}

void init()
{
    int status;

    //we capture the current video mode
    last_mode = _iocs_crtmod(-1);

    //status = _iocs_crtmod(8); //this mode is 512 x 512 256 colours 31 KHz
    status = _iocs_crtmod(9); //this mode is 512 x 512 256 colours 15 KHz

    _iocs_b_curoff(); //disable the cursor

    _iocs_g_clr_on();

     _iocs_vpage(1);

    //if any error...
    if(status < 0){
        _dos_c_print("Can't set that resolution\r\n");
        _dos_exit2(status);
    }
}

void termin()
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


void interrupt vsync_disp()
{
    scroll_a = (short *) Y_SCROLL_PAGE_A;
    scroll_b = (short *) Y_SCROLL_PAGE_B;

    //copy palette 0
    memcpy((short *)G_PALETTE_START, (short *) palette0, 512);
    //_iocs_b_memset((void *)G_PALETTE_START, (void *) palette0, 512);



    _iocs_vpage(1);
/*
    _iocs_dmamove(
        &palette0,                   //buffer A, the source
        (short*)G_PALETTE_START,    //buffer B, the destination
        DMA_MODE(
             DMA_DIR_A_TO_B,        //from A to B
             DMA_PLUS_PLUS,       //move the pointer forward as it reads
             DMA_PLUS_PLUS        //move the pointer forward as it writes
        ),
        512             //size of the memory block we are moving
    );
*/


    //put back the scroll

}


void interrupt crtcras()
{
    scroll_a = (short *) Y_SCROLL_PAGE_C;
    scroll_b = (short *) Y_SCROLL_PAGE_D;

    //copy palette 1
    memcpy((short *)G_PALETTE_START, (short *) palette1, 512);
    //_iocs_b_memset((short *)G_PALETTE_START, (short *) palette1, 512);


    /*
    _iocs_dmamove(
        &palette1,                   //buffer A, the source
        (short*)G_PALETTE_START,    //buffer B, the destination
        DMA_MODE(
             DMA_DIR_A_TO_B,        //from A to B
             DMA_PLUS_PLUS,       //move the pointer forward as it reads
             DMA_PLUS_PLUS        //move the pointer forward as it writes
        ),
        512             //size of the memory block we are moving
    );
    */

    _iocs_vpage(2);
    page_num = 2;
    y = 256;
}

void interrupt hsyncst31()
{
    *scroll_a = ++y;
    *scroll_b = y;
}


void interrupt hsyncst15()
{
    /*
    y += 2;

    *scroll_a = y;
    *scroll_b = y;

    */
    scroll_a += 2;
    *scroll_b += 2;
}
