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

/* Processing routine when pressed CTRL-C */
void terminate()
{
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
    uint32_t t;

    int super;

    union FileConf fconf;

    void vsync_disp();
    void timer_interrupt();

    _dos_allclose();

    last_mode = _iocs_crtmod(-1);

    //status = _iocs_crtmod(2); //this mode is 512 x 512 16 colours

    //status = _iocs_crtmod(6); //this mode is 256 x 256 16 colours

    //status = _iocs_crtmod(8); //this mode is 512 x 512 256 colours, it doesn't work here

    //status = _iocs_crtmod(10); //this mode is 256 x 256 256 colours


    //status = _iocs_crtmod(12);  //this mode is 512 x 512 65536 colours

    status = _iocs_crtmod(14);  //this mode is 256 x 256 65536 colours

    _iocs_g_clr_on();
    _iocs_vpage(1);

    //if any error...
    if(status < 0){
        _dos_c_print("Can't set that resolution\r\n");
        _dos_exit2(status);
    }


    fconf.config = 0;
    fconf.flags.access_dictionary = ACCESS_NORMAL;
    fconf.flags.sharing = SHARING_COMPATIBILITY_MODE;
    fconf.flags.mode = MODE_R;

    status = start_timer();

    { //Load LandScape BEGING

        //we open the palette file
        file_handler = _dos_open(
            "16BitLandSc.pic",
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

        _dos_read(file_handler, (char*)GVRAM_START, sizeof (uint16_t) * (512 * 512));

        printf(
           "time finish %d\n\n",
           millisecond() - t
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
    } //Load LandScape ENDING

    { //Load big sonic BEGIN
        //we open the palette file
        file_handler = _dos_open(
            "16BSonic.pic",
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

        {
            uint16_t buffer[32 * 48];
            struct _putptr putbuf;
            uint8_t cont;

            status = _dos_read(file_handler, (char*)&buffer, sizeof buffer);

            putbuf.x1 = 50;
            putbuf.x2 = putbuf.x1 + 31;
            putbuf.y1 = 100;
            putbuf.y2 = putbuf.y1 + 47;
            putbuf.buf_start = buffer;
            putbuf.buf_end = (void *) (buffer + sizeof buffer);

            status = _iocs_putgrm(&putbuf);

            if(status < 0){
                switch(status){
                    case -1: _dos_c_print("Graphics not available. use _iocs_g_clr_on()\r\n"); break;
                    case -2: _dos_c_print("Coordinate specification is abnormal\r\n"); break;
                    case -3: _dos_c_print("Buffer capacity is too small\r\n"); break;
                }
                //_iocs_crtmod(last_mode);
                //_dos_exit2(status);
            }
        }

        printf(
           "time finish %d\n\n",
           millisecond() - t
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
    } //Load big sonic END




    //signal(SIGINT, terminate);	/* Processing routine settings when is pressed CTRL-C */
/*
    _iocs_vdispst(
        &vsync_disp,
        0,//0: vertical blanking interval 1: vertical display period
        1
    );
*/
    _dos_c_print("Press a key.\r\n");

    //waiting for a keystroke.
    _dos_getchar();

    terminate();
}
