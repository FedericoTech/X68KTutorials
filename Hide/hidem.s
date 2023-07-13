;---------------------------------------------------------
;	Manual sample parent process (hidem.s)
;	Runs once at the beginning and becomes the parent process of the background process
;	Also runs when sending a password to stop the thread
;
;---------------------------------------------------------
	.include		doscall.mac
	.text
	.even
start:
    lea		stkptr,sp			; user stack setting
    bsr		version_chk			; DOS version check
    bsr		cl_cpy				; copy commandline
    bsr		get_id				; Are there threads?
    tst		d0
    bmi		exec_pr				; if not, run child process
    bra		pw_send				; if thre is, pass the password to the thread
exit_ok:
    DOS		_EXIT
exec_pr:						; child process execution
    bsr		m_free
    bsr		psp_read			; copy of your file name
    clr.l	-(sp)				; environment
    pea		cl_buff				; command line
    pea		(a0)				; child process file name
    move.w	d1, -(sp)			; mode
    DOS		_EXEC
    lea		14(sp),sp
    bsr		exec_chk
    bra		act_pr
m_free:							; free memory behind parent process
    lea		start-$100,a0
    lea		start-$100+$38,a1
    lea		$10(a0),a0
    move.l	(a1),a1
    sub.l	a0,a1
    move.l	a1,-(sp)
    move.l	a0,-(sp)
    DOS		_SETBLOCK
    addq.l	#8,sp
psp_read:						; copy filename from PSP
    move.b	start-$100+$60,d1
    add.b	#1,d1
    asl.w	#8,d1
    clr.b	d1
    lea		team_work,a1
    lea		start-$100+$80,a2
    bsr		str_cpy
    lea		start-$100+$c4,a2
    bsr		str_cpy
    lea		team_work,a0
    rts
exec_chk:
    cmp.l	#$10000,d0
    blt		exec_err
    bsr		get_id				; get thread id
    tst		d0
    bmi		nf_err
    rts
act_pr:
    move.l	#0,d5
    move.w	#$fffb,d6			; active thread
    bsr		send_cmd
    tst		d0
    bmi		send_err
    bra		exit_ok
pw_send:
    cmp.b	#4,(a3)				; is the command line 4 characters?
    bne		thd_err
    addq.l	#1,a3
    bsr		num_chk				; Is the commandline a number?
    move.l	#4,d5				; Send the password to the thread
    move.w	#$1000,d6
    bsr		send_cmd
    DOS		_CHANGE_PR
    move.l	#0,d5				; Send password to thread
    move.w	#$1000,d6
    bsr		send_cmd
    tst.l	d0
    bpl		send_err
    bra		end_msg
; Common subrutine
version_chk:
    DOS		_VERNUM				; Get DOS version
    cmp.w	#$200,d0			; Error before version 1.99
    blt		ver_err
    cmp.w	#$231,d0			; Version 2.50 and above
    bhi		ver_err				; 2.50 is judget to be equivalent to 3.00
    clr.l	d0					; Others are recognized as 2.00
    rts
cl_cpy:						    ; copy command line
    move.l	start-$100+$20,a2
    move.l	a2,a3
    cmp.b	#0,(a2)
    beq		use_disp
    lea		cl_buff,a1
str_cpy:						; string copy
    move.b	(a2)+,(a1)+
    tst.b	(a2)
    bne		str_cpy
    rts
send_wait:
    DOS		_CHANGE_PR
send_cmd:
    move.l	d5,-(sp)			; send command to thread
    lea		cl_buff,a0
    addq.l	#1,a0
    move.l	a0,-(sp)
    move.w	d6,-(sp)
    move.w	thd_id,-(sp)
    move.w	my_id,-(sp)
    DOS		_SEND_PR
    lea		14(sp),sp
    cmp.l	#-28,d0
    beq		send_wait
    rts
get_id:							; Subrutine for ID
    pea		my_tbl				; Find my ID
    move.w	#-2,-(sp)
    DOS		_GET_PR
    addq.l	#6,sp
    move.w	d0,my_id
gets_id:
    pea		thd_tbl				; get thread ID
    move.w	#-1,-(sp)
    DOS		_GET_PR
    addq.l	#6,sp
    move.w	d0,thd_id
    rts
num_chk:						; Is it a number?
    move.w	#3,d4
n_cmp:
    cmp.b	#$30,(a3)
    blt		thd_err
    cmp.b	#$39,(a3)+
    bhi		thd_err
    dbra	d4,n_cmp
    rts

; error messages
end_msg:
    pea		end_mes
	bra		err_msg
exec_err:
    pea		exr_msg
	bra		err_msg
nf_err:
    pea     nfr_msg
    bra     err_msg
use_disp:
    pea		use_msg
	bra		err_msg
par_err:
    pea		par_msg
	bra		err_msg
thd_err:
    pea		thd_msg
	bra		err_msg
send_err:
    pea		sdr_msg
	bra		err_msg
ver_err
    pea		ver_msg
	bra		err_msg
err_msg:
    DOS		_PRINT
	addq.l	#4,sp
	bra		exit_ok
	.data
end_mes:	dc.b	'File released!',$0d,$0a,0
nfr_msg:	dc.b	'Failed to register file',$0d,$0a,0
exr_msg:	dc.b	'execute child process',$0d,$0a,0
use_msg:	dc.b	'Usage hide [filename]',$0d,$0a
			dc.b	'Prohibits access to files until an arbitrary time.',$0d,$0a
			dc.b	'You will be asked for to enter'
			dc.b	'a password and time',$0d,$0a
			dc.b	'to unlock the command, follow the password'
			dc.b	'Please enter',$0d,$0a,$0d,$0a
			dc.b	'>hide XXXX (X is a half-width number)',$0d,$0a,$0d,$0a,0
thd_msg:	dc.b	'The file is already registered',$0d,$0a,0
sdr_msg:	dc.b	'communicate',$0d,$0a,0
par_msg:	dc.b	'Wrong parameters',$0d,$0a,0
ver_msg:	dc.b	'The DOS version is different',$0d,$0a,0
crlf:		dc.b	$0d,$0a,0
team_work:	ds.b	50
cl_buff:	ds.b	16			; commandline buffer
my_id:		even
			dc.w	-1			; my ID
thd_id:		dc.w	-1			; Thread ID
my_tbl		ds.b	116			; my management table
thd_tbl:	ds.b	96			; Thread management table
			dc.b	'NEWZ',0	; Thread name
			even
			dc.l	0
			ds.l	1024
stkptr:
			.end	start
