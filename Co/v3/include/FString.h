/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef FString_h
#define FString_h
 
typedef void* FString;

#ifdef __cplusplus
extern "C"{
#endif
FString  FStringCreate          (int);
FString  FStringCreateFromC     (char*,int*);
int      FStringInitializeFromC (FString,int,char*);
char*    FStringConvertToC      (FString,int);
void     FStringDelete          (FString);
#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindFString(0); /**/
#endif

#endif  /*FString_h*/
