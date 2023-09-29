#ifdef __MARIKO_CC__
	#include <doslib.h>
	#include <iocslib.h>
	#include <interrupt.h>
#else
	#include <dos.h>
	#include <iocs.h>
	#define interrupt __attribute__ ((interrupt_handler))
#endif

#include <math.h>
#include <stdarg.h>

#define 	TIMER_C 	0x45		/* 0x45 vector number to hook  MFP Timer-C (Mouse/cursor/FDD control, etc.) */
#define 	KEYWORDS	"MMV"

struct resident {
    struct {
        struct _mep mep; //16   bytes
        struct _psp psp; //240 bytes
    } procc;    //256 bytes
    char keyword[4];    //here the program starts
    void * oldvector;
};

extern char keyword[4];
extern void * oldvector;

asm( "	_keyword:	.ds.b	4	    " );
asm( "	_oldvector: .dc.l	0		" );

int counter;
int currentime = 0;
volatile char mutex = 1;

const int centerX = 500;
const int centerY = 100;
const float PI = M_PI;
const float radians = (float)M_PI / 180.0f;

const float hour_arm_length = 32.0f;
const float minute_arm_length = 40.0f;
const float second_arm_length = 50.0f;

#define TEXT_PLANE 2

void my_print(const char *format, ...);

inline void calculate_arms(const float tick, const float armLength, short * endX, short * endY);

void interrupt process_start();

void showStatus(int status);

//entry point
int main(int argc, char *argv[])
{
    struct resident * _resident, * resident_aux;

    { //this block is to find out whether the program is resident already or not
        unsigned int resident_start_addr;

        _resident = (struct resident *)((unsigned int) _dos_getpdb() - sizeof(struct _mep));    //we gather information about this instance
        resident_start_addr = (unsigned int) _resident;                                         //including _mep and _psp


        //we search for the resident process as long as there is a prev_mp address
        while(resident_start_addr != 0){
            char keyword[4] = {0};

            // we read a long word with _iocs_b_lpeek so that we don't need to go Super
            resident_start_addr = _iocs_b_lpeek((void *) resident_start_addr);

            resident_aux = (struct resident *) resident_start_addr;

            //we copy three byte in our buffer. we use _iocs_b_memstr so that we don't need to go Super
            _iocs_b_memstr(
                resident_aux->keyword,
                keyword,
                3
            );

            //if we find the process
            if(strncmp(KEYWORDS, keyword, 3) == 0){
                oldvector = resident_aux->oldvector;
                break;
            }
        }
    } //end of the block to find out whether the program is resident.

    //if no arguments we print help
    if(argc == 1){
        _dos_c_print("Resident program demonstration.\r\n");

        printf("The residen program %s set.\n", oldvector == 0 ? "is not" : "is");

        _dos_c_print(
            "/s to set.\r\n"
            "/r to remove.\r\n"
        );

    } else {
        //we are setting the the resident process
        if(strncmp("/s", argv[1], 2) == 0
           || strncmp("/S", argv[1], 2) == 0
        ){
            //if there is no oldvector...
            if(oldvector == 0){
                unsigned int resident_addr, end_addr;
                resident_addr = (unsigned int) &_resident->keyword;            //the program starts right where the keyword is.
                end_addr = (unsigned int) _resident->procc.mep.block_end;      //where is where the program ends

                //we set the keyword
                _iocs_b_memstr(
                    KEYWORDS,
                    &keyword,
                    3
                );

                //we get the current vector address
                oldvector = _dos_intvcs(TIMER_C, process_start);

                //we make the program resident
                _dos_keeppr(
                    end_addr - resident_addr,   //memory from beginning of the program
                    0                           //exit code.
                );

                _dos_print("unreachable\r\n");
            } else {
                _dos_c_print("The program is resident already\r\n");
            }
        }

        //we are removing the resident process
        if(strncmp("/r", argv[1], 2) == 0
           || strncmp("/R", argv[1], 2) == 0
        ){
            if(oldvector != 0){

                _dos_intvcs(TIMER_C, oldvector);

                _dos_s_mfree(&resident_aux->procc.psp); //psp_addr

                { // we remove the clock
                    struct _txfillptr txfillptr = {
                        /* unsigned short vram_page */  TEXT_PLANE, //0, 1, 2, 4. Plane 2 is fixed. It won't move up with the scroll
                        /* short x */                   centerX - 50,
                        /* short y */                   centerY - 50,
                        /* short x1 */                  100,
                        /* short y1 */                  100,
                        /* unsigned short fill_patn */  0x0000
                    };

                    _iocs_txfill(&txfillptr);
                }

                _dos_c_print("resident program stopped\r\n");
            } else {
                _dos_c_print("The program is not resident yet\r\n");
            }
        }
    }
    _dos_exit();
}

