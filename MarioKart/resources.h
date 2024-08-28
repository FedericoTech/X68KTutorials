
#ifndef RESOURCES_H
#define RESOURCES_H

#include <stdint.h>

int Res_loadResources();

uint16_t* res_tilemap;

const uint16_t* Res_getTilemap();

int Res_unloadResources();

#endif // RESOURCES_H
