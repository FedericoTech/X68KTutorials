/*
    This is a replacement for the nfiles.S included in newlib ...
    the naming convention is inconsistent, it should be _dos_nfiles,
    not _dos_filebuf
*/
| int _dos_nfiles (struct _dos_filbuf *);
.text
.even
.global _dos_nfiles
.type _dos_nfiles,@function
_dos_nfiles:
        move.l  %sp@(4), %sp@-
        .short  0xff4f
        addq.l  #4, %sp
        rts
