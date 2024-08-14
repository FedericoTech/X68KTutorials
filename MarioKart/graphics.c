
#include "stdafx.h"

#include "resources.h"
#include "graphics.h"
#include "input.h"


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

int frame = 0;

static int8_t last_mode;

volatile uint16_t scr_x0 = 0;
volatile uint16_t scr_y0 = 0;

volatile uint16_t scr_x1 = 0;
volatile uint16_t scr_y1 = 392; // h: 256 + 1/2 h: 128 + tile row: 8 = 392



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

void static interrupt crtcras();

void static interrupt vsync_disp();

static inline int Graph_settingUpScrolls();

static inline int Graph_settingUpInterruptions();

static inline int Graph_tearDownInterruptions();

static inline int Graph_tearDownScreen();

int Graph_initializeGraphics()
{
    int status;

    PRINT_FUNCTION();


    if(status = Graph_settingUpScrolls()){
        return status;
    }

    if(status = Graph_settingUpInterruptions()){
        return status;
    }

    return 0;
}

int Graph_settingUpScreen()
{
    int status;

    PRINT_FUNCTION();

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

    //we activate the sprites
    _iocs_sp_on();

    return 0;
}

static inline int Graph_settingUpScrolls()
{
    int status;

    PRINT_FUNCTION();

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

    //this is to illustrate how the union works
    tr.flags.vf = 0;         // VF ON
    tr.flags.hf = 0;         // HF OFF
    tr.flags.palette = 1;    // palette number
    tr.flags.pcg = 0;        // pcg number

    {
        uint16_t i, j, cont;
        const uint16_t * tilemap = Res_getTilemap();

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
                return status;
            }
        }
    }

    return 0;
}

void Graph_clearScreen() {
    // Clear the screen
}

void Graph_drawSprites() {
    // Draw game sprites
}

void Graph_presentFrame() {
    // Present the drawn frame
}

int Graph_closeGraphics() {
    // Clean up graphics resources
    int status;

    if(status = Graph_tearDownInterruptions()){
        return status;
    }

    if(status = Graph_tearDownScreen()){
        return status;
    }

    return 0;
}

static inline int Graph_settingUpInterruptions()
{
    int status;

    if(status = _iocs_vdispst(
        vsync_disp,
        0,//0: vertical blanking interval 1: vertical display period
        1
    )){
        return status;
    }

    if(status = _iocs_crtcras(
        &crtcras,
        280 // line
    )){
        return status;
    }

    return 0;
}

static inline int Graph_tearDownInterruptions()
{

    _iocs_crtcras (
        (void *)0,
        0 //int_ luster
    );

    _iocs_vdispst(
        (void *)0,
        0,//0: vertical blanking interval 1: vertical display period
        0
    );

    return 0;
}

static inline int Graph_tearDownScreen()
{
    _iocs_sp_off();

    //we activate the console cursor
    _iocs_b_curon();

    //we restore the video mode
    _iocs_crtmod(last_mode);
}

void static interrupt crtcras()
{
    // we hide player 1's scroll
    _iocs_bgctrlst (
        BG_A,  //Background specification (0/1)
        BG_TM_A,  //Specifying a text page (0/1)
        0   //Show / Hide specification (0: Hide 1: Show)
    );
}

void static interrupt vsync_disp()
{
    Game_handleInput();

    //if up (↑) key pressed
    if(input_player_1 & INPUT_UP){
        scr_y0--;
    }

    //if down (↓) key pressed
    if(input_player_1 & INPUT_DOWN){
        scr_y0++;
    }

    //if left (←) key pressed
    if(input_player_1 & INPUT_LEFT){
        scr_x0--;
    }

    //if right (→) key pressed
    if(input_player_1 & INPUT_RIGHT){
        scr_x0++;
    }

    //we move player 1's scroll
    _iocs_bgscrlst(
        VERTICAL_BLANKING_DETECTION | BG_A, //(1 << 31) | 0, // and Background specification (0/1)
        scr_x0, //x
        scr_y0  //y
    );

    //we show player 1's scroll
    _iocs_bgctrlst (
        BG_A,  //Background specification (0/1)
        BG_TM_A,  //Specifying a text page (0/1)
        1   //Show / Hide specification (0: Hide 1: Show)
    );


    //player 2

    //if up (W) key pressed
    if(input_player_2 & INPUT_UP){
        scr_y1--;
    }

    //if down (S) key pressed
    if(input_player_2 & INPUT_DOWN){
        scr_y1++;
    }

    //if left (A) key pressed
     if(input_player_2 & INPUT_LEFT){
        scr_x1--;
    }

    //if right (D) key pressed
    if(input_player_2 & INPUT_RIGHT){
        scr_x1++;
    }

    //we update player 2's scroll
    _iocs_bgscrlst(
        VERTICAL_BLANKING_DETECTION | BG_B, //(1 << 31) | 0, // and Background specification (0/1)
        scr_x1, //x
        scr_y1  //y
    );

    /*
    if(x0 > 1023){
        int cont;

        for(cont = 0; cont < 63; cont++){
            _iocs_bgtextst(
                BG_TM_A,
                x0 % 8,
                cont,
                49
            );
        }
    }

    if(x1 > 1023){
        int cont;

        for(cont = 0; cont < 63; cont++){
            _iocs_bgtextst(
                BG_TM_B,
                x1 % 8,
                cont,
                49
            );
        }
    }
    */

}
