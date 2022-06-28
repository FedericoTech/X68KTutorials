#ifndef _PALETTES_H_
#define _PALETTES_H_

#include "stdafx.h"

class Bmp; //forward declaration.

class Palettes {

    public:
        static int reformatPalette(const char *fileSource, const char *fileDest);

        static void savePalette(Bmp bmp, const char *fileDest);
};

#endif
