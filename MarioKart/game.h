#ifndef GAME_H
#define GAME_H

void Game_initializeGame();
void Game_handleInput();
void Game_updateGame();
void Game_renderGraphics();
void Game_cleanupGame();
int Game_shouldExit();

#endif // GAME_H
