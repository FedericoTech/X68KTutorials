
#ifndef INPUT_H
#define INPUT_H

#define INPUT_UP 0x1
#define INPUT_DOWN 0x2
#define INPUT_LEFT 0x4
#define INPUT_RIGHT 0x8

#define INPUT_ESC 0x1

extern int input_player_1;
extern int input_player_2;
extern int others;

void Input_processInput();

#endif // INPUT_H
