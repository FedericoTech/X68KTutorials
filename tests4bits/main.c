#ifdef __MARIKO_CC__
	#include <doslib.h>
	#include <iocslib.h>
#else
	#include <dos.h>
	#include <iocs.h>
	#include <stdio.h>
	#define interrupt __attribute__ ((interrupt_handler))
#endif

#include <stdint.h>

#define GVRAM_START	    0xC00000    // Graphics Vram Page 0
#define GVRAM_PAGE_1	0xC80000	// Graphics Vram Page 1 (256/16 color only)
#define GVRAM_PAGE_2	0xD00000	// Graphics Vram Page 2 (16 color only)
#define GVRAM_PAGE_3	0xd80000	// Graphics Vram Page 3 (16 color only)

#define DMA_DIR_A_TO_B  0x00
#define DMA_DIR_B_TO_A  0x80

#define DMA_A_FIXED     0x00
#define DMA_A_PLUS_PLUS 0x04
#define DMA_A_MINUS_3   0x08

#define DMA_B_FIXED     0x00
#define DMA_B_PLUS_PLUS 0x01
#define DMA_B_MINUS_3   0x02

#define DMA_MODE(direction, A, B) (direction | A | B)

#define DMA_MAX_BLAST   0xff00

#define DMA_STATUS_IDLE 0x00
#define DMA_STATUS_IN_DMAMOVE_OP 0x8A
#define DMA_STATUS_IN_DMAMOV_A_OP 0x8B
#define DMA_STATUS_IN_DMAMOV_L_OP 0x8C

int main(void)
{
    int8_t last_mode;
    uint32_t status;


    uint8_t colour;

    last_mode = _iocs_crtmod(-1);

    status = _iocs_crtmod(4); //this mode is 512 x 512 16 colours

    //                              Actual  Viewport    Colours     Pages   Frequency
    //status = _iocs_crtmod(0); //  1024    512 x 512   16          1       31 KkHz
    //status = _iocs_crtmod(1); //  1024    512 x 512   16          1       15 KkHz

    //status = _iocs_crtmod(2); //  1024    256 x 256   16          1       31 KkHz
    //status = _iocs_crtmod(3); //  1024    256 x 256   16          1       15 KkHz

    status = _iocs_crtmod(4); //  512     512 x 512   16          4       31 KkHz
    //status = _iocs_crtmod(5); //  512     512 x 512   16          4       15 KkHz

    //status = _iocs_crtmod(6); //  512     256 x 256   16          4       31 KkHz
    //status = _iocs_crtmod(7); //  512     256 x 256   16          4       15 KkHz


    _iocs_g_clr_on();

//Painted with the CPU START
    {

        uint16_t *vram_addr = (void *)GVRAM_PAGE_3;
        int cont, cont2;

        //enter super user to access memory
        status = _dos_super(0);

        colour = 0x0001;

        //each row
        for(cont = 0; cont < 512; cont++){
            //each column
            for(cont2 = 0; cont2 < 512; cont2++){
                //we copy 2 bytes at the time
                *vram_addr++ = colour;
            }
        }
        _dos_super(status);
    }
//Painted with the CPU END


//Painted with _dmamove START
    {
        int cont;
        colour = 0x0002;

        for(cont = 0; cont < (512 * 512 * sizeof(uint16_t)) - DMA_MAX_BLAST; cont += DMA_MAX_BLAST){
            //we check that the dma has finished
            while(_iocs_dmamode() != DMA_STATUS_IDLE){
                printf("wait for dma\n");
            }

            _iocs_dmamove(
                &colour,                        //buffer A, the source
                (void *)GVRAM_PAGE_2 + cont,    //buffer B, the destination
                DMA_MODE(
                     DMA_DIR_A_TO_B,    //from A to B
                     DMA_A_FIXED,       //keep reading from colour
                     DMA_B_PLUS_PLUS    //move the pointer forward as it writes
                ),
                DMA_MAX_BLAST              //size of the memory block we are moving
            );
        }

    }
//Painted with _dmamove END

//Painted with _dmamov_a START
    {
        int cont;
        int iterations = (512 * 512 * sizeof(uint16_t)) / DMA_MAX_BLAST;
        struct _chain data[iterations];

        colour = 0x0003;

        printf("%d \n", iterations);

        for(cont = 0; cont < iterations; cont++){
            data[cont].addr = (void *) (GVRAM_PAGE_1 + cont * DMA_MAX_BLAST);
            data[cont].len = DMA_MAX_BLAST;
        }

        while(_iocs_dmamode() != DMA_STATUS_IDLE){
            printf("wait for dma\n");
        }

        _iocs_dmamov_a(
            data,                       // buffer A, the destination
            &colour,                    // buffer B, the source
            DMA_MODE(
                 DMA_DIR_B_TO_A,        // from B to A
                 DMA_A_PLUS_PLUS,       // move the pointer forward as it writes
                 DMA_B_FIXED            // keep reading from colour
            ),
            iterations                  // num of items in the array
        );
    }
//Painted with _dmamov_a END

//Painted with _dmamov_l START
    {
        struct _chain2 chunk0, chunk1, chunk2, chunk3, chunk4, chunk5, chunk6, chunk7;

        colour = 0x0004;

        chunk7.addr = (void *) GVRAM_START + DMA_MAX_BLAST * 7;
        chunk7.len = DMA_MAX_BLAST;
        chunk7.next = 0;

        chunk6.addr = (void *) GVRAM_START + DMA_MAX_BLAST * 6;
        chunk6.len = DMA_MAX_BLAST;
        chunk6.next = &chunk1;

        chunk5.addr = (void *) GVRAM_START + DMA_MAX_BLAST * 5;
        chunk5.len = DMA_MAX_BLAST;
        chunk5.next = &chunk2;

        chunk4.addr = (void *) GVRAM_START + DMA_MAX_BLAST * 4;
        chunk4.len = DMA_MAX_BLAST;
        chunk4.next = &chunk3;

        chunk3.addr = (void *) GVRAM_START + DMA_MAX_BLAST * 3;
        chunk3.len = DMA_MAX_BLAST;
        chunk3.next = &chunk7;

        chunk2.addr = (void *) GVRAM_START + DMA_MAX_BLAST * 2;
        chunk2.len = DMA_MAX_BLAST;
        chunk2.next = &chunk4;

        chunk1.addr = (void *) GVRAM_START + DMA_MAX_BLAST;
        chunk1.len = DMA_MAX_BLAST;
        chunk1.next = &chunk5;

        chunk0.addr = (void *) GVRAM_START;
        chunk0.len = DMA_MAX_BLAST;
        chunk0.next = &chunk6;

        while(_iocs_dmamode() != DMA_STATUS_IDLE){
            printf("wait for dma\n");
        }

        _iocs_dmamov_l(
            &chunk0,            // buffer A, the destination
            &colour,            // buffer B, the source
            DMA_MODE(
                DMA_DIR_B_TO_A, // from A to B
                DMA_A_PLUS_PLUS,// move the pointer forward as it writes
                DMA_B_FIXED     // keep reading from colour
            )
        );
    }
//Painted with _dmamov_l END


    /*

    vram_addr = (void *)GVRAM_START;

    //each row
    for(cont = 0; cont < 512; cont++){
        //each column
        for(cont2 = 0; cont2 < 512; cont2++){
            //we copy 2 bytes at the time
            *vram_addr++ = 0x0004;
        }
    }

   */

    _dos_c_print("Press a key.\r\n");

    //waiting for a keystroke.
    _dos_getchar();

    _iocs_crtmod(last_mode);
    _dos_exit();
}
