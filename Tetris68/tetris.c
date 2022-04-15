#include "tetris.h"

const uint8_t tetromino[7][16] = {
    {
        0, TILE_1,  0,  0,
        0, TILE_1,  0,  0,
        0, TILE_1,  0,  0,
        0, TILE_1,  0,  0
    },{
        0, 0,       TILE_2, 0,
        0, TILE_2,  TILE_2, 0,
        0, TILE_2,  0,      0,
        0, 0,       0,      0
    },{
        0, 0,       0,      0,
        0, TILE_3,  TILE_3, 0,
        0, TILE_3,  TILE_3, 0,
        0, 0,       0,      0
    },{
        0, 0,       TILE_4, 0,
        0, TILE_4,  TILE_4, 0,
        0, 0,       TILE_4, 0,
        0, 0,       0,      0
    },{
        0, TILE_5,  0,      0,
        0, TILE_5,  TILE_5, 0,
        0, 0,       TILE_5, 0,
        0, 0,       0,      0
    },{
        0, TILE_6,  0,      0,
        0, TILE_6,  0,      0,
        0, TILE_6,  TILE_6, 0,
        0, 0,       0,      0
    },{
        0, 0,       TILE_8, 0,
        0, 0,       TILE_8, 0,
        0, TILE_8,  TILE_8, 0,
        0, 0,       0,      0
    }
};

volatile bool draw = FALSE;

PlayerContext *initContext()
{
    PlayerContext *aux = _dos_malloc(sizeof (PlayerContext));


    aux->nCurrentRotation = 0;
    aux->nCurrentX = FIELD_WIDTH / 2;
    aux->nCurrentY = 0;
    aux->nSpeed = 50;
    aux->nSpeedCount = 0;
    aux->nLineAnimation = 0;
    aux->bForceDown = FALSE;
    aux->bRotateHold = FALSE;
    aux->bGameOver = TRUE;
    aux->nPieceCount = 0;
    aux->nScore = 0;
    aux->bRepaint = TRUE;

    aux->pScreen = _dos_malloc(sizeof (uint16_t) * (FIELD_HEIGHT - 1) * (FIELD_WIDTH - 2));

    memset(
        aux->pScreen,
        '\0',
        sizeof (uint16_t) * (FIELD_HEIGHT - 1) * (FIELD_WIDTH - 2)
    );

    reset_next_piece(aux);

    reset_field(aux);

	empty_lines(aux);

    return aux;
} //initContext

void reset_next_piece(PlayerContext *context)
{
    context->pPieceMap = updateRotation(
        context->nCurrentPiece,
        context->nCurrentRotation
    );
} //reset_next_piece

void reset_field(PlayerContext *context)
{
    uint8_t y, x;
    // Board Boundary
    for (y = 0; y < FIELD_HEIGHT; y++){
        for (x = 0; x < FIELD_WIDTH; x++){

			context->pField[y * FIELD_WIDTH + x] = (
                x == 0
                || x == FIELD_WIDTH -1
                || y == FIELD_HEIGHT -1
            ) ? TILE_7 : TILE_0;
		}
	}
} //reset_field

uint8_t Rotate(uint8_t px, uint8_t py, uint8_t r)
{
	uint8_t pi = 0;
	switch (r % 4)
	{
        case 0: // 0 degrees			// 0  1  2  3
            pi = py * 4 + px;			// 4  5  6  7
            break;						// 8  9 10 11
                                        //12 13 14 15

        case 1: // 90 degrees			//12  8  4  0
            pi = 12 + py - (px * 4);	//13  9  5  1
            break;						//14 10  6  2
                                        //15 11  7  3

        case 2: // 180 degrees			//15 14 13 12
            pi = 15 - (py * 4) - px;	//11 10  9  8
            break;						// 7  6  5  4
                                        // 3  2  1  0

        case 3: // 270 degrees			// 3  7 11 15
            pi = 3 - py + (px * 4);		// 2  6 10 14
            break;						// 1  5  9 13
                                        // 0  4  8 12
        }

	return pi;
} //Rotate

