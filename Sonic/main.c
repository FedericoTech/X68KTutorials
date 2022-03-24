#include "utils.h"
#include <signal.h>

#include <stdio.h>

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

int8_t last_mode; //in this var we will store the video that was before we ran the program

uint8_t *s_still;

uint8_t *s_look_u;

uint8_t *s_look_d;

uint8_t *s_run[4];

uint8_t *s_spin[4];

uint8_t *s_wait[2];

uint8_t *s_stop[2];

uint8_t *s_walk[6];

volatile int frame = 0;
volatile int draw = 0;

void terminate();

char * loadData(char * buffer, const char * filename, int8_t config);

void interrupt vsync_disp();
/*
struct _filbuf {
    unsigned char searchatr;
    unsigned char driveno;
    unsigned long dirsec;
    unsigned short dirlft;
    unsigned short dirpos;
    char filename[8];
    char ext[3];
    unsigned char atr;
    unsigned short time;
    unsigned short date;
    unsigned int_ filelen;
    char name[23];
};
*/


int main(void)
{
    int32_t status;

    union FileConf fconf;

    volatile uint16_t *palette_addr = (uint16_t *)S_PALETTE_START;

    volatile uint16_t *pcg_addr = (uint16_t *)PCG_START;

    last_mode = _iocs_crtmod(-1); //we capture the current video mode

    status = _iocs_crtmod(10); // 8 this mode is 512 x 512 256 colours

    _iocs_g_clr_on();
    _iocs_b_curoff();

    //load palette
    fconf.config = 0;
    fconf.flags.access_dictionary = ACCESS_NORMAL;
    fconf.flags.sharing = SHARING_COMPATIBILITY_MODE;
    fconf.flags.mode = MODE_R;

    s_still = loadData(NULL, "s_still.spt", fconf.config);

    s_look_u = loadData(NULL, "s_look_u.spt", fconf.config);

    s_look_d = loadData(NULL, "s_look_d.spt", fconf.config);

    s_wait[0] = loadData(NULL, "s_wait_0.spt", fconf.config);
    s_wait[1] = loadData(NULL, "s_wait_1.spt", fconf.config);

    s_stop[0] = loadData(NULL, "s_stop_0.spt", fconf.config);
    s_stop[1] = loadData(NULL, "s_stop_1.spt", fconf.config);

    {
        struct _filbuf sf;

        int cont = 0;

        status = _dos_files (&sf, "s_run_?.spt", 48);
        while(status == 0){
            s_run[cont++] = loadData(NULL, sf.name, fconf.config);
            status= _dos_nfiles(&sf);
        }

        cont = 0;
        status = _dos_files (&sf, "s_spin_?.spt", 48);
        while(status == 0){
            s_spin[cont++] = loadData(NULL, sf.name, fconf.config);
            status= _dos_nfiles(&sf);
        }

        cont = 0;
        status = _dos_files (&sf, "s_walk_?.spt", 48);
        while(status == 0){
            s_walk[cont++] = loadData(NULL, sf.name, fconf.config);
            status= _dos_nfiles(&sf);
        }
    }

    _iocs_sp_init();
    _iocs_sp_on();

    _dos_super(0);
    loadData((char*)palette_addr, "sonic.pal", fconf.config);
    _dos_super(0);

    {
        int x = 50;
        int y = 50;

        int i = 0;
        int j = 0;

        int cont = 0;

        union {
            struct {
                int8_t vf:1;
                int8_t hf:1;
                int8_t :2;           //padding
                int8_t palette:4;
                int8_t pcg;
            } flags;
            uint16_t code;
        } sp_register;

        sp_register.flags.vf = 0;         // VF ON
        sp_register.flags.hf = 0;         // HF OFF
        sp_register.flags.palette = 1;    // palette number
        sp_register.flags.pcg = 0;        // pcg number

        for(i = 0; i < 48; i += 16){
            for(j = 0; j < 48; j += 16){

                sp_register.flags.pcg = cont;

                status = _iocs_sp_regst(
                    sp_register.flags.pcg | VERTICAL_BLANKING_NO_DETECT,    //int_ spno sprite number (0-127) //int_ mode bit 31 0: Vertical blanking interval detection post-setting 1: Not detected
                    j + x,                                  //int_ x X coordinates (0-1023 16 displayed on the far left
                    i + y,                                  //int_ y Y " (" " Top ")
                    sp_register.code,                   //code pattern code
                    3                       //int_ prw priority
                );
                ++cont;
            }
        }
    }

    signal(SIGINT, terminate);	/* Processing routine settings when is pressed CTRL-C */

    _iocs_vdispst(
        &vsync_disp,
        1,//0: vertical blanking interval 1: vertical display period
        1
    );

    _dos_c_print("Press a key.\r\n");

    {
        int cont;
        int cont2;
        int cont3;

        while(1){
            while(draw == 0){
                ;
            }

            if(_iocs_dmamode()== DMA_STATUS_IDLE){

                _iocs_dmamove(
                    s_walk[frame],           //buffer A, the source
                    (uint16_t*)pcg_addr,    //buffer B, the destination
                    DMA_MODE(
                         DMA_DIR_A_TO_B,    //from A to B
                         DMA_PLUS_PLUS,     //move the pointer forward as it reads
                         DMA_PLUS_PLUS      //move the pointer forward as it writes
                    ),
                    1152                    //size of the memory block we are moving
                );
            }

            draw = 1;

            if(_dos_inpout(0xFF) != 0){
                break;
            }
        }
    }

    terminate();
    _dos_exit();
}

