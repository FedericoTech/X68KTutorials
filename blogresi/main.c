#ifdef __MARIKO_CC__
	#include <doslib.h>
	#include <iocslib.h>
#else
	#include <dos.h>
	#include <iocs.h>
	#include <stdio.h>

	//substitutions for Lyndux toolchain

	#define interrupt __attribute__ ((interrupt_handler))
	#define _mep dos_mep
	#define _psp dos_psp
	#define oldvector _oldvector

	// _dos_getpdb() is broken
	extern int _PSP;
	#define _dos_getpdb() (void*)_PSP
#endif

#define 	TIMER_C 	0x45
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

void interrupt process_start();	//forward declaration valid before the assembly.

//these have to be the very first thing
asm( "	_keyword:	.ds.b	4	    " ); // zero initialized 4 byte storage for keyword
asm( "	_oldvector: .dc.l	0		" ); // oldvector address also initialized by 0

int main(int argc, char *argv[])
{
    struct resident * _resident, * resident_aux;

    int resident_start_addr;
    struct _psp* psp = _dos_getpdb();

    _resident = (struct resident *)((unsigned int) psp - sizeof(struct _mep));

    resident_start_addr = (unsigned int) _resident->procc.mep.prev_mp;

	while(resident_start_addr != 0){
		char keyword[4] = {0}; // we will copy the first 4 bytes found in other programs in this var.
        void* beginning_of_program;
        // we read a long word with _iocs_b_lpeek so that we don't need to go Super
        resident_start_addr = _iocs_b_lpeek((void *) resident_start_addr);

		//we capture the address of the beginning of the program
		resident_aux = (struct resident *) resident_start_addr;

		//we copy three byte in our buffer. we use _iocs_b_memstr so that we don't need to go Super
        _iocs_b_memstr(
            resident_aux->keyword,
            keyword,
            3
        );

        //if we find the process
        if(strncmp(KEYWORDS, keyword, 3) == 0){
            //we copy the current value of the offset 4 from the beginning of the program which is the  oldvector declared in assembly.
            oldvector =  (void *) _iocs_b_lpeek(&resident_aux->oldvector);
            break;
        }
	}

	// if found...
	if(oldvector) {
		// we remove it
		_dos_intvcs(TIMER_C, oldvector); //we reestablish 0x45
		_dos_mfree(&resident_aux->procc.psp); //psp_addr
		_dos_c_print("resident program stopped\r\n");
	//if not found
	} else {
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

        _dos_c_print("resident program started\r\n");

        //we make the program resident
        _dos_keeppr(
            end_addr - resident_addr,   //memory from beginning of the program
            0                           //exit code.
        );
	}
}

int counter;
int volatile mutex = 1;

//the resident program is this interrupt
void interrupt process_start() //it has to be an interrupt
{
	//we check whether the interrupt is still being processed
	if(mutex){
		mutex = 0; //we hold the mutex

		//only do things every 200 cycles
        if(++counter % 100 == 0){
            //we get the current position of the cursor
            int cursor_pos = _iocs_b_locate(-1, -1);

            //we get the current colour.
            char previous_color = _iocs_b_color(-1);


            //we move the cursor to the top left corner
            _iocs_b_locate(0, 0);

            //we change the colour
            _iocs_b_color(2 + 4 + 8);

            printf("Count %d\n", counter);

            //we re establish cursor's position
            _iocs_b_locate((cursor_pos >> 16) & 0xffff, cursor_pos & 0xffff);

            //we re establish text colour
            _iocs_b_color(previous_color);
        }

		mutex = 1;
	}
}
