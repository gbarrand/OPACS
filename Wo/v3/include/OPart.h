/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef OPart_h
#define OPart_h

typedef struct _OPartRecord  *OPart;

#ifdef __cplusplus
extern "C"{
#endif
void  OPartClearClass              ();
OPart OPartCreate                  (void*);
OPart OPartGetIdentifier           (char*);
OPart OPartGetIdentifierFromWidget (void*);
void  OPartInitialize              (OPart,void*,void*,char*,int,int);
void* OPartGetWidget               (OPart);
void  OPartMarkIthResource         (OPart,int,int);
void  OPartDelete                  (OPart);
#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindOPart(0);  /**/
#endif

#endif  /*OPart_h*/