//this function is inline to have a copy of it where ever is used
void my_print(const char* format, ...)
{
    //we get the current position of the cursor
    int cursor_pos = _iocs_b_locate(-1, -1);

    va_list args;
    va_start(args, format);

    //we move the cursor to the top left corner
    _iocs_b_locate(0, 0);
    //we print the message

    vprintf(format, args);

    //we re establish cursor's position
    _iocs_b_locate((cursor_pos >> 16) & 0xffff, cursor_pos & 0xffff);

    va_end(args);
}

inline void calculate_arms(const float tick, const float armLength, short* endX, short* endY)
{
    float angle = tick * 6.0f - 90.0f; // Angle of rotation in degrees (adjust as needed)

    // Convert the angle to radians
    float angleRadians = radians * angle;

    // Calculate the endpoint coordinates based on the angle
    *endX = (short) armLength * cos(angleRadians);
    *endY = (short) armLength * sin(angleRadians); // Subtract because Y-axis is inverted
}

//the resident program is this interrupt
void interrupt process_start() //it has to be an interrupt
{
    //we check whether the interrupt is still being processed
    if(mutex){
        mutex = 0; //we hold the mutex
        //only do things every 200 cycles
        if(++counter % 100 == 0){

            int auxtime = _dos_gettim2();

            //if time has changed...
            if(auxtime != currentime){
                currentime = auxtime;

                //clear clock
                {
                    struct _txfillptr txfillptr = {
                        /* unsigned short vram_page */  TEXT_PLANE, //0, 1, 2, 4. Plane 2 is fixed. It won't move up with the scroll
                        /* short x */                   centerX - 50,
                        /* short y */                   centerY - 50,
                        /* short x1 */                  100,
                        /* short y1 */                  100,
                        /* unsigned short fill_patn */  0x0000
                    };

                    _iocs_txfill(&txfillptr);
                }

                //we draw the box
                {
                    struct _tboxptr tboxptr = {
                        /* unsigned short vram_page */  TEXT_PLANE, //0, 1, 2, 4. Plane 2 is fixed. It won't move up with the scroll
                        /* short x */                   centerX - 50,
                        /* short y */                   centerY - 50,
                        /* short x1 */                  100,
                        /* short y1 */                  100,
                        /* unsigned short line_style */ (0xf0 << (counter % 8)) | (0xf0 >> (8 - (counter % 8))) // dashed
                    };

                    _iocs_txbox(&tboxptr);
                }


                {
                    //perpendicular line
                    struct _tlineptr tlptr = {
                        /* unsigned short vram_page */  TEXT_PLANE, //0, 1, 2, 4. Plane 2 is fixed. It won't move up with the scroll
                        /* short x */                   centerX,
                        /* short y */                   centerY,
                        /* short x1 (length) */         0,  //for initialization purposes
                        /* short y1 (length) */         0,  //for initialization purposes
                        /* unsigned short line_style */ 0xff // solid
                    };

                    //vertical line
                    struct _ylineptr ylineptr = {
                        /* unsigned short vram_page */  TEXT_PLANE,
                        /* short x */                   centerX,
                        /* short y */                   centerY,
                        /* short y1 */                  0,  //for initialization purposes,
                        /* unsigned short line_style */ 0xff // solid
                    };

                    //horizontal line
                    struct _xlineptr xlineptr = {
                        /* unsigned short vram_page */  TEXT_PLANE,
                        /* short x */                   centerX,
                        /* short y */                   centerY,
                        /* short x1 */                  0,  //for initialization purposes,
                        /* unsigned short line_style */ 0xff // solid
                    };

                    char hour = (currentime >> 16);
                    char minute = ((currentime >> 8) & 0x3F);
                    char second = (currentime & 0x3F);


                    float aux_hour = ((float) hour) + (1.0f / (float)minute);

                    //hour
                    //if the hour is in a vertical position...
                    if(aux_hour == 12.0f || aux_hour == 24.0f || aux_hour == 6.0f || aux_hour == 18.0f){
                        ylineptr.y1 = (int)(hour_arm_length * (aux_hour == 12.0f || aux_hour == 24.0f ? 1.0f : -1.0f));

                        //we  draw the second arm vertically
                        _iocs_txyline(&ylineptr);

                    //if the hour is in an horizontal position...
                    } else if (aux_hour == 9.0f || aux_hour == 21.0f || aux_hour == 3.0f || aux_hour == 15.0f){
                         xlineptr.x1 = (int)(hour_arm_length * (aux_hour == 9.0f || aux_hour == 21.0f ? -1.0f : 1.0f));

                         //we  draw the second arm horizontally
                        _iocs_txxline(&xlineptr);
                    //if in another position...
                    } else {
                        calculate_arms(
                           aux_hour * 5.0f,
                           hour_arm_length,
                           &tlptr.x1,
                           &tlptr.y1
                        );
                        //we  draw the second arm
                        _iocs_txline(&tlptr);
                    }

                    //minute
                    //if the minute is in a vertical position...
                    if(minute == 0 || minute == 30){
                        ylineptr.y1 = (int)second_arm_length * (minute == 30 ? 1 : -1);

                        //we  draw the minute arm vertically
                        _iocs_txyline(&ylineptr);
                    //if the minute is in an horizontal position...
                    } else if(minute == 15 || minute == 45) {
                        xlineptr.x1 = (int)second_arm_length * (minute == 45 ? -1 : 1);

                        //we  draw the second arm horizontally
                        _iocs_txxline(&xlineptr);
                    //if in another position...
                    } else {
                        calculate_arms(minute, minute_arm_length, &tlptr.x1, &tlptr.y1);

                        //we  draw the second arm
                        _iocs_txline(&tlptr);
                    }

                    //second
                    //if the second is in a vertical position...
                    if(second == 0 || second == 30){
                        ylineptr.y1 = (int)second_arm_length * (second == 30 ? 1 : -1);

                        //we  draw the second arm vertically
                        _iocs_txyline(&ylineptr);
                    //if the second is in a vertical position...
                    } else if(second == 15 || second == 45) {
                        xlineptr.x1 = (int)second_arm_length * (second == 45 ? -1 : 1);

                        //we  draw the second arm horizontally
                        _iocs_txxline(&xlineptr);
                    //if in another position...
                    } else {

                        calculate_arms(second, second_arm_length, &tlptr.x1, &tlptr.y1);

                        //we  draw the second arm
                        _iocs_txline(&tlptr);
                    }
                }
            }
        }

        mutex = 1; //we release the mutex
    }
}

