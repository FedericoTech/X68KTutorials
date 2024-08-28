
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

#define BG_SHOW 1
#define BG_HIDE 0

#define BG_A 0
#define BG_B 1

#define BG_TM_A 0
#define BG_TM_B 1

#define TILEMAP_A 0xEBC000
#define TILEMAP_B 0xEBE000

#define DMA_DIR_A_TO_B 0
#define DMA_DIR_B_TO_A 1

#define DMA_FIXED 0
#define DMA_PLUS_PLUS 1
#define DMA_MINUS_3 2

#define DMA_MODE(direction, MAC, DAC) (direction << 7 | (MAC << 2) | DAC)

#define DMA_STATUS_IDLE 0x00
#define DMA_STATUS_IN_DMAMOVE_OP 0x8A
#define DMA_STATUS_IN_DMAMOV_A_OP 0x8B
#define DMA_STATUS_IN_DMAMOV_L_OP 0x8C

int frame = 0;

extern uint16_t * res_tilemap;
uint16_t (*tilemap)[128];

static int8_t last_mode;

volatile uint16_t scr_x0 = 0;
volatile uint16_t scr_y0 = 0;

volatile uint16_t scr_x1 = 0;
volatile uint16_t scr_y1 = 0; // h: 256 + 1/2 h: 128 + tile row: 8 = 392

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

void static interrupt crtcras_1p();

void static interrupt crtcras_2p();

void static interrupt vsync_disp_1p();

