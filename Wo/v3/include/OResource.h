/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef OResource_h
#define OResource_h

#ifndef NO_X_H
#include <X11/Intrinsic.h>
#endif

#ifdef __cplusplus
extern "C"{
#endif
typedef char**(*OResourceGetOptionsFunction)(char*,int*);
typedef char* (*OResourceConvertValueToStringFunction)(Widget,char*,char*,XtArgVal*);
typedef int   (*OResourceConvertStringToChangeFunction)(Widget,char*,char*,char*);
typedef char* (*OResourceGetCallbackDataFunction)(Widget,void*,char*);

void   OResourceClearClass                       ();
void   OResourceAddGetOptionsFunction            (OResourceGetOptionsFunction);
void   OResourceAddConvertValueToStringFunction  (OResourceConvertValueToStringFunction);
void   OResourceAddConvertStringToChangeFunction (OResourceConvertStringToChangeFunction);
void   OResourceAddGetCallbackDataFunction       (OResourceGetCallbackDataFunction);
char** OResourceGetOptions                       (char*,int*);
char*  OResourceConvertValueToString             (Widget,char*,char*,XtArgVal*);
int    OResourceConvertStringToChange            (Widget,char*,char*,char*);
char*  OResourceGetCallbackData                  (Widget,void*,char*);
int    OResourceIsAttachment                     (char*,char*);
int    OResourceIsItemList                       (char*,char*);
int    OResourceIsReferenceList                  (char*,char*);
int    OResourceIsCursor                         (char*,char*);
int    OResourceIsFontList                       (char*,char*);
int    OResourceIsFont                           (char*,char*);
int    OResourceIsFontStruct                     (char*,char*);
int    OResourceIsPixmap                         (char*,char*);
int    OResourceIsMultiLine                      (char*,char*);
#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindOResource(0);  /**/
#endif

#endif  /*OResource_h*/
