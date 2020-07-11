/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef COption_h
#define COption_h

typedef void(*COptionDoFunction)(int);

#ifdef __cplusplus
extern "C"{
#endif
void  COptionSetSilent  (int);
void  COptionInitialize (COptionDoFunction);
void  COptionAddEntry   (char*,int);
void  COptionVisit      (int,char**);
char* COptionGoForth    ();
void  COptionClearClass ();
#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindCOption(0); /**/
#endif

#endif  /*COption_h*/
