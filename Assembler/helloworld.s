	.include		doscall.mac
	.text
	.even

main:
	lea		hello_msg,a1
	bsr     dos_print           * print
	bra		exit_ok				* exit

exit_ok
	DOS		_EXIT

dos_print:
    move.l	a1,-(sp)
	DOS		_PRINT
	addq.l	#4,sp
	rts
    .data
hello_msg:
    dc.b	'Hello world eo eo!!',$0d,$0a,0

    .end	main	    * run from main.
