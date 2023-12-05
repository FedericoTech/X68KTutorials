#ifdef __MARIKO_CC__
	#include <doslib.h>
	#include <iocslib.h>
#else
	#include <dos.h>
	#include <iocs.h>
	#define interrupt __attribute__ ((interrupt_handler))

	#define _tboxptr iocs_tboxptr
	#define _txfillptr iocs_txfillptr
	#define _trevptr iocs_trevptr
	#define _xlineptr iocs_xlineptr
	#define _ylineptr iocs_ylineptr
	#define _tlineptr iocs_tlineptr
#endif

char volatile mutex = 0;

int x = 0;
int y = 0;

void interrupt update()
{
    if(!mutex){
        int color;
        mutex = 1;


        _iocs_scroll(
            /* mode */  8,      //0, 1, 2, 3 set graphic pages, 8 set text page. 4, 5, 6, 7 get coordenads graphic pages and 9 get coordenads text page. coordenads in register d0. hw X, lw Y.
            /* x */     ++x,
            /* y */     ++y
        );

        color = _iocs_tpalet2(x, -1);

        _iocs_tpalet2(x, ++color);

        mutex = 0;
    }
}

int main(void)
{
    struct _tboxptr tboxptr = {
        /* unsigned short vram_page */  0x800f,
        /* short x */                   10,
        /* short y */                   100,
        /* short x1 */                  50,
        /* short y1 */                  50,
        /* unsigned short line_style */ 0xCA
    };

    struct _txfillptr txfillptr = {
        /* unsigned short vram_page */  1, //0, 1, 2, 4
        /* short x */                   70,
        /* short y */                   100,
        /* short x1 */                  50,
        /* short y1 */                  50,
        /* unsigned short fill_patn */  0xF0F0
    };

    struct _trevptr trevptr = {
        /* unsigned short vram_page */  2,
        /* short x */                   130,
        /* short y */                   100,
        /* short x1 */                  50,
        /* short y1 */                  50
    };

    struct _xlineptr xlineptr = {
        /* unsigned short vram_page */  3,
        /* short x */                   190,
        /* short y */                   100,
        /* short x1 */                  50,
        /* unsigned short line_style */ 0xCA
    };

    struct _ylineptr ylineptr = {
        /* unsigned short vram_page */  4,
        /* short x */                   250,
        /* short y */                   100,
        /* short y1 */                  50,
        /* unsigned short line_style */ 0xCA
    };

    struct _tlineptr tlineptr = {
        /* unsigned short vram_page */  5,  //0, 1, 2, 4
        /* short x */                   260,
        /* short y */                   100,
        /* short x1 */                  50,
        /* short y1 */                  50,
        /* unsigned short line_style */ 0xCA
    };





    //clear the screen
    _iocs_b_clr_st();

    _iocs_txbox(&tboxptr);

    _iocs_txfill(&txfillptr);

    _iocs_txrev(&trevptr);

    _iocs_txxline(&xlineptr);

    _iocs_txyline(&ylineptr);

    _iocs_txline(&tlineptr);

    _iocs_timerdst(
        update,
        7,  // mode 7 = 50.0 sec
        0  // every 20 times the unit set in the mode
    );

    while(1){
        //if any key hit...
        if(_dos_inpout(0xFF) != 0){
            break;
        }
    }

    _iocs_timerdst(
        (void *)0,   //Processing address (interrupt disabled at 0)
        0,              //Unit time (1 = 1.0, 2 = 2.5, 3 = 4.0, 4 = 12.5, 5 = 16.0, 6 = 25.0, 7 = 50.0, micro sec unit)
        0               //Counter (when 0, treat as 256)
    );

    _iocs_scroll(
        /* mode */  8,      //0, 1, 2, 3 set graphic pages, 8 set text page. 4, 5, 6, 7 get coordenads graphic pages and 9 get coordenads text page. coordenads in register d0. hw X, lw Y.
        /* x */     0,
        /* y */     0
    );


    _dos_c_print("Done.\r\n");

    _dos_exit();
}
