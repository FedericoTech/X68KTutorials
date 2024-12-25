/*
    This is a replacement for the files.S included in newlib ...
    the naming convention is inconsistent, it should be _dos_files,
    not _dos_filebuf
*/

| int _dos_files (struct _dos_filbuf *, const char *, int);
.text
.even
.global _fix_dos_files
.type _fix_dos_files,@function
_fix_dos_files:
	move.w	%sp@(14), %sp@-
	move.l	%sp@(10), %sp@-
	move.l	%sp@(10), %sp@-
	.short	0xff4e
	lea	%sp@(10), %sp
	rts