void drawNextPiece(uint8_t nPlayer)
{
    //pieceAux = _dos_malloc(sizeof(uint16_t) * 16);

    union tile_register tr;
    tr.code = 0;
    tr.flags.vf = 0;         // VF ON
    tr.flags.hf = 0;         // HF OFF
    tr.flags.palette = 1;    // palette number
    tr.flags.pcg = 0;        // pcg number

    {
        uint8_t x, y, i = 0;
        for(y = 0; y < 4; y++){
            for(x = 0; x < 4; x++){

                tr.flags.pcg = tetromino[players[nPlayer]->nNextPiece][i];

                _iocs_bgtextst(
                    BG_A,
                    (nPlayer * FIELD_WIDTH) + 1 + x,
                    y + 2,
                    tr.code
                );
                ++i;
            }
        }
    }

    //_dos_s_mfree(pieceAux);
} //drawNextPiece

uint8_t *updateRotation(uint8_t nTetromino, uint8_t nRotation)
{
    uint8_t* pPieceMap = _dos_malloc(sizeof(uint8_t) * 16);
    int8_t y, x;

    for(y = 0; y < 4; y++){
        for(x = 0; x < 4; x++){
            pPieceMap[(y * 4) + x] = tetromino[nTetromino][Rotate(x, y, nRotation)];
        }
    }
    return pPieceMap;
} //updateRotation

bool DoesPieceFitContext(PlayerContext *context, uint8_t rotation, int8_t nPosX, int8_t nPosY)
{
    uint8_t *pPieceMap;
    uint8_t py;

    nPosX += context->nCurrentX;
    nPosY += context->nCurrentY;

    //if there is a rotation...
    if(rotation > 0){
        //we create a buffer with the new piece.
        pPieceMap = updateRotation(
            context->nCurrentPiece,
            rotation + context->nCurrentRotation
        );
    //if there is not a rotation...
    } else {
        //we link the buffer
        pPieceMap = context->pPieceMap;
    }

	// All Field cells >0 are occupied
	for (py = 0; py < 4; py++) {
        uint8_t px;
        for (px = 0; px < 4; px++) {
			// Get index into field
			uint8_t fi = (nPosY + py) * FIELD_WIDTH + (nPosX + px);

			// Check that test is in bounds. Note out of bounds does
			// not necessarily mean a fail, as the long vertical piece
			// can have cells that lie outside the boundary, so we'll
			// just ignore them

			//if position y is in legal range...
			if (nPosY + py >= 0 && nPosY + py < FIELD_HEIGHT) {
                //if position x is in legal range...
                if (nPosX + px >= 0 && nPosX + px < FIELD_WIDTH) {

					// In Bounds so do collision check
					if (
                        pPieceMap[(py * 4) + px] != TILE_0 //if the tile is not empty...
                        && context->pField[fi] != TILE_0
                    ){
                        if(rotation > 0){
                            _dos_s_mfree(pPieceMap);
                        }
						return FALSE; // fail on first hit
					}
				} //if position x is in legal range... END
			}//if position y is in legal range... END
		}
	}

	if(rotation > 0){
        _dos_s_mfree(context->pPieceMap);
        context->pPieceMap = pPieceMap;
    }

	return TRUE;
} //DoesPieceFitContext

uint8_t next_empty(PlayerContext *context){
    uint8_t x;

    for(x = 0; x < sizeof(context->lines); x++){
        if(context->lines[x] == 0){
            return x;
        }
    }
    return 4;
}

void empty_lines(PlayerContext *context)
{
    memset(
        context->lines,
        '\0',
        sizeof (context->lines)
    );
} //empty_lines

void line_animation(PlayerContext *context){

    //last animation frame
    if(context->nLineAnimation == 1){
        uint8_t i;
        for(i = 0; i < sizeof (context->lines); i++){
            if(context->lines[i] != 0){
                uint8_t px;
                for (px = 1; px < FIELD_WIDTH - 1; px++){
                    uint8_t py;
                    for (py = context->lines[i]; py > 0; py--){
                        //this is to lower the lines above
                        context->pField[(py * FIELD_WIDTH) + px] = context->pField[((py - 1) * FIELD_WIDTH) + px];
                    }
                    context->pField[px] = 0;
                }
            }
        }
        empty_lines(context);
    //other animation frames
    } else {
        if(context->nLineAnimation > 1){
            uint8_t count = abs(random() % 7);
            uint8_t py ;
            for(py = 0; py < sizeof (context->lines); py++){
                if(context->lines[py] != 0){
                    uint8_t px;
                    // Remove Line, set to =
                    for (px = 2; px < FIELD_WIDTH - 2; px++){
                        if(++count > 8){
                            count = 0;
                        }
                        context->pField[(context->lines[py] * FIELD_WIDTH) + px] = count;
                    }
                }
            }
        }
    }
    --context->nLineAnimation;
    context->bRepaint = TRUE;
} //line_animation