void static interrupt vsync_disp_2p();

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
        BG_A,       //Background specification (0/1)
        BG_TM_A,    //Specifying a text page (0/1)
        BG_SHOW     //Show / Hide specification (0: Hide 1: Show)
    );

    //this is to illustrate how the union works
    tr.flags.vf = 0;         // VF ON
    tr.flags.hf = 0;         // HF OFF
    tr.flags.palette = 1;    // palette number
    tr.flags.pcg = 0;        // pcg number

    tilemap = (uint16_t(*)[128])res_tilemap;


    {
        //#define TILEMAP_DMA

        #ifdef TILEMAP_DMA
        //Populating the tilemap in video memory by DMA (much faster)
        {
            int count;
            //each chain is a row of tiles
            struct _chain viewport[64];

            //we initialize the chains
            for(count = 0; count < 64; count++){
                //we set the beginning of each row
                viewport[count].addr = (uint32_t *)((uint32_t) res_tilemap + (count * 128 * 2));
                //and the length of each row
                viewport[count].len = 64 * sizeof (uint16_t);
            }

            //we copy a square of 64 x 64 from the tilemap heap to the video memory
            _iocs_dmamov_a(
                viewport,               //  rows from the heap
                (char *)TILEMAP_A,      // into memory
                DMA_MODE(
                    DMA_DIR_A_TO_B,    //from A to B
                    DMA_PLUS_PLUS,     //move the pointer forward as it reads
                    DMA_PLUS_PLUS      //move the pointer forward as it writes
                ),
                64  //64 rows
            );

            //now we simply copy the tilemap A into the tilemap B in video memory
            _iocs_dmamove(
                (char *)TILEMAP_A,          //buffer A, the source
                (char *)TILEMAP_B,          //buffer B, the destination
                DMA_MODE(
                     DMA_DIR_A_TO_B,        //from A to B
                     DMA_PLUS_PLUS,         //move the pointer forward as it reads
                     DMA_PLUS_PLUS          //move the pointer forward as it writes
                ),
                64 * 64 * sizeof(uint16_t)  //size of the memory block we are moving
            );
        }
        #else
        //Populating the tilemap in video memory by _iocs_bgtextst and nested loops (slower)
        {
            uint16_t i, j, cont;
            //we traverse the height
            for(j = 0, cont = 0; j < 64; j++){
                //we traverse the width
                for(i = 0; i < 64; i++, cont++){

                    tr.code = res_tilemap[cont];

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
        #endif // TILEMAP_DMA
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
        vsync_disp_2p,
        0,//0: vertical blanking interval 1: vertical display period
        1
    )){
        return status;
    }

    if(status = _iocs_crtcras(
        crtcras_2p,
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

void static inline updateTilemap(char dir_x, char dir_y, volatile uint16_t *scr_x, volatile uint16_t *scr_y, char tm, char height)
{

    int i, j, aux_tm;
    int tm_x = (*scr_x = (*scr_x + dir_x) & 1023) >> 3; // / 8; //px to tiles
    int tm_y = (*scr_y = (*scr_y + dir_y) & 1023) >> 3; // / 8; //px to tiles

    //if going to the right...
    if(dir_x > 0){

        aux_tm = (tm_x + 33) & 127; //33 columns ahead

        //we draw a column of tiles on the right displaced 32 columns
        for(j = tm_y; j < tm_y + height; j++){

            tr.code = tilemap[j & 127][aux_tm];
            //tr.flags.palette++;

            _iocs_bgtextst(
                tm,
                tm_x + 33, //33 columns ahead
                j,
                tr.code
            );
        }
    //if going to the left...
    } else if(dir_x < 0){

        aux_tm = tm_x & 127;

        //we draw a column of tiles on the left
        for(j = tm_y; j < tm_y + height; j++){

            tr.code = tilemap[j & 127][aux_tm];
            //tr.flags.palette++;

            _iocs_bgtextst(
                tm,
                tm_x,
                j,
                tr.code
            );
        }
    }

    //if going down
    if(dir_y > 0){

        aux_tm = (tm_y + height) & 127;

        for(i = tm_x; i < tm_x + 34; i++){  //from first column to the 34th

            tr.code = tilemap[aux_tm][i & 127];
            //tr.flags.palette++;

            _iocs_bgtextst(
                tm,
                i,
                tm_y + height,
                tr.code
            );
        }

    //if going up
    } else if(dir_y < 0){

        aux_tm = tm_y & 127;

        for(i = tm_x; i < tm_x + 34; i++){ //from first column to the 34th
            tr.code = tilemap[aux_tm][i & 127];
            //tr.flags.palette++;

            _iocs_bgtextst(
                tm,
                i,
                tm_y,
                tr.code
            );
        }
    }
}

void static interrupt crtcras_1p()
{

}

void static interrupt vsync_disp_1p()
{
    char dir_x = 0, dir_y = 0;

    //we move scroll for player 1
    _iocs_bgscrlst(
        VERTICAL_BLANKING_DETECTION | BG_A, //(1 << 31) | 0, // and Background specification (0/1)
        scr_x0, //x
        scr_y0 //y
    );

    // //we set player 2's tilemap in scroll 0
    _iocs_bgctrlst (
        BG_A,  //Background specification (0/1)
        BG_TM_A,  //Specifying a text page (0/1)
        BG_SHOW  //Show / Hide specification (0: Hide 1: Show)
    );

    Game_handleInput();

    //if left (←) key pressed
    if(input_player_1 & INPUT_LEFT){
        dir_x = -1;
    }

    //if right (→) key pressed
    if(input_player_1 & INPUT_RIGHT){
        dir_x = 1;
    }

    //if up (↑) key pressed
    if(input_player_1 & INPUT_UP){
        dir_y = -1;
    }

    //if down (↓) key pressed
    if(input_player_1 & INPUT_DOWN){
        dir_y = 1;
    }

    if(dir_x || dir_y){
        updateTilemap(dir_x, dir_y, &scr_x0, &scr_y0, BG_TM_A, 34);
    }
}

void static interrupt crtcras_2p()
{
    // //we set player 2's tilemap in scroll 0
    _iocs_bgctrlst (
        BG_A,  //Background specification (0/1)
        BG_TM_B,  //Specifying a text page (0/1)
        BG_SHOW  //Show / Hide specification (0: Hide 1: Show)
    );

    //we move scroll for player 2
    _iocs_bgscrlst(
        VERTICAL_BLANKING_DETECTION | BG_A, //(1 << 31) | 0, // and Background specification (0/1)
        scr_x1, //x
        (scr_y1 - 128) & 1023//y
    );
}

void static interrupt vsync_disp_2p()
{
    char dir_x = 0, dir_y = 0;

    // //we set player 2's tilemap in scroll 0
    _iocs_bgctrlst (
        BG_A,  //Background specification (0/1)
        BG_TM_A,  //Specifying a text page (0/1)
        BG_SHOW  //Show / Hide specification (0: Hide 1: Show)
    );

    //we move scroll for player 1
    _iocs_bgscrlst(
        VERTICAL_BLANKING_DETECTION | BG_A, //(1 << 31) | 0, // and Background specification (0/1)
        scr_x0, //x
        scr_y0  //y
    );

    Game_handleInput();

    //if up (↑) key pressed
    if(input_player_1 & INPUT_UP){
        dir_y = -1;
    }

    //if down (↓) key pressed
    if(input_player_1 & INPUT_DOWN){
        dir_y = 1;
    }

    //if left (←) key pressed
    if(input_player_1 & INPUT_LEFT){
        dir_x = -1;
    }

    //if right (→) key pressed
    if(input_player_1 & INPUT_RIGHT){
        dir_x = 1;
    }

    if(dir_x || dir_y){
        updateTilemap(dir_x, dir_y, &scr_x0, &scr_y0, BG_TM_A, 17);  //17
    }

    //player 2
    dir_x = dir_y = 0;

    //if up (W) key pressed
    if(input_player_2 & INPUT_UP){
        dir_y = -1;
    }

    //if down (S) key pressed
    if(input_player_2 & INPUT_DOWN){
        dir_y = 1;
    }

    //if left (A) key pressed
    if(input_player_2 & INPUT_LEFT){
        dir_x = -1;
    }

    //if right (D) key pressed
    if(input_player_2 & INPUT_RIGHT){
        dir_x = 1;
    }

    if(dir_x || dir_y){
        updateTilemap(dir_x, dir_y, &scr_x1, &scr_y1, BG_TM_B, 17);  //17
    }
}
