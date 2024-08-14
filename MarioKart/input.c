#include "stdafx.h"
#include "input.h"

int input_player_1 = 0x0;
int input_player_2 = 0x0;
int others = 0x0;

static void Input_handleKeyPress(int key);

void Input_processInput()
{
    /**
     * Capturing several keys pressed at the same time.
     * We capture the flags of the group of keys the cursor keys belong to.
     * for groups: https://gamesx.com/wiki/doku.php?id=x68000:keycodes
     */
    uint8_t keys = _iocs_bitsns(0x7); //retrieving the state of the keys: CLR	↓	→	↑	←	UNDO	R_DOWN	R_UP

    input_player_1 = 0; //we restart player 1 controls

    //if left (←) key pressed
    if(keys & 0x08){
        input_player_1 |= INPUT_LEFT;
    }

    //if right (→) key pressed
    if(keys & 0x20){
        input_player_1 |= INPUT_RIGHT;
    }

    //if up (↑) key pressed
    if(keys & 0x10){
        input_player_1 |= INPUT_UP;
    }

    //if down (↓) key pressed
    if(keys & 0x40){
        input_player_1 |= INPUT_DOWN;
    }

    //player 2
    input_player_2 = 0; //we restart player 1 controls

    keys = _iocs_bitsns(0x2); //W   UP  //retrieving the state of the keys: U	Y	T	R	E	W	Q	TAB

    //if up (W) key pressed
    if(keys & 0x4){
        input_player_2 |= INPUT_UP;
    }


    keys = _iocs_bitsns(0x3);   //A S, Left & Down //retrieving the state of the keys: S A	CR	[{	@`	P	O	I

    //if left (A) key pressed
    if(keys & 0x40){
        input_player_2 |= INPUT_LEFT;
    }

    //if down (S) key pressed
    if(keys & 0x80){
        input_player_2 |= INPUT_DOWN;
    }


    keys = _iocs_bitsns(0x4);   // D Right /retrieving the state of the keys: ;+ L	K	J	H	G	F	D

    //if right (D) key pressed
    if(keys & 0x1){
        input_player_2 |= INPUT_RIGHT;
    }

    /*
    //other keys
    others = 0;

    keys = _iocs_bitsns(0x0);

    if(keys & 0x2){
        others |= INPUT_ESC;
    }
    */
}

static void Input_handleKeyPress(int key) {
    // Example key handling
    if (key == 'Q') {
        //exitFlag = 1;
    }
}