void terminate()			/* Processing routine when pressed CTRL-C */
{
    _dos_s_mfree(s_still);
    _dos_s_mfree(s_look_u);
    _dos_s_mfree(s_look_d);

    _dos_s_mfree(s_run[0]);
    _dos_s_mfree(s_run[1]);
    _dos_s_mfree(s_run[2]);
    _dos_s_mfree(s_run[3]);

    _dos_s_mfree(s_spin[0]);
    _dos_s_mfree(s_spin[1]);
    _dos_s_mfree(s_spin[2]);
    _dos_s_mfree(s_spin[3]);

    _dos_s_mfree(s_wait[0]);
    _dos_s_mfree(s_wait[1]);

    _dos_s_mfree(s_stop[0]);
    _dos_s_mfree(s_stop[1]);

    _dos_s_mfree(s_walk[0]);
    _dos_s_mfree(s_walk[1]);
    _dos_s_mfree(s_walk[2]);
    _dos_s_mfree(s_walk[3]);
    _dos_s_mfree(s_walk[4]);
    _dos_s_mfree(s_walk[5]);

    //we restore the video mode
    _iocs_crtmod(last_mode);
}

char * loadData(char * buffer, const char * filename, int8_t config)
{
    int16_t file_handler;
    int32_t status;
    int32_t status2;

    void interrupt vsync_disp();

    //we open the palette file
    file_handler = _dos_open(
        filename,
        config
    );

    //if any error...
    if(file_handler < 0){
        terminate();
        _dos_c_print("Can't open the file\r\n");
        _dos_c_print(getErrorMessage(file_handler));

        _dos_exit2(file_handler);
    }

    status = _dos_seek(
       file_handler,
       0, //offset
       2  //0 = beginning, 1 = on the spot, 2 = end
    );

    _dos_seek(
       file_handler,
       0, //offset
       0  //0 = beginning, 1 = on the spot, 2 = end
    );

    if(buffer == NULL){
        buffer = _dos_malloc(status);
    }

    status2 = _dos_read(file_handler, buffer, status);

    //now we close the file
    status = _dos_close(file_handler);

    //if any error...
    if(status < 0){
        terminate();
        _dos_c_print("Can't close the file\r\n");
        _dos_c_print(getErrorMessage(status));
        _dos_exit2(status);
    }

    return buffer;
} //loadData

void interrupt vsync_disp()
{
    draw = 1;

    if(++frame > 5){
        frame = 0;
    }
}


