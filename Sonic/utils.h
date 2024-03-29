#ifndef _UTILS_H_
#define _UTILS_H_

#include <stdint.h>

#define OPEN_ACCESS_DICTIONARY           0x80    //0b10000000 //1 not for users
#define OPEN_ACCESS_NORMAL               0x00    //0b00000000 //0

#define OPEN_SHARING_TOTAL               0x08    //0b00010000  //4 allow others to write and read
#define OPEN_SHARING_WRITE_ONLY          0x0C    //0b00001100  //3 allow others to write only
#define OPEN_SHARING_READ_ONLY           0x08    //0b00001000  //2 allow others to read only
#define OPEN_SHARING_RESTRICTED          0x04    //0b00000100  //1 don't allow others anything
#define OPEN_SHARING_COMPATIBILITY_MODE  0x00    //0b00000000  //0

#define OPEN_MODE_RW                     0x02    //0b00000010  //2 open for write and read
#define OPEN_MODE_W                      0x01    //0b00000001  //1 open only for writing
#define OPEN_MODE_R                      0x00    //0b00000000  //0 open only for reading

#define OPENING_MODE(access, sharing, mode) (access | sharing | mode)

union FileConf {
    struct {
        int8_t access_dictionary:1;
        int8_t :2;      //padding
        int8_t sharing:3;
        int8_t mode:2;
    } flags;
    int8_t config;
};

#define SEEK_MODE_BEGINNING  0
#define SEEK_MODE_CURRENT 1
#define SEEK_MODE_END 2

const char *getErrorMessage(int8_t code);

uint32_t start_timer();

uint32_t start_stop();

uint32_t millisecond();

#endif
