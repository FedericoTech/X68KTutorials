#ifndef CLOCK_H
#define CLOCK_H

#ifdef __MARIKO_CC__
	#include <interrupt.h>
#else
	#define interrupt __attribute__ ((interrupt_handler))
#endif



void interrupt process_start();

void clear_clock();

#endif
