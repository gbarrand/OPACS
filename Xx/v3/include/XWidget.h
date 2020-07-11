/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef XWidget_h
#define XWidget_h

#define XMSG_BUILD        1
#define XMSG_TO_MOVE      3
#define XMSG_GO_REFRESH   4
#define XMSG_REFRESH      5
#define XMSG_CALLBACK     6
#define XMSG_MOVE         7

#ifndef NO_X_H
#include <X11/Intrinsic.h>
#endif

#ifndef XtRShapeWindow
#define XtRShapeWindow "ShapeWindow"
#endif

      /*12345678901234567890123456789012*/
#ifdef HAS_SHORT_NAME
#define XWidgetGetAndConvertStringResourceToInteger  XWidgetGetResourceToInteger
#define XWidgetGetAndConvertStringResourceToDouble   XWidgetGetResourceToDouble
#define XWidgetGetAndConvertStringResourceToBoolean  XWidgetGetResourceToBoolean
#define XWidgetGetAndConvertStringResourceToWidget   XWidgetGetResourceToWidget
#endif

#include <XDisplay.h>  
#ifdef __cplusplus
extern "C"{
#endif
typedef int        (*XDisplayGrabFunction)(XEvent*);

typedef void       (*XResourceChangeFunction)(void*);
typedef Boolean    (*XClassIsValidFunction)(WidgetClass);
typedef WidgetClass(*XClassGetIdentifierFunction)(char*);

typedef void       (*XWidgetShellFunction)(Widget,char*);
typedef void       (*XWidgetVisitFunction)(Widget);
typedef char*      (*XWidgetGetLocalTargetFunction)(Widget);
typedef XEvent*    (*XWidgetCallbackEventFunction)(XtPointer);

void         XDisplayUpdate                            (Display*);
XDisplayGrabFunction XDisplaySetGrabDispatchFunction           (XDisplayGrabFunction);
int          XDisplayIsGrabbingPointer                 ();
Window       XDisplayGrabPointer                       (Display*,unsigned int,Cursor,Position*,Position*);
void         XDisplayDoRubberBandGrab                  (Display*,unsigned int,Cursor,Position*,Position*,Position*,Position*);

char**       XWidgetGetResourceOptions                 (char*,int*);
void         XWidgetInitializeClass                    (Display*);
void         XWidgetClearClass                         ();
void         XWidgetSetTop                             (Widget);
Widget       XWidgetGetTop                             ();
Widget*      XWidgetGetIdentifiers                     ();
WidgetList   XWidgetGetLeaves                          (Widget);
Boolean      XWidgetIsValid                            (Widget);
Widget       XWidgetGetIdentifier                      (char*);
XrmQuark     XWidgetGetQuarkifiedName                  (Widget);
void         XWidgetSetName                            (Widget,char*);
int          XWidgetGetNumberOfSubWidgets              (Widget);
Boolean      XWidgetIsInHierarchy                      (Widget,Widget);
Widget       XWidgetFindInHierarchy                    (Widget,XrmQuark);
void         XWidgetVisitHierarchyForward              (Widget,XWidgetVisitFunction);
Cardinal     XWidgetGetRecordSize                      (Widget);
void         XWidgetRealize                            (Widget);
void         XWidgetMapChildren                        (Widget);
char*        XWidgetGetClassName                       (Widget);
Boolean      XWidgetIsConstraint                       (Widget);
int          XWidgetGetResources                       (Widget,char*,XtResourceList);
void         XWidgetGetIthResource                     (Widget,int,XtResourceList);
int          XWidgetGetNumberOfResources               (Widget);
Boolean      XWidgetIsVisible                          (Widget);
void         XWidgetInstallColormap                    (Widget);
void         XWidgetUninstallColormap                  (Widget);
void         XWidgetIconify                            (Widget);
void         XWidgetUniconify                          (Widget);
void         XWidgetGetPosition                        (Widget,Position*,Position*);
void         XWidgetGetDimension                       (Widget,Dimension*,Dimension*);
void         XWidgetSetSensitive                       (Widget,Boolean);
void         XWidgetSetPosition                        (Widget,Position,Position);
void         XWidgetSetDimension                       (Widget,Dimension,Dimension);
void         XWidgetSetPositionAndDimension            (Widget,Position,Position,Dimension,Dimension);
void         XWidgetGetRootPosition                    (Widget,Position*,Position*);
int          XWidgetSendMessage                        (Widget,long,long);

void         XWidgetSetResource                        (Widget,char*,XtArgVal);
void         XWidgetSetPixelResource                   (Widget,char*,Pixel);
void         XWidgetSetWidgetResource                  (Widget,char*,Widget);
void         XWidgetSetIntegerResource                 (Widget,char*,int);

XtArgVal     XWidgetGetResource                          (Widget,char*);
XtPointer    XWidgetGetAddressResource                   (Widget,char*);
Pixel        XWidgetGetPixelResource                     (Widget,char*);
Boolean      XWidgetGetBooleanResource                   (Widget,char*);
int          XWidgetGetIntegerResource                   (Widget,char*);
Widget       XWidgetGetWidgetResource                    (Widget,char*);
int          XWidgetGetAndConvertStringResourceToInteger (Widget,char*);
double       XWidgetGetAndConvertStringResourceToDouble  (Widget,char*);
Boolean      XWidgetGetAndConvertStringResourceToBoolean (Widget,char*);
Widget       XWidgetGetAndConvertStringResourceToWidget  (Widget,char*);
WidgetList   XWidgetGetWidgetListResource                (Widget,char*,int*);
Widget       XWidgetGetFirstEntryOfWidgetListResource    (Widget,char*);

Boolean      XWidgetIsEntryOfWidgetListResource        (Widget,char*,Widget);
void         XWidgetAddEntryInWidgetListResource       (Widget,char*,Widget);
void         XWidgetRemoveEntryFromWidgetListResource  (Widget,char*,Widget);
Cursor       XWidgetConvertStringToCursor              (Widget,char*);
void         XWidgetSetCursor                          (Widget,char*);
WidgetList   XWidgetGetChildren                        (Widget,int*);
WidgetList   XWidgetGetPopupChildren                   (Widget,int*);
WidgetList   XWidgetGetAllChildren                     (Widget,int*);
Widget       XWidgetGetFirstChild                      (Widget);
char*        XWidgetBuildName                          (Widget,char*);
int          XWidgetGetMaximumNameIndex                (Widget,char*);
XWidgetCallbackEventFunction XWidgetSetCallbackEventFunction           (XWidgetCallbackEventFunction);
XWidgetVisitFunction         XWidgetSetMapFunction                     (XWidgetVisitFunction);
void         XWidgetMap                                (Widget);
void         XWidgetMapWindow                          (Widget);
XWidgetVisitFunction         XWidgetSetUnmapFunction                   (XWidgetVisitFunction);
void         XWidgetUnmap                              (Widget);
void         XWidgetUnmapWindow                        (Widget);
void         XWidgetRaise                              (Widget);
void         XWidgetLower                              (Widget);
void         XWidgetPopup                              (Widget);
void         XWidgetPopdown                            (Widget);
void         XWidgetManage                             (Widget);
void         XWidgetUnmanage                           (Widget);
void         XWidgetMove                               (Widget,Position,Position);
void         XWidgetCallCallbacks                      (Widget,char*);
Pixel        XWidgetConvertStringToPixel               (Widget,char*);
Font         XWidgetConvertStringToFont                (Widget,char*);
XFontStruct* XWidgetConvertStringToFontStruct          (Widget,char*);
void         XWidgetShowArming                         (Widget);
void         XWidgetShowDisarming                      (Widget);
Widget       XWidgetGetShell                           (Widget);
void         XWidgetSetWindowManagerCloseFunction       (Widget,XtActionProc);
void         XWidgetPutPixmapPageInPS_Stream           (Widget);
void         XWidgetPutFrameInPS_Stream                (Widget);
void         XWidgetPutPixmapInPS_Stream               (Widget);
void         XWidgetSavePixmapInGIF_File               (Widget,char*);
void         XWidgetSavePixmapInXBM_File               (Widget,char*,char*,unsigned long);
void         XWidgetSwapWindows                        (Widget,Widget,Widget);
void         XWidgetPutPixmapInRootWindow              (Widget);
Pixmap       XWidgetGetPixmap                          (Widget);
Pixmap       XWidgetLoadPixmapFile                     (Widget,char*);
void         XWidgetSetShape                           (Widget,XShape,...);
void         XWidgetExecuteChangeSizeFunction          (Widget);
void         XWidgetExecuteExposeFunction              (Widget);
Widget       XWidgetGetNextBrother                     (Widget);
XWidgetGetLocalTargetFunction  XWidgetSetGetLocalTargetFunction             (XWidgetGetLocalTargetFunction);
XWidgetGetLocalTargetFunction  XWidgetGetGetLocalTargetFunction             ();
Widget       XWidgetGetLocalTarget                     (Widget);
int          XWidgetSetAttribute                       (Widget,char*,void*,void*,int);
int          XWidgetGetAttribute                       (Widget,char*,void*,void*,int*);
int          XWidgetDoMethod                           (Widget,char*,void*,int,char**,void*,int*);
char*        XWidgetGetInformations                    (Widget);

void         XWidgetSetThisWidget                      (Widget);
Widget       XWidgetGetThisWidget                      ();
void         XWidgetSetCallData                        (XtPointer,int);
XtPointer    XWidgetGetCallData                        (int*);
void         XWidgetSetCallValue                       (char*);
char*        XWidgetGetCallValue                       ();

void         XWidgetSetTarget                          (Widget);
Widget       XWidgetGetTarget                          ();
void         XWidgetSetTargetChangedFunction           (XWidgetVisitFunction);
void         XWidgetSetTargetBorderPixel               (Pixel);
void         XWidgetSetTargetBorderWidth               (Dimension);

char**       WidgetListGetNames                        (WidgetList,int);
WidgetList   WidgetListCreateFromString                (char*);
char*        WidgetListConvertToString                 (WidgetList);

void                 XClassSetIsValidFunction                  (XClassIsValidFunction);
void                 XClassSetGetIdentifierFunction            (XClassGetIdentifierFunction);
WidgetClass          XClassGetIdentifier                       (char*);
Boolean              XClassIsValid                             (WidgetClass);
char*                XClassGetName                             (WidgetClass);
void                 XClassIncrementWidgetRecordSize           (WidgetClass,Cardinal);
Boolean              XClassIsSubclass                          (WidgetClass,WidgetClass);
XWidgetVisitFunction XClassGetDestroyFunction                  (WidgetClass);

void         XResourceChangesInitialize                ();
void         XResourceChangesCommit                    ();
void         XResourceChangesAddEntry                  (char*,XtArgVal,XResourceChangeFunction);
void         XResourceChangesFlush                     ();
int          XResourceChangesSetWidget                 (Widget);
void         XtConvertStringToResourceChange           (Widget,char*,char*,char*);

int          XWidgetSpawnShellScript                   (Widget,char*,XWidgetShellFunction);
void         XWidgetSetGrabButtonAndCursor             (unsigned int,char*);
int          XWidgetGrabRootWindowPart                 (Widget);
Widget       XWidgetGrab                               (Widget);
#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindXWidget(0); /**/
#endif

#ifdef __CI_SPECIFIC__
#define XtMapWidget   XWidgetMap
#define XtUnmapWidget XWidgetUnmap

#define thisLocalTargetWidget (XWidgetGetLocalTarget(XWidgetGetThisWidget()))
#define thisWidget            (XWidgetGetThisWidget())
#define targetWidget          (XWidgetGetTarget())

#define callData              (XWidgetGetCallData(0))
#define thisXEvent            ((XEvent*)XWidgetGetCallData(0))
#endif


#endif  /*XWidget_h*/
