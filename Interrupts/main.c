#ifdef __MARIKO_CC__
	#include <doslib.h>
	#include <iocslib.h>

	#include <interrupt.h>
#else
	#include <dos.h>
	#include <iocs.h>
	#define interrupt __attribute__ ((interrupt_handler))
#endif

#define NULL 0

#define FN_JOYGET 0x013b

#define INT_EXITVC 0xfff0
#define INT_CTRLVC 0xfff1
#define INT_ERRJVC 0xfff2

void interrupt timer1();         // for timing

void interrupt timer2();         // for timing

void exiting_program();

void interrupt ctrl_c_exiting();

void interrupt error_exiting();

void my_iocs_joyget();         // custom _iocs_joyget function

int iocs_joyget_addr;

int main(void)
{

/*
    exiting_program;

    ctrl_c_exiting;

    error_exiting;
*/
    //we set up these interruptions to do things when exiting the program
    _dos_intvcs(INT_EXITVC, exiting_program);
    _dos_intvcs(INT_CTRLVC, ctrl_c_exiting);
    _dos_intvcs(INT_ERRJVC, error_exiting);

    //we backup the original function number
    iocs_joyget_addr = _iocs_b_intvcs(FN_JOYGET, (int)&my_iocs_joyget);
    //iocs_joyget_addr = (int) _dos_intvcs(FN_JOYGET, my_iocs_joyget); //equivalent?

/*
    _iocs_timerdst(
        timer1, //Processing address (interrupt disabled at 0)
        7,      //Unit time (1 = 1.0, 2 = 2.5, 3 = 4.0, 4 = 12.5, 5 = 16.0, 6 = 25.0, 7 = 50.0, micro sec unit)
        255       //Counter (when 0, treat as 256)
    );
*/
    _dos_c_print("Press a key.\r\n");

    while(_dos_inpout(0xFF) == 0){
        asm("nop");
        printf("iocs_joyget! %x\r\n", _iocs_joyget(1));
    }


    _iocs_timerdst(
        (void *)NULL,   //Processing address (interrupt disabled at 0)
        0,              //Unit time (1 = 1.0, 2 = 2.5, 3 = 4.0, 4 = 12.5, 5 = 16.0, 6 = 25.0, 7 = 50.0, micro sec unit)
        0               //Counter (when 0, treat as 256)
    );



    _dos_c_print("Done !\r\n");

    _dos_exit();
}

void interrupt timer1()
{
    _iocs_b_intvcs(0x44, (int)&timer2);
}

void interrupt timer2()
{
    printf("iocs_joyget! %d\r\n", _iocs_joyget(1));
}

void exiting_program()
{
    //we re establish the original function number
    _iocs_b_intvcs(FN_JOYGET, iocs_joyget_addr);
     _dos_c_print("Exiting the program.\r\n");
}

void interrupt ctrl_c_exiting()
{
    //we re establish the original function number
    _iocs_b_intvcs(FN_JOYGET, iocs_joyget_addr);
    _dos_c_print("You pressed Ctrl + C.\r\n");
}

void interrupt error_exiting()
{
    //we re establish the original function number
    _iocs_b_intvcs(FN_JOYGET, iocs_joyget_addr);
    _dos_c_print("There was an error.\r\n");
}

void my_iocs_joyget()
{
    //volatile register unsigned int_ reg_d0 __asm ("d0");
    //reg_d0 = -2;

    _dos_c_print("my_iocs_joyget!\r\n");
    ((void (*)(void)) iocs_joyget_addr)();
}
