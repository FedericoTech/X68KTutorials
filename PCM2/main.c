#ifdef __MARIKO_CC__
	#include <doslib.h>
	#include <iocslib.h>
#else
	#include <dos.h>
	#include <iocs.h>
	#define interrupt __attribute__ ((interrupt_handler))
#endif

#include <limits.h>
#include <math.h>


#define ADPCM_MODE_NO_WAIT  0x0  //0b000000000000000
#define ADPCM_MODE_WAIT     0x1  //0b100000000000000

#define ADPCM_SAMPLE_3_9KHZ  0x0  //0b000000000000000
#define ADPCM_SAMPLE_5_2KHZ  0x1  //0b000000010000000
#define ADPCM_SAMPLE_7_8KHZ  0x2  //0b000000100000000
#define ADPCM_SAMPLE_10_4KHZ 0x3  //0b000000110000000
#define ADPCM_SAMPLE_15_6KHZ 0x4  //0b000001000000000

#define ADPCM_SPK_MUTE   0x0
#define ADPCM_SPK_LEFT   0x1
#define ADPCM_SPK_RIGHT  0x2
#define ADPCM_SPK_STEREO 0x3

#define ADPCM_MODE(mode, sample, speaker) (mode << 15 | sample << 8 | speaker)

#define ADPCM_CTRL_END 0
#define ADPCM_CTRL_SUSPEND 1
#define ADPCM_CTRL_RESUME 2
//_iocs_adpcmmod(int)

#define ADPCM_STATUS_IDLE 0
#define ADPCM_STATUS_OUT 2
#define ADPCM_STATUS_INP 3
#define ADPCM_STATUS_AOT 12
#define ADPCM_STATUS_AIN 14
#define ADPCM_STATUS_LOT 22
#define ADPCM_STATUS_LIN 24
// _iocs_adpcmsns()

#define SOUND_LENTH 512 * 16

unsigned char *buffer;

int main(void)
{
    int cont;
    float rads;

    _dos_c_print("Hello world !\r\n");

    buffer = (unsigned char *) _dos_malloc(sizeof(char) * SOUND_LENTH);

    rads = (float) M_PI / 180;

    memset(buffer,              0x00, 512);
    memset(buffer + 512 * 1,    0x01, 512);
    memset(buffer + 512 * 2,    0x02, 512);
    memset(buffer + 512 * 3,    0x03, 512);
    memset(buffer + 512 * 4,    0x04, 512);
    memset(buffer + 512 * 5,    0x05, 512);
    memset(buffer + 512 * 6,    0x06, 512);
    memset(buffer + 512 * 7,    0x07, 512);
    memset(buffer + 512 * 8,    0x08, 512);
    memset(buffer + 512 * 9,    0x09, 512);
    memset(buffer + 512 * 10,   0x0a, 512);
    memset(buffer + 512 * 11,   0x0b, 512);
    memset(buffer + 512 * 12,   0x0c, 512);
    memset(buffer + 512 * 13,   0x0d, 512);
    memset(buffer + 512 * 14,   0x0e, 512);
    memset(buffer + 512 * 15,   0x0f, 512);

/*
    memset(buffer,              0x00, 512);
    memset(buffer + 512 * 1,    0x10, 512);
    memset(buffer + 512 * 2,    0x20, 512);
    memset(buffer + 512 * 3,    0x30, 512);
    memset(buffer + 512 * 4,    0x40, 512);
    memset(buffer + 512 * 5,    0x50, 512);
    memset(buffer + 512 * 6,    0x60, 512);
    memset(buffer + 512 * 7,    0x70, 512);
    memset(buffer + 512 * 8,    0x80, 512);
    memset(buffer + 512 * 9,    0x90, 512);
    memset(buffer + 512 * 10,   0xa0, 512);
    memset(buffer + 512 * 11,   0xb0, 512);
    memset(buffer + 512 * 12,   0xc0, 512);
    memset(buffer + 512 * 13,   0xd0, 512);
    memset(buffer + 512 * 14,   0xe0, 512);
    memset(buffer + 512 * 15,   0xf0, 512);
*/
/*
    memset(buffer,              0x0f, 512);
    memset(buffer + 512 * 1,    0x1e, 512);
    memset(buffer + 512 * 2,    0x2d, 512);
    memset(buffer + 512 * 3,    0x3c, 512);
    memset(buffer + 512 * 4,    0x4b, 512);
    memset(buffer + 512 * 5,    0x5a, 512);
    memset(buffer + 512 * 6,    0x69, 512);
    memset(buffer + 512 * 7,    0x78, 512);
    memset(buffer + 512 * 8,    0x87, 512);
    memset(buffer + 512 * 9,    0x96, 512);
    memset(buffer + 512 * 10,   0xa5, 512);
    memset(buffer + 512 * 11,   0xb4, 512);
    memset(buffer + 512 * 12,   0xc3, 512);
    memset(buffer + 512 * 13,   0xd2, 512);
    memset(buffer + 512 * 14,   0xe1, 512);
    memset(buffer + 512 * 15,   0xf0, 512);
*/
    //for(cont = 0; cont < SOUND_LENTH; cont += 2){
        //buffer[cont] = random() % 16;

        //float sine = sin((cont >> 3) * rads);
        //buffer[cont] = (sine * (float) 14) / 2; //UCHAR_MAX; //USHRT_MAX;


        //buffer[cont] = 14;

        //buffer[cont] = (((cont >> 7) % 14) + 1) << 4 | ((cont+1 >> 7) % 14) + 1;

        //printf("%d %f %d\n", cont, sine, *buffer);
    //}

    //_dos_exit();

    //_dos_c_print("Play\r\n");

    _iocs_adpcmout(
        buffer,
        ADPCM_MODE(
            ADPCM_MODE_NO_WAIT,
            ADPCM_SAMPLE_3_9KHZ, //ADPCM_SAMPLE_15_6KHZ,
            ADPCM_SPK_STEREO
        ),
        sizeof (unsigned char) * SOUND_LENTH
    );

    while(_iocs_adpcmsns() != ADPCM_STATUS_IDLE){
        //_dos_c_print("playing!\r\n");
        if(_dos_inpout(0xFF) != 0){
            _iocs_adpcmmod(ADPCM_CTRL_END);
            break;
        }
    }

    _dos_c_print("Done\r\n");

    _dos_exit();
}
