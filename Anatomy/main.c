#ifdef __MARIKO_CC__
	#include <doslib.h>
	#include <iocslib.h>
#else
	#include <dos.h>
	#include <iocs.h>
	#define interrupt __attribute__ ((interrupt_handler))
#endif

#include <stdio.h>
#include <signal.h>

char last_mode;                 // video mode before the program started

volatile int _timer = 0;        // time

void interrupt timer();         // for timing

void interrupt vsync_disp();    // for vertical sync

void interrupt hsyncst();       // for horizontal sync

void interrupt crtcras();       // for raster sync

void init();

void terminate();

int main(void)
{
    init();

    //body of our program
    _dos_c_print("Hello world !\r\n");
    _dos_c_print("Press a key.\r\n");

    while(_dos_inpout(0xFF) == 0){
        _dos_c_locate(0,2);
        printf("counting %d \r\n", _timer);
    }

    //end of our program
    terminate();

    return 0;
}

void init()
{
    last_mode = _iocs_crtmod(-1);   //capture the video mode before the program

    _iocs_b_curoff(); //disable the cursor

    _iocs_b_clr_al(); //clear the whole screen

    /**
     * Other initialization actions.
     */

     _iocs_timerdst(
        timer,  //Processing address (interrupt disabled at 0)
        7,       //Unit time (1 = 1.0, 2 = 2.5, 3 = 4.0, 4 = 12.5, 5 = 16.0, 6 = 25.0, 7 = 50.0, micro sec unit)
        20       //Counter (when 0, treat as 256)
    );

    signal(SIGINT, terminate); //for the Control + C
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

    _iocs_timerdst(
        (void *)NULL,   //Processing address (interrupt disabled at 0)
        0,              //Unit time (1 = 1.0, 2 = 2.5, 3 = 4.0, 4 = 12.5, 5 = 16.0, 6 = 25.0, 7 = 50.0, micro sec unit)
        0               //Counter (when 0, treat as 256)
    );

    //we activate the console cursor
    _iocs_b_curon();

    //we restore the video mode
    _iocs_crtmod(last_mode);

    //we exit the program
    _dos_exit();
}

void interrupt timer()
{
    ++_timer;
}

void interrupt vsync_disp()
{
    //whatever you do in this event
}

void interrupt hsyncst()
{
    //whatever you do in this event
    //it has to be real quick
}

void interrupt crtcras()
{
    //whatever you do in this event
}
