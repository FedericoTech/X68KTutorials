.include		doscall.mac

.globl _asm_hello
.globl _asm_hello_callback
.globl _asm_sum
.globl _asm_sum_callback
.globl _asm_running_c_function
.globl _print_format

	.text
	.even
_asm_hello:
    pea		hello    ; we load the address of hello into a1
    ;move.l	#hello,-(sp)    ; we push the address of hello into the stack
	DOS		_PRINT          ; we call the function print
	addq.l	#4,sp           ; we deallocate the stack by moving it up 4 bytes. addq is better because the instruction includes a 3 bit offset that can be 1 - 8
	rts

_asm_hello_callback:
    jsr    _hello_from_c
    rts

_asm_sum:
    move.l  4(sp), d0   ; retrieve the first param from the stack
    move.l  8(sp), d1   ; retrive the second param from the stack taking into account that the SP has moved 4 bytes
    add.l   d1, d0      ; add d1 into d0.
    rts                 ; it is understood that the result is in d0.

_asm_sum_callback:
    move.l  8(sp), -(sp)    ; retrieve the second param from the stack and copy it back to the stack in the following position
    move.l  8(sp), -(sp)    ; retrive the first param from the stack and copy it back to the stack in the following position (notice that we take from 12 because the stack has moved)
    jsr     _add_from_c     ; we call the add_from_c function.
    addq.l	#8, sp          ; we deallocate the stack by moving it up 8 bytes. addq is better because the instruction includes a 3 bit offset that can be 1 - 8
    rts;

_asm_running_c_function:
    move.l  12(sp), -(sp)   ; we retrieve the second param and push it back onto the stack
    move.l  12(sp), -(sp)   ; we retrieve the first param and push it back onto the stak
    move.l  12(sp), a0      ; we load the address of the function that will do the operation into a0
    jsr     (a0)            ; we run the function from C whatever it is
    addq.l	#8,sp           ; we deallocate the stack by moving it up 8 bytes. addq is better because the instruction includes a 3 bit offset that can be 1 - 8
    rts

_print_format:
    move.l  16(sp), -(sp)    ; we push operant 1 into the stack
    move.l  16(sp), -(sp)   ; we push operant 2 into the stack
    move.l  16(sp), -(sp)   ; we push sum "result" into the stack
    move.l  16(sp), -(sp)   ; we push the format into the stack
    jsr     _printf
    add.l   #16, sp         ; we deallocate the stack by moving it up 8 bytes. because the offset is higher than 8 we use add.
    rts

    .data
hello:
    dc.b	'Hello from ASM',$0d,$0a,0
