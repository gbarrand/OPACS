/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef OHandle_h
#define OHandle_h

typedef struct _OHandleRecord*  OHandle;

#ifndef NO_C_H
#include <stdarg.h>
#endif
#ifdef __cplusplus
extern "C"{
#endif
OHandle  OHandleCreate        (void*,void*);
void     OHandleDelete        (OHandle);
void*    OHandleGetType       (OHandle);
void*    OHandleGetIdentifier (OHandle);
void     OHandleInitialize    (OHandle,void*,void*);
void     OHandleClear         (OHandle);
#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindOHandle(0); /**/
#endif

#endif  /*OHandle_h*/

