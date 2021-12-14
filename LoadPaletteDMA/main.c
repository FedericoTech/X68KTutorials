#include <dos.h>
#include <stdint.h>
#include "utils.h"

/**
  * In this example we are loading the palettes from the file palette.pal
  * The data is stored in binary format, in 16 bit words sequentially ordered.
  * Every palette is 32 bytes long.
  *
  * Then we make use of the DMA by the function:
  *
  * int _iocs_dmamove (void *addr1, void *addr2, uint8_t mode, int_ cnt);
  *
  * The first param is the memory address of let's say A, also known as DAC.
  *
  * The second param is the memory address of let's say B, also known as MAC.
  *
  * The third param mode is a field of flags to configure the transfer of data.
  *
  * The bit 7 sets the direction, 0 is from A to B and 1 is from B to A.
  *
  * The bits 3 and 4 is to code how the pointer of A moves.
  *
  * The bits 0 and 2 is to code how the pointer of B moves.
  *
  * If 0 the pointer doesn't move but stays. This is useful to reset areas of memory
  *
  * If 1 the pointer moves ++ which is forward 1 at the time.
  *
  * If 2 the pointer moves --3 which is backward  1 at the time.
  *
  * The forth param is the size of data we want to move.
  */

#define S_PALETTE_START 0xe82220

#define DMA_DIR_A_TO_B  0b00000000
#define DMA_DIR_B_TO_A  0b10000000

#define DMA_A_FIXED     0b00000000
#define DMA_A_PLUS_PLUS 0b00000100
#define DMA_A_MINUS_3   0b00001000

#define DMA_B_FIXED     0b00000000
#define DMA_B_PLUS_PLUS 0b00000001
#define DMA_B_MINUS_3   0b00000010

#define DMA_MODE(direction, A, B) (direction | A | B)

#define DMA_STATUS_IDLE 0x00
#define DMA_STATUS_IN_DMAMOVE_OP 0x8A
#define DMA_STATUS_IN_DMAMOV_A_OP 0x8B
#define DMA_STATUS_IN_DMAMOV_L_OP 0x8C

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

    //whereas there are palettes...
    while(_dos_read(file_handler, (char*)&colours, sizeof(colours))) {

        _iocs_dmamove(
            &colours,                   //buffer A, the source
            (uint16_t*)palette_addr,    //buffer B, the destination
            DMA_MODE(
                 DMA_DIR_A_TO_B,        //from A to B
                 DMA_A_PLUS_PLUS,       //move the pointer forward as it reads
                 DMA_B_PLUS_PLUS        //move the pointer forward as it writes
            ),
            sizeof(colours)             //size of the memory block we are moving
        );

        //wait for complition.
        while(_iocs_dmamode() != DMA_STATUS_IDLE){
            ;
        }

        //move to the next palette
        palette_addr += 16;
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
