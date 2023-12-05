#ifdef __MARIKO_CC__
	#include <doslib.h>
	#include <iocslib.h>
#else
	#include <dos.h>
	#include <iocs.h>
	#define interrupt __attribute__ ((interrupt_handler))

	#define _txfillptr iocs_txfillptr

	#define random rand
#endif

const unsigned short colours[] = {
    0x0,    //black
    0x5294, //dark-grey
    0x20,   //deep-blue
    0xfffe, //light blue
    0x400,  //dark red
    0x70c,  //bright red
    0x420,  //dark purple
    0x7fe,  //bright purple
    0x8000, //dark green
    0xf800, //light green
    0x8020, //dark cyan
    0xf83e, //bright cyan
    0x8400, //dark yellow
    0xffc0, //bright yellow
    0xad6a, //dark white
    0xe3    //bright white
};

volatile char mutex = 0, mutex2 = 0;

int x = 0;

//this interruption moves the screen to the left
void interrupt vsync_disp()
{
    if(!mutex){
        mutex = 1;

        _iocs_scroll(
            /* mode */  8,      //0, 1, 2, 3 set graphic pages, 8 set text page. 4, 5, 6, 7 get coordenads graphic pages and 9 get coordenads text page. coordenads in register d0. hw X, lw Y.
            /* x */     ++x,
            /* y */     0
        );

        mutex = 0;
    }
}

//this interruption keeps the scroll still bellow certain height
void interrupt crtcras()
{
    if(!mutex2){
        mutex2 = 1;

        _iocs_scroll(
            /* mode */  8,      //0, 1, 2, 3 set graphic pages, 8 set text page. 4, 5, 6, 7 get coordenads graphic pages and 9 get coordenads text page. coordenads in register d0. hw X, lw Y.
            /* x */     0,
            /* y */     0
        );

        mutex2 = 0;
    }
}


int main(void)
{
    int cont;

    unsigned short colours_backup[16];

    for(cont = 0; cont < sizeof(colours) / sizeof(colours[0]); cont++){

        //we backup the original color
        colours_backup[cont] = _iocs_tpalet(cont, -1);

        //this function only has a palette of 7 colours
        /*
        _iocs_tpalet(
             cont,  // 0 = 0, 1 = 1, 2 = 2, 3 = 3, [4 - 7] = [4 - 7], [8 - 15] = [8 - 15]
             colours[cont] //colour code
        );
        */

        //this function can set up to 16 colours independently
        _iocs_tpalet2(
            cont,
            colours[cont]
        );
    }

    //we set the interruption that moves the screen
    _iocs_vdispst(
        vsync_disp,
        0, //0: vertical blanking interval 1: vertical display period
        1
    );

    //we set the interruption that keeps the scroll still bellow line 502
    _iocs_crtcras (
        crtcras,
        512 - 10 //int_ luster
    );

    //main loop
    while(1){

        struct _txfillptr txfillptr = {
            /* unsigned short vram_page */  0x8000 | 0xf,   // this is to remove the space the rectangle is going to take
            /* short x */                   abs(random() % (1024 - 100)),
            /* short y */                   abs(random() % (512 - 140)),
            /* short x1 */                  abs(random() % 100),
            /* short y1 */                  abs(random() % 100),
            /* unsigned short fill_patn */  0x0             // this is to remove the space the rectangle is going to take
        };

        _iocs_txfill(&txfillptr); //we clear the space.

        txfillptr.vram_page = 0x8000 | abs(random() % 15); //now we pick a colour from the palette.
        txfillptr.fill_patn = 0xffff;   //we set that we are going to paint
        _iocs_txfill(&txfillptr);       //we now paint the rectangle

        //if we hit any key...
        if(_dos_inpout(0xff) != 0){

            //we clear the paint
            txfillptr.vram_page = 0x8000 | 0xf;
            txfillptr.x = 0;
            txfillptr.y = 0;
            txfillptr.x1 = 768;
            txfillptr.y1 = 512 - 40;
            txfillptr.fill_patn = 0;

            _iocs_txfill(&txfillptr);

            //we exit the loop
            break;
        }
    }

    //we remove the interruption
    _iocs_vdispst(
        (void *)0,
        0, //0: vertical blanking interval 1: vertical display period
        0
    );

    //we remove the interruption
    _iocs_crtcras (
        (void *)0,
        0 //int_ luster
    );

    //we set the screen back to its place
    _iocs_scroll(
        /* mode */  8,      //0, 1, 2, 3 set graphic pages, 8 set text page. 4, 5, 6, 7 get coordenads graphic pages and 9 get coordenads text page. coordenads in register d0. hw X, lw Y.
        /* x */     0,
        /* y */     0
    );

    //we re establish the colours
    for(cont = 0; cont < sizeof(colours) / sizeof(colours[0]); cont++){
        _iocs_tpalet2(
             cont,
             colours_backup[cont]
        );
    }

    _dos_exit();
}
