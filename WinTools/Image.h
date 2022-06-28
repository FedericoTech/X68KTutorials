#ifndef _IMAGE_H_
#define _IMAGE_H_

#include "stdafx.h"

class Image {
    public:
        static int saveAs16BitsImage(Bmp bmp, const char *fileDest);

        static int saveAs8BitsImage(Bmp bmp, const char *fileDest);

        static int saveAs4BitsImage(Bmp bmp, const char *fileDest);
};


#endif

