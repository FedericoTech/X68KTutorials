#include <dos.h>
#include <stdint.h>

//permissions for using with _dos_open
#define ACCESS_DICTIONARY 1 //not for users
#define ACCESS_NORMAL 0

#define SHARING_TOTAL 4                 //allow others to write and read
#define SHARING_WRITE_ONLY 3            //allow others to write only
#define SHARING_READ_ONLY 2             //allow others to read only
#define SHARING_RESTRICTED 1            //don't allow others anything
#define SHARING_COMPATIBILITY_MODE 0

#define MODE_RW 2                       //open for write and read
#define MODE_W 1                        //open only for writing
#define MODE_R 0                        //open only for reading

#define OPENING_MODE(access, sharing, mode) ((access << 7) | (sharing << 2) | mode)

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
        _dos_exit2(file_number);
    }

    //the message we are going to store.
    char message[] = "Hello World!";

    //we write the message in the file
    status = _dos_write (
        file_number,    //this is like the file handler in stdio but represented by an integer
        message,        //the message
        sizeof(message) //the size of the message
    );

    //if any error...
    if(status < 0){
        _dos_c_print("Can't write in the file\r\n");
        _dos_exit2(status);
    }

    //now we close the file
    status = _dos_close (file_number);

    //if any error...
    if(status < 0){
        _dos_c_print("Can't close the file\r\n");
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
        _dos_exit2(file_number);
    }

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
        _dos_exit2(file_number);
    }

    //we print the message
    _dos_c_print(message_in);

    //we close the file with the handler number
    status = _dos_close(file_number);

    //if any error...
    if(status < 0){
        _dos_c_print("Can't close the file\r\n");
        _dos_exit2(status);
    }

    //we delete the file
    status = _dos_delete ("file");

    //if any error...
    if(status < 0){
        _dos_c_print("Can't delete the file\r\n");
        _dos_exit2(status);
    }

    _dos_c_print("\r\nDone\r\n");

    _dos_exit();
}
