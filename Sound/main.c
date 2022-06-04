#ifdef __MARIKO_CC__
	#include <doslib.h>
	#include <iocslib.h>
#else
	#include <dos.h>
	#include <iocs.h>
	#define interrupt __attribute__ ((interrupt_handler))
#endif

/*

int_ _iocs_adpcmsns (void_)

static __inline void _iocs_adpcmmod (int_ mode)

static __inline void _iocs_adpcminp (void *addr, int_ mode, int_ len)
static __inline void _iocs_adpcmout (const void *addr, int_ mode, int_ len)

static __inline void _iocs_adpcmain (const struct _chain *addr, int_ mode, int_ cnt)
static __inline void _iocs_adpcmaot (const struct _chain *addr, int_ mode, int_ cnt)


static __inline void _iocs_adpcmlin (const struct _chain2 *addr, int_ mode)
static __inline void _iocs_adpcmlot (const struct _chain2 *addr, int_ mode)

*/

#define ADPCM_MODE_NO_WAIT 0x0000  //0b000000000000000
#define ADPCM_MODE_WAIT    0x8000  //0b100000000000000

#define ADPCM_SAMPLE_3_9KHZ  0x000  //0b000000000000000
#define ADPCM_SAMPLE_5_2KHZ  0x080  //0b000000010000000
#define ADPCM_SAMPLE_7_8KHZ  0x100  //0b000000100000000
#define ADPCM_SAMPLE_10_4KHZ 0x180  //0b000000110000000
#define ADPCM_SAMPLE_15_6KHZ 0x200  //0b000001000000000

#define ADPCM_SPK_MUTE   0x0
#define ADPCM_SPK_LEFT   0x1
#define ADPCM_SPK_RIGHT  0x2
#define ADPCM_SPK_STEREO 0x3

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


char *buffer;

int main(void)
{
    _dos_c_print("Speak !\r\n");

    buffer = (char *)_dos_malloc(sizeof(char) * 512 * 256);

    _iocs_adpcminp (
        buffer,
        ADPCM_MODE(
            ADPCM_MODE_WAIT,
            ADPCM_SAMPLE_15_6KHZ,
            ADPCM_SPK_STEREO
        ),
        sizeof (char) * 512 * 256
    );

    while(_iocs_adpcmsns() != ADPCM_STATUS_IDLE){
        _dos_c_print("recording!\r\n");
        if(_dos_inpout(0xFF) != 0){
            _iocs_adpcmmod(ADPCM_CTRL_END);
            break;
        }
    }

    _dos_c_print("Press a key!\r\n");

    _dos_getchar();

    _iocs_adpcmout(
        buffer,
        ADPCM_MODE(
            ADPCM_MODE_WAIT,
            ADPCM_SAMPLE_15_6KHZ,
            ADPCM_SPK_STEREO
        ),
        sizeof (char) * 512 * 256
    );

    while(_iocs_adpcmsns() != ADPCM_STATUS_IDLE){
        _dos_c_print("playing!\r\n");
        if(_dos_inpout(0xFF) != 0){
            _iocs_adpcmmod(ADPCM_CTRL_END);
            break;
        }
    }

    _dos_c_print("Done\r\n");

    _dos_exit();
}
