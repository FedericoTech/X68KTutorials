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

#define ACCESS_DICTIONARY           0x80    //0b10000000 //1 not for users
#define ACCESS_NORMAL               0x00    //0b00000000 //0

#define SHARING_TOTAL               0x08    //0b00010000  //4 allow others to write and read
#define SHARING_WRITE_ONLY          0x0C    //0b00001100  //3 allow others to write only
#define SHARING_READ_ONLY           0x08    //0b00001000  //2 allow others to read only
#define SHARING_RESTRICTED          0x04    //0b00000100  //1 don't allow others anything
#define SHARING_COMPATIBILITY_MODE  0x00    //0b00000000  //0

#define MODE_RW                     0x02    //0b00000010  //2 open for write and read
#define MODE_W                      0x01    //0b00000001  //1 open only for writing
#define MODE_R                      0x00    //0b00000000  //0 open only for reading

#define OPENING_MODE(access, sharing, mode) (access | sharing | mode)

#define SEEK_MODE_BEGINNING  0
#define SEEK_MODE_CURRENT 1
#define SEEK_MODE_END 2

const char *getErrorMessage(int8_t code);

#endif


