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

#define 	TIMER_C 	0x45		/* vector number to hook  MFP Timer-C (Mouse/cursor/FDD control, etc.) */
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
int collisions = 0;
volatile char mutex = 1;
int tick = 0;

const int centerX = 500;
const int centerY = 100;
const float armLength = 50.0f;
const float PI = M_PI;

#define TEXT_PLANE 2

inline void my_print(const char*);

//the resident program is this interrupt
void interrupt process_start() //it has to be an interrupt
{
    //we check whether the interrupt is still being processed
    if(mutex){
        mutex = 0; //we hold the mutex
        //only do things every 200 cycles
        if(++counter % 200 == 0){
            char buffer[50];

            switch(command){
                case 0: //just a text counter in the top-left corner
                    sprintf(buffer, "counter: %d\n", counter);
                    my_print(buffer);
                    break;
                case 1: //we are drawing something
                    {
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
                            float angle = (float) (tick++ * 6); // Angle of rotation in degrees (adjust as needed)

                            // Convert the angle to radians
                            float angleRadians = (angle * PI) / 180.0f;

                            // Calculate the endpoint coordinates based on the angle
                            int endX = (int) armLength * cos(angleRadians);
                            int endY = (int) armLength * sin(angleRadians); // Subtract because Y-axis is inverted



                            struct _tboxptr tboxptr = {
                                /* unsigned short vram_page */  TEXT_PLANE, //0, 1, 2, 4. Plane 2 is fixed. It won't move up with the scroll
                                /* short x */                   centerX - 50,
                                /* short y */                   centerY - 50,
                                /* short x1 */                  100,
                                /* short y1 */                  100,
                                /* unsigned short line_style */ (0xf0 << (tick % 8)) | (0xf0 >> (8 - (tick % 8))) // dashed
                            };

                            //we draw this arm
                            struct _tlineptr tlptr = {
                                /* unsigned short vram_page */  TEXT_PLANE, //0, 1, 2, 4. Plane 2 is fixed. It won't move up with the scroll
                                /* short x */                   centerX,
                                /* short y */                   centerY,
                                /* short x1 (length) */         endX,
                                /* short y1 (length) */         endY,
                                /* unsigned short line_style */ 0xff // solid
                            };

                            //we draw the box
                            _iocs_txbox(&tboxptr);

                            //we  draw the arm
                            _iocs_txline(&tlptr);
                        }
                    }
                    break;
            }
        }
        mutex = 1; //we release the mutex
    }
}

//this function is inline to have a copy of it where ever is used
inline void my_print(const char *message)
{
    //we get the current position of the cursor
    int cursor_pos = _iocs_b_locate(-1, -1);

    //we move the cursor to the top left corner
    _iocs_b_locate(0, 0);
    //we print the message
    _dos_c_print(message);
    //we re establish cursor's position
    _iocs_b_locate((cursor_pos >> 16) & 0xffff, cursor_pos & 0xffff);
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
                char *endptr;
                short num = strtol(argv[2], &endptr, 5);
                if (*endptr == '\0') {
                    resident_aux->command = num;
                } else {
                    _dos_c_print("Input is not a valid number.\r\n");
                }
            }
        }
    }
    _dos_exit();
}
