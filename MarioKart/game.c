
#include "stdafx.h"
#include "game.h"
#include "input.h"
//#include "update.h"

#include "graphics.h"
#include "resources.h"

static int exitFlag = 0;

static int32_t status;

void Game_initializeGame()
{
    PRINT_FUNCTION();
    // Initialize resources, graphics, and input systems

    if(status = Graph_settingUpScreen()){
        _dos_c_print(Utils_getErrorMessage(status));
        exitFlag = 1;
    }

    if(status = Res_loadResources()){
        _dos_c_print(Utils_getErrorMessage(status));
        exitFlag = 1;
    }

    if(status = Graph_initializeGraphics()){
        _dos_c_print(Utils_getErrorMessage(status));
        exitFlag = 1;
    }

    //initializeGraphics();
    exitFlag = 0;
}

void Game_handleInput() {
    // Read and process user inputs
    Input_processInput();
}

void Game_updateGame() {
    // Update game logic
    //updatePhysics();
}

void Game_renderGraphics() {
    // Draw game objects
    //clearScreen();
    //drawSprites();
    //presentFrame();
}

void Game_cleanupGame()
{
    // Free resources and perform cleanup
    Graph_closeGraphics();
    Res_unloadResources();
}

int Game_shouldExit() {
    return exitFlag;
}
