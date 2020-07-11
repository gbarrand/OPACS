/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef CString_h
#define CString_h

#ifndef NO_C_H
#include <stdarg.h>
#endif

#define CStringNULL ((char*)NULL)

#ifdef __cplusplus
extern "C"{
#endif
void    CStringClearClass               ();
char*   CStringDuplicate                (char*);
void    CStringDelete                   (char*);
char*   CStringCreate                   (int);
int     CStringCompare                  (char*,char*);
int     CStringHasOnlySpaces            (char*);
int     CStringIsWord                   (char*);
void    CStringTrim                     (char*);
char*   CStringGetLastNotSpacePosition  (char*);
char*   CStringGetFirstNotSpacePosition (char*);
int     CStringRaise                    (char*);
int     CStringLower                    (char*);
int     CStringHasUpperCase             (char*);
int     CStringRemoveDelimiters         (char*);
char*   CStringCreateF                  (int,char*,...);
char*   CStringCreateV                  (int,char*,va_list);
char*   CStringCreateFormatted          (char*,...);
int     CStringPrintF                   (char*,char*,...);
char*   CStringGetFirstWord             (char*);
char*   CStringGetSecondWord            (char*);
char*   CStringGetThirdWord             (char*);
int     CStringMatchPattern             (char*,char*);
int     CStringIsPrintable              (char*,char*,int*);
void    CStringInitialize               (char*,char);
int     CStringBeginWith                (char*,char);
int     CStringCountLines               (char*);
long    CStringConvertToLong            (char*,int*);
double  CStringConvertToDouble          (char*,int*);
int     CStringIsLong                   (char*,long*);
int     CStringConvertToBoolean         (char*,int*);
double* CStringConvertToDoubles         (char*,int*);
char**  CStringGetWords                 (char*,char*,int*);
int     CStringReplaceWord              (char**,char*,char*);
void    CStringConcatenate              (char**,char*);
int     CStringReplacePart              (char**,char*,char*);
char*   CStringGetFirstWordPosition     (char*,char*);
int     CStringHasWord                  (char*,char*);
double  CStringGetHersheyLength         (char*,int,double);
#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindCString(0); /**/
#endif

#endif  /*CString_h*/
