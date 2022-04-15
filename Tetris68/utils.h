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
#include <stddef.h>
#include <signal.h>

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

#define DMA_DIR_A_TO_B 0
#define DMA_DIR_B_TO_A 1

#define DMA_FIXED 0
#define DMA_PLUS_PLUS 1
#define DMA_MINUS_3 2

#define DMA_MODE(direction, MAC, DAC) (direction << 7 | (MAC << 2) | DAC)

#define DMA_STATUS_IDLE 0x00
#define DMA_STATUS_IN_DMAMOVE_OP 0x8A
#define DMA_STATUS_IN_DMAMOV_A_OP 0x8B
#define DMA_STATUS_IN_DMAMOV_L_OP 0x8C

#define PCG_START 0xEB8000 // Start of PCG memory
#define S_PALETTE_START 0xe82220 //Start of Palette

//sprite palette utils
#define VERTICAL_BLANKING_DETECTION 0x80000000     //0b10000000000000000000000000000000
#define VERTICAL_BLANKING_NO_DETECT 0

union tile_register {
    struct {
        int8_t vf:1;
        int8_t hf:1;
        int8_t :2;           //padding
        int8_t palette:4;
        int8_t pcg;
    } flags;
    uint16_t code;
};

int8_t last_mode; //in this var we will store the video that was before we ran the program

void init_system();

void shut_system();

const char *getFileErrorMessage(int8_t code);

uint32_t start_timer();

uint32_t start_stop();

uint32_t millisecond();

char * loadData(char * buffer, const char * filename, int8_t config);

#endif