uint8_t titleScreen()
{
    //VDP_clearPlane(BG_A, FALSE);
    //VDP_clearPlane(BG_B, FALSE);

    uint8_t nextScreen = SCREEN_RECORDS;
    uint16_t timer = 0;

    /*
    VDP_drawImage(
        BG_A,
        &image_frameTop,
        0,
        0
    );

    VDP_drawImage(
        BG_A,
        &image_frameButton_presentation,
        0,
        19
    );

    VDP_drawImageEx(
        BG_B,
        &image_CastleGame,
        TILE_ATTR_FULL(PAL1, TRUE, FALSE, FALSE, curTileInd),
        0,
        0,
        TRUE,
        DMA
    );

    DMA_waitCompletion();
    */

    bool bExit = FALSE;

    /*
    void joyEventStart(uint16_t joy, uint16_t changed, uint16_t state)
    {
        // START button state changed --> pause / unpause
        if (changed & state & BUTTON_START)
        {
            switch(joy){
                case JOY_1:
                    players[0]->bGameOver = FALSE;
                    break;
                case JOY_2:
                    players[2]->bGameOver = FALSE;
                    break;
                case JOY_3:
                    players[1]->bGameOver = FALSE;
                    break;
            }
            //nextScreen = SCREEN_GAME; //declared in titleScreen
            //bExit = TRUE; //declared in titleScreen
            //JOY_setEventHandler(NULL);
        }
    }
    */

    //JOY_setEventHandler(joyEventStart);

    while(!bExit){
        //VDP_waitVSync();

        timer++;
        if(timer > 600){
            bExit = TRUE;
            //JOY_setEventHandler(NULL);
        }
    }

    return nextScreen;
} //titleScreen

uint8_t recordsScreen()
{
    //VDP_clearPlane(BG_A, FALSE);
    //VDP_clearPlane(BG_B, FALSE);
    uint8_t nextScreen = SCREEN_TITLE;
    uint16_t timer = 0;

    /*
    VDP_drawImage(
        BG_A,
        &image_frameTop,
        0,
        0
    );

    VDP_drawImage(
        BG_A,
        &image_frameButtonRecords,
        0,
        19
    );

    PAL_setPalette (PAL2, image_recordsScreen.palette->data);

    VDP_drawImageEx(
        BG_B,
        &image_recordsScreen,
        TILE_ATTR_FULL(PAL2, TRUE, FALSE, FALSE, curTileInd),
        4,
        2,
        TRUE,
        DMA
    );

    DMA_waitCompletion();
    */

    bool bExit = FALSE;
    /*
    void joyEventStart(u16 joy, u16 changed, u16 state)
    {
        // START button state changed --> pause / unpause
        if (changed & state & BUTTON_START){
            switch(joy){
                case JOY_1:
                    players[0]->bGameOver = FALSE;
                    break;
                case JOY_2:
                    players[2]->bGameOver = FALSE;
                    break;
                case JOY_3:
                    players[1]->bGameOver = FALSE;
                    break;
            }
            nextScreen = SCREEN_GAME;
            bExit = TRUE;
            JOY_setEventHandler(NULL);
        }
    }

    JOY_setEventHandler(joyEventStart);
    */
    while(!bExit){
        //VDP_waitVSync();

        timer++;
        if(timer > 600){
            bExit = TRUE;
            //JOY_setEventHandler(NULL);
        }
    }

    return nextScreen;
} //recordsScreen

void interrupt vsync_disp()
{
    draw = TRUE;
}

