#ifndef _PCGS_H_
#define _PCGS_H_

#include "stdafx.h"

class PCGs
{
    public:
        static int hardcodedPCG(const char* fileDest);

        static int reformatPCG(const char* fileSource, const char* fileDest);
};


#endif
