#include <dos.h>
#include <stdint.h>
#include "utils.h"

/**
  * In this example we are loading the palettes from the file palette.pal
  * The data is stored in binary format, in 16 bit words sequentially ordered.
  * Every palette is 32 bytes long.
  *
  * Then we are setting the bytes straight away into memory with the super user mode.
  */

#define S_PALETTE_START 0xe82220


int main(void)
{
    int16_t file_handler, status;

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

    //we will collect the palettes pallete by pallete in this array.
    uint16_t colours[16];

    volatile uint16_t *palette_addr = (uint16_t *)S_PALETTE_START;

    //we copy the address into this other pointer to not lose it.
    volatile uint16_t *pa  = palette_addr;

    //we set the super user mode to gain access to reserved areas of memory
    _dos_super(0);

    //whereas there are palettes...
    while(_dos_read(file_handler, (char*)&colours, sizeof(colours))) {

        uint16_t *pBuffer = (uint16_t *)&colours;
        *pa++ = *pBuffer++;
        *pa++ = *pBuffer++;
        *pa++ = *pBuffer++;
        *pa++ = *pBuffer++;
        *pa++ = *pBuffer++;
        *pa++ = *pBuffer++;
        *pa++ = *pBuffer++;
        *pa++ = *pBuffer++;
        *pa++ = *pBuffer++;
        *pa++ = *pBuffer++;
        *pa++ = *pBuffer++;
        *pa++ = *pBuffer++;
        *pa++ = *pBuffer++;
        *pa++ = *pBuffer++;
        *pa++ = *pBuffer++;
        *pa++ = *pBuffer++;
    }

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
