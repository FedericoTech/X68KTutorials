#ifdef __MARIKO_CC__
	#include <doslib.h>
	#include <iocslib.h>
#else
	#include <dos.h>
	#include <iocs.h>
	#define interrupt __attribute__ ((interrupt_handler))
#endif

int main(void)
{
    _dos_c_print("Hello world !\r\n");

    _dos_exit();
}
