/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef momo_h
#define momo_h
 
typedef void(*mo_notify_function)(char*);

#ifdef __cplusplus
extern "C"{
#endif
void  mo_set_notify_proc (mo_notify_function);
char* mo_tmpnam          (void);

/*To build correclty shared libs on AIX.*/
extern int    binary_transfer;    
extern char*  use_this_url_instead;  
extern int    force_dump_to_file;
extern char*  force_dump_filename;
extern int    loading_inlined_images;
#ifdef __cplusplus
}
#endif

#endif  /*momo_h*/
