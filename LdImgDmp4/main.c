#include "utils.h"
//#include <signal.h>
#include <stdio.h>

#define reverse_bytes_32(num) ( ((num & 0xFF000000) >> 24) | ((num & 0x00FF0000) >> 8) | ((num & 0x0000FF00) << 8) | ((num & 0x000000FF) << 24) )
#define reverse_bytes_24(num) ( ((num & 0xFF0000) >> 16) | (num & 0x00FF00) | ((num & 0x0000FF) << 16) )
#define reverse_bytes_16(num) ( ((num & 0xFF00) >> 8) | ((num & 0x00FF) << 8))

#define rgb888_2grb(r, g, b, i) ( ((b&0xF8)>>2) | ((g&0xF8)<<8) | ((r&0xF8)<<3) | i )
#define rgb888_2rgb(r, g, b, i) ( ((r&0xF8)<<8) | ((g&0xF8)<<3) | ((b&0xF8)>>2) | i )

#define GVRAM_START	    0xC00000    // Graphics Vram Page 0
#define GVRAM_PAGE_1	0xC80000	// Graphics Vram Page 1 (256/16 color only)
#define GVRAM_PAGE_2	0xD00000	// Graphics Vram Page 2 (16 color only)
#define GVRAM_PAGE_3	0xd80000	// Graphics Vram Page 3 (16 color only)

#define DMA_DIR_A_TO_B  0x00
#define DMA_DIR_B_TO_A  0x80

#define DMA_A_FIXED     0x00
#define DMA_A_PLUS_PLUS 0x04
#define DMA_A_MINUS_3   0x08

#define DMA_B_FIXED     0x00
#define DMA_B_PLUS_PLUS 0x01
#define DMA_B_MINUS_3   0x02

#define DMA_MODE(direction, A, B) (direction | A | B)

#define DMA_MAX_BLAST   0xff00

#define DMA_STATUS_IDLE 0x00
#define DMA_STATUS_IN_DMAMOVE_OP 0x8A
#define DMA_STATUS_IN_DMAMOV_A_OP 0x8B
#define DMA_STATUS_IN_DMAMOV_L_OP 0x8C





volatile uint16_t x = 0;
volatile uint16_t y = 0;

void interrupt vsync_disp()
{

    x ++;
    y ++;

    if(x > 512){
        x = 0;
    }

    if(y > 512){
        y = 0;
    }

    _iocs_home(
        0,
        x,
        y
    );
}

volatile int8_t last_mode;

volatile uint32_t _t = 0;



/* Processing routine when pressed CTRL-C */
void terminate()
{
    /* Un interrupt */
	_iocs_vdispst(
        (void *)0,
        0,//0: vertical blanking interval 1: vertical display period
        0
    );

    stop_timer();

    //we deactivate the sprites
    _iocs_sp_off();

    //we activate the console cursor
    _iocs_b_curon();

    //we restore the video mode
    _iocs_crtmod(last_mode);
    _dos_exit();
}

/**
 * This program is meant to be run with the XM6 Pro-68k emulator which has development tools.
 * You need to look at option: "View / Video / Graphic 16-Color 1024x1024" panel to see how the
 * video memory is drown upon.
 */

