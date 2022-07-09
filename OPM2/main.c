#ifdef __MARIKO_CC__
	#include <doslib.h>
	#include <iocslib.h>
#else
	#include <dos.h>
	#include <iocs.h>
	#define interrupt __attribute__ ((interrupt_handler))
#endif

void opm_int();

int main(void)
{
    _dos_c_print("Hello world !\r\n");

    _iocs_opmintst(opm_int);

    while(_iocs_opmsns() != 0){
        asm("nop");
    }

    _iocs_opmset (0x20,  0xc7); //11 000 111    LR FFF CCC 	Chn 0-7 	F=Feedback, C=Connection
    _iocs_opmset (0x80,  0x1f); //00 0 11111    KK - AAAAA  Slot 1-32. Keyscale / Attack 	K=Keycale, A=attack
    _iocs_opmset (0xe0,  0x0f); //0 00 01111    E -- FFFFF 	Noise 	E=noise enable  F=Frequency (Noise only on Chn7 Slot32)
    _iocs_opmset (0x28,  0x4a); //0 100 1010    - OOO NNNN  Chn0-7 KeyCode 	O=Octave, N=Note
    _iocs_opmset (0x08,  0x08); //0 0001 000    - SSSS CCC 	Key On (Play Sound) 	C=Channel S=Slot (C2  M2  C1  M1)

    {
        int cont;
        for(cont = 0; cont < 202400; cont++){
            asm("nop");
        }
    }

    _iocs_opmset (0x28,  0x41); //0 100 1010    - OOO NNNN  Chn0-7 KeyCode 	O=Octave, N=Note
    _iocs_opmset (0x08,  0x08); //0 0001 000    - SSSS CCC 	Key On (Play Sound) 	C=Channel S=Slot (C2  M2  C1  M1)

    while(_iocs_opmsns() != 0){
        asm("nop");
    }

    {
        int cont;
        for(cont = 0; cont < 202400; cont++){
            asm("nop");
        }
    }

    _iocs_opmset (0x28,  0x42); //0 100 1010    - OOO NNNN  Chn0-7 KeyCode 	O=Octave, N=Note
    _iocs_opmset (0x08,  0x08); //0 0001 000    - SSSS CCC 	Key On (Play Sound) 	C=Channel S=Slot (C2  M2  C1  M1)

    _iocs_opmintst((void *) 0);

    _dos_c_print("done\r\n");

    _dos_exit();
}

void opm_int()
{
    _dos_c_print("OPM!\r\n");
}
