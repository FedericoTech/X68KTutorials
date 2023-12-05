	.include		doscall.mac
	.include		iocscall.mac
	.text
	.even

main:

    move.l #31, -(sp)   ;y
    move.l #0, -(sp)    ;x
    jsr __iocs_b_locate
    addq.l #8, sp

	pea	    msg_calling_functions
	jsr     __dos_print   ; print


	move.w #31, d2      ;y
    move.w #0,  d1      ;x
    IOCS _B_LOCATE
    addq.l	#8, sp

    pea msg_dos_calls
	DOS _PRINT          ; print
	addq.l	#4, sp

	bra		exit_ok		; exit

exit_ok
	DOS		_EXIT
    .data

msg_calling_functions:
    dc.b	'Calling functions', $0d, $0a,0

msg_dos_calls:
    dc.b	'Dos calls', $0d, $0a, 0

    .end	main	    * run from main.
