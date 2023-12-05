#ifdef __MARIKO_CC__
	#include <doslib.h>
#else
	#include <dos.h>
	#include <stdio.h>
	#define _mep dos_mep
	#define _psp dos_psp

	//extern int _PSP;
	//#define _dos_getpdb() (void*)_PSP

	#define oldvector _oldvector
	#define keyword _keyword
#endif

//#include "main.h"

#include "utils.h"
#include "clock.h"


#define 	TIMER_C 	0x45		/* 0x45 vector number to hook  MFP Timer-C (Mouse/cursor/FDD control, etc.) */
#define 	KEYWORDS	"MMV"

struct resident {
    struct {
        struct _mep mep; //16   bytes
        struct _psp psp; //240 bytes
    } procc;    //256 bytes
#ifndef __MARIKO_CC__
    char dummy[0xf4]; //human68k/lib/crt0.o in lydux
#endif
    char keyword[4];    //here the program starts
    void * oldvector;
};


extern char keyword[4];
extern void * oldvector;

int volatile can_be_declared_here;

/**
 * above can be declarations but not initializations nor functions.
 * The very first thing below this comment is those assembly commands
 * reserving space.
 * The object file 'main.o' has to be linked first.
 * You can achieve this by manipulating your '.cbp' project file and in
 * <Unit filename="main.c"> adding the element <Option weight="1" />
 * This way we will find these two variables right after the Program Segment Prefix struct
 * at the beginning of the program.
 */

//these have to be the very first thing
asm( "	_keyword:	.ds.b	4	    " );
asm( "	_oldvector: .dc.l	0		" );

int volatile an_initialized_here_but_not_avobe = 0;

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
                // oldvector = resident_aux->oldvector //seems to work this way too but to be sure I use _iocs_b_lpeek
                oldvector = (void *) _iocs_b_lpeek(&resident_aux->oldvector);
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

                _dos_mfree(&resident_aux->procc.psp); //psp_addr

                // we remove the clock
                clear_clock();

                _dos_c_print("resident program stopped\r\n");
            } else {
                _dos_c_print("The program is not resident yet\r\n");
            }
        }
    }
    _dos_exit();
}


