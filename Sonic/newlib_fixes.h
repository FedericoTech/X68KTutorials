#ifndef _NEWLIB_FIXES_H_
#define _NEWLIB_FIXES_H_

extern int _fix_dos_files(struct dos_filbuf *, const char *, int);
extern int _fix_dos_nfiles(struct dos_filbuf *);

#endif /* _NEWLIB_FIXES_H_ */
