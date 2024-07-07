#ifdef __MARIKO_CC__
	#include <doslib.h>
	#include <iocslib.h>
#else
	#include <dos.h>
	#include <iocs.h>
	#include <stdio.h>
	#define interrupt __attribute__ ((interrupt_handler))
#endif

#include <stdint.h>

#include "utils.h"

#include <stdint.h>

//sprite palette utils
#define VERTICAL_BLANKING_DETECTION 0x80000000     //0b10000000000000000000000000000000
#define VERTICAL_BLANKING_NO_DETECT 0

#define SET_VBD_V(vbd, v) (vbd | v)

#define S_PALETTE_START 0xe82220

#define SP_DEFCG_8X8_TILE 0
#define SP_DEFCG_16X16_TILE 1

#define BG_A 0
#define BG_B 1

#define BG_TM_A 0
#define BG_TM_B 1

volatile uint16_t x0 = 0;
volatile uint16_t y0 = 0;

volatile uint16_t x1 = 0;
volatile uint16_t y1 = 392; // h: 256 + 1/2 h: 128 + tile row: 8 = 392

void interrupt vsync_disp()
{
    /**
     * Capturing several keys pressed at the same time.
     * We capture the flags of the group of keys the cursor keys belong to.
     * for groups: https://gamesx.com/wiki/doku.php?id=x68000:keycodes
     */
    uint8_t keys = _iocs_bitsns(0x7); //retrieving the state of the keys: CLR	↓	→	↑	←	UNDO	R_DOWN	R_UP

    if(
       keys & 0x08  //if left (←) key pressed
       //&& x0 - 1 >= 0
    ){
        x0--;
    }

    if(
        keys & 0x20 //if right (→) key pressed
        //&& x0 + 1 <= 512
    ){
        x0++;
    }

    if(
        keys & 0x10 //if up (↑) key pressed
        //&& y0 - 1 >= 0
    ){
        y0--;
    }

    if(
        keys & 0x40 //if down (↓) key pressed
        //&& y0 + 1 <= 512
    ){
        y0++;
    }

    //we move player 1's scroll
    _iocs_bgscrlst(
        VERTICAL_BLANKING_DETECTION | BG_A, //(1 << 31) | 0, // and Background specification (0/1)
        x0, //x
        y0  //y
    );

    //we show player 1's scroll
    _iocs_bgctrlst (
        BG_A,  //Background specification (0/1)
        BG_TM_A,  //Specifying a text page (0/1)
        1   //Show / Hide specification (0: Hide 1: Show)
    );


    //player 2

    keys = _iocs_bitsns(0x2); //W   UP  //retrieving the state of the keys: U	Y	T	R	E	W	Q	TAB

    if(
        keys & 0x4 //if up (W) key pressed
        //&& y1 - 1 >= 0
    ){
        y1--;
    }


    keys = _iocs_bitsns(0x3);   //A S, Left & Down //retrieving the state of the keys: S A	CR	[{	@`	P	O	I

     if(
       keys & 0x40  //if left (A) key pressed
       //&& x1 - 1 >= 0
    ){
        x1--;
    }

    if(
        keys & 0x80 //if down (S) key pressed
        //&& y1 + 1 <= 512
    ){
        y1++;
    }


    keys = _iocs_bitsns(0x4);   // D Right /retrieving the state of the keys: ;+ L	K	J	H	G	F	D

    if(
        keys & 0x1 //if right (D) key pressed
        //&& x1 + 1 <= 512
    ){
        x1++;
    }

    //we hide player 2's scroll
    _iocs_bgctrlst (
        BG_B,  //Background specification (0/1)
        BG_TM_B,  //Specifying a text page (0/1)
        0   //Show / Hide specification (0: Hide 1: Show)
    );

    //we update player 2's scroll
    _iocs_bgscrlst(
        VERTICAL_BLANKING_DETECTION | BG_B, //(1 << 31) | 0, // and Background specification (0/1)
        x1, //x
        y1  //y
    );
}

void interrupt crtcras()
{
    // we hide player 1's scroll
    _iocs_bgctrlst (
        BG_A,  //Background specification (0/1)
        BG_TM_A,  //Specifying a text page (0/1)
        0   //Show / Hide specification (0: Hide 1: Show)
    );

    // we show player 2's scroll
    _iocs_bgctrlst (
        BG_B,  //Background specification (0/1)
        BG_TM_B,  //Specifying a text page (0/1)
        1   //Show / Hide specification (0: Hide 1: Show)
    );
}

