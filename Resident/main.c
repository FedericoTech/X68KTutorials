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
    short command;
    void * oldvector;
};

extern char keyword[4];
extern short command;
extern void * oldvector;


asm( "	_keyword:	.ds.b	4	    " );
asm( "	_command:   .ds.w	1		" );
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

//the resident program is this interrupt
void interrupt process_start() //it has to be an interrupt
{
    //we check whether the interrupt is still being processed
    if(mutex){
        mutex = 0; //we hold the mutex
        //only do things every 200 cycles
        if(++counter % 100 == 0){

            switch(command){
                case 0: //just a text counter in the top-left corner
                    {
                        int auxtime = _dos_gettim2();
                        my_print("counter: %d\n", counter);



                        //if time has changed...
                        if(auxtime != currentime){
                            currentime = auxtime;

                            my_print(
                                "Time: %02d:%02d:%02d\n",
                                currentime >> 16,
                                (currentime >> 8) & 0x3F,
                                currentime & 0x3F
                            );
                        }
                    }
                    break;
                case 1: //we are drawing an analog clock.
                    {
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

                            {
                                struct _tboxptr tboxptr = {
                                    /* unsigned short vram_page */  TEXT_PLANE, //0, 1, 2, 4. Plane 2 is fixed. It won't move up with the scroll
                                    /* short x */                   centerX - 50,
                                    /* short y */                   centerY - 50,
                                    /* short x1 */                  100,
                                    /* short y1 */                  100,
                                    /* unsigned short line_style */ (0xf0 << (counter % 8)) | (0xf0 >> (8 - (counter % 8))) // dashed
                                };

                                //we draw the box
                                _iocs_txbox(&tboxptr);
                            }


                            {
                                struct _tlineptr tlptr = {
                                    /* unsigned short vram_page */  TEXT_PLANE, //0, 1, 2, 4. Plane 2 is fixed. It won't move up with the scroll
                                    /* short x */                   centerX,
                                    /* short y */                   centerY,
                                    /* short x1 (length) */         0,  //for initialization purposes
                                    /* short y1 (length) */         0,  //for initialization purposes
                                    /* unsigned short line_style */ 0xff // solid
                                };

                                struct _ylineptr ylineptr = {
                                    /* unsigned short vram_page */  TEXT_PLANE,
                                    /* short x */                   centerX,
                                    /* short y */                   centerY,
                                    /* short y1 */                  0,  //for initialization purposes,
                                    /* unsigned short line_style */ 0xff // solid
                                };

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
/*
                                my_print(
                                    "Time: %02d:%02d:%02d  length %f, %aux_hour:%f \n",
                                    currentime >> 16,
                                    (currentime >> 8) & 0x3F,
                                    currentime & 0x3F,
                                    hour_arm_length,
                                    aux_hour
                                );
*/
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
                    break;
            }
        }
        mutex = 1; //we release the mutex
    }
}

inline void calculate_arms(const float tick, const float armLength, short * endX, short * endY)
{
    float angle = tick * 6.0f - 90.0f; // Angle of rotation in degrees (adjust as needed)

    // Convert the angle to radians
    float angleRadians = radians * angle;

    // Calculate the endpoint coordinates based on the angle
    *endX = (short) armLength * cos(angleRadians);
    *endY = (short) armLength * sin(angleRadians); // Subtract because Y-axis is inverted
}


//this function is inline to have a copy of it where ever is used
void my_print(const char *format, ...)
{
    //we get the current position of the cursor
    int cursor_pos = _iocs_b_locate(-1, -1);

    va_list args;
    va_start(args, format);

    //we move the cursor to the top left corner
    _iocs_b_locate(0, 0);
    //we print the message
    //_dos_c_print(message);
    vprintf(format, args);
    //we re establish cursor's position
    _iocs_b_locate((cursor_pos >> 16) & 0xffff, cursor_pos & 0xffff);
    va_end(args);
}



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

            //printf("[%s]\n", keyword);

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
             "/c [0-1] to change command.\r\n"
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
                command = 1;

                //we make the program resident
                _dos_keeppr(
                    end_addr - resident_addr,   //memory from beginning of the program
                    0                           //exit code.
                );

                _dos_c_print("this line doesn't get to run\r\n");
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
                _dos_mfree(&resident_aux->procc.psp); //psp_addr

                _dos_c_print("resident program stopped\r\n");
            } else {
                _dos_c_print("The program is not resident yet\r\n");
            }
        }

        if(argc > 2){
            if(strncmp("/c", argv[1], 2) == 0
               || strncmp("/C", argv[1], 2) == 0
            ){
                //if the program is resident...
                if(oldvector != 0){
                    char *endptr;
                    short num = strtol(argv[2], &endptr, 5);
                    if (*endptr == '\0') {
                        resident_aux->command = num;
                    } else {
                        _dos_c_print("Input is not a valid number.\r\n");
                    }
                //if the program isn't resident yet...
                } else {
                    _dos_c_print("The program is not resident yet\r\n");
                }
            }
        }
    }
    _dos_exit();
}