int main(void)
{
    int16_t file_handler;
    int32_t status;
    uint32_t t;

    union FileConf fconf;

    void vsync_disp();

    //close all previous files
    _dos_allclose();

    //retrieve current mode
    last_mode = _iocs_crtmod(-1);

    //                              Actual  Viewport    Colours     Pages   Frequency
    //status = _iocs_crtmod(0); //  1024    512 x 512   16          1       31 KkHz
    //status = _iocs_crtmod(1); //  1024    512 x 512   16          1       15 KkHz

    //status = _iocs_crtmod(2); //  1024    256 x 256   16          1       31 KkHz
    //status = _iocs_crtmod(3); //  1024    256 x 256   16          1       15 KkHz

    status = _iocs_crtmod(4); //  512     512 x 512   16          4       31 KkHz
    //status = _iocs_crtmod(5); //  512     512 x 512   16          4       15 KkHz

    //status = _iocs_crtmod(6); //  512     256 x 256   16          4       31 KkHz
    //status = _iocs_crtmod(7); //  512     256 x 256   16          4       15 KkHz

    _iocs_g_clr_on();
    _iocs_vpage(1);

    //if any error...
    if(status < 0){
        _dos_c_print("Can't set that resolution\r\n");
        _dos_exit2(status);
    }


    fconf.config = 0; //reset the whole union
    fconf.flags.access_dictionary = ACCESS_NORMAL;
    fconf.flags.sharing = SHARING_COMPATIBILITY_MODE;
    fconf.flags.mode = MODE_R;

    status = start_timer();
/*


    if(status){
        printf("Status is %d which it's different from 0\n", status);
        _dos_c_print(
             "Probably there is a PROCESS in the CONFIG.SYS for\r\n"
             "multiprocessing that disables _iocs_vdispst().\r\n"
         );
    }
*/

    //we open the palette file
    file_handler = _dos_open(
        "4BitLandSc.dmp", //"ello.raw", //"4BitLandSc.dmp",
        fconf.config
    );

    //if any error...
    if(file_handler < 0){
        _dos_c_print("Can't open the file\r\n");
        _dos_c_print(getErrorMessage(file_handler));
        _iocs_crtmod(last_mode);
        _dos_exit2(file_handler);
    }

    t = millisecond();
    printf("timerset %d\n", t);

    //we are loading the file straight away into the first page.
    status = _dos_read(file_handler, (void *) GVRAM_START, (512 * 512 * 2));

    printf(
       "read %d bytes from file in \x81\x60%d milliseconds.\n\n",
       status,
       (millisecond() - t) * 10
    );

    t = millisecond();

    //these two instructions much faster than looping but slower than DMA
    _iocs_b_memstr((void *) GVRAM_START, (void *) GVRAM_PAGE_1, 512 * 512 * 2);
    //_iocs_b_memset((void *) GVRAM_PAGE_1, (void *) GVRAM_START, 512 * 512 * 2);

    //Painted with _dmamove START
    {
        int cont;
        int iterations = (512 * 512 * sizeof(uint16_t)) / DMA_MAX_BLAST;

        for(cont = 0; cont < iterations; cont ++){
            //we check that the dma has finished
            while(_iocs_dmamode() != DMA_STATUS_IDLE){
                printf("wait for dma\n");
            }

            _iocs_dmamove(
                (void *) GVRAM_START + cont * DMA_MAX_BLAST,            //buffer A, the source
                (void *) GVRAM_PAGE_2 + cont * DMA_MAX_BLAST,    //buffer B, the destination
                DMA_MODE(
                     DMA_DIR_A_TO_B,    //from A to B
                     DMA_A_PLUS_PLUS,       //keep reading from colour
                     DMA_B_PLUS_PLUS    //move the pointer forward as it writes
                ),
                DMA_MAX_BLAST              //size of the memory block we are moving
            );
        }
    }
    //Painted with _dmamove END

    {
        uint16_t *vram_addr = (uint16_t *)GVRAM_PAGE_3;
        uint16_t *buffer = (uint16_t *)GVRAM_START;

        int cont, cont2, k;

        //we set the super user mode to gain access to reserved areas of memory
        status = _dos_super(0);
        //each row
        for(cont = 0, k = 0; cont < 512; cont++){
            //each column
            for(cont2 = 0; cont2 < 512; cont2++){
                //we copy 2 bytes at the time
                *vram_addr++ = buffer[k++];
            }
        }
        _dos_super(status);
    }



    printf(
       "painted in \x81\x60%d milliseconds\n\n",
       (millisecond() - t) * 10
    );

    //now we close the file
    status = _dos_close(file_handler);

    //if any error...
    if(status < 0){
        _dos_c_print("Can't close the file\r\n");
        _dos_c_print(getErrorMessage(status));
        _iocs_crtmod(last_mode);
        _dos_exit2(status);
    }
    //Load LandScape ENDING

    {
        uint16_t g_colours[16];

        //we open the palette file
        file_handler = _dos_open(
            "4BitLandSc.pal",
            fconf.config
        );

        //if any error...
        if(file_handler < 0){
            _dos_c_print("Can't open the file\r\n");
            _dos_c_print(getErrorMessage(file_handler));
            _iocs_crtmod(last_mode);
            _dos_exit2(file_handler);
        }

        //we read the whole palette file
        status = _dos_read(file_handler, (char*)&g_colours, sizeof(g_colours));

        if(status > 0){

            uint8_t colour_in_palette;
            for(colour_in_palette = 0; colour_in_palette < 16; colour_in_palette++){
                status = _iocs_gpalet(                  //returns 32 bit with the code of the colour just set or if colour is -1
                    colour_in_palette,                  //0 - 255
                    g_colours[colour_in_palette]        // colour code, -1 to retrieve the code in that cell
                );

                //if any issue...
                if(status < 0){
                    _dos_c_print("The graphic screen is not initialized \r\n");
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
    }

    _dos_c_print("Press a key.\r\n");

    //waiting for a keystroke.
    _dos_getchar();

    terminate();
}
