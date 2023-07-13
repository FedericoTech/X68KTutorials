.include		doscall.mac

.globl _asm_hello
.globl _asm_hello_callback
.globl _asm_sum
.globl _asm_sum_callback
.globl _asm_running_c_function

	.text
	.even
_asm_hello:
    ;lea		hello,sp    ; we load the address of hello into a1
    move.l	#hello,-(sp)    ; we push the address of hello into the stack
	DOS		_PRINT          ; we call the function print
	addq.l	#4,sp           ; we deallocate the stack by moving it up 4 bytes
	rts

_asm_hello_callback:
    jsr    _hello_from_c
    rts

_asm_sum:
    move.l  4(sp), d0   ; retrieve the first param from the stack
    move.l  8(sp), d1   ; retrive the second param from the stack taking into account that the SP has moved 4 bytes
    add.l   d1, d0      ; add d1 into d0
    rts                 ; it is understood that the result is in d0

_asm_sum_callback:
    move.l  4(sp), -(sp)   ; retrieve the first param from the stack and copy it back to the stack in the following position
    move.l  12(sp), -(sp)   ; retrive the second param from the stack and copy it back to the stack in the following position (notice that we take from 12 because the stack has moved)
    jsr     _add_from_c
    addq.l	#8,sp          ; we deallocate the stack by moving it up 8 bytes
    rts;

_asm_running_c_function:
    move.l	4(sp), a0       ; we load 4 bytes from the address pointed by SP which contains the address to the function passed
    move.l  8(sp), -4(sp)    ; retrieve the first param from the stack taking into account that the SP has moved 8 bytes
    move.l  12(sp), -8(sp)   ; retrive the second param from the stack taking into account that the SP has moved 16 bytes
    subq.l	#8,sp           ; now we move the stack
    jsr     (a0)            ; we run the function from C whatever it is
    addq.l	#8,sp           ; we deallocate the stack by moving it up 8 bytes
    rts


    .data
hello:
    dc.b	'Hello from ASM',$0d,$0a,0
