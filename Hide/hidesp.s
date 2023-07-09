*----------------------------------------------------------
*
*	Backgroun processing sample		child process (hidsp.s)
*
*	The part from startp to endp is the resident part (background processing body)
*	and the rest is the non-resident part that is executed only once.
*
*-----------------------------------------------------------
	.include		doscall.mac
	.text
	.even
* Beginning of resident division
startp:
            move.l	sp,sp_work
            bsr		vect_set			* change destination of error report
send_chk:
            bsr		time_chk
            cmp.l	#-1,d0
            beq		fin
            lea		mes_buff,a3
            move.w	10(a3),d1			* Check for communication
            cmp.w	#-1,d1
            beq		no_send
            move.w	8(a3),d2			* Communication command
            cmp.w	#$1000,d2			* Check receive password command
            beq		pw_clr
            cmp.w	#$fff9,d2			* abandon Thread request
            bne		no_com_send
exit:							* abandon Thread
            DOS		_KILL_PR
no_com_send:					* initialize communication buffer
            move.w	#0,8(a3)
            move.w	#-1,10(a3)
no_send:
            move.l	#100,-(sp)
            DOS		_SLEEP_PR			* sleep and wait
            addq.l	#4,sp
            bra		send_chk
* comment
*
vect_set:						* Setting the processing destination when importing
            pea		ctrl_c
            move.w	#_CTRLVC,-(sp)
            DOS		_INTVCS
            addq.l	#6,sp
            rts
ctrl_c:					        * Error abort rutine
            move.l	sp_work,sp			* current SP for abort temporarily
            bra		no_send				* give up execution right and go to main
*	Handling messages (passwords)
time_chk:						* time comparison
            DOS		_GETTIM2
            tst		d0
            bmi		exit
            move.l	d0,d1
            clr.w	d0
            swap	d0
            cmp.b	hour,d0
            bne		exit_ts
            move.l	d1,d0
            lsr		#8,d0
            cmp.b	min,d0
            bne		exit_ts
            move.l	#-1,d0
exit_ts:
            rts
pw_clr:
            move.w	#1,d0				* Read common area
            bsr		com_rw
            lea.l	com_buff,a0
            lea.l	mes_data,a1
            addq.l	#2,a0
            move.l	#3,d1
pw_cmp:							* password comparison
            cmp.b	(a0)+,(a1)+
            bne		break
            dbra	d1,pw_cmp
            sub.l	#6,a0				* file close
            move.w	(a0),-(sp)
            DOS		_CLOSE
            addq.l	#2,sp
fin:
            pea		com_name			* Discard common name
            move.w	#5,-(sp)
            DOS		_COMMON
            addq.l	#6,sp
            DOS		_CHANGE_PR
            bra		exit
break:
            bra		no_com_send
com_rw:							* Common read/write subrutines
            move.l	#20,-(sp)
            pea		com_buff
            move.l	#0,-(sp)
            pea		com_name
            move.w	d0,-(sp)
            DOS		_COMMON
            lea		18(sp),sp
            rts
* Thread data
*	.data
com_name:
			dc.b	'GT-R',0
* Thread work area
*	.bss
hour		ds.b	1
min			ds.b	1
sp_work:	even
			ds.l	1
com_buff:	ds.b	20
mes_buff:						* inter-thread communication management buffer
			dc.l	4			*
			dc.l	mes_data
			dc.w	0
			dc.w	-1
mes_data:						* actual message
			ds.b	4
*			.stack
			ds.l	2048		* system stack for threads
ssp1:
			ds.l	512			* user stack for threads
usp1:
endp:
* Temporary from here
*	.text
main:
            lea		user,sp
            lea		com_buff,a0			* Common buffer
            bsr		fopen				* pointer file open
            bsr		get_pw				* password input
            bsr		get_tm				* time input
            bsr		com_w				* write common area
            bsr		thred_open			* create new thread
            bmi		exit_ok
            bra		keep				* end of residency
fopen:
            move.w	#$0,-(sp)			* Disable file access via sharing
            addq.l	#1,a2
            move.l	a2,-(sp)
            DOS		_OPEN
            addq.l	#6,sp
            tst		d0
            bmi		open_err
            move.w	d0,(a0)+
            rts
get_pw:
            lea		pw_msg,a1			* password input
            bsr		disp
            move.w	#$39,d3
            move.w	#3,d1
i_pw:
            bsr		i_num
            add.b	#$30,d5
            move.b	d5,(a0)+
            dbra	d1,i_pw
            lea		clrf,a1
            bsr		disp
            rts
get_tm:
            lea		tm_msg,a1			* time input
            bsr		disp
            move.w	#$32,d3
            bsr		i_num
            move.b	#$39,d3
            cmp.b	#2,d5
            beq		tg13
            bra		tg12
tg13:
            move.b	#$33,d3
tg12:
            bsr		tg11
            move.b	d4,hour
            move.w	#$3a,-(sp)
            DOS		_PUTCHAR
            addq.l	#2,sp
            move.b	#$35,d3
            bsr		i_num
            move.b	#$39,d3
            bsr		tg11
            move.b	d4,min
            lea		clrf,a1
            bsr		disp
            rts
tg11:
            mulu	#10,d5
            move.b	d5,d4
            bsr		i_num
            add.b	d5,d4
            rts
i_num:
            DOS		_GETC
            cmp.b	#$30,d0
            blt		i_num
            cmp.b	d3,d0
            bhi		i_num
            move.b	d0,d5
            sub.b	#$30,d5
            move.w	d0,-(sp)
            DOS		_PUTCHAR
            addq.l	#2,sp
            rts
com_w:
            move.l	#2,d0				* write file handle in common area
            bsr		com_rw
            tst		d0
            bmi		exit_ok
            rts
thred_open:						* spawn a thread
            clr.l	-(sp)
            pea		mes_buff
            pea		startp
            move.w	#0,-(sp)
            pea		ssp1
            pea		usp1
            move.w	#$10,-(sp)
            pea		thd_name
            DOS		_OPEN_PR
            lea		28(sp),sp
            tst		d0
            bmi		exit_ok
            rts
keep:
            move.l	#endp,a0			* terminate the thread
            sub.l	#startp,a0
            move.l	a0,-(sp)
            DOS		_KEEPPR
*
exit_ok:
            DOS		_EXIT
open_err:
            lea		or_msg,a1
            bsr		disp
            bra		exit_ok
disp:
            move.l	a1,-(sp)
            DOS		_PRINT
            addq.l	#4,sp
            rts
            .data
clrf:		dc.b	$0d,$0a,0
thd_name:	dc.b	'NEWZ',0
pw_msg:		dc.b	'Please enter password(XXXX;X single-byte number) >',0
tm_msg:		dc.b	'Please enter time(HH:MM; H:hour,M:minute) >',0
start_thd:	dc.b	'Denied access',$0d,$0a,0
or_msg:		dc.b	'File can not be opened.',$0d,$0a,0
		.stack
		ds.l	256
user:
		.end	main			* run from main.
