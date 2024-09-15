#ifdef __MARIKO_CC__
    #include <doslib.h>
    #include <iocslib.h>
#else
    #include <dos.h>
    #include <iocs.h>

    //from Mariko to Lydux
    #define interrupt __attribute__ ((interrupt_handler))
    #define _fillptr iocs_fillptr
    #define _putptr iocs_putptr
#endif

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

    union FileConf fconf;

    void vsync_disp();
    void timer_interrupt();

    uint8_t *buffer;

    _dos_allclose();

    last_mode = _iocs_crtmod(-1);

//                              Actual  Viewport    Colours     Pages   Frequency
    //status = _iocs_crtmod(0); //  1024    512 x 512   16          1       31 KkHz
    //status = _iocs_crtmod(1); //  1024    512 x 512   16          1       15 KkHz

    //status = _iocs_crtmod(2); //  1024    256 x 256   16          1       31 KkHz
    //status = _iocs_crtmod(3); //  1024    256 x 256   16          1       15 KkHz

    //status = _iocs_crtmod(4); //  512     512 x 512   16          4       31 KkHz
    //status = _iocs_crtmod(5); //  512     512 x 512   16          4       15 KkHz

    //status = _iocs_crtmod(6); //  512     256 x 256   16          4       31 KkHz
    //status = _iocs_crtmod(7); //  512     256 x 256   16          4       15 KkHz

    status = _iocs_crtmod(8); //  512     512 x 512   256         2       31 KkHz
    //status = _iocs_crtmod(9); //  512     512 x 512   256         2       15 KkHz

    //status = _iocs_crtmod(10); // 512     256 x 256   256         2       31 KkHz
    //status = _iocs_crtmod(11); // 512     256 x 256   256         2       15 KkHz


    //status = _iocs_crtmod(12); // 512     512 x 512   65536       1       31 KkHz
    //status = _iocs_crtmod(14); // 512     256 x 256   65536       1       15 KkHz

    _iocs_g_clr_on();

    //if any error...
    if(status < 0){
        _dos_c_print("Can't set that resolution\r\n");
        _dos_exit2(status);
    }

    fconf.config = 0;
    fconf.flags.access_dictionary = ACCESS_NORMAL;
    fconf.flags.sharing = SHARING_COMPATIBILITY_MODE;
    fconf.flags.mode = MODE_R;

//Load Image BEGING

    //we open the palette file
    file_handler = _dos_open(
        "8BitsColors.pic",
        fconf.config
    );

    //if any error...
    if(file_handler < 0){
        _dos_c_print("Can't open the file\r\n");
        _dos_c_print(getErrorMessage(file_handler));
        _iocs_crtmod(last_mode);
        _dos_exit2(file_handler);
    }

    buffer = (uint8_t *)_dos_malloc(512 * 512);

    _dos_c_print("Loading Image\r\n");

    t = _dos_time_pr();

    _dos_read(file_handler, (char*)buffer, 512 * 512);

    printf(
       "read the file in %d milliseconds.\n\n",
       _dos_time_pr() - t
    );

    {
        //we set a background color in page 1 which is under page 0
        struct _fillptr fill = {
            0, 0,       // x1, y1
            512, 512,   // x2, y2
            0x00        // at 8bits, index
        };

        // in 8 bits,   page 0 = 0, 2, page 1 = 1, 3
        _iocs_apage(1); //we are going to draw in page 1

        status = _iocs_fill(&fill);

        if(status < 0){
            switch(status){
                case -1: _dos_c_print("Graphics not available. use _iocs_g_clr_on()\r\n"); break;
                case -2: _dos_c_print("Specified palette code cannot be used in the current mode\r\n"); break;
            }
        }
    }

    // in 8 bits,       page 0 = 0, 2, page 1 = 1, 3
    _iocs_apage(0);   // we a going to draw on page 0

    {
        struct _putptr putbuf;

        putbuf.x1 = 0;
        putbuf.x2 = 511;
        putbuf.y1 = 0;
        putbuf.y2 = 511;
        putbuf.buf_start = buffer;
        putbuf.buf_end = (void *) ((int)buffer + (512 * 512));

        t = _dos_time_pr();

        status = _iocs_putgrm(&putbuf);

        if(status < 0){
            switch(status){
                case -1: _dos_c_print("Graphics not available. use _iocs_g_clr_on()\r\n"); break;
                case -2: _dos_c_print("Coordinate specification is abnormal\r\n"); break;
                case -3: _dos_c_print("Buffer capacity is too small\r\n"); break;
            }
        }
    }

    // in 8 bits, 0 no pages,   page 0 = 1, 3  page 1 = 2, 4
    _iocs_vpage(4);   // we activate pages 0 and 1

    _dos_mfree(buffer);

    printf(
       "painted in %d milliseconds\n\n",
       _dos_time_pr() - t
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
//Load Image ENDING

    {
        uint16_t g_colours[256];

        //we open the palette file
        file_handler = _dos_open(
            "8BitsColors.pal",
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
            for(colour_in_palette = 0; colour_in_palette < 255; colour_in_palette++){
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

/*
    { //Load big sonic BEGIN
        //we open the palette file
        file_handler = _dos_open(
            "8BSonic.pic",
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

            status = _dos_read(file_handler, (char*)&buffer, sizeof(buffer));

            putbuf.x1 = 50;
            putbuf.x2 = putbuf.x1 + 31;
            putbuf.y1 = 100;
            putbuf.y2 = putbuf.y1 + 47;
            putbuf.buf_start = buffer;
            putbuf.buf_end = (void *) ((int_)buffer + sizeof(buffer));

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
*/
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
