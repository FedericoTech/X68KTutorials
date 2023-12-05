#ifdef __MARIKO_CC__
	#include <doslib.h>
	#include <iocslib.h>
#else
	#include <dos.h>
	#include <iocs.h>
	#include <stdio.h>
	#define interrupt __attribute__ ((interrupt_handler))
#endif

extern void asm_hello();
extern void asm_hello_callback();
extern int asm_sum(int, int);
extern int asm_sum_callback(int, int);
extern int asm_running_c_function(int(*)(int, int), int, int);
extern void print_format(char *, int, int, int);

void hello_from_c()
{
    _dos_c_print("Hello from C !\r\n");
}

int add_from_c(int a, int b)
{
    //printf("a: %d, b: %d\n", a, b);
    return a + b;
}

int sub_from_c(int a, int b)
{
    return a - b;
}

int mul_from_c(int a, int b)
{
    return a * b;
}

int main(void)
{
    int result;
    //_dos_c_print("Hello world !\r\n");

    asm_hello();

    asm_hello_callback();

    result = asm_sum(5, 2);

    printf("result from asm_sum() is: %d and should be 7\n", result);

    result = asm_sum_callback(13, 2);

    printf("result from asm_sum_callback() is: %d and should be 15\n", result);

    result = asm_running_c_function(add_from_c, 2, 8);

    printf("result from asm_running_c_function(add_from_c()) is: %d and should be 10\n", result);

    result = asm_running_c_function(sub_from_c, 8, 2);

    printf("result from asm_running_c_function(sub_from_c()) is: %d and should be 6\n", result);

    result = asm_running_c_function(mul_from_c, 2, 8);

    printf("result from asm_running_c_function(mul_from_c()) is: %d and should be 16\n", result);

    result = asm_running_c_function(asm_sum, 2, 8);

    printf("result from asm_running_c_function(asm_sum()) is: %d and should be 10\n", result);

    result = asm_running_c_function(asm_sum_callback, 2, 8);

    printf("result from asm_running_c_function(asm_sum_callback()) is: %d and should be 10\n", result);

    print_format("%d + %d = %d\n", 1, 2, asm_sum(1, 2));

    _dos_exit();
}
