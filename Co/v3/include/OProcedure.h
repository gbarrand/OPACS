/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef OProcedure_h
#define OProcedure_h
 
typedef struct _OProcedureRecord *OProcedure;

#ifdef __cplusplus
extern "C"{
#endif
OProcedure  OProcedureCreate           (char*);
void        OProcedureDelete           (OProcedure);
char*       OProcedureGetName          (OProcedure);
void        OProcedureSetBeginPosition (OProcedure,int);
int         OProcedureGetBeginPosition (OProcedure);
void        OProcedureSetEndPosition   (OProcedure,int);
int         OProcedureGetEndPosition   (OProcedure);
void        OProcedureSetTitlePosition (OProcedure,int);
int         OProcedureGetTitlePosition (OProcedure);
void        OProcedureSetUserData      (OProcedure,void*);
void*       OProcedureGetUserData      (OProcedure);
#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindOProcedure(0); /**/
#endif

#endif  /*OProcedure_h*/
