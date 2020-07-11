/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef XxCommands_h
#define XxCommands_h

#include <XWidget.h>
#include <OShell.h>
#ifdef __cplusplus
extern "C"{
#endif
void XxAddCommands           (OShell);
void XxSetTreatInputFunction (XWidgetShellFunction);

int XxExecute_thisWidget              (int,char**,OProcess);
int XxExecute_thisLocalTargetWidget   (int,char**,OProcess);
int XxExecute_targetWidget            (int,char**,OProcess);
int XxExecute_thisWidgetParent        (int,char**,OProcess);

int XxExecute_getWidgetParent         (int,char**,OProcess);
int XxExecute_raiseWidget             (int,char**,OProcess);
int XxExecute_lowerWidget             (int,char**,OProcess);
int XxExecute_mapWidget               (int,char**,OProcess);
int XxExecute_unmapWidget             (int,char**,OProcess);
int XxExecute_manageWidget            (int,char**,OProcess);
int XxExecute_unmanageWidget          (int,char**,OProcess);
int XxExecute_iconifyWidget           (int,char**,OProcess);
int XxExecute_uniconifyWidget         (int,char**,OProcess);
int XxExecute_setWidgetPosition       (int,char**,OProcess);
int XxExecute_setWidgetDimension      (int,char**,OProcess);
int XxExecute_saveWidgetPixmap        (int,char**,OProcess);
int XxExecute_callWidgetCallbacks     (int,char**,OProcess);
int XxExecute_isWidgetSubclass        (int,char**,OProcess);
int XxExecute_isNotWidgetSubclass     (int,char**,OProcess);

int XxExecute_saveRootWindowPixmap    (int,char**,OProcess);
int XxExecute_putRootWindowPixmapInPS (int,char**,OProcess);
int XxExecute_grabWidget              (int,char**,OProcess);
int XxExecute_grabWidgetName          (int,char**,OProcess);
int XxExecute_captureAndSavePixmap    (int,char**,OProcess);
int XxExecute_getDisplayFonts         (int,char**,OProcess);
int XxExecute_getDisplayColors        (int,char**,OProcess);
int XxExecute_createMailbox           (int,char**,OProcess);
int XxExecute_sendMessageToMailbox    (int,char**,OProcess);
int XxExecute_sendFileToDisplayBuffer (int,char**,OProcess);
int XxExecute_saveDisplayBuffer       (int,char**,OProcess);
int XxExecute_requireDisplayBuffer    (int,char**,OProcess);
int XxExecute_ringBell                (int,char**,OProcess);
int XxExecute_spawnShellScript        (int,char**,OProcess);
#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindXxCommands(0); /**/
#endif

#endif  /*XxCommands_h*/



