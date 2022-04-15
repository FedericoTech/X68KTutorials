#include "utils.h"
#include "tetris.h"

//void shut_system();

//void interrupt vsync_disp();

int main(void)
{
    int status;

    union FileConf fconf;

/*
    uint8_t state;
    while(_dos_inpout(0xFF) == 0){
        state = (~_iocs_joyget(1));
        _dos_c_locate(0,0);
        printf("up %#x        \n", state & 0x01 );
        printf("down %#x        \n", state & 0x02 );
        printf("left %#x        \n", state & 0x04 );
        printf("right %#x        \n", state & 0x08 );

        printf("button 1 %#x        \n", state & 0x20 );
        printf("button 2 %#x        \n", state & 0x40 );
    }
    _dos_exit();
*/

    fconf.config = 0;
    fconf.flags.access_dictionary = ACCESS_NORMAL;
    fconf.flags.sharing = SHARING_COMPATIBILITY_MODE;
    fconf.flags.mode = MODE_R;

    init_system();

    loadData((char*)S_PALETTE_START, "tetris.pal", fconf.config);

    loadData((char*)PCG_START, "tetris.til", fconf.config);

    /*
     * here I'm setting the scroll 1 to work with the tilemap 0 and to
     * make it shown.
     * In this example I demonstrate that the tilemap can be either
     */
    status = _iocs_bgctrlst (
        1,  //Background specification (0/1)
        0,  //Specifying a text page (0/1)
        1   //Show / Hide specification (0: Hide 1: Show)
    );

    players[0] = initContext();
    players[0]->nJoy = 0;
    players[0]->nPlayer = 0;

    players[1] = initContext();
    players[1]->nJoy = 1;
    players[1]->nPlayer = 1;

    players[2] = initContext();
    players[2]->nJoy = 2;
    players[2]->nPlayer = 2;

    players[1]->bGameOver = FALSE;

    game_play();

    _dos_c_print("Press a key.\r\n");

    //waiting for a keystroke.
    _dos_getchar();

    shut_system();
    _dos_exit();
} //main


