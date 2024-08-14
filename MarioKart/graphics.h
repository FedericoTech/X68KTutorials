
#ifndef GRAPHICS_H
#define GRAPHICS_H

extern int frame;

int Graph_settingUpScreen();

int Graph_initializeGraphics();

void Graph_clearScreen();
void Graph_drawSprites();
void Graph_presentFrame();

int Graph_closeGraphics();

#endif // GRAPHICS_H
