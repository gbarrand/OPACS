/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef CiCommands_h
#define CiCommands_h

#include <OShell.h>
#ifdef __cplusplus
extern "C"{
#endif
void CiAddCommands (OShell);

int CiExecute_doCiScript    (int,char**,OProcess);
int CiExecute_doCiFile      (int,char**,OProcess);
int CiExecute_parseCiScript (int,char**,OProcess);
#ifdef __cplusplus
}
#endif

#endif  /*CiCommands_h*/



