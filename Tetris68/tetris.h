#ifndef _TETRIS_H_
#define _TETRIS_H_

#include "utils.h"

#define TILE_0 TILE_USERINDEX       //black
#define TILE_1 TILE_USERINDEX + 1   //green
#define TILE_2 TILE_USERINDEX + 2   //light blue
#define TILE_3 TILE_USERINDEX + 3   //red
#define TILE_4 TILE_USERINDEX + 4   //dark blue
#define TILE_5 TILE_USERINDEX + 5   //light green
#define TILE_6 TILE_USERINDEX + 6   //yellow            //16 + 7
#define TILE_7 TILE_USERINDEX + 7   //grey
#define TILE_8 TILE_USERINDEX + 8   //pink

#define FIELD_WIDTH 10 //12
#define FIELD_HEIGHT 18

#define SCREEN_TITLE 0
#define SCREEN_RECORDS 1
#define SCREEN_SELECT_PLAYER 2
#define SCREEN_GAME 3
#define SCREEN_END 4

#define Y_SCREEN 8

//const uint8_t tetromino[7][16];

//from genesis
typedef uint8_t bool;

#define TRUE 1
#define FALSE 0

#define TILE_USERINDEX 0
#define PAL0 0
#define PAL1 1
#define BG_A 0
#define BG_B 1

#define BUTTON_A 0x20
#define BUTTON_B 0x40

#define BUTTON_UP 0x1
#define BUTTON_DOWN 0x2
#define BUTTON_RIGHT 0x8
#define BUTTON_LEFT 0x4

//from genesis

typedef struct
{
    uint8_t pField[FIELD_WIDTH * FIELD_HEIGHT];
    uint16_t *pScreen;
    uint8_t *pPieceMap;
    uint8_t lines[4];
    uint8_t nPlayer;
    uint8_t nNextPiece;
    uint8_t nCurrentPiece;
    uint8_t nCurrentRotation;
    uint8_t nCurrentX;
	uint8_t nCurrentY;
	uint8_t nSpeed;
	uint8_t nSpeedCount;
	uint8_t nLineAnimation;
	uint16_t timer;
	bool bStarted;
	bool bPressStart;
	bool bForceDown;
	bool bRotateHold;
	uint8_t nPieceCount;
	uint16_t nScore;
	uint16_t nJoy;
	bool bGameOver;
	bool bRepaint;
} PlayerContext;

PlayerContext *players[3];
PlayerContext *context;

uint16_t *pieceAux;

PlayerContext *initContext();

void reset_next_piece(PlayerContext *context);

uint8_t Rotate(uint8_t px, uint8_t py, uint8_t r);

void drawNextPiece(uint8_t nPlayer);

void reset_field(PlayerContext *context);

uint8_t* updateRotation(uint8_t nTetromino, uint8_t nRotation);

bool DoesPieceFitContext(PlayerContext *context, uint8_t rotation, int8_t nPosX, int8_t nPosY);

uint8_t next_empty(PlayerContext *context);

void empty_lines(PlayerContext *context);

void line_animation(PlayerContext *context);

void game_play();

static void joyEvent(uint16_t joy, uint16_t changed, uint16_t state);

uint8_t titleScreen();

uint8_t reordsScreen();

#endif
