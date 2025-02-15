#ifdef __MARIKO_CC__
	#include <doslib.h>
	#include <iocslib.h>
#else
	#include <dos.h>
	#include <iocs.h>
	#include <stdio.h>
	#define interrupt __attribute__ ((interrupt_handler))
#endif

unsigned char buffer[76] = {
    0x00, 0x01, 0xFF, 0x00, 0x00, 0x07, 0xFF, 0x80,
    0x20, 0x3F, 0xE3, 0x80, 0x0F, 0x7E, 0x61, 0x80,
    0x82, 0x58, 0x61, 0x80, 0x2D, 0x78, 0x61, 0x80,
    0x03, 0x70, 0x61, 0x80, 0x20, 0x38, 0x61, 0x80,
    0x0F, 0x7B, 0xFF, 0x80, 0x20, 0x3C, 0x61, 0x80,
    0x0F, 0x78, 0x61, 0x80, 0x80, 0x48, 0x61, 0x80,
    0x0D, 0x78, 0x61, 0x80, 0x03, 0x70, 0x61, 0x80,
    0x24, 0x38, 0xFD, 0x80, 0x0B, 0x7F, 0xFF, 0x80,
    0x80, 0x0F, 0xFF, 0x80, 0x0F, 0x7C, 0x00, 0x80,
    0x03, 0x70, 0x00, 0x00
};

struct {
    float x;
    float y;
    float ratio;
    int colour;
    int order;
} wind[20];

volatile char last_mode;

int main(void)
{
    long int status;

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

    //status = _iocs_crtmod(8); //  512     512 x 512   256         2       31 KkHz
    //status = _iocs_crtmod(9); //  512     512 x 512   256         2       15 KkHz

    //status = _iocs_crtmod(10); // 512     256 x 256   256         2       31 KkHz
    //status = _iocs_crtmod(11); // 512     256 x 256   256         2       15 KkHz


    //status = _iocs_crtmod(12); // 512     512 x 512   65536       1       31 KkHz
    //status = _iocs_crtmod(14); // 512     256 x 256   65536       1       15 KkHz

    //if any error...
    if(status < 0){
        _dos_c_print("Can't set that resolution\r\n");
        _dos_exit2(status);
    }

    _iocs_g_clr_on();

    {
        char flush = 0;
        int i = 0;
        int count = 0;
        float ratio = 1.0f;
        struct _fntbuf buf;


        buf.xl = 25;   // width
        buf.yl = 18;   // height

        _iocs_b_memset(buf.buffer, buffer, 72);

        for(i = 0; i < 20; i++){
            wind[i].x = abs(random() % 515);
            wind[i].y = abs(random() % 515);
            wind[i].colour = abs(random() % 15);
            wind[i].ratio = 1.0f;
            wind[i].order = random() % 20;
        }

        while(!_dos_inpout(0xFF)){

            _iocs_apage(flush);
            _iocs_wipe();

            for(i = 0; i < 20; i++){

                wind[i].order++;

                if(wind[i].order > 5){
                    wind[i].order *= -1;
                    wind[i].ratio = 1.0f;
                }


                if(wind[i].order > 0){

                    _iocs_gcolor(wind[i].colour);

                    wind[i].ratio += 1.0f;

                    _iocs_gbitput3(
                        wind[i].x,        //position x
                        wind[i].y,        //position y
                        wind[i].ratio, //x_ratio
                        wind[i].ratio, //y_ratio,
                        &buf
                    );
                }
            }
            /*
            _iocs_gbitput3(
                abs(random() % 256),    //position x
                abs(random() % 256),    //position y
                abs(random() % 5),      //x_ratio
                abs(random() % 5),      //y_ratio,
                &buf
            );
            */



            _iocs_vpage(flush+1);

            flush = !flush;
            count++;
        }
    }

    //we restore the video mode
    _iocs_crtmod(last_mode);

    _dos_exit();
}
