| something

.set _PRINT, 0xff09

.global asm_hello
.type asm_hello,@function

.global asm_hello_callback
.type asm_hello_callback,@function

.global asm_sum
.type asm_sum,@function

.global asm_sum_callback
.type asm_sum_callback,@function

.global asm_running_c_function
.type asm_running_c_function,@function

.global print_format
.type print_format,@function

	.text
	.even
asm_hello:
    pea		hello               | we load the address of hello into a1
    | move.l	#hello, %sp@-   | we push the address of hello into the stack
	.short  _PRINT          | we call the function print
	addq.l	#4, %sp         | we deallocate the stack by moving it up 4 bytes. addq is better because the instruction includes a 3 bit offset that can be 1 - 8
	rts

asm_hello_callback:
    jsr    hello_from_c
    rts

asm_sum:
    move.l  %sp@(4), %d0    | retrieve the first param from the stack
    move.l  %sp@(8), %d1    | retrive the second param from the stack taking into account that the SP has moved 4 bytes
    add.l   %d1, %d0        | add d1 into d0.
    rts                     | it is understood that the result is in d0.

asm_sum_callback:
    move.l  %sp@(8), %sp@-  | retrieve the second param from the stack and copy it back to the stack in the following position
    move.l  %sp@(8), %sp@-  | retrive the first param from the stack and copy it back to the stack in the following position (notice that we take from 12 because the stack has moved)
    jsr     add_from_c     | we call the add_from_c function.
    addq.l	#8, %sp         | we deallocate the stack by moving it up 8 bytes. addq is better because the instruction includes a 3 bit offset that can be 1 - 8
    rts;

asm_running_c_function:
    move.l  %sp@(12), %sp@- | we retrieve the second param and push it back onto the stack
    move.l  %sp@(12), %sp@- | we retrieve the first param and push it back onto the stak
    move.l  %sp@(12), %a0    | we load the address of the function that will do the operation into a0
    jsr     (%a0)            | we run the function from C whatever it is
    addq.l	#8, %sp         | we deallocate the stack by moving it up 8 bytes. addq is better because the instruction includes a 3 bit offset that can be 1 - 8
    rts

print_format:
    move.l  %sp@(16), %sp@- | we push operand 1 into the stack
    move.l  %sp@(16), %sp@- | we push operand 2 into the stack
    move.l  %sp@(16), %sp@- | we push sum "result" into the stack
    move.l  %sp@(16), %sp@- | we push the format into the stack
    jsr     printf
    add.l   #16, %sp        | we deallocate the stack by moving it up 8 bytes. because the offset is higher than 8 we use add.
    rts

    .data
hello:
    .string "Hello from ASM\r\n"

