| main (__argc, __argv, environ)

.set Y_VALUE, 31
.set X_VALUE, 0

.set _B_LOCATE, 0x23
.set _PRINT, 0xff09
.set _EXIT, 0xff00

.text
.even
.global main
.type main,@function

main:
    move.l %sp@(4), %d0   | __argc (number of command-line arguments)
    move.l %sp@(8), %a0   | __argv (pointer to an array of pointers to arguments)
    move.l %sp@(12), %a1  | environ (pointer to an array of pointers to environment strings)

    move.l #Y_VALUE, %sp@-  |y
    move.l #X_VALUE, %sp@-  |x
    jsr _iocs_b_locate
    addq.l #8, %sp

	pea	    msg_calling_functions
	jsr     _dos_print     | print


	move.w  #Y_VALUE, %d2   |y
    move.w  #X_VALUE, %d1   |x
    moveq   #_B_LOCATE, %d0
    trap	#15
    move.l	%sp@+, %d2

    pea msg_dos_calls
	.short _PRINT           | print
	addq.l	#4, %sp

	bra		exit_ok		    | exit

exit_ok:
	.short		_EXIT

.data

msg_calling_functions:
    .string	"Calling functions\r\n"

msg_dos_calls:
    .string	"Dos calls\r\n"

    .end	main	    | run from main.
