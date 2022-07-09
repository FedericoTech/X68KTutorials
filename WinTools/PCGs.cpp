#include "PCGs.h"

int PCGs::hardcodedPCG(const char* fileDest)
{
    std::ofstream pcgFileOut;

    uint32_t star_crafts[] = {
        //tile 0
        0x00000000,
        0x0000000d,
        0x0000000d,
        0x000000cb,
        0x000000cb,
        0x000000cb,
        0x003000cd,
        0x003000cd,
        //tile 1
        0x00300bcd,
        0x02320bcd,
        0x0232bbcd,
        0x0232bbcd,
        0x0232bbcd,
        0x023200cd,
        0x055500cd,
        0x00000055,
        //tile 2
        0x00000000,
        0xd0000000,
        0xd0000000,
        0xbc000000,
        0xbc000000,
        0xbc000000,
        0xdc000300,
        0xdc000300,
        //tile 3
        0xdcb00300,
        0xdcb02300,
        0xdcb02300,
        0xdcbb2300,
        0xdcbb2300,
        0xdc002300,
        0xdc005550,
        0x55000000,
    };

    pcgFileOut.open(fileDest, std::ios::binary);

    for (uint32_t buffer : star_crafts){

        buffer = REVERSE_BYTES_32(buffer);

        pcgFileOut.write((char*)&buffer, sizeof(uint32_t));
    }

    pcgFileOut.close();

    return 0;
}

int PCGs::reformatPCG(const char* fileSource, const char* fileDest)
{
    /*
    ofstream pcgFileOut;

    Bmp bmp = Bmp::fromFile(fileSource);

    //bmp.print2();
*/
    return 0;
}
