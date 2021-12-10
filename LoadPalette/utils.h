#ifndef _UTILS_H_
#define _UTILS_H_

#define ACCESS_DICTIONARY 1 //not for users
#define ACCESS_NORMAL 0

#define SHARING_TOTAL 4
#define SHARING_WRITE_ONLY 3
#define SHARING_READ_ONLY 2
#define SHARING_RESTRICTED 1
#define SHARING_COMPATIBILITY_MODE 0

#define MODE_RW 2
#define MODE_W 1
#define MODE_R 0

#define OPENING_MODE(access, sharing, mode) ((access << 7) | (sharing << 2) | mode)

#endif
