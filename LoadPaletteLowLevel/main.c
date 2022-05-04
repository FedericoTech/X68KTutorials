#include "utils.h"

/**
  * In this example we are loading the palettes from the file palette.pal
  * The data is stored in binary format, in 16 bit words sequentially ordered.
  * Every palette is 32 bytes long.
  *
  * Then we are setting the bytes straight away into memory with the super user mode.
  */

#define S_PALETTE_START 0xe82220

int32_t main(void)
{
    int16_t file_handler, status;
    int32_t super;

    //we will collect the palettes pallete by pallete in this array.
    uint16_t colours[16];


    //we copy the address into this other pointer to not lose it.
    volatile uint16_t *pa  = (uint16_t *)S_PALETTE_START;

    //we open the palette file
    file_handler = _dos_open(
        "palette.pal",
        OPENING_MODE(
            ACCESS_NORMAL,
            SHARING_COMPATIBILITY_MODE,
            MODE_R
        )
    );

    //if any error...
    if(file_handler < 0){
        _dos_c_print("Can't open the file\r\n");
        _dos_exit2(file_handler);
    }

    //we set the super user mode to gain access to reserved areas of memory
    super = _dos_super(0);

    _dos_c_print("before\r\n");
    //whereas there are palettes...
    while(_dos_read(file_handler, (char*) colours, sizeof colours)) {

        uint16_t *pBuffer = (uint16_t *) colours; //0

        *++pa = *++pBuffer; //1
        *++pa = *++pBuffer; //2
        *++pa = *++pBuffer; //3
        *++pa = *++pBuffer; //4
        *++pa = *++pBuffer; //5
        *++pa = *++pBuffer; //6
        *++pa = *++pBuffer; //7
        *++pa = *++pBuffer; //8
        *++pa = *++pBuffer; //9
        *++pa = *++pBuffer; //10
        *++pa = *++pBuffer; //11
        *++pa = *++pBuffer; //12
        *++pa = *++pBuffer; //13
        *++pa = *++pBuffer; //14
        *++pa = *++pBuffer; //15

        _dos_c_print("turn\r\n");
    }

    //back to user mode
    _dos_super(super);

    //now we close the file
    status = _dos_close(file_handler);

    //if any error...
    if(status < 0){
        _dos_c_print("Can't close the file\r\n");
        _dos_exit2(status);
    }

    _dos_c_print("Look into the palette window of your emulator and press a key.\r\n");

    //waiting for a keystroke.
    _dos_getchar();

    _dos_exit();
}
