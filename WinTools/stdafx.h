#ifndef _STDAFX_H_
#define _STDAFX_H_

#include <iostream>
#include <fstream>
#include <stdint.h>
#include <cmath>
#include <tr1/functional>

#include "bmp.h"

#define REVERSE_BYTES_32(num) ( ((num & 0xFF000000) >> 24) | ((num & 0x00FF0000) >> 8) | ((num & 0x0000FF00) << 8) | ((num & 0x000000FF) << 24) )
#define REVERSE_BYTES_24(num) ( ((num & 0xFF0000) >> 16) | (num & 0x00FF00) | ((num & 0x0000FF) << 16) )
#define REVERSE_BYTES_16(num) ( ((num & 0xFF00) >> 8) | ((num & 0x00FF) << 8))

#define RGB888_2GRB(r, g, b, i) ( ((b&0xF8)>>2) | ((g&0xF8)<<8) | ((r&0xF8)<<3) | i )
#define RGB888_2RGB(r, g, b, i) ( ((r&0xF8)<<8) | ((g&0xF8)<<3) | ((b&0xF8)>>2) | i )

//using namespace std;

#endif
