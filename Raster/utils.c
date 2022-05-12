#include "utils.h"

static volatile uint32_t _time = 0;

static void interrupt timer_interrupt()
{
    _time++;
}

__inline uint32_t start_timer()
{
    return _iocs_timerdst(
        timer_interrupt,	//Processing address (interrupt disabled at 0)
        7,					//Unit time (1 = 1.0, 2 = 2.5, 3 = 4.0, 4 = 12.5, 5 = 16.0, 6 = 25.0, 7 = 50.0, micro sec unit)
        20					//Counter (when 0, treat as 256)
    );
}

__inline uint32_t stop_timer()
{
    return _iocs_timerdst(
        (void *)0,
        0,
        0
    );
}

__inline uint32_t millisecond()
{
    return _time;
}

const char *getErrorMessage(int8_t code)
{
    char *message;

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
        default: message = "No error\r\n"; break;
    }

    return message;
}

char * loadData(char * buffer, const char * filename, int8_t config)
{
    int16_t file_handler;
    int32_t status;
    int32_t status2;

    void interrupt vsync_disp();

    //we open the palette file
    file_handler = _dos_open(
        filename,
        config
    );

    //if any error...
    if(file_handler < 0){
        terminate();
        _dos_c_print("Can't open the file\r\n");
        _dos_c_print(filename);
        _dos_c_print("\r\n");
        _dos_c_print(getErrorMessage(file_handler));

        _dos_exit2(file_handler);
    }

    status = _dos_seek(
       file_handler,
       0, //offset
       2  //0 = beginning, 1 = on the spot, 2 = end
    );

    _dos_seek(
       file_handler,
       0, //offset
       0  //0 = beginning, 1 = on the spot, 2 = end
    );

    if(buffer == NULL){
        buffer = _dos_malloc(status);
    }

    status2 = _dos_read(file_handler, buffer, status);

    //printf("%s %d\n", filename, status2);

    //now we close the file
    status = _dos_close(file_handler);

    //if any error...
    if(status < 0){
        terminate();
        _dos_c_print("Can't close the file\r\n");
        _dos_c_print(getErrorMessage(status));
        _dos_exit2(status);
    }

    return buffer;
} //loadData
