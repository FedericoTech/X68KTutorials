#ifdef __MARIKO_CC__
	#include <doslib.h>
	#include <iocslib.h>
#else
	#include <dos.h>
	#include <iocs.h>
	#define interrupt __attribute__ ((interrupt_handler))
#endif

void showStatus(int);

/*
struct _psp {
    char *env;
    void *exit;
    void *ctrlc;
    void *errexit;
    char *comline;
    unsigned char handle[12];
    void *bss;
    void *heap;
    void *stack;
    void *usp;
    void *ssp;
    unsigned short sr;
    unsigned short abort_sr;
    void *abort_ssp;
    void *trap10;
    void *trap11;
    void *trap12;
    void *trap13;
    void *trap14;
    unsigned int_ osflg;
    unsigned char reserve_1[28];
    char exe_path[68];
    char exe_name[24];
    char reserve_2[36];
};
*/



int main(void)
{
    {
        struct _psp *psp;
        //we request info about the present process
        psp = _dos_getpdb();

        //we show the executable name
        _dos_c_print(psp->exe_name);
        _dos_c_print("\r\n");
    }

    /**
     * Testing: void *_dos_malloc (int_ size)
     */
    {
        int status;

        char *some_memory = _dos_malloc(1024);       //will success both, allocation and resizing
        //char *some_memory = _dos_malloc(0x1000000); //will file for sure
        //char *some_memory = _dos_malloc(1189840); //max allocable so the resize will fail

        _dos_c_print("Testing: void *_dos_malloc (int_ size)\r\n");

        status = (unsigned int)some_memory;

        //printf("[%x][%d]\r\n", (status >> 24) & 0xFF, ((status >> 24) & 0xFF) == 0x81);

        if(((status >> 24) & 0xFF) == 0x81){
            printf("only this %d bytes are available\r\n", status & 0x00FFFFFF);
            _dos_exit2((status >> 24) & 0xFF);
        } else if(((status >> 24) & 0xFF) == 0x82) {
            printf("not size at all can be allocated [%d]\r\n", status & 0x00FFFFFF);
            _dos_exit2((status >> 24) & 0xFF);
        } else {
            printf("block successfully allocated [%x]\r\n", status);
        }

        //we increase the block
        status = _dos_setblock(some_memory, 2048);

        //if not all the memory could be resized...
        if(((status >> 24) & 0xFF) == 0x81){
            printf("only this %d bytes are available for resizing.\r\n", status & 0x00FFFFFF);
        } else if(((status >> 24) & 0xFF) == 0x82) {
            printf("not size at all could be resized [%d]\r\n", status & 0x00FFFFFF);
        } else {
            //if status is 0 then it was successful
            printf("block successfully increased [%d]\r\n", status);
        }

        status = _dos_mfree(some_memory);
        if(status == 0) {
            printf("block successfully freed [%d]\r\n", status);
        } else {
            printf("block couldn't be freed [%d]\r\n", status);
            showStatus(status);
            _dos_exit2(status);
        }
    }

    /**
     * Testing: void *_dos_malloc2 (int_ md, int_ size)
     *
     * MD = 0 Search from lower level
     * MD = 1 Search from the minimum required block
	 * MD = 2 Search from top
     */
    {
        int status;

        char *some_memory;

        //we alocate these tree consecutive blocks
        char *block0 = _dos_malloc(1024);
        char *block1 = _dos_malloc(512);
        char *block2 = _dos_malloc(1024);

        printf("block0: %x block1: %x block2: %x\r\n", block0, block1, block2);

        //we remove the block in the middle to leave a hole
        _dos_mfree(block1);


        //now we test
        some_memory = _dos_malloc2(0, 1024); //low level, address: dda20
        //some_memory = _dos_malloc2(1, 1024); //min require block, address: dda20
        //some_memory = _dos_malloc2(2, 1024); //min require block, address: 1ffc00 //_dos_setblock doesn't seem to work with this option as it allocates on the top so it can't grow

        _dos_mfree(block0);

        _dos_c_print("Testing: void *_dos_malloc2 (int_ md, int_ size)\r\n");

        status = (unsigned int)some_memory;

        printf("adress: %x dec: %d\r\n", status, status);

        if(((status >> 24) & 0xFF) == 0x81){
            printf("only this %d bytes are available\r\n", status & 0x00FFFFFF);
            _dos_exit2((status >> 24) & 0xFF);
        } else if(((status >> 24) & 0xFF) == 0x82) {
            printf("not size at all can be allocated [%d]\r\n", status & 0x00FFFFFF);
            _dos_exit2((status >> 24) & 0xFF);
        } else {
            printf("block successfully allocated [%x]\r\n", status);
        }

        //we increase the block
        status = _dos_setblock(some_memory, 2048);

        //if not all the memory could be resized...
        if(((status >> 24) & 0xFF) == 0x81){
            printf("only this %d bytes are available for resizing.\r\n", status & 0x00FFFFFF);
        } else if(((status >> 24) & 0xFF) == 0x82) {
            printf("not size at all could be resized [%d]\r\n", status & 0x00FFFFFF);
        } else {
            //if status is 0 then it was successful
            printf("block successfully increased [%d]\r\n", status);
        }

        _dos_mfree(block2);
        status = _dos_mfree(some_memory);
        if(status == 0) {
            printf("block successfully freed [%d]\r\n", status);
        } else {
            printf("block couldn't be freed [%d]\r\n", status);
            showStatus(status);
            _dos_exit2(status);
        }
    }

    //void *_dos_malloc0 (int_ md, int_ size, struct _psp *pspadr)
    //void *_dos_malloc2 (int_ md, int_ size)

    //void *_dos_s_malloc (int_ md, int_ size)
    //void *_dos_s_malloc0 (int_ md, int_ size, struct _psp *pspadr)

    // struct _psp stands for Process stack pointer
    //struct _psp *_dos_getpdb (void_);
    //struct _psp *_dos_setpdb (struct _psp *);



    _dos_exit();
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
