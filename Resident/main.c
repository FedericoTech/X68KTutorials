#ifdef __MARIKO_CC__
	#include <doslib.h>
	#include <iocslib.h>
#else
	#include <dos.h>
	#include <iocs.h>
	#define interrupt __attribute__ ((interrupt_handler))
#endif

/*==========================================================*\
| Resident program skeleton								 	|
| Matsuura Hiroaki(hal@kmc.kyoto-u.ac.jp)					|
| This program is free and without warranty. 				|
| You are responsible for executing, modifying, 			|
| and distributing modified products of this program. 		|
\*==========================================================*/

#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#define 	TIMER_C 	0x45		/* vector number to hook  MFP Timer-C (Mouse/cursor/FDD control, etc.) */
#define 	KEYWORDS	"MMV"		/* keyword */

/*-------------------------------------------------------------*\
| 0x000 Start of memory management pointer		   				|
| 0x010 the beginning of the process management pointer (=_PSP) |
| 0x100 Beginning of program area (=_PSTA)	   					|
\*-------------------------------------------------------------*/
#define 	OFFSET_PROCESS	0x10    //16
#define 	OFFSET_PROGRAM	0x100   //256   OFFSET_PROGRAM - OFFSET_PROCESS = 240

/* The following three variables are set in the startup routine (__main). */
#ifndef __STDLIB_H		/* stdlib.h is also defined, thus preventing double definition*/
extern int _PSP;		/* Start address of process management pointer. Like _dos_getpdb() */
#endif
extern int _PSTA;		/* Start address of program area */
extern int _HEND;		/* Last address of heap area */

/*=================================================================*\
| Permanent Department												|
|																   	|
| Routine that performs actual processing when resident. 			|
| It is easier to process such as address calculation by placing	|
| the keyword that serves as a mark when the resident is released	|
| and the command passing part to the resident part at the beginning|
| of the resident part.		   										|
\*=================================================================*/
/* Declare variables and functions in asm() with extern */
extern char keyword[4];
extern int oldvector;
extern int command;

void process_start();

/* This is the start address (=_PSTA) of the program area */
asm( "	_keyword:	.ds.b	4		" );	/* keyword */
asm( "	_oldvector: .dc.l	0		" );	/* where to record the original vector */
asm( "	_command:	.dc.l	0		" );	/* Command recipient location (not used) */

asm( "	_process_start: 			" );	/* Resident section execution start address */
asm( "	movem.l d0-d7/a0-a6,-(sp)	" );	/* save register */
asm( "	bsr 	_prcs				" );	/* Call resident section main routine */
asm( "	movem.l (sp)+,d0-d7/a0-a6	" );	/* register recovery */
asm( "	move.l	_oldvector,-(sp)	" );	/* Event turnaround */
asm( "	rts 						" );

/*======================================================================================*\
| Resident section main routine															 |
|																						 |
| Program here what you want to process in the resident part.							 |
| Time-consuming processing and memory-tampering processing are definitely not allowed.	 |
\*======================================================================================*/
void prcs()
{
	int poi;
	int mx, my, cx, cy;

	_iocs_ms_curon();
	//we get mouse position which is given in a 32bit int
	poi = _iocs_ms_curgt();
	//we get the X from the most significant half
	mx = (poi >> 16) & 0xffff;
	//we get the Y from the less significant half
	my = poi & 0xffff;

	//we get the cursor position
	poi = _iocs_b_locate(-1, -1);
	//we get the X from the most significant half
	cx = (poi >> 16) & 0xffff;
	//we get the Y from the less significant half
	cy = poi & 0xffff;

	//we translate cursor coordinades to mouse coordinades
	cx *= 8;
	cy *= 16;

	//the mouse now goes after the cursor
	if( mx > cx )	mx--;
	if( mx < cx )	mx++;
	if( my > cy )	my--;
	if( my < cy )	my++;

	//we update the mouse's position.
	_iocs_ms_curst(mx, my);
}

/*=================================*\
| Non-resident department from here |
\*=================================*/
/*==========*\
| How to use |
\*==========*/
void usage(char *execname, int *addr)
{
	_dos_c_print("\nResident program example\r\n");
	printf("%s {-,/}(option)\n" , execname);
	_dos_c_print("option: s   :be resident\r\n");
	_dos_c_print("        r   :cancel the residency\r\n");
	_dos_c_print("        t   :Let's test the execution of a resident routine without making it resident\r\n");
	_dos_c_print("        SHIFT :Press key to exit\r\n");
	_dos_c_print("        h,? :give this help\r\n\n");
	printf("the current %s is resident\n", execname);
	if(addr == NULL){
		_dos_c_print("plug\r\n\n");
	} else {
		_dos_c_print("vinegar\r\n");
		printf("Resident address $%x, Vector number $%x (original vector $%x)\n\n", addr, TIMER_C, oldvector);
	}
	_dos_exit2(1);
}

