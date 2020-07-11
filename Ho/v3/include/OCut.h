/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef OCut_h
#define OCut_h
 
typedef struct _OCutRecord     *OCut;

#ifdef __cplusplus
extern "C"{
#endif
void     OCutClearClass     ();
OCut*    OCutGetIdentifiers ();
OCut     OCutGetIdentifier  (char*);
double*  OCutGetCurrentRow  (int*);
OCut     OCutMake           ();
OCut     OCutCreate         (char*);
int      OCutIsValid        (OCut);
void     OCutDelete         (OCut);
void     OCutSetName        (OCut,char*);
char*    OCutGetName        (OCut);
void     OCutSetScript      (OCut,char*);
char*    OCutGetScript      (OCut);
void     OCutSetColumns     (OCut,int,char**);
int      OCutIsRowAccepted  (OCut,int,double*);
int      OCutSetAttribute   (OCut,char*,void*,void*,int);
int      OCutGetAttribute   (OCut,char*,void*,void*,int*);
#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindOCut(0); /**/
#endif

#endif  /*OCut_h*/
