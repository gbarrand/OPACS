/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef XmCommands_h
#define XmCommands_h

#include <OShell.h>
#ifdef __cplusplus
extern "C"{
#endif
void XmAddCommands (OShell);

int XmExecute_buildWidgetGrid       (int,char**,OProcess);
int XmExecute_setWidgetOptions      (int,char**,OProcess);
int XmExecute_setWidgetOption       (int,char**,OProcess);
int XmExecute_setWidgetOptionScript (int,char**,OProcess);
int XmExecute_getWidgetOption       (int,char**,OProcess);
int XmExecute_popWidgetAside        (int,char**,OProcess);
int XmExecute_setWidgetTextSize     (int,char**,OProcess);
int XmExecute_popWidgetBalloon      (int,char**,OProcess);
int XmExecute_attachWidget          (int,char**,OProcess);
int XmExecute_createCompoundWidget  (int,char**,OProcess);
int XmExecute_editWidgetResource    (int,char**,OProcess);

int XmExecute_appendText            (int,char**,OProcess);
int XmExecute_warn                  (int,char**,OProcess);
int XmExecute_ask                   (int,char**,OProcess);
int XmExecute_askPassword           (int,char**,OProcess);
int XmExecute_askText               (int,char**,OProcess);
int XmExecute_askList               (int,char**,OProcess);
int XmExecute_askFile               (int,char**,OProcess);

int XmExecute_buildZones            (int,char**,OProcess);
int XmExecute_createGridFile        (int,char**,OProcess);
#ifdef __cplusplus
}
#endif

#endif  /*XmCommands_h*/



