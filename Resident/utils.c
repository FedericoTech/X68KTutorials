#include <stdarg.h>
#include <stdio.h>

#include "utils.h"



//this function is inline to have a copy of it where ever is used
void my_print(const char* format, ...)
{
    //we get the current position of the cursor
    int cursor_pos = _iocs_b_locate(-1, -1);

    va_list args;
    va_start(args, format);

    //we move the cursor to the top left corner
    _iocs_b_locate(0, 0);
    //we print the message

    vprintf(format, args);

    //we re establish cursor's position
    _iocs_b_locate((cursor_pos >> 16) & 0xffff, cursor_pos & 0xffff);

    va_end(args);
}


void showStatus(int status)
{
    if(status < 0){
        switch(status){
            case -1: _dos_c_print("Executed invalid function code\r\n"); break;
            case -2: _dos_c_print("Specified file not found\r\n"); break;
            case -3: _dos_c_print("Specified directory not found\r\n"); break;
            case -4: _dos_c_print("Too many open files\r\n"); break;
            case -5: _dos_c_print("Cannot access directory or volume label\r\n"); break;
            case -6: _dos_c_print("Specified handle is not open\r\n"); break;
            case -7: _dos_c_print("Memory manager region was destroyed\r\n"); break;
            case -8: _dos_c_print("Not enough memory to execute\r\n"); break;
            case -9: _dos_c_print("Invalid memory manager pointer specified\r\n"); break;
            case -10: _dos_c_print("Illegal environment specified\r\n"); break;
            case -11: _dos_c_print("Abnormal executable file format\r\n"); break;
            case -12: _dos_c_print("Abnormal open access mode\r\n"); break;
            case -13: _dos_c_print("Error in selecting a filename\r\n"); break;
            case -14: _dos_c_print("Called with invalid parameter\r\n"); break;
            case -15: _dos_c_print("Error in selecting a drive\r\n"); break;
            case -16: _dos_c_print("Cannot remove current directory\r\n"); break;
            case -17: _dos_c_print("Cannot ioctrl device\r\n"); break;
            case -18: _dos_c_print("No more files found\r\n"); break;
            case -19: _dos_c_print("Cannot write to specified file\r\n"); break;
            case -20: _dos_c_print("Specified directory already registered\r\n"); break;
            case -21: _dos_c_print("Cannot delete because file exists\r\n"); break;
            case -22: _dos_c_print("Cannot name because file exists\r\n"); break;
            case -23: _dos_c_print("Cannot create file because disk is full\r\n"); break;
            case -24: _dos_c_print("Cannot create file because directory is full\r\n"); break;
            case -25: _dos_c_print("Cannot seek to specified location\r\n"); break;
            case -26: _dos_c_print("Specified supervisor mode with supervisor status on\r\n"); break;
            case -27: _dos_c_print("Thread with same name exists\r\n"); break;
            case -28: _dos_c_print("Interprocess communication buffer is write-protected\r\n"); break;
            case -29: _dos_c_print("Cannot start any more background processes\r\n"); break;
            case -32: _dos_c_print("Not enough lock regions\r\n"); break;
            case -33: _dos_c_print("Locked; cannot access\r\n"); break;
            case -34: _dos_c_print("Handler for specified drive is opened\r\n"); break;
            case -35: _dos_c_print("Symbolic link nest exceeded 16 steps (lndrv)\r\n"); break;
            case -80: _dos_c_print("File exists\r\n"); break;
            default: printf("unknown status %d\r\n", status);
        }
        return;
    }

    printf("Status: %d\r\n", status);
}
