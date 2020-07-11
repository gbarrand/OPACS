/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef XmTools_h
#define XmTools_h

/*HAS_XM*/

typedef enum {
  XmBalloonShapeRectangular,
  XmBalloonShapeOval
} XmBalloonShape;

#ifndef NO_X_H
#include <X11/Intrinsic.h>
#endif

#ifndef NO_X_H
#include <Xm/Xm.h>             /*no_cigen*/
#endif

#ifndef NO_C_H
#include <stdarg.h>
#include <stdio.h>
#endif

#ifdef __cplusplus
extern "C"{
#endif
typedef void  (*XmMarkFunction)(Widget,char*);
typedef Widget(*XmGetWidgetFunction)();
typedef int   (*XmProcessEventsFunction)();
typedef void  (*XmVisitWidgetFunction)(Widget);
typedef Widget(*XmCreateWidgetFunction)(Widget,char*,ArgList,Cardinal);

void       XmInitializeClass                     (Display*);
void       XmClearClass                          ();
char*      XmConvertResourceValueToString        (Widget,char*,char*,XtArgVal*);
int        XmConvertStringToResourceChange       (Widget,char*,char*,char*);
char**     XmGetResourceOptions                  (char*,int*);
char*      XmGetCallbackData                     (Widget,void*,char*);
XEvent*    XmGetCallbackEvent                    (XtPointer);

XmFontList XmConvertStringToFontList             (char*,Display*);
char*      XmConvertFontListToString             (XmFontList);
char*      XmConvertCompoundStringToString       (XmString,int);

void       XmRaiseWidget                         (Widget);
void       XmLowerWidget                         (Widget);
void       XmMapAndRaiseWidget                   (Widget);
void       XmMapWidget                           (Widget);
void       XmUnmapWidget                         (Widget);
void       XmSetCompoundStringResourceFromString (Widget,char*,char*);
void       XmSetSideAttachment                   (Widget,char*,char*,int,void*,XmMarkFunction);
void       XmGlueWidget                          (Widget,XmMarkFunction);
void       XmAttachWidgetAtPositionAndMark       (Widget,int,int,int,int,XmMarkFunction);
void       XmSetBalloonShape                     (XmBalloonShape);
void       XmPopBalloon                          (Widget,char*);

void       XmOptionInitialize                    (Widget,int,char**,char*);
void       XmOptionSetOption                     (Widget,char*);
char*      XmOptionGetOption                     (Widget);
void       XmOptionVisit                         (Widget,XmVisitWidgetFunction);

void       XmLabelSetLabelString                 (Widget,char*);

void       XmListSetItemsFromFile                (Widget,char*);
void       XmListSetItemsFromStream              (Widget,FILE*);
void       XmListSetItemsFromText                (Widget,int,char**);
void       XmListSetSelectedItem                 (Widget,char*);
char*      XmListGetSelectedItem                 (Widget);
void       XmListAppendString                    (Widget,char*);

void       XmFormBuildGrid                       (Widget,char*,XmCreateWidgetFunction,int,int,Boolean);
void       XmFormGetNumberOfRowsAndColumns       (Widget,int*,int*);
void       XmFormAlignChildren                   (Widget,int,int);
void       XmAttachWidgetAtPosition              (Widget,int,int,int,int);

void       XmMenuPopAside                        (Widget,Widget,XEvent*);

void       XmLabelSetShapeAccordingToPixmap      (Widget);
void       XmTextSetNumberOfRowsAndColumns       (Widget,int,int);
Widget     XmTextConvertValueToWidget            (Widget);
double     XmTextConvertValueToDouble            (Widget);
int        XmTextConvertValueToInteger           (Widget);
int        XmTextConvertValueToBoolean           (Widget);
char*      XmTextGetValue                        (Widget);
void       XmTextSetValueFromWidgetName          (Widget,Widget);
void       XmTextSetValueF                       (Widget,int,char*,...);
void       XmTextAppendValueV                    (Widget,int,char*,va_list);
void       XmTextAppendValueF                    (Widget,int,char*,...);
void       XmTextSetValueFromFile                (Widget,char*);
void       XmTextSetValueFromStream              (Widget,FILE*);
void       XmTextPutPixmapInPS_Stream            (Widget);
void       XmTextAppendString                    (Widget,char*);

int        XmSelectionBoxConvertTextToInteger    (Widget);
double     XmSelectionBoxConvertTextToDouble     (Widget);
void       XmSelectionBoxInitialize              (Widget,int,XmStringTable);
void       XmSelectionBoxSetText                 (Widget,int,char**);

char*      XmStringTableConvertToString          (XmStringTable,int);
XmString*  XmStringTableCreateFromString         (char*);
XmString*  XmStringTableCreateFromText           (int,char**);
int        XmStringTableGetSize                  (XmString*);
void       XmStringTableDelete                   (XmString*);

Widget     XmCreateShellText                     (Widget,char*,ArgList,Cardinal);

XmGetWidgetFunction XmDialogSetGetFirstShellFunction       (XmGetWidgetFunction);
void       XmDialogSetLocalLoopFunction          (XmProcessEventsFunction);
char*      XmDialogAskForString                  (char*,char*);
char*      XmDialogAskForBufferedPassword        (char*);
char*      XmDialogAskForPassword                (char*);
int        XmDialogAskForInteger                 (char*,int);
double     XmDialogAskForDouble                  (char*,double);
Boolean    XmDialogAskForBoolean                 (char*);
char*      XmDialogAskForList                    (char*,int,char**,Boolean);
char*      XmDialogAskForText                    (char*,char*,int,int);
char*      XmDialogAskForFile                    (char*,char*);
void       XmDialogWarn                          (char*);

void       XmConsoleSetExitFunction              (XmVisitWidgetFunction);
void       XmConsolePrintF                       (char*,...);
void       XmConsolePrintV                       (char*,va_list);
void       XmConsolePrint                        (char*);
void       XmConsolePutF                         (int,char*,...);
void       XmConsolePutV                         (int,char*,va_list);
void       XmConsoleDestroy                      ();
Widget     XmConsoleGetTextWidget                ();
#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindXmTools(0);
#endif

#endif  /*XmTools_h*/
