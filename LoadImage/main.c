#include "utils.h"
//#include <signal.h>
#include <stdio.h>

#define reverse_bytes_32(num) ( ((num & 0xFF000000) >> 24) | ((num & 0x00FF0000) >> 8) | ((num & 0x0000FF00) << 8) | ((num & 0x000000FF) << 24) )
#define reverse_bytes_24(num) ( ((num & 0xFF0000) >> 16) | (num & 0x00FF00) | ((num & 0x0000FF) << 16) )
#define reverse_bytes_16(num) ( ((num & 0xFF00) >> 8) | ((num & 0x00FF) << 8))

#define rgb888_2grb(r, g, b, i) ( ((b&0xF8)>>2) | ((g&0xF8)<<8) | ((r&0xF8)<<3) | i )
#define rgb888_2rgb(r, g, b, i) ( ((r&0xF8)<<8) | ((g&0xF8)<<3) | ((b&0xF8)>>2) | i )

#define GVRAM_START	0xC00000	// Start of graphics vram

volatile uint16_t x = 0;
volatile uint16_t y = 0;

volatile int8_t last_mode;

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

int main(void)
{
    int16_t file_handler;
    int32_t status;

    void vsync_disp();
    void timer_interrupt();

    _dos_allclose();

    last_mode = _iocs_crtmod(-1);

    //status = _iocs_crtmod(2); //this mode is 512 x 512 16 colours

    //status = _iocs_crtmod(6); //this mode is 256 x 256 16 colours

    //status = _iocs_crtmod(8); //this mode is 512 x 512 256 colours, it doesn't work here

    //status = _iocs_crtmod(10); //this mode is 256 x 256 256 colours


    status = _iocs_crtmod(12);  //this mode is 512 x 512 65536 colours

    _iocs_g_clr_on();
    _iocs_vpage(1);

    //if any error...
    if(status < 0){
        _dos_c_print("Can't set that resolution\r\n");
        _dos_exit2(status);
    }

    {
        union FileConf fconf;

        fconf.flags.access_dictionary = ACCESS_NORMAL;
        fconf.flags.sharing = SHARING_COMPATIBILITY_MODE;
        fconf.flags.mode = MODE_R;

        //we open the palette file
        file_handler = _dos_open(
            "R.pic",
            fconf.config
        );
    }

    //if any error...
    if(file_handler < 0){
        _dos_c_print("Can't open the file\r\n");
        _dos_c_print(getErrorMessage(file_handler));
        _iocs_crtmod(last_mode);
        _dos_exit2(file_handler);
    }

    {
        volatile uint16_t *vram_addr = (uint16_t *)GVRAM_START;

        //we copy the address into this other pointer to not lose it.
        volatile uint16_t *va  = vram_addr;

        uint32_t t;


        status = start_timer();

        t = millisecond();

        #ifdef SUP
            //we set the super user mode to gain access to reserved areas of memory
            _dos_super(0);

            _dos_read(file_handler, (char*)va, sizeof(uint16_t) * (512 * 512));

            //back to user mode
            _dos_super(0);
        #else
            {
                uint16_t buffer[512 * 32];
                struct _putptr putbuf;
                uint8_t cont;

                putbuf.x1 = 0;
                putbuf.x2 = 511;
                putbuf.buf_start = buffer;
                putbuf.buf_end = (void *) ((int_)buffer + sizeof(buffer));

                _iocs_g_clr_on();

                for(cont = 0; cont < 16; cont++){

                    status = _dos_read(file_handler, (char*)&buffer, sizeof(buffer));

                    putbuf.y1 = 32 * cont;
                    putbuf.y2 = putbuf.y1 + 31;

                    status = _iocs_putgrm(&putbuf);

                    if(status < 0){
                        switch(status){
                            case -1: _dos_c_print("Graphics not available. use _iocs_g_clr_on()\r\n"); break;
                            case -2: _dos_c_print("Coordinate specification is abnormal\r\n"); break;
                            case -3: _dos_c_print("Buffer capacity is too small\r\n"); break;
                        }
                    }
                }
            }
        #endif

        printf(
           "time finish %d\n\n",
           millisecond() - t
        );

        //now we close the file
        status = _dos_close(file_handler);
    }

    //if any error...
    if(status < 0){
        _dos_c_print("Can't close the file\r\n");
        _dos_c_print(getErrorMessage(status));
        _iocs_crtmod(last_mode);
        _dos_exit2(status);
    }

    //signal(SIGINT, terminate);	/* Processing routine settings when is pressed CTRL-C */

    _iocs_vdispst(
        &vsync_disp,
        0,//0: vertical blanking interval 1: vertical display period
        1
    );

    _dos_c_print("Press a key.\r\n");

    //waiting for a keystroke.
    _dos_getchar();

    terminate();
}
