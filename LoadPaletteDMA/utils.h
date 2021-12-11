#ifndef _UTILS_H_
#define _UTILS_H_

#define ACCESS_DICTIONARY           0b10000000 //1 not for users
#define ACCESS_NORMAL               0b00000000 //0

#define SHARING_TOTAL               0b00010000  //4 allow others to write and read
#define SHARING_WRITE_ONLY          0b00001100  //3 allow others to write only
#define SHARING_READ_ONLY           0b00001000  //2 allow others to read only
#define SHARING_RESTRICTED          0b00000100  //1 don't allow others anything
#define SHARING_COMPATIBILITY_MODE  0b00000000  //0

#define MODE_RW                     0b00000010  //2 open for write and read
#define MODE_W                      0b00000001  //1 open only for writing
#define MODE_R                      0b00000000  //0 open only for reading

#define OPENING_MODE(access, sharing, mode) (access | sharing | mode)

#endif