/*===============*\
| Temporary staff |
\*===============*/
void main(int argc, char **argv)
{
	int ssp;
	int *addr;
    struct _psp *pspadr;
/*-------------------------------------------------------------------------------------------------*\
| Check if it is already resident.														          	|
| When the processing of this part is finished, the variables take the following values.	        |
|																		  			        		|
|       when already resident		        		                   when not yet resident   		|
|               addr	    resident process memory management pointer            NULL 				|
\*-------------------------------------------------------------------------------------------------*/

    pspadr = _dos_getpdb();


    ssp = _iocs_b_super(0);

printf("pspadr == _PSP ? [%d]\n", (int)pspadr == _PSP);
printf("struct _psp size [%d]\n", sizeof(struct _psp));
printf("_PSP: %d\n", _PSP);
printf("_PSP + size: %d\n", _PSP + sizeof(struct _psp));
printf("_PSP - OFFSET_PROCESS: %d\n", _PSP - OFFSET_PROCESS);
printf("_PSP - OFFSET_PROCESS + OFFSET_PROGRAM: %d\n", _PSP - OFFSET_PROCESS + OFFSET_PROGRAM);

//printf("_PSP - 16 == _PSTA ? [%d]\n", (_PSP - 16) == _PSTA);

//OFFSET_PROCESS = 16
//OFFSET_PROGRAM = 256



    //pspadr = (void*)_PSP;
	addr = (int*)(_PSP - OFFSET_PROCESS);	/* previous memory management pointer */
	oldvector = 0;

	while( addr != NULL ){		/* NULL then it's over */
		/* move to previous memory management pointer */
		addr = (int*)*addr;
		/* Compare top of program with keywords */
		if( strncmp(KEYWORDS, (char*)((int)addr + OFFSET_PROGRAM), 3) == 0 ){
			/* Take out the "original vector" stored in the resident part */
			oldvector = *(int*)((int)addr + OFFSET_PROGRAM + 4);
			break;
		}
	}
    _iocs_b_super(ssp);
/*-----------------------------------------------------------------*\
| Examine the arguments. It is unkind to suddenly stay permanently! |
\*-----------------------------------------------------------------*/
	//if there is no params...
	if(argc == 1){
        //we show the menu
		usage(argv[0], addr);
	}

	if(
		(argv[1][0] != '/')
		&& (argv[1][0] != '-')
	){
		usage(argv[0], addr);
	}

	switch( argv[1][1] ){
		case 'H':
		case 'h':
		case '?':	usage(argv[0], addr);
/*--------*\
| Resident |
\*--------*/
		case 's':
		case 'S':
		{
			if( addr != NULL ){
				printf("%s is already resident. Can't stay any longer\n", argv[0]);
				_dos_exit2(1);
			}
	/* set of keywords */
			strcpy(keyword, KEYWORDS);
	/* hook the vector */
			oldvector = _iocs_b_intvcs(TIMER_C, (int)process_start);
			printf("Vector number $%x hook up(original vector $%x)\n", TIMER_C, oldvector);
	/* Resident end. */
			printf("%s is resident.\n", argv[0]);
			_dos_keeppr(_HEND - _PSTA, 0);
		}
/*---------------------*\
| Resident cancellation |
\*---------------------*/
		case 'r':
		case 'R':
		{
			if( addr == NULL ){
				printf("%s is not resident\n", argv[0]);
				_dos_exit2(1);
			}
	/* return vector */
			_iocs_b_intvcs(TIMER_C, oldvector);
			printf("Vector number $%x to the original vector $%x back to\n", TIMER_C, oldvector);
	/* cancel residency */
			printf("%s unpersisted.\n", argv[0]);
			_dos_mfree((void*)((int)addr + OFFSET_PROCESS));
			_iocs_ms_curof();
			_dos_exit();
		}
/*---------------------*\
| Resident routine test |
\*---------------------*/
		case 't':
		case 'T':
		{
			volatile char *sft = (char*)0x80e;

			if(addr == NULL){
                ssp = _iocs_b_super(0);
				while((*sft & 1) == 0){
                    prcs();
                }
                _iocs_b_super(ssp);
				_iocs_ms_curof();
				_dos_exit();
			} else {
				printf("%s is already resident. It does not test the execution of resident routines.\n", argv[0]);
				_dos_exit2(1);
			}
		}
/*--------------*\
| optional error |
\*--------------*/
		default:
		{
			printf("invalid option%s\n", argv[1]);
			usage(argv[0], addr);
		}
	}
}
