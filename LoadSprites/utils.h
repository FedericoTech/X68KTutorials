
#ifndef _UTILS_H_
#define _UTILS_H_

#ifdef __MARIKO_CC__
    #include <doslib.h>
    #include <iocslib.h>
#else
    #include <dos.h>
    #include <iocs.h>
    #define interrupt __attribute__ ((interrupt_handler))
#endif

#include <stdint.h>

//sprite palette utils
#define VERTICAL_BLANKING_DETECTION 0x80000000     //0b10000000000000000000000000000000
#define VERTICAL_BLANKING_NO_DETECT 0

#define SET_VBD_V(vbd, v) (vbd | v)

#define SP_DEFCG_8X8_TILE 0
#define SP_DEFCG_16X16_TILE 1

int32_t loadPalette(void);

int8_t loadPCGs(void);

#endif
