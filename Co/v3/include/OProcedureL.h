/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef OProcedureL_h
#define OProcedureL_h
 
#include <OProcedure.h>
typedef OProcedure* OProcedureList;

#ifdef __cplusplus
extern "C"{
#endif
OProcedureList  OProcedureListCreate (int,char**,char*,char*);
void            OProcedureListDelete (OProcedureList);
#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindOProcedureL(0); /**/
#endif

#endif  /*OProcedureL_h*/
