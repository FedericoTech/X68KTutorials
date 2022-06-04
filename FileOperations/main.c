#ifdef __MARIKO_CC__
    #include <doslib.h>
    #include <iocslib.h>
#else
    #include <dos.h>
    #include <iocs.h>
    #define interrupt __attribute__ ((interrupt_handler))
#endif

#include <stdint.h>

/**
 * In this example we are creating a file, then storing a message in it and finally closing the file.
 * Afterwards we open de file again in read mode and retrieve the message.
 * At the end we remove the file.
 */

//permissions for using with _dos_open
#define ACCESS_DICTIONARY           0x80    //0b10000000 //1 not for users
#define ACCESS_NORMAL               0x00    //0b00000000 //0

#define SHARING_TOTAL               0x08    //0b00010000  //4 allow others to write and read
#define SHARING_WRITE_ONLY          0x0C    //0b00001100  //3 allow others to write only
#define SHARING_READ_ONLY           0x08    //0b00001000  //2 allow others to read only
#define SHARING_RESTRICTED          0x04    //0b00000100  //1 don't allow others anything
#define SHARING_COMPATIBILITY_MODE  0x00    //0b00000000  //0

#define MODE_RW                     0x02    //0b00000010  //2 open for write and read
#define MODE_W                      0x01    //0b00000001  //1 open only for writing
#define MODE_R                      0x00    //0b00000000  //0 open only for reading

#define OPENING_MODE(access, sharing, mode) (access | sharing | mode)

const char *getErrorMessage(int8_t code);

int main(void)
{
    int16_t file_number, status = 0;

    //We create a file and open it so we get its number to manage it.
    file_number = _dos_create(
        "file", //file name
        0       //if 0 overwrite if exist, if 1 don't overwrite
    );

    //if any error...
    if(file_number < 0){
        _dos_c_print("Can't create the file\r\n");
        _dos_c_print(getErrorMessage(file_number));
        _dos_exit2(file_number);
    }

    {
        //the message we are going to store.
        char message[] = "Hello World!";

        //we write the message in the file
        status = _dos_write (
            file_number,    //this is like the file handler in stdio but represented by an integer
            message,        //the message
            sizeof message //the size of the message
        );
    }

    //if any error...
    if(status < 0){
        _dos_c_print("Can't write in the file\r\n");
        _dos_c_print(getErrorMessage(status));
        _dos_exit2(status);
    }

    //now we close the file
    status = _dos_close(file_number);

    //if any error...
    if(status < 0){
        _dos_c_print("Can't close the file\r\n");
        _dos_c_print(getErrorMessage(status));
        _dos_exit2(status);
    }

    //now we open the same file and capture the handler number
    file_number = _dos_open(
        "file",                             //file name
        OPENING_MODE(
            ACCESS_NORMAL,
            SHARING_COMPATIBILITY_MODE,
            MODE_R                          //only read mode
        )
    );

    //if any error...
    if(file_number < 0){
        _dos_c_print("Can't open the file\r\n");
        _dos_c_print(getErrorMessage(file_number));
        _dos_exit2(file_number);
    }

    {
        //buffer where we will store the data from the file
        char message_in[13] = {' '};

        //we read the file and capture the number of characters we manage to read
        int16_t bytes_read = _dos_read(
            file_number,        //handler number
            message_in,         //destination
            sizeof(message_in)  //bytes to read
        );

        //if any error...
        if(bytes_read < 0){
            _dos_c_print("Can't read from the file\r\n");
            _dos_c_print(getErrorMessage(file_number));
            _dos_exit2(file_number);
        }

        //we print the message
        _dos_c_print(message_in);
    }

    //we close the file with the handler number
    status = _dos_close(file_number);

    //if any error...
    if(status < 0){
        _dos_c_print("Can't close the file\r\n");
        _dos_c_print(getErrorMessage(status));
        _dos_exit2(status);
    }

    //we delete the file
    status = _dos_delete ("file");

    //if any error...
    if(status < 0){
        _dos_c_print("Can't delete the file\r\n");
        _dos_c_print(getErrorMessage(status));
        _dos_exit2(status);
    }

    _dos_c_print("\r\nDone\r\n");

    _dos_exit();
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
