#include "stdafx.h"

#include "game.h"
#include "input.h"


int main(void)
{
    int32_t status;
    int key;

    PRINT_FUNCTION();

    Game_initializeGame();

    if(Game_shouldExit()){
        Game_cleanupGame();
        _dos_exit2(1);
    }

    //status = _iocs_window (100, 100, 412, 412);

    //as long a we don't press ESCAPE
    while(_dos_inkey() != 27){
    //while(!others){
        //Game_handleInput();
        //input_player_1 = input_player_2 = 0x0;
        ;
    }

    //waiting for a keystroke.
    _dos_getchar();

    Game_cleanupGame();
    _dos_exit();
}