void game_play()
{
    bool bGameOver = FALSE;
    int status;
    uint8_t nPlayer;
    int cont;

    union tile_register tr;

    tr.code = 0;

    tr.flags.vf = 0;         // VF ON
    tr.flags.hf = 0;         // HF OFF
    tr.flags.palette = 1;    // palette number
    tr.flags.pcg = 0;        // pcg number

    //we fill the buffer with this PGC
    status = _iocs_bgtextcl (
       BG_A,   //Background specification (0/1)
       tr.code
    );

    for(nPlayer = 0; nPlayer < 3; nPlayer++) {
        tr.flags.pcg = TILE_7;        // pcg number

        //we draw the walls
        for(cont = Y_SCREEN; cont < Y_SCREEN + FIELD_HEIGHT -1; cont++){
            //we draw left border
            _iocs_bgtextst(
                BG_A,                       //bg
                nPlayer * FIELD_WIDTH,      //x
                cont,                       //y
                tr.code                     //tile
            );

            //we draw the right border
            _iocs_bgtextst(
                BG_A,                                       //bg
                (nPlayer * FIELD_WIDTH) + FIELD_WIDTH -1,   //x
                cont,                                       //y
                tr.code                                     //tile
            );

        }

        //we draw the bottom
        for(cont = 0; cont < FIELD_WIDTH; cont++){
            _iocs_bgtextst(
                BG_A,                           //bg
                (nPlayer * FIELD_WIDTH) + cont, //x
                FIELD_HEIGHT -1 + Y_SCREEN,     //y
                tr.code                         //tile
            );
        }

        //we set the piece of the player who pressed start
        if(!players[nPlayer]->bGameOver){
            //drawNextPiece(nPlayer);
        }
    }

    _iocs_vdispst(
        &vsync_disp,
        0,//0: vertical blanking interval 1: vertical display period
        1
    );

    // Main Loop
    while (!bGameOver){

        uint8_t nPlayer;
        bool bFirst = TRUE;

        //wait for the frame
        while(!draw){
            ;
        }

        //we pool all the players
        for(nPlayer = 0; nPlayer < 2; nPlayer++) {

            uint8_t state = 0;

            state = (~_iocs_joyget(players[nPlayer]->nJoy));

            context = players[nPlayer];

            //if the joystick is valid.. only player 0 and 1
            if(state != 0xFF && state != 0x00){
                //if still gameover...
                if(context->bGameOver){
                    if(state & BUTTON_B){
                        context->bGameOver = FALSE;
                        //return;
                    }
                //if not gameover...
                } else {
                    int8_t xMove = 0;

                    xMove += (
                        state & BUTTON_RIGHT
                        && DoesPieceFitContext(
                            context,
                            0,  //rotation
                            1,
                            0
                        )
                    ) ? 1 : 0; //to the right

                    xMove -= (
                        state & BUTTON_LEFT
                        && DoesPieceFitContext(
                            context,
                            0,  //rotation
                            -1,
                            0
                        )
                    ) ? 1 : 0; //to the left

                    if(xMove != 0){
                        context->nCurrentX += xMove;
                        context->bRepaint = TRUE;
                    }

                    //context->bForceDown = status & BUTTON_DOWN;


                    if (state & BUTTON_A) {
                        context->nCurrentRotation += (
                            context->bRotateHold
                            && DoesPieceFitContext(
                                context,
                                1,  //rotation
                                0,
                                0
                            )
                        ) ? 1 : 0;
                        context->bRepaint = TRUE;

                        context->bRotateHold = FALSE;
                    } else {
                        context->bRotateHold = TRUE;
                    }
                } //if not gameover... END
            } //if players 1 or 2... END
        } //we pool all the players END

        for(nPlayer = 0; nPlayer < 3; nPlayer++) {
            context = players[nPlayer];

            //if gameover...
            if(context->bGameOver){
                if(context->timer > 60){
                    uint8_t x = (nPlayer * 10) + 2;
                    //if pressed start...
                    if(context->bPressStart){
                        //delete text
                        context->bPressStart = FALSE;

                        _dos_c_locate(
                            x,
                            7
                        );
                        _iocs_b_print("     ");

                        _dos_c_locate(
                            x,
                            8
                        );
                        _iocs_b_print("     ");
                    //if not pressed yet...
                    } else {
                        context->bPressStart = TRUE;
                        //print text
                        _dos_c_locate(
                            x,
                            7
                        );
                        _iocs_b_print("press");

                        _dos_c_locate(
                            x,
                            8
                        );
                        _iocs_b_print("start");
                    }
                    _dos_c_locate( 100, 100 );
                    _iocs_b_print("\n");

                    context->timer = 0;
                } else {
                    context->timer++;
                }
                continue;
            //if not gameover...
            } else {
                //if is not gameover but it hasn't started yet...
                if(!context->bStarted){
                    uint8_t x = (nPlayer * 10) + 2;
                    //delete screen
                    memset(
                        context->pScreen,
                        '\0',
                        sizeof (uint16_t) * (FIELD_HEIGHT - 1) * (FIELD_WIDTH - 2)
                    );

                    //reset_next_piece(context);
                    reset_field(context);
                    empty_lines(context);
                    context->timer = 0;

                    context->nCurrentPiece = context->nNextPiece;
                    context->nNextPiece = abs(random() % 7);

                    drawNextPiece(context->nPlayer);

                    //delete text
                    _dos_c_locate(
                        x,
                        7
                    );
                    _iocs_b_print("     ");

                    _dos_c_locate(
                        x,
                        8
                    );
                    _iocs_b_print("     ");

                    context->bStarted = TRUE;
                    context->bRepaint = TRUE;
                //if it already started playing...
                } else {

                    //if there is an animation on
                    if(context->nLineAnimation > 0){
                        line_animation(context);
                        continue;
                    //if playing...
                    } else {
                        uint16_t value;
                        uint8_t state = 0;

                        //we calculate the time
                        ++context->nSpeedCount;

                        //state = JOY_readJoypad(context->nJoy);

                        if(nPlayer < 3){
                            state = (~_iocs_joyget(players[nPlayer]->nJoy));
                        }

                        context->bForceDown = (context->nSpeedCount == context->nSpeed)
                            || state & BUTTON_DOWN ;

                        //time to go down
                        if (context->bForceDown) {
                            // Update difficulty every 50 pieces
                            context->nSpeedCount = 0;
                            ++context->nPieceCount;

                            if (context->nPieceCount % 50 == 0){ //50
                                if (context->nSpeed >= 10){
                                    --context->nSpeed;
                                }
                            }

                            // Test if piece can be moved down
                            if (
                                DoesPieceFitContext(
                                    context,
                                    0,  //rotation
                                    0,
                                    1
                                )
                            ) {
                                ++context->nCurrentY; // It can, so do it!
                                context->bRepaint = TRUE;
                            // Test if piece can't be moved down
                            } else {
                                uint8_t py;
                                //we draw the piece in the field
                                for (py = 0; py < 4; py++){
                                    uint8_t px;
                                    for (px = 0; px < 4; px++){

                                        uint8_t tile = context->pPieceMap[(py * 4) + px];

                                        if (tile != TILE_0){
                                            context->pField[((context->nCurrentY + py) * FIELD_WIDTH) + context->nCurrentX + px] = tile;
                                        }
                                    }
                                }

                                // Check for lines
                                for (py = 0; py < 4; py++){
                                    if(context->nCurrentY + py < FIELD_HEIGHT - 1){
                                        bool bLine = TRUE;
                                        uint8_t px;

                                        for (px = 1; px < FIELD_WIDTH - 1; px++){
                                            bLine &= (context->pField[(context->nCurrentY + py) * FIELD_WIDTH + px]) > 0;
                                        }

                                        if (bLine) {
                                            // Remove Line, set to =
                                            for (px = 1; px < FIELD_WIDTH - 1; px++){
                                                context->pField[(context->nCurrentY + py) * FIELD_WIDTH + px] = px;
                                            }

                                            context->lines[next_empty(context)] = context->nCurrentY + py;
                                        }
                                    }
                                }

                                //KLog_U1("next_empty()B ", next_empty(context));
                                context->nScore += 25;

                                if(next_empty(context)>0){
                                    context->nScore += (1<<next_empty(context))*100;
                                }

                                // Pick New Piece
                                context->nCurrentX = FIELD_WIDTH / 2;
                                context->nCurrentY = 0;
                                context->nCurrentRotation = 0;
                                context->nCurrentPiece = context->nNextPiece;
                                context->nNextPiece = abs(random() % 7);

                                _dos_s_mfree(context->pPieceMap);

                                context->pPieceMap = updateRotation(
                                    context->nCurrentPiece,
                                    context->nCurrentRotation
                                );

                                // If piece does not fit straight away, game over!
                                context->bGameOver = !DoesPieceFitContext(
                                    context,
                                    0, //rotation
                                    0,
                                    0
                                );

                                //not cameover yet...
                                if(!context->bGameOver){
                                    //printf("drawNextPiece\n");

                                    drawNextPiece(context->nPlayer);
                                //it's gameover...
                                } else {

                                    //printf("             \n");
                                    //delete screen
                                    context->bStarted = FALSE;
                                }

                            } // Test if piece can't be moved down END

                            context->bRepaint = TRUE;
                        } ////time to go down END
                    } ////if playing... END
                } ////if it already started playing... END
            } ////if not gameover... END

            //repaint?
            if(context->bRepaint){
                uint8_t y;
                uint8_t px;


                union tile_register tr;
                tr.code = 0;
                tr.flags.vf = 0;         // VF ON
                tr.flags.hf = 0;         // HF OFF
                tr.flags.palette = 1;    // palette number
                tr.flags.pcg = TILE_0;        // pcg number


                // Draw Field
                for (y = 0; y < FIELD_HEIGHT - 1; y++){
                    uint8_t x;
                    for (x = 1; x < FIELD_WIDTH - 1; x++){

                        tr.flags.pcg = context->pField[(y * FIELD_WIDTH) + x];

                        context->pScreen[(y * (FIELD_WIDTH - 2)) + x -1] = tr.code;
                    }
                }

                // Draw Current Piece
                for (px = 0; px < 4; px++){
                    uint8_t py;
                    for (py = 0; py < 4; py++){
                        if(
                           context->nCurrentX + px > 0
                           && context->nCurrentX + px < 11
                        ){
                            uint8_t tile = context->pPieceMap[(py * 4) + px];

                            if(tile != TILE_0){
                                tr.flags.pcg = tile;

                                context->pScreen[((context->nCurrentY + py) * (FIELD_WIDTH - 2)) + (context->nCurrentX + px -1)]
                                    = tr.code;
                            }
                        }
                    }
                }
            }//repaint? END

            // Animate Line Completion
            if (next_empty(context)>0) {
                context->nLineAnimation = 4;
            }
        } //we pool all the players...END



        bFirst = TRUE;



        for(nPlayer = 0; nPlayer < 3; nPlayer++){
            //repaint?
            if(players[nPlayer]->bRepaint){
                uint8_t x, y, i = 0;

                union tile_register tr;
                tr.code = 0;
                tr.flags.vf = 0;         // VF ON
                tr.flags.hf = 0;         // HF OFF
                tr.flags.palette = 1;    // palette number
                tr.flags.pcg = TILE_0;        // pcg number



                for(y = Y_SCREEN; y < Y_SCREEN + FIELD_HEIGHT - 1; y++){
                    for(x = (nPlayer * FIELD_WIDTH) + 1; x < (nPlayer * FIELD_WIDTH) + FIELD_WIDTH - 1; x++){

                        tr.flags.pcg = players[nPlayer]->pScreen[i];

                        //_dos_c_locate(0,2);
                        //printf("x: %d, y: %d, i: %d, tile: %#x\n", x, y, i, tr.code);

                        _iocs_bgtextst(
                            BG_A,
                            x,
                            y,
                            tr.code
                        );
                        ++i;
                    }
                }

                players[nPlayer]->bRepaint = FALSE;
                bFirst = FALSE;
            }//repaint? END
        }


        //SYS_enableInts();

        //KLog_U1("FPS: ", getFPS());

        if(
           players[0]->bGameOver
           && players[1]->bGameOver
           && players[2]->bGameOver
        ){

           bGameOver = TRUE;
           //JOY_setEventHandler(NULL);
       }

       draw = FALSE;
    } //// Main Loop

    //_dos_c_locate(10, 10);
    //we delete any text left
    //VDP_clearTextLineBG(BG_B, 13);
    //VDP_clearTextLineBG(BG_B, 14);

    return;
} //game_play
