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

#define OPENING_MODE(access, sharing, mode) (access | sharing | mode)

#define SEEK_MODE_BEGINNING  0
#define SEEK_MODE_CURRENT 1
#define SEEK_MODE_END 2


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



char *buffer;

int main(void)
{
    int16_t file_handler;
    int32_t status;
    int32_t size;

    _dos_c_print("loading wave!\r\n");

    //we open the palette file
    file_handler = _dos_open(
        "result.raw",
        OPENING_MODE(
            ACCESS_NORMAL,
            SHARING_COMPATIBILITY_MODE,
            MODE_R
        )
    );

    //if any error...
    if(file_handler < 0){
        _dos_c_print("Can't open the palette file\r\n");
        _dos_exit();
    }

    //we move the header to the end to get the file size.
    size = _dos_seek(
       file_handler,
       0, //offset
       2  //0 = beginning, 1 = on the spot, 2 = end
    );

    //we put the header back to the beginning
    _dos_seek(
       file_handler,
       0, //offset
       0  //0 = beginning, 1 = on the spot, 2 = end
    );

    buffer = (char *) _dos_malloc(size);

    if(_dos_read(file_handler, (char*) buffer, size)){
        _dos_c_print("read\r\n");
    }

    status = _dos_close(file_handler);

    //if any error...
    if(status < 0){
        _dos_c_print("Can't close the palette file\r\n");
        _dos_exit();
    }

    _dos_c_print("now we will play it!\r\n");

    {
        int c = 0;

        while(c < size){
             //we play the next chunk of music
             _iocs_adpcmout(
                buffer + (c * 512),
                ADPCM_MODE(
                    ADPCM_MODE_WAIT,
                    ADPCM_SAMPLE_15_6KHZ,
                    ADPCM_SPK_STEREO
                ),
                512
            );

            //we move the pointer
            ++c;

            //if still playing...
            if(_iocs_adpcmsns() != ADPCM_STATUS_IDLE){
                //we print to show activity
                printf("playing! %d\r\n", c);
                if(_dos_inpout(0xFF) != 0){
                    _iocs_adpcmmod(ADPCM_CTRL_END);
                    break;
                }
            }
        }
    }

    _dos_c_print("Done\r\n");

    _dos_exit();
}
