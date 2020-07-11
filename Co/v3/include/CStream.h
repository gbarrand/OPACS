/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef CStream_h
#define CStream_h

#ifndef NO_C_H
#include <stdio.h>   
#endif

#ifdef __cplusplus
extern "C"{
#endif
void            CStreamInitializeStatus      ();
int             CStreamGetStatus             ();
void            CStreamPrintF                (FILE*,char*,...);
void            CStreamDump                  (FILE*);
long            CStreamGetSize               (FILE*);
void            CStreamAppend                (FILE*,FILE*);
void            CStreamInitializeBuffer      (FILE*);
char*           CStreamGetString             (FILE*);
char*           CStreamRead                  (FILE*,unsigned long*);
char*           CStreamReadOnceThrough       (FILE*,unsigned long*);
char*           CStreamGetStrings            (FILE*,char,int*,char***);
char**          CStreamGetText               (FILE*,int*);
void            CStreamPutText               (FILE*,int,char**);
#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindCStream(0); /**/
#endif

#endif  /*CStream_h*/