int main(void)
{
    int32_t status;
    int8_t last_mode;
    uint16_t* tilemap;

    //retrieve current mode
    last_mode = _iocs_crtmod(-1);

    //                              Actual  Viewport    Colours     Pages   Frequency
    //status = _iocs_crtmod(0); //  1024    512 x 512   16          1       31 KkHz
    //status = _iocs_crtmod(1); //  1024    512 x 512   16          1       15 KkHz

    //status = _iocs_crtmod(2); //  1024    256 x 256   16          1       31 KkHz
    //status = _iocs_crtmod(3); //  1024    256 x 256   16          1       15 KkHz

    //status = _iocs_crtmod(4); //  512     512 x 512   16          4       31 KkHz
    //status = _iocs_crtmod(5); //  512     512 x 512   16          4       15 KkHz

    status = _iocs_crtmod(6); //  512     256 x 256   16          4       31 KkHz
    //status = _iocs_crtmod(7); //  512     256 x 256   16          4       15 KkHz

    _iocs_b_curoff();   //disable the cursor

    _iocs_g_clr_on();   //start graphic system

    //we initialise the sprites
    _iocs_sp_init();

    {
        int16_t file_handler;
        union FileConf fconf;


        fconf.config = 0; //reset the whole union
        fconf.flags.access_dictionary = ACCESS_NORMAL;
        fconf.flags.sharing = SHARING_COMPATIBILITY_MODE;
        fconf.flags.mode = MODE_R;


        {
            // buffer for 3 palettes
            uint16_t s_colours[3][16] = {0};

            //we open the palette file
            file_handler = _dos_open(
                "tiled.pal",
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
            status = _dos_read(file_handler, (char*)&s_colours, sizeof s_colours);

            if(status > 0){

                uint8_t colour_in_palette, palette_index;
                for(palette_index = 0; palette_index < 3; palette_index++){
                    for(colour_in_palette = 0; colour_in_palette < 16; colour_in_palette++){
                         status = _iocs_spalet( //returns 32 bit integer
                            SET_VBD_V(
                                VERTICAL_BLANKING_DETECTION,
                                colour_in_palette   //0 - 15 if higher it only takes from 0-15
                            ),
                            palette_index + 1,  //the first palette for tiles is 1. 0 is the palette for text
                            s_colours[palette_index][colour_in_palette]
                        );

                        //if any issue...
                        if(status < 0){
                            _dos_c_print("The graphic screen is not initialized \r\n");
                        }
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

        {
            uint8_t tile[32];
            uint8_t cont;

            //we open the palette file
            file_handler = _dos_open(
                "tiled.ts",
                fconf.config
            );

            //if any error...
            if(file_handler < 0){
                _dos_c_print("Can't open the file\r\n");
                _dos_c_print(getErrorMessage(file_handler));
                _iocs_crtmod(last_mode);
                _dos_exit2(file_handler);
            }

            //we go through the 84 tiles of the tileset
            for(cont = 0; cont < 84; cont++){
                //we read current tile
                status = _dos_read(file_handler, (char*) tile, sizeof tile);

                //we load the tile as an 8 x 8 tile in "cont" position
                status = _iocs_sp_defcg(
                    cont,               // position in 8 x 8 tiles
                    SP_DEFCG_8X8_TILE,  // 8 x 8 tile = 1
                    tile                // pointer to the data
                );
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

        //we activate the sprites
        _iocs_sp_on();

        /**
         * setting scroll 0 with tilemap 0 for player 1
         */
        status = _iocs_bgctrlst (
            BG_A,  //Background specification (0/1)
            BG_TM_A,  //Specifying a text page (0/1)
            1   //Show / Hide specification (0: Hide 1: Show)
        );

        /**
         * setting scroll 1 with tilemap 1 for player 2
         */
        status = _iocs_bgctrlst (
            BG_B,  //Background specification (0/1)
            BG_TM_B,  //Specifying a text page (0/1)
            1   //Show / Hide specification (0: Hide 1: Show)
        );

        {
            uint16_t i, j;
            uint16_t cont;
            {
                //we make room for a 128x128-tile tilemap
                tilemap = (uint16_t *) _dos_malloc(128 * 128 * sizeof(uint16_t));

                //we open the palette file
                file_handler = _dos_open(
                    "tiled.tm",
                    fconf.config
                );

                //if any error...
                if(file_handler < 0){
                    _dos_c_print("Can't open the file\r\n");
                    _dos_c_print(getErrorMessage(file_handler));
                    _iocs_crtmod(last_mode);
                    _dos_exit2(file_handler);
                }

                status = _dos_read(file_handler, (char*) tilemap, 128 * 128 * sizeof(uint16_t));

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

            {
            //this is to illustrate the format in which each cell of the tilemap is expressed
                union {
                    struct {
                        int8_t vf:1;
                        int8_t hf:1;
                        int8_t :2;           //padding
                        int8_t palette:4;
                        int8_t pcg;
                    } flags;
                    uint16_t code;
                } tr;

                //this is to illustrate how the union works
                tr.flags.vf = 0;         // VF ON
                tr.flags.hf = 0;         // HF OFF
                tr.flags.palette = 1;    // palette number
                tr.flags.pcg = 0;        // pcg number

                //we traverse the height
                for(j = 0, cont = 0; j < 64; j++){
                    //we traverse the width
                    for(i = 0; i < 64; i++, cont++){

                        tr.code = tilemap[cont];

                        //we store the same data en both tilemaps
                        _iocs_bgtextst(
                            BG_TM_A,
                            i,
                            j,
                            tr.code
                        );

                        _iocs_bgtextst(
                            BG_TM_B,
                            i,
                            j,
                            tr.code
                        );
                    }
                    //back to the row

                    //we skip the next 64 columns as the tilemap is only 64 x 64
                    cont += 64;

                    //if any error...
                    if(status < 0){
                        _dos_c_print("Can't open the file\r\n");
                        _dos_c_print(getErrorMessage(status));
                        _iocs_crtmod(last_mode);
                        _dos_exit2(file_handler);
                    }
                }
            }
        }
    }

    {

        //status = _iocs_window (100, 100, 412, 412);

        _iocs_vdispst(
            vsync_disp,
            0,//0: vertical blanking interval 1: vertical display period
            1
        );

        _iocs_crtcras(
            &crtcras,
            280 // line
        );


        //as long a we don't press ESCAPE
        while(_dos_inkey() != 27){
            ;
        }

        _iocs_crtcras (
            (void *)0,
            0 //int_ luster
        );

        _iocs_vdispst(
            (void *)0,
            0,//0: vertical blanking interval 1: vertical display period
            0
        );
    }

    _dos_mfree(tilemap);

    //we activate the console cursor
    _iocs_b_curon();

    //waiting for a keystroke.
    _dos_getchar();

    //we restore the video mode
    _iocs_crtmod(last_mode);

    _dos_exit();
}