void showStatus(int status)
{
    if(status < 0){
        switch(status){
            case -1: _dos_c_print("Executed invalid function code\r\n"); break;
            case -2: _dos_c_print("Specified file not found\r\n"); break;
            case -3: _dos_c_print("Specified directory not found\r\n"); break;
            case -4: _dos_c_print("Too many open files\r\n"); break;
            case -5: _dos_c_print("Cannot access directory or volume label\r\n"); break;
            case -6: _dos_c_print("Specified handle is not open\r\n"); break;
            case -7: _dos_c_print("Memory manager region was destroyed\r\n"); break;
            case -8: _dos_c_print("Not enough memory to execute\r\n"); break;
            case -9: _dos_c_print("Invalid memory manager pointer specified\r\n"); break;
            case -10: _dos_c_print("Illegal environment specified\r\n"); break;
            case -11: _dos_c_print("Abnormal executable file format\r\n"); break;
            case -12: _dos_c_print("Abnormal open access mode\r\n"); break;
            case -13: _dos_c_print("Error in selecting a filename\r\n"); break;
            case -14: _dos_c_print("Called with invalid parameter\r\n"); break;
            case -15: _dos_c_print("Error in selecting a drive\r\n"); break;
            case -16: _dos_c_print("Cannot remove current directory\r\n"); break;
            case -17: _dos_c_print("Cannot ioctrl device\r\n"); break;
            case -18: _dos_c_print("No more files found\r\n"); break;
            case -19: _dos_c_print("Cannot write to specified file\r\n"); break;
            case -20: _dos_c_print("Specified directory already registered\r\n"); break;
            case -21: _dos_c_print("Cannot delete because file exists\r\n"); break;
            case -22: _dos_c_print("Cannot name because file exists\r\n"); break;
            case -23: _dos_c_print("Cannot create file because disk is full\r\n"); break;
            case -24: _dos_c_print("Cannot create file because directory is full\r\n"); break;
            case -25: _dos_c_print("Cannot seek to specified location\r\n"); break;
            case -26: _dos_c_print("Specified supervisor mode with supervisor status on\r\n"); break;
            case -27: _dos_c_print("Thread with same name exists\r\n"); break;
            case -28: _dos_c_print("Interprocess communication buffer is write-protected\r\n"); break;
            case -29: _dos_c_print("Cannot start any more background processes\r\n"); break;
            case -32: _dos_c_print("Not enough lock regions\r\n"); break;
            case -33: _dos_c_print("Locked; cannot access\r\n"); break;
            case -34: _dos_c_print("Handler for specified drive is opened\r\n"); break;
            case -35: _dos_c_print("Symbolic link nest exceeded 16 steps (lndrv)\r\n"); break;
            case -80: _dos_c_print("File exists\r\n"); break;
            default: printf("unknown status %d\r\n", status);
        }
        return;
    }

    printf("Status: %d\r\n", status);
}
