/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef WoCommands_h
#define WoCommands_h

#include <OShell.h>
#ifdef __cplusplus
extern "C"{
#endif
void WoAddCommands (OShell);

int WoExecute_dot_osh                  (int,char**,OProcess);
int WoExecute_postScript               (int,char**,OProcess);
int WoExecute_realize                  (int,char**,OProcess);
int WoExecute_thisValue                (int,char**,OProcess);
int WoExecute_setWidgetResource        (int,char**,OProcess);
int WoExecute_setAndMarkWidgetResource (int,char**,OProcess);
int WoExecute_getWidgetResource        (int,char**,OProcess);
int WoExecute_findWidget               (int,char**,OProcess);
#ifdef __cplusplus
}
#endif

#endif  /*WoCommands_h*/



