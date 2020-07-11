/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef OWidget_h
#define OWidget_h

#ifndef NO_X_H
#include <X11/Intrinsic.h>
#endif

typedef int(*OWidgetDragAndDropFunction)(Widget,XtPointer);

#include <OTrace.h>
#ifdef __cplusplus
extern "C"{
#endif
void             OWidgetSetTrace                       (OTrace);
XtCallbackProc   OWidgetSetDestroyFunction              (XtCallbackProc);
Widget           OWidgetGetFirstShell                  ();
char**           OWidgetGetShells                      (int*);
int              OWidgetHasMappedShell                 ();
void             OWidgetInitializeClass                ();
void             OWidgetClearClass                     ();
void             OWidgetSetHelp                        (Boolean);
Boolean          OWidgetGetHelp                        ();
void             OWidgetSetDragAndDrop                 (char*,XtActionProc,OWidgetDragAndDropFunction,OWidgetDragAndDropFunction);
void             OWidgetSetForcedBackgroundExecution   (Boolean);
char*            OWidgetGetThisValue                   ();
Widget           OWidgetCreate                         (char*,Widget,char*,Boolean);
Widget           OWidgetBuildToolkitPart               (char*,Widget,char*,void*,int,int);
Widget           OWidgetGetIdentifier                  (char*);
void*            OWidgetConstruct                      (void*,int,char**,char**);
void             OWidgetDoRubberBandGrab               (Widget,int*,int*,int*,int*);
void             OWidgetSetName                        (Widget,char*);
int              OWidgetIsShell                        (Widget);
void             OWidgetAddCallback                    (Widget,char*,char*);
void             OWidgetCallCallbacks                  (Widget,char*);
char*            OWidgetFindCurrentCallbackName        (Widget);
void             OWidgetAddEventHandler                (Widget,EventMask,char*);
void             OWidgetRemoveEventHandler             (Widget,EventMask);
char*            OWidgetGetAndConvertResourceToString  (Widget,char*);
int              OWidgetGetAndConvertResourceToInteger (Widget,char*);
double           OWidgetGetAndConvertResourceToDouble  (Widget,char*);
Boolean          OWidgetGetAndConvertResourceToBoolean (Widget,char*);
Widget           OWidgetGetAndConvertResourceToWidget  (Widget,char*);
void             OWidgetAssignResource                 (Widget,char*,char*,char*);
void             OWidgetMarkResource                   (Widget,char*,int);
int              OWidgetHasResource                    (Widget,char*);
void             OWidgetMarkResourceToBeSaved          (Widget,char*);
char**           OWidgetGetMarkedResources             (Widget,int*,int);
void             OWidgetBeginResourceChanges           ();
void             OWidgetCommitResourceChanges          ();
void             OWidgetSetResourceFromString          (Widget,char*,char*,Boolean);
void             OWidgetSetResourceFromInteger         (Widget,char*,int,Boolean);
void             OWidgetSetResourceFromBoolean         (Widget,char*,Boolean,Boolean);
void             OWidgetSetResourceFromDouble          (Widget,char*,double,Boolean);
void             OWidgetSetResourceFromWidget          (Widget,char*,Widget,Boolean);
void             OWidgetRequireResourceScriptExecution (Widget,char*);
void             OWidgetRequireScriptExecution         (Widget,char*,XtPointer,int);
void             OWidgetSendScript                     (Widget,char*,int);
void             OWidgetVerifySyntaxOfCallbacks        (Widget);
char*            OWidgetGetInformations                (Widget);
Widget           OWidgetGetParent                      (Widget);
void*            OWidgetGetFile                        (Widget);
void             OWidgetSetPrivatePart                 (Widget,Widget,void*,char*,Boolean,Boolean);
void             OWidgetSetVolatile                    (Widget,int);
int              OWidgetIsVolatile                     (Widget);
void             OWidgetSetVolatileChild               (Widget,int);
int              OWidgetIsVolatileChild                (Widget);
char*            OWidgetGetType                        (Widget);
void             OWidgetDelete                         (Widget,Boolean);
void             OWidgetKillChildren                   (Widget,Boolean);
void*            OWidgetBuildPrivatePart               (Widget);
void             OWidgetSetFile                        (Widget,void*);
Widget           OWidgetGetShell                       (Widget);
char**           OWidgetGetPossibleChildren            (Widget,int*);
Widget           OWidgetDuplicate                      (Widget,char*,Widget,Boolean);
Widget           OWidgetDuplicateHierarchy             (Widget,char*,Widget,Boolean);
void             OWidgetMove                           (Widget,Position,Position,Boolean);
void             OWidgetSaveHierarchy                  (Widget,char*);
void             OWidgetSetWindowManagerCloseAction    (Widget);
void             OWidgetPutPageInPS_Stream             (Widget);
char**           OWidgetGetResources                   (Widget,char*,int*);
char*            OWidgetGetLocalTarget                 (Widget);
int              OWidgetSetAttribute                   (Widget,char*,void*,void*,int);
int              OWidgetGetAttribute                   (Widget,char*,void*,void*,int*);
int              OWidgetDoMethod                       (Widget,char*,void*,int,char**,void*,int*);
#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindOWidget(0);  /**/
#endif

#ifdef __CI_SPECIFIC__
#define thisValue (OWidgetGetThisValue())
#endif


#endif  /*OWidget_h*/
