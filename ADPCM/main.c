#ifdef __MARIKO_CC__
	#include <doslib.h>
	#include <iocslib.h>
#else
	#include <dos.h>
	#include <iocs.h>
	#include <stdio.h>
	#define interrupt __attribute__ ((interrupt_handler))
#endif

#include <limits.h>
#include <math.h>
#include <stdint.h>
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

#define SEEK_MODE_BEGINNING  0
#define SEEK_MODE_CURRENT 1
#define SEEK_MODE_END 2


#define ADPCM_MODE_NO_WAIT   0x0000 //0b0000000000000000
#define ADPCM_MODE_WAIT      0x8000 //0b1000000000000000

#define ADPCM_SAMPLE_3_9KHZ  0x000  //0b0000000000000000
#define ADPCM_SAMPLE_5_2KHZ  0x100  //0b0000000100000000
#define ADPCM_SAMPLE_7_8KHZ  0x200  //0b0000001000000000
#define ADPCM_SAMPLE_10_4KHZ 0x300  //0b0000001100000000
#define ADPCM_SAMPLE_15_6KHZ 0x400  //0b0000010000000000

#define ADPCM_SPK_MUTE   0x00
#define ADPCM_SPK_LEFT   0x01
#define ADPCM_SPK_RIGHT  0x02
#define ADPCM_SPK_STEREO 0x03

#define ADPCM_MODE(mode, sample, speaker) (mode | sample | speaker)

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

//if the chunk sent to the pcm is this number or less the return is inmediate
#define MUSIC_CHUNK 65280

char *buffer_a, *buffer_b;

void terminate();

int main(int argc, char *argv[])
{
    int16_t file_handler;
    int32_t status;
    int32_t size;
    char *filename = "result.raw";

    if(argc > 1){
        filename = argv[1];
    }

    signal(SIGINT, terminate); //for the Control + C

    _dos_c_print("loading pcm!\r\n");

    //we open the palette file
    file_handler = _dos_open(
        filename,
        OPENING_MODE(
            ACCESS_NORMAL,
            SHARING_COMPATIBILITY_MODE,
            MODE_R
        )
    );

    //if any error...
    if(file_handler < 0){
        _dos_c_print("Can't open the pcm file\r\n");
        _dos_exit();
    }

    //we move the header to the end to get the file size.
    size = _dos_seek(
       file_handler,
       0, //offset
       SEEK_MODE_END  //0 = beginning, 1 = on the spot, 2 = end
    );

    //we put the header back to the beginning
    _dos_seek(
       file_handler,
       0, //offset
       SEEK_MODE_BEGINNING  //0 = beginning, 1 = on the spot, 2 = end
    );

    buffer_a = (char *) _dos_malloc(MUSIC_CHUNK * 2);
    buffer_b = buffer_a + MUSIC_CHUNK;

    {
        int c = 0;
        int offset = 0;

        _dos_c_print("before read\r\n");
        if(_dos_read(file_handler, (char*) buffer_a, MUSIC_CHUNK)){
            _dos_c_print("after read\r\n");
        }

        do{
            int remainding_bytes = size - offset;

            //we play the next chunk of music
            _iocs_adpcmout(
                c % 2 == 0 ? buffer_a : buffer_b,
                ADPCM_MODE(
                    ADPCM_MODE_WAIT,
                    ADPCM_SAMPLE_15_6KHZ,
                    ADPCM_SPK_STEREO
                ),
                remainding_bytes < MUSIC_CHUNK ? remainding_bytes : MUSIC_CHUNK
            );

            //we move the pointer
            ++c;
            offset += MUSIC_CHUNK;

            _dos_c_print("before read\r\n");
            if(_dos_read(file_handler, (char*) (c % 2 == 0 ? buffer_a : buffer_b), MUSIC_CHUNK)){
                _dos_c_print("after read\r\n");
            }

            //if still playing...
            while(_iocs_adpcmsns() != ADPCM_STATUS_IDLE){
                //we print to show activity
                printf("playing! %d\r\n", c);
                //if we hit a key...
                if(_dos_inpout(0xFF) != 0){
                    terminate();
                    //we get out the loops
                    goto end;
                }
            }
        } while(offset < size);
        end: ; // to escape the double loop.
    }

    //we free memory
    _dos_mfree(buffer_a);

    status = _dos_close(file_handler);

    //if any error...
    if(status < 0){
        _dos_c_print("Can't close the pcm file\r\n");
        _dos_exit();
    }

    _dos_c_print("Done\r\n");

    _dos_exit();
}

void terminate()
{
    //we stop the music
    _iocs_adpcmmod(ADPCM_CTRL_END);
}
