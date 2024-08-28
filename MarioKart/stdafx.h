#ifndef STDAFX_H_INCLUDED
#define STDAFX_H_INCLUDED

#ifdef __MARIKO_CC__
	#include <doslib.h>
	#include <iocslib.h>
#else
	#include <dos.h>
	#include <iocs.h>
	#include <stdio.h>
	#define interrupt __attribute__ ((interrupt_handler))
#endif

#include <stdint.h>
//#include <errno.h>

#include "utils.h"

#define DEBUG

#if defined(DEBUG) && !defined(__MARIKO_CC__)
    #define PRINT_FUNCTION() printf("%s\n", ___FUNCTION___)
#else
    #define PRINT_FUNCTION() // Do nothing
#endif


#endif // STDAFX_H_INCLUDED
