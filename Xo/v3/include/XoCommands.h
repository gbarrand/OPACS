/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef XoCommands_h
#define XoCommands_h

#include <OShell.h>
#ifdef __cplusplus
extern "C"{
#endif
void XoAddCommands               (OShell);

int XoExecute_targetCamera       (int,char**,OProcess);
int XoExecute_thisCamera         (int,char**,OProcess);
int XoExecute_getWidgetCamera    (int,char**,OProcess);
int XoExecute_getWidgetPlotter   (int,char**,OProcess);

int XoExecute_plotCurve          (int,char**,OProcess);
int XoExecute_magnifyPlot        (int,char**,OProcess);
int XoExecute_setPlotLayout      (int,char**,OProcess);
int XoExecute_assignToPlotters   (int,char**,OProcess);
int XoExecute_attachToPlotters   (int,char**,OProcess);
int XoExecute_showTopic          (int,char**,OProcess);
int XoExecute_createRotatedImage (int,char**,OProcess);
int XoExecute_saveImage          (int,char**,OProcess);
int XoExecute_updatePlot         (int,char**,OProcess);
int XoExecute_saveContent        (int,char**,OProcess);

int XoExecute_cancelGoRefresh    (int,char**,OProcess);
int XoExecute_getWWW             (int,char**,OProcess);
#ifdef __cplusplus
}
#endif

#endif  /*XoCommands_h*/






