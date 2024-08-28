#include "utils.h"

#ifndef __MARIKO_CC__
    #include <stdio.h>
#endif // __MARIKO_CC__

const char *Utils_getErrorMessage(int8_t code)
{
    char *message;

    if(code >= 0){
        return "No error\r\n";
    }

    switch(code)
    {
        case -1: message = "Executed invalid function code\r\n"; break;
        case -2: message = "Specified file not found\r\n"; break;
        case -3: message = "Specified directory not found\r\n"; break;
        case -4: message = "Too many open files\r\n"; break;
        case -5: message = "Cannot access directory or volume label\r\n"; break;
        case -6: message = "Specified handle is not open\r\n"; break;
        case -7: message = "Memory manager region was destroyed\r\n"; break;
        case -8: message = "Not enough memory to execute\r\n"; break;
        case -9: message = "Invalid memory manager pointer specified\r\n"; break;
        case -10: message = "Illegal environment specified\r\n"; break;
        case -11: message = "Abnormal executable file format\r\n"; break;
        case -12: message = "Abnormal open access mode\r\n"; break;
        case -13: message = "Error in selecting a filename\r\n"; break;
        case -14: message = "Called with invalid parameter\r\n"; break;
        case -15: message = "Error in selecting a drive\r\n"; break;
        case -16: message = "Cannot remove current directory\r\n"; break;
        case -17: message = "Cannot ioctrl device\r\n"; break;
        case -18: message = "No more files found"; break;
        case -19: message = "Cannot write to specified file\r\n"; break;
        case -20: message = "Specified directory already registered\r\n"; break;
        case -21: message = "Cannot delete because file exists\r\n"; break;
        case -22: message = "Cannot name because file exists\r\n"; break;
        case -23: message = "Cannot create file because disk is full\r\n"; break;
        case -24: message = "Cannot create file because directory is full\r\n"; break;
        case -25: message = "Cannot seek to specified location\r\n"; break;
        case -26: message = "Specified supervisor mode with supervisor status on\r\n"; break;
        case -27: message = "Thread with same name exists\r\n"; break;
        case -28: message = "Interprocess communication buffer is write-protected\r\n"; break;
        case -29: message = "Cannot start any more background processes\r\n"; break;
        case -32: message = "Not enough lock regions\r\n"; break;
        case -33: message = "Locked; cannot access\r\n"; break;
        case -34: message = "Handler for specified drive is opened\r\n"; break;
        case -35: message = "Symbolic link nest exceeded 16 steps (lndrv)\r\n"; break;
        case -80: message = "File exists\r\n"; break;
        default: message = "Unknown error\r\n"; break;
    }

    return message;
}
