#ifndef _UTILS_H_
#define _UTILS_H_

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

const char *Utils_getErrorMessage(int8_t code);

#endif
