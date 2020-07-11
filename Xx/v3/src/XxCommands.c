/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <X11/Intrinsic.h>

#include <CString.h>
#include <CPrinter.h>
#include <CList.h>
#include <CText.h>
#include <CPS.h>
#include <CFile.h>
#include <OType.h>
#include <OColor.h>

#include <XMailbox.h>
#include <XSelection.h>
#include <XWidget.h>
#include <XxTypes.h>

#include <XxCommands.h>

#ifdef __cplusplus
extern "C"{
#endif
static void TreatInput  (Widget,char*);
#ifdef __cplusplus
}
#endif

#define MINIMUM(a,b)  ((a)<(b)?a:b)
#define MAXIMUM(a,b)  ((a)>(b)?a:b)

static struct 
{
  Position             grabX,grabY;
  Dimension            grabW,grabH;
  XWidgetShellFunction treatInputFunction;
} Class = {0,0,0,0,NULL};
/***************************************************************************/
void XxSetTreatInputFunction (
 XWidgetShellFunction a_routine
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  Class.treatInputFunction = a_routine;
}
/***************************************************************************/
void XxAddCommands (
 OShell a_osh
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(OCommandGetIdentifier("thisWidget")!=NULL) return; /*done*/

  XxSetTypes ();

  OShellAddNewCommand (a_osh,"Xx/thisWidget"              ,XxExecute_thisWidget);
  OShellAddNewCommand (a_osh,"Xx/thisLocalTargetWidget"   ,XxExecute_thisLocalTargetWidget);
  OShellAddNewCommand (a_osh,"Xx/targetWidget"            ,XxExecute_targetWidget);
  OShellAddNewCommand (a_osh,"Xx/thisWidgetParent"        ,XxExecute_thisWidgetParent);

  OShellAddNewCommand (a_osh,"Xx/getWidgetParent"         ,XxExecute_getWidgetParent);
  OShellAddNewCommand (a_osh,"Xx/raiseWidget"             ,XxExecute_raiseWidget);
  OShellAddNewCommand (a_osh,"Xx/lowerWidget"             ,XxExecute_lowerWidget);
  OShellAddNewCommand (a_osh,"Xx/mapWidget"               ,XxExecute_mapWidget);
  OShellAddNewCommand (a_osh,"Xx/unmapWidget"             ,XxExecute_unmapWidget);
  OShellAddNewCommand (a_osh,"Xx/manageWidget"            ,XxExecute_manageWidget);
  OShellAddNewCommand (a_osh,"Xx/unmanageWidget"          ,XxExecute_unmanageWidget);
  OShellAddNewCommand (a_osh,"Xx/iconifyWidget"           ,XxExecute_iconifyWidget);
  OShellAddNewCommand (a_osh,"Xx/uniconifyWidget"         ,XxExecute_uniconifyWidget);
  OShellAddNewCommand (a_osh,"Xx/setWidgetPosition"       ,XxExecute_setWidgetPosition);
  OShellAddNewCommand (a_osh,"Xx/setWidgetDimension"      ,XxExecute_setWidgetDimension);
  OShellAddNewCommand (a_osh,"Xx/callWidgetCallbacks"     ,XxExecute_callWidgetCallbacks);
  OShellAddNewCommand (a_osh,"Xx/saveWidgetPixmap"        ,XxExecute_saveWidgetPixmap);
  OShellAddNewCommand (a_osh,"Xx/isWidgetSubclass"        ,XxExecute_isWidgetSubclass);
  OShellAddNewCommand (a_osh,"Xx/isNotWidgetSubclass"     ,XxExecute_isNotWidgetSubclass);

  OShellAddNewCommand (a_osh,"Xx/saveRootWindowPixmap"    ,XxExecute_saveRootWindowPixmap);
  OShellAddNewCommand (a_osh,"Xx/putRootWindowPixmapInPS" ,XxExecute_putRootWindowPixmapInPS);
  OShellAddNewCommand (a_osh,"Xx/grabWidget"              ,XxExecute_grabWidget);
  OShellAddNewCommand (a_osh,"Xx/grabWidgetName"          ,XxExecute_grabWidgetName);
  OShellAddNewCommand (a_osh,"Xx/captureAndSavePixmap"    ,XxExecute_captureAndSavePixmap);
  OShellAddNewCommand (a_osh,"Xx/getDisplayFonts"         ,XxExecute_getDisplayFonts);
  OShellAddNewCommand (a_osh,"Xx/getDisplayColors"        ,XxExecute_getDisplayColors);
  OShellAddNewCommand (a_osh,"Xx/createMailbox"           ,XxExecute_createMailbox);
  OShellAddNewCommand (a_osh,"Xx/sendMessageToMailbox"    ,XxExecute_sendMessageToMailbox);
  OShellAddNewCommand (a_osh,"Xx/sendFileToDisplayBuffer" ,XxExecute_sendFileToDisplayBuffer);
  OShellAddNewCommand (a_osh,"Xx/saveDisplayBuffer"       ,XxExecute_saveDisplayBuffer);
  OShellAddNewCommand (a_osh,"Xx/requireDisplayBuffer"    ,XxExecute_requireDisplayBuffer);
  OShellAddNewCommand (a_osh,"Xx/ringBell"                ,XxExecute_ringBell);

  OShellAddNewCommand (a_osh,"Xx/spawnShellScript"        ,XxExecute_spawnShellScript);

}
/***************************************************************************/
int XxExecute_thisWidget (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> thisWidget
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget widget;
/*.........................................................................*/
  if(a_argn!=1) 
    {
      CWarnF("Zero argument expected in :\n%s\n",OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return EXIT_FAILURE;
    }

  widget  = XWidgetGetThisWidget();
  if(widget==NULL)
    {
      OProcessPutHandles (a_process,NULL);
      return       EXIT_FAILURE;
    }

  OProcessPutString (a_process,CStringCreateF(128,"Widget/%lu",widget));
  
  a_args            = NULL;
  return            EXIT_SUCCESS;
}
/***************************************************************************/
int XxExecute_thisWidgetParent (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> thisWidgetParent
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget widget,parent;
/*.........................................................................*/
  if(a_argn!=1) 
    {
      CWarnF("Zero argument expected in :\n%s\n",OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return EXIT_FAILURE;
    }

  widget  = XWidgetGetThisWidget();
  if(widget==NULL)
    {
      OProcessPutHandles (a_process,NULL);
      return       EXIT_FAILURE;
    }

  parent = XtParent(widget);
  if(parent==NULL)
    {
      OProcessPutHandles (a_process,NULL);
      return       EXIT_FAILURE;
    }

  OProcessPutString (a_process,CStringCreateF(128,"Widget/%lu",parent));
  
  a_args            = NULL;
  return            EXIT_SUCCESS;
}
/***************************************************************************/
int XxExecute_targetWidget (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> targetWidget
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget widget;
/*.........................................................................*/
  if(a_argn!=1) 
    {
      CWarnF("Zero argument expected in :\n%s\n",OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return EXIT_FAILURE;
    }

  widget = XWidgetGetTarget();
  if(widget==NULL)
    {
      OProcessPutHandles (a_process,NULL);
      return       EXIT_FAILURE;
    }

  OProcessPutString (a_process,CStringCreateF(128,"Widget/%lu",widget));

  a_args            = NULL;
  return            EXIT_SUCCESS;
}
/***************************************************************************/
int XxExecute_thisLocalTargetWidget (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> thisLocalTargetWidget
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget      widget;
/*.........................................................................*/
  if(a_argn!=1) 
    {
      CWarnF("Zero argument expected in :\n%s\n",OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return EXIT_FAILURE;
    }

  widget        = XWidgetGetLocalTarget (XWidgetGetThisWidget());
  if(widget==NULL)
    {
      OProcessPutHandles (a_process,NULL);
      return       EXIT_FAILURE;
    }

  OProcessPutString (a_process,CStringCreateF(128,"Widget/%lu",widget));

  a_args            = NULL;
  return            EXIT_SUCCESS;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
int XxExecute_getWidgetParent (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> getWidgetParent <string:Widget>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget widget,parent;
/*.........................................................................*/
  if(a_argn!=2) 
    {
      CWarnF("One argument expected in :\n%s\n",OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return EXIT_FAILURE;
    }

  widget      = XWidgetGetIdentifier (a_args[1]);
  if(widget==NULL)
    {
      CWarnF ("Unknown widget %s in :\n%s\n",a_args[1],OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return EXIT_FAILURE;
    }

  parent = XtParent(widget);
  if(parent==NULL)
    {
      OProcessPutHandles (a_process,NULL);
      return       EXIT_FAILURE;
    }

  OProcessPutString   (a_process,CStringCreateF(128,"Widget/%lu",parent));

  return EXIT_SUCCESS;
}
/***************************************************************************/
int XxExecute_raiseWidget (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> raiseWidget <string:Widget>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget widget;
/*.........................................................................*/
  if(a_argn!=2) 
    {
      CWarnF("One argument expected in :\n%s\n",OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return EXIT_FAILURE;
    }

  widget      = XWidgetGetIdentifier (a_args[1]);
  if(widget==NULL)
    {
      CWarnF ("Unknown widget %s in :\n%s\n",a_args[1],OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return EXIT_FAILURE;
    }

  XWidgetRaise        (widget);
  OProcessPutHandles (a_process,NULL);

  return EXIT_SUCCESS;
}
/***************************************************************************/
int XxExecute_lowerWidget (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> lowerWidget <string:Widget>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget widget;
/*.........................................................................*/
  if(a_argn!=2) 
    {
      CWarnF("One argument expected in :\n%s\n",OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return EXIT_FAILURE;
    }

  widget      = XWidgetGetIdentifier (a_args[1]);
  if(widget==NULL)
    {
      CWarnF ("Unknown widget %s in :\n%s\n",a_args[1],OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return EXIT_FAILURE;
    }

  XWidgetLower        (widget);
  OProcessPutHandles (a_process,NULL);

  return EXIT_SUCCESS;
}
/***************************************************************************/
int XxExecute_mapWidget (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> mapWidget <string:Widget>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget widget;
/*.........................................................................*/
  if(a_argn!=2)
    {
      CWarnF("One argument expected in :\n%s\n",OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return EXIT_FAILURE;
    }

  widget      = XWidgetGetIdentifier (a_args[1]);
  if(widget==NULL)
    {
      CWarnF ("Unknown widget %s in :\n%s\n",a_args[1],OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return EXIT_FAILURE;
    }

  XWidgetMap          (widget);
  OProcessPutHandles (a_process,NULL);

  return EXIT_SUCCESS;
}
/***************************************************************************/
int XxExecute_unmapWidget (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> unmapWidget <string:Widget>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget widget;
/*.........................................................................*/
  if(a_argn!=2) 
    {
      CWarnF("One argument expected in :\n%s\n",OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return EXIT_FAILURE;
    }

  widget      = XWidgetGetIdentifier (a_args[1]);
  if(widget==NULL)
    {
      CWarnF ("Unknown widget %s in :\n%s\n",a_args[1],OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return EXIT_FAILURE;
    }

  XWidgetUnmap        (widget);
  OProcessPutHandles (a_process,NULL);

  return EXIT_SUCCESS;
}
/***************************************************************************/
int XxExecute_manageWidget (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> manageWidget <string:Widget>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget widget;
/*.........................................................................*/
  if(a_argn!=2)
    {
      CWarnF("One argument expected in :\n%s\n",OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return EXIT_FAILURE;
    }

  widget      = XWidgetGetIdentifier (a_args[1]);
  if(widget==NULL)
    {
      CWarnF ("Unknown widget %s in :\n%s\n",a_args[1],OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return EXIT_FAILURE;
    }

  XWidgetManage       (widget);
  OProcessPutHandles (a_process,NULL);

  return EXIT_SUCCESS;
}
/***************************************************************************/
int XxExecute_unmanageWidget (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> unmanageWidget <string:Widget>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget widget;
/*.........................................................................*/
  if(a_argn!=2)
    {
      CWarnF("One argument expected in :\n%s\n",OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return EXIT_FAILURE;
    }

  widget      = XWidgetGetIdentifier (a_args[1]);
  if(widget==NULL)
    {
      CWarnF ("Unknown widget %s in :\n%s\n",a_args[1],OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return EXIT_FAILURE;
    }

  XWidgetUnmanage     (widget);
  OProcessPutHandles (a_process,NULL);

  return EXIT_SUCCESS;
}
/***************************************************************************/
int XxExecute_iconifyWidget (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> iconifyWidget <string:Widget>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget widget;
/*.........................................................................*/
  if(a_argn!=2)
    {
      CWarnF("One argument expected in :\n%s\n",OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return EXIT_FAILURE;
    }

  widget      = XWidgetGetIdentifier (a_args[1]);
  if(widget==NULL)
    {
      CWarnF ("Unknown widget %s in :\n%s\n",a_args[1],OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return EXIT_FAILURE;
    }

  XWidgetIconify      (widget);
  OProcessPutHandles (a_process,NULL);

  return EXIT_SUCCESS;
}
/***************************************************************************/
int XxExecute_uniconifyWidget (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> uniconifyWidget <string:Widget>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget widget;
/*.........................................................................*/
  if(a_argn!=2)
    {
      CWarnF("One argument expected in :\n%s\n",OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return EXIT_FAILURE;
    }

  widget      = XWidgetGetIdentifier (a_args[1]);
  if(widget==NULL)
    {
      CWarnF ("Unknown widget %s in :\n%s\n",a_args[1],OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return EXIT_FAILURE;
    }

  XWidgetUniconify    (widget);
  OProcessPutHandles (a_process,NULL);

  return EXIT_SUCCESS;
}
/***************************************************************************/
int XxExecute_setWidgetPosition (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> setWidgetPosition <string:Widget> <integer:x> <integer:y>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget   widget;
  int      status;
  Position x,y;
/*.........................................................................*/
  if(a_argn!=4) 
    {
      CWarnF("Three arguments expected in :\n%s\n",OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return EXIT_FAILURE;
    }

  widget = XWidgetGetIdentifier (a_args[1]);
  if(widget==NULL)
    {
      CWarnF ("Unknown widget %s in :\n%s\n",a_args[1],OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return EXIT_FAILURE;
    }

  x      = (Position)CStringConvertToLong(a_args[2],&status);
  if(status==0) 
    {
      CWarnF ("Argument %s not a number in :\n%s\n",a_args[2],OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return EXIT_FAILURE;
    }
  y      = (Position)CStringConvertToLong(a_args[3],&status);
  if(status==0) 
    {
      CWarnF ("Argument %s not a number in :\n%s\n",a_args[3],OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return EXIT_FAILURE;
    }

  XWidgetMove   (widget,x,y);

  OProcessPutHandles  (a_process,NULL);

  return   EXIT_SUCCESS;
}
/***************************************************************************/
int XxExecute_setWidgetDimension (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> setWidgetDimension <Widget> <integer:width> <integer:height>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget    widget;
  int       status;
  Dimension width,height;
/*.........................................................................*/
  if(a_argn!=4)
    {
      CWarnF("Three arguments expected in :\n%s\n",OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return EXIT_FAILURE;
    }

  widget      = XWidgetGetIdentifier (a_args[1]);
  if(widget==NULL)
    {
      CWarnF ("Unknown widget %s in :\n%s\n",a_args[1],OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return EXIT_FAILURE;
    }

  width  = (Position)CStringConvertToLong(a_args[2],&status);
  if(status==0) 
    {
      CWarnF ("Argument %s not a number in :\n%s\n",a_args[2],OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return EXIT_FAILURE;
    }
  height = (Position)CStringConvertToLong(a_args[3],&status);
  if(status==0) 
    {
      CWarnF ("Argument %s not a number in :\n%s\n",a_args[3],OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return EXIT_FAILURE;
    }

  XWidgetSetDimension   (widget,width,height);

  OProcessPutHandles  (a_process,NULL);

  return   EXIT_SUCCESS;
}
/***************************************************************************/
int XxExecute_saveWidgetPixmap (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> saveWidgetPixmap <string:Widget> [ps,gif,xbm] <string:file name>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget widget;
/*.........................................................................*/
  if(a_argn!=4) 
    {
      CWarnF("Three arguments expected in :\n%s\n",OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return EXIT_FAILURE;
    }

  widget      = XWidgetGetIdentifier (a_args[1]);
  if(widget==NULL)
    {
      CWarnF ("Unknown widget %s in :\n%s\n",a_args[1],OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return EXIT_FAILURE;
    }

  if(strcmp(a_args[2],"ps")==0)
    { 
      CPS_OpenFileForWriting           (a_args[3]);
      CPS_SetBitsPerPixel              (8);
      CPS_SetColorScheme               (CPS_Color);
      XWidgetPutPixmapPageInPS_Stream  (widget);
      CPS_CloseStream                   ();
    }
  else if(strcmp(a_args[2],"gif")==0)
    { 
      XWidgetSavePixmapInGIF_File (widget,a_args[3]);
    }
  else if(strcmp(a_args[2],"xbm")==0)
    { 
      char*                       fname;
      char*                       name;
      fname                       = CStringDuplicate (a_args[3]);
      name                        = CFileGetName (fname);
      XWidgetSavePixmapInXBM_File (widget,fname,name,0L);
      CStringDelete               (fname);
      CStringDelete               (name);
    }
  else
    {
      CWarnF              ("Bad option %s (ps, gif, xbm expected) in :\n%s\n",a_args[2],OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return              EXIT_FAILURE;
    }

  OProcessPutHandles    (a_process,NULL);

  return EXIT_SUCCESS;
}
/***************************************************************************/
int XxExecute_isWidgetSubclass (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> isWidgetSubclass <string:Widget> <string:class>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget      widget;
  WidgetClass wclass;
/*.........................................................................*/
  if(a_argn!=3) 
    {
      CWarnF("Two arguments expected in :\n%s\n",OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  widget      = XWidgetGetIdentifier (a_args[1]);
  if(widget==NULL)
    {
      CWarnF ("Unknown widget %s in :\n%s\n",a_args[1],OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  wclass = XClassGetIdentifier (a_args[2]);
  if(wclass==NULL)
    {
      CWarnF ("Unknown widget class %s in :\n%s\n",a_args[2],OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  if(!XtIsSubclass(widget,wclass))  return EXIT_FAILURE;
  else                              return EXIT_SUCCESS;
}
/***************************************************************************/
int XxExecute_isNotWidgetSubclass (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> isNotWidgetSubclass <string:Widget> <string:class>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget      widget;
  WidgetClass wclass;
/*.........................................................................*/
  if(a_argn!=3) 
    {
      CWarnF("Two arguments expected in :\n%s\n",OProcessGetCommand(a_process));
      return EXIT_SUCCESS;
    }

  widget      = XWidgetGetIdentifier (a_args[1]);
  if(widget==NULL)
    {
      CWarnF ("Unknown widget %s in :\n%s\n",a_args[1],OProcessGetCommand(a_process));
      return EXIT_SUCCESS;
    }

  wclass = XClassGetIdentifier (a_args[2]);
  if(wclass==NULL)
    {
      CWarnF ("Unknown widget class %s in :\n%s\n",a_args[2],OProcessGetCommand(a_process));
      return EXIT_SUCCESS;
    }

  if(!XtIsSubclass(widget,wclass))  return EXIT_SUCCESS;
  else                              return EXIT_FAILURE;
}
/***************************************************************************/
int XxExecute_callWidgetCallbacks (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> callWidgetCallbacks -               <string:callback name>
  osh> callWidgetCallbacks <string:Widget> <string:callback name>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget* widgets = NULL;
/*.........................................................................*/
  if(a_argn!=3) 
    {
      CWarnF              ("Two arguments expected in :\n%s\n",OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return              EXIT_FAILURE;
    }

  widgets = (Widget*)   OProcessGetTypedObjects (a_process,a_args[1],"Widget");

 {Widget*  itema;
  for(itema = widgets;(itema!=NULL) && (*itema!=NULL);itema++)
    {
      XWidgetCallCallbacks  (*itema,a_args[2]);
    }}

  CListDelete ((CList)widgets);

  OProcessPutHandles (a_process,NULL);

  return EXIT_SUCCESS;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
int XxExecute_saveRootWindowPixmap  (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> saveRootWindowPixmap <string:Widget> <integer:x> <integer:y> <integer:width> <integer:height> [ps,gif,xbm] <string:file name> 
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget    widget;
  Display*  xdisp;
  Pixmap    pixmap;
  int       status;
  Position  x,y;
  Dimension width,height;
/*.........................................................................*/
  if(a_argn!=8) 
    {
      CWarnF("Seven arguments expected in :\n%s\n",OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  widget = XWidgetGetIdentifier (a_args[1]);
  if(widget==NULL)
    {
      CWarnF ("Unknown widget %s in :\n%s\n",a_args[1],OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  x      = (Position)CStringConvertToLong(a_args[2],&status);
  if(status==0) 
    {
      CWarnF ("Argument %s not a number in :\n%s\n",a_args[2],OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }
  y      = (Position)CStringConvertToLong(a_args[3],&status);
  if(status==0) 
    {
      CWarnF ("Argument %s not a number in :\n%s\n",a_args[3],OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }
  width  = (Position)CStringConvertToLong(a_args[4],&status);
  if(status==0) 
    {
      CWarnF ("Argument %s not a number in :\n%s\n",a_args[4],OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }
  height = (Position)CStringConvertToLong(a_args[5],&status);
  if(status==0) 
    {
      CWarnF ("Argument %s not a number in :\n%s\n",a_args[5],OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  xdisp  = XtDisplay(widget);
  pixmap = XDisplayGetWindowPixmap (xdisp,XDefaultRootWindow(xdisp),x,y,x+width,y+height);
  if(pixmap==0L)
    {
      CWarnF ("Can't get root window pixmap in :\n%s\n",OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }
   
  if(strcmp(a_args[6],"ps")==0)
    { 
      CPS_OpenFileForWriting           (a_args[7]);
      CPS_SetBitsPerPixel              (8);
      CPS_SetColorScheme               (CPS_Color);
      XDisplayPutPixmapInPS_Stream     (xdisp,pixmap);
      CPS_CloseStream                  ();
    }
  else if(strcmp(a_args[6],"gif")==0)
    { 
      FILE*    file;
      XImage*  image;
      file     = CFileOpenForWriting (a_args[7]);
      if(file==NULL) 
	{
	  XFreePixmap         (xdisp,pixmap);
	  CWarnF              ("Can't open file %s in :\n%s\n",a_args[7],OProcessGetCommand(a_process));
	  OProcessPutHandles (a_process,NULL);
	  return              EXIT_FAILURE;
	}
      image                        = XDisplayGetPixmapImage (xdisp,pixmap);
      XDisplayPutImageInGIF_Stream (xdisp,image,file);
      if(image!=NULL)    XDestroyImage (image);
      fclose             (file);
    }
  else if(strcmp(a_args[6],"xbm")==0)
    { 
      char*                       fname;
      char*                       name;
      FILE*                       file;
      XImage*                     image;
      fname                       = CStringDuplicate (a_args[7]);
      name                        = CFileGetName (fname);
      file                        = CFileOpenForWriting (fname);
      if(file==NULL) 
	{
	  CStringDelete       (fname);
	  CStringDelete       (name);
	  XFreePixmap         (xdisp,pixmap);
	  CWarnF              ("Can't open file %s in :\n%s\n",fname,OProcessGetCommand(a_process));
	  OProcessPutHandles (a_process,NULL);
	  return              EXIT_FAILURE;
	}
      image                       = XDisplayGetPixmapImage (xdisp,pixmap);
      XImagePutBitmapInXBM_Stream (image,file,name,0L);
      if(image!=NULL)             XDestroyImage (image);
      fclose                      (file);
      CStringDelete               (fname);
      CStringDelete               (name);
    }
  else
    {
      CWarnF              ("Bad option %s (ps, gif, xbm expected) in :\n%s\n",a_args[6],OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return              EXIT_FAILURE;
    }

  XFreePixmap                  (xdisp,pixmap);

  return EXIT_SUCCESS;
}
/***************************************************************************/
int XxExecute_putRootWindowPixmapInPS  (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> putRootWindowPixmapInPS <string:Widget> <integer:x> <integer:y> <integer:width> <integer:height>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget    widget;
  Display*  xdisp;
  Pixmap    pixmap;
  int       status;
  Position  x,y;
  Dimension width,height;
/*.........................................................................*/
  if(a_argn!=6) 
    {
      CWarnF("Five arguments expected in :\n%s\n",OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  widget = XWidgetGetIdentifier (a_args[1]);
  if(widget==NULL)
    {
      CWarnF ("Unknown widget %s in :\n%s\n",a_args[1],OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  x      = (Position)CStringConvertToLong(a_args[2],&status);
  if(status==0) 
    {
      CWarnF ("Argument %s not a number in :\n%s\n",a_args[2],OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }
  y      = (Position)CStringConvertToLong(a_args[3],&status);
  if(status==0) 
    {
      CWarnF ("Argument %s not a number in :\n%s\n",a_args[3],OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }
  width  = (Position)CStringConvertToLong(a_args[4],&status);
  if(status==0) 
    {
      CWarnF ("Argument %s not a number in :\n%s\n",a_args[4],OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }
  height = (Position)CStringConvertToLong(a_args[5],&status);
  if(status==0) 
    {
      CWarnF ("Argument %s not a number in :\n%s\n",a_args[5],OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  xdisp  = XtDisplay(widget);
  pixmap = XDisplayGetWindowPixmap (xdisp,XDefaultRootWindow(xdisp),x,y,x+width,y+height);
  if(pixmap==0L)
    {
      CWarnF ("Can't get root window pixmap in :\n%s\n",OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }
   
  XDisplayPutPixmapInPS_Stream (xdisp,pixmap);
  XFreePixmap                  (xdisp,pixmap);

  return EXIT_SUCCESS;
}
/***************************************************************************/
int XxExecute_grabWidget (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> grabWidget <string:Widget>
  osh> grabWidget <string:Widget> <integer:button> <string:cursor>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget widget,grabbed;
/*.........................................................................*/
  if( (a_argn!=2) && (a_argn!=4) )
    {
      CWarnF("One or three arguments expected in :\n%s\n",OProcessGetCommand(a_process));
      OProcessPutString (a_process,NULL);
      return EXIT_FAILURE;
    }

  widget = XWidgetGetIdentifier (a_args[1]);
  if(widget==NULL)
    {
      CWarnF ("Unknown widget %s in :\n%s\n",a_args[1],OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return EXIT_FAILURE;
    }

  if(a_argn==4)
    {
      int      button;
      int      status;
      button   = (int)CStringConvertToLong(a_args[2],&status);
      if(status==0) 
	{
	  CWarnF ("Argument %s not a number in :\n%s\n",a_args[2],OProcessGetCommand(a_process));
	  OProcessPutString (a_process,NULL);
	  return EXIT_FAILURE;
	}
      XWidgetSetGrabButtonAndCursor(button,a_args[3]);
    }
  else
    {
      XWidgetSetGrabButtonAndCursor(0,NULL);
    }

  grabbed = XWidgetGrab(widget);
  if(grabbed!=NULL)
    {
      OProcessPutString (a_process,CStringCreateF(64,"Widget/%lu",grabbed));
    }
  else
    {
      OProcessPutHandles   (a_process,NULL);
    }

  return EXIT_SUCCESS;
}
/***************************************************************************/
int XxExecute_grabWidgetName (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> grabWidgetName <string:Widget>
  osh> grabWidgetName <string:Widget> <integer:button> <string:cursor>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget widget,grabbed;
/*.........................................................................*/
  if( (a_argn!=2) && (a_argn!=4) )
    {
      CWarnF("One or three arguments expected in :\n%s\n",OProcessGetCommand(a_process));
      OProcessPutString (a_process,NULL);
      return EXIT_FAILURE;
    }

  widget = XWidgetGetIdentifier (a_args[1]);
  if(widget==NULL)
    {
      CWarnF ("Unknown widget %s in :\n%s\n",a_args[1],OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return EXIT_FAILURE;
    }

  if(a_argn==4)
    {
      int      button;
      int      status;
      button   = (int)CStringConvertToLong(a_args[2],&status);
      if(status==0) 
	{
	  CWarnF ("Argument %s not a number in :\n%s\n",a_args[2],OProcessGetCommand(a_process));
	  OProcessPutString (a_process,NULL);
	  return EXIT_FAILURE;
	}
      XWidgetSetGrabButtonAndCursor(button,a_args[3]);
    }
  else
    {
      XWidgetSetGrabButtonAndCursor(0,NULL);
    }

  grabbed = XWidgetGrab(widget);
  if( (grabbed==NULL) || (XtName(grabbed)==NULL) )
    {
      OProcessPutString (a_process,NULL);
      return            EXIT_FAILURE;
    }

  OProcessPutString  (a_process,CStringDuplicate(XtName(grabbed)));

  return EXIT_SUCCESS;
}
/***************************************************************************/
int XxExecute_captureAndSavePixmap (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> captureAndSavePixmap <string:Widget> [ps,gif,xbm] <string: file name>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget            widget;
  Display*          xdisp;
  Position          begx,begy,endx,endy;
  Position          grabX,grabY;
  Dimension         grabW,grabH;
  Pixmap            pixmap;
/*.........................................................................*/
  if(a_argn!=4)
    {
      CWarnF("Three arguments expected in :\n%s\n",OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return EXIT_FAILURE;
    }

  widget = XWidgetGetIdentifier (a_args[1]);
  if(widget==NULL)
    {
      CWarnF ("Unknown widget %s in :\n%s\n",a_args[1],OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return EXIT_FAILURE;
    }

  xdisp             = XtDisplay(widget);
  XDisplayDoRubberBandGrab   (xdisp,1,(Cursor)XWidgetConvertStringToCursor(widget,"crosshair"),&begx,&begy,&endx,&endy);
  grabX             = MINIMUM (begx,endx);
  grabY             = MINIMUM (begy,endy);
  grabW             = MAXIMUM (begx,endx) - grabX;
  grabH             = MAXIMUM (begy,endy) - grabY;

  pixmap            = XDisplayGetWindowPixmap (xdisp,XDefaultRootWindow(xdisp),grabX,grabY,grabX+grabW,grabY+grabH);
  if(pixmap!=0L)
    {
      if(strcmp(a_args[2],"ps")==0)
	{ 
	  CPS_OpenFileForWriting       (a_args[3]);
	  CPS_SetBitsPerPixel          (8);
	  CPS_SetColorScheme           (CPS_Color);
	  XDisplayPutPixmapInPS_Stream (xdisp,pixmap);
	  CPS_CloseStream              ();
	}
      else if(strcmp(a_args[2],"gif")==0)
	{ 
	  FILE*    file;
	  file     = CFileOpenForWriting (a_args[3]);
	  if(file!=NULL) 
	    {
	      XImage*            image;
	      image              = XDisplayGetPixmapImage (xdisp,pixmap);
	      XDisplayPutImageInGIF_Stream (xdisp,image,file);
	      if(image!=NULL)    XDestroyImage (image);
	      fclose             (file);
	    }
	  else
	    {
	      XFreePixmap         (xdisp,pixmap);
	      CWarnF              ("Can't open %s in :\n%s\n",a_args[3],OProcessGetCommand(a_process));
	      OProcessPutHandles (a_process,NULL);
	      return              EXIT_FAILURE;
	    }
	}
      else if(strcmp(a_args[2],"xbm")==0)
	{ 
	  char*                fname;
	  char*                name;
	  FILE*                file;
	  fname                = CStringDuplicate (a_args[3]);
	  name                 = CFileGetName (fname);
	  file                 = CFileOpenForWriting (fname);
	  if(file!=NULL) 
	    {
	      XImage*            image;
	      image              = XDisplayGetPixmapImage (xdisp,pixmap);
	      XImagePutBitmapInXBM_Stream (image,file,name,0L);
	      if(image!=NULL)          XDestroyImage (image);
	      fclose             (file);
	    }
	  else
	    {
	      XFreePixmap         (xdisp,pixmap);
	      CWarnF              ("Can't open %s in :\n%s\n",fname,OProcessGetCommand(a_process));
	      OProcessPutHandles (a_process,NULL);
	      return              EXIT_FAILURE;
	    }
	  CStringDelete        (fname);
	  CStringDelete        (name);
	}
      else
	{
	  XFreePixmap         (xdisp,pixmap);
	  CWarnF              ("Bad option %s (ps, gif, xbm expected) in :\n%s\n",a_args[2],OProcessGetCommand(a_process));
	  OProcessPutHandles (a_process,NULL);
	  return              EXIT_FAILURE;
	}
      XFreePixmap       (xdisp,pixmap);
    }

  OProcessPutHandles    (a_process,NULL);

  return EXIT_SUCCESS;
}
/***************************************************************************/
int XxExecute_getDisplayFonts (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> getDisplayFonts <string:Widget>
  osh> getDisplayFonts <string:Widget> [*,<string:name>]
  osh> getDisplayFonts <string:Widget> [*,<string:name>]  <integer:maximum number of font retrived>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget widget;
  char*  name;
  int    max = 1000;
  char*  string = NULL;
/*.........................................................................*/
  if( (a_argn!=2) && (a_argn!=3) && (a_argn!=4) )
    {
      CWarnF("One, two or three arguments expected in :\n%s\n",OProcessGetCommand(a_process));
      OProcessPutString (a_process,NULL);
      return EXIT_FAILURE;
    }

  widget = XWidgetGetIdentifier (a_args[1]);
  if(widget==NULL)
    {
      CWarnF ("Unknown widget %s in :\n%s\n",a_args[1],OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return EXIT_FAILURE;
    }

  if(a_argn==2)
    {
      name = CStringDuplicate("*");
    }
  else
    {
      name = CStringDuplicate(OProcessGetString(a_process,a_args[2]));
    }

  if(a_argn==4)
    {
      int    status;
      int    value;
      value  = (int)CStringConvertToLong(a_args[3],&status);
      if(status==0) 
	{
	  CWarnF ("Argument %s not a number in :\n%s\n",a_args[3],OProcessGetCommand(a_process));
	  OProcessPutString (a_process,NULL);
	  return EXIT_FAILURE;
	}
      max = value;
    }
  
  if( (name!=NULL) && (max!=0) )
    {
      int            linen;
      char**         lines;
      lines          = XListFonts(XtDisplay(widget),name,max,&linen);
      string         = CTextConvertToString (linen,lines,"\n");
      XFreeFontNames (lines);
    }
  CStringDelete  (name);

  OProcessPutString (a_process,string);

  return EXIT_SUCCESS;
}
/***************************************************************************/
int XxExecute_getDisplayColors (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> getDisplayColors
  osh> getDisplayColors <index>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int    linen;
  char** lines;
/*.........................................................................*/
  if( (a_argn!=1) && (a_argn!=2) )
    {
      CWarnF            ("Zero or one argument expected in :\n%s\n",OProcessGetCommand(a_process));
      OProcessPutString (a_process,NULL);
      return            EXIT_FAILURE;
    }

  lines          = OColorGetNames (&linen);

  if( (linen!=0) && (lines!=NULL)  ) 
    {
      if(a_argn==1)
	{
	  OProcessPutString (a_process,CTextConvertToString (linen,lines,"\n"));
	}
      else
	{
	  int      status;
	  int      index = 0;
	  index    = (int)CStringConvertToLong(a_args[1],&status);
	  if(status==0) 
	    {
	      CWarnF              ("Argument %s not a number in :\n%s\n",a_args[1],OProcessGetCommand(a_process));
	      OProcessPutHandles (a_process,NULL);
	      return              EXIT_FAILURE;
	    }
	  if((index<0) || (index>=linen))
	    {
	      CWarnF              ("Index %d out of range [0,%d] in :\n%s\n",index,linen-1,OProcessGetCommand(a_process));
	      OProcessPutHandles (a_process,NULL);
	      return              EXIT_FAILURE;
	    }
	  else
            {
	      OProcessPutString (a_process,CStringDuplicate (lines[index]));
	    }
	}
    }
  else
    {
      CWarnF            ("No color found for:\n%s\n",OProcessGetCommand(a_process));
      OProcessPutString (a_process,NULL);
      return            EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}
/***************************************************************************/
int XxExecute_createMailbox (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> createMailbox <string:Widget> <string:name>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget   widget;
  XMailbox mbx;
/*.........................................................................*/
  if(a_argn!=3)
    {
      CWarnF            ("One argument expected in :\n%s\n",OProcessGetCommand(a_process));
      OProcessPutString (a_process,NULL);
      return            EXIT_FAILURE;
    }

  widget = XWidgetGetIdentifier (a_args[1]);
  if(widget==NULL)
    {
      CWarnF ("Unknown widget %s in :\n%s\n",a_args[1],OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return EXIT_FAILURE;
    }

  mbx               = XMailboxCreate (a_args[2],XtDisplay(widget));
  OProcessPutString (a_process,CStringCreateF (64,"%lu",mbx));

  return            EXIT_SUCCESS;
}
/***************************************************************************/
int XxExecute_sendMessageToMailbox (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> sendMessageToMailbox <string:Widget> <string:XMailbox> <string: message>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget widget;
/*.........................................................................*/
  if(a_argn!=4)
    {
      CWarnF            ("Three arguments expected in :\n%s\n",OProcessGetCommand(a_process));
      OProcessPutString (a_process,NULL);
      return            EXIT_FAILURE;
    }

  widget = XWidgetGetIdentifier (a_args[1]);
  if(widget==NULL)
    {
      CWarnF ("Unknown widget %s in :\n%s\n",a_args[1],OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return EXIT_FAILURE;
    }

  XMailboxSendString   (XMailboxGetIdentifier(a_args[2],XtDisplay(widget)),a_args[3]);
  OProcessPutHandles  (a_process,NULL);

  return EXIT_SUCCESS;
}
/***************************************************************************/
int XxExecute_sendFileToDisplayBuffer (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> sendFileToDisplayBuffer <string:Widget> <string:file name>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget widget;
/*.........................................................................*/
  if(a_argn!=3)
    {
      CWarnF              ("Two argument2 expected in :\n%s\n",OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return              EXIT_FAILURE;
    }

  widget = XWidgetGetIdentifier (a_args[1]);
  if(widget==NULL)
    {
      CWarnF ("Unknown widget %s in :\n%s\n",a_args[1],OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return EXIT_FAILURE;
    }

  XSelectionSendFile  (XtDisplay(widget),a_args[2]);
  OProcessPutHandles (a_process,NULL);

  return EXIT_SUCCESS;
}
/***************************************************************************/
int XxExecute_saveDisplayBuffer (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> saveDisplayBuffer <string:file name>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_argn!=2)
    {
      CWarnF              ("One argument expected in :\n%s\n",OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return              EXIT_FAILURE;
    }

  XSelectionPutBufferInFile (a_args[1]);
  OProcessPutHandles       (a_process,NULL);

  return EXIT_SUCCESS;
}
/***************************************************************************/
int XxExecute_requireDisplayBuffer (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> requireDisplayBuffer <string:Widget>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget widget;
/*.........................................................................*/
  if(a_argn!=2)
    {
      CWarnF              ("One argument expected in :\n%s\n",OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return              EXIT_FAILURE;
    }

  widget = XWidgetGetIdentifier (a_args[1]);
  if(widget==NULL)
    {
      CWarnF ("Unknown widget %s in :\n%s\n",a_args[1],OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return EXIT_FAILURE;
    }

  XSelectionRequireData (XtDisplay(widget));
  OProcessPutHandles   (a_process,NULL);

  return EXIT_SUCCESS;
}
/***************************************************************************/
int XxExecute_ringBell (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> ringBell <string:Widget>
  osh> ringBell <string:Widget> <integer:iteration>
  osh> ringBell <string:Widget> <integer:iteration> <integer:volume>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget   widget;
  int      number,volume,status;
/*.........................................................................*/
  if( (a_argn!=2) && (a_argn!=3) && (a_argn!=4)  )
    {
      CWarnF("One, two or three arguments expected in :\n%s\n",OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  widget      = XWidgetGetIdentifier (a_args[1]);
  if(widget==NULL)
    {
      CWarnF ("Unknown widget %s in :\n%s\n",a_args[1],OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  number = 1;
  if(a_argn>=3)
    {
      number = (Position)CStringConvertToLong(a_args[2],&status);
      if(status==0) 
	{
	  CWarnF ("Argument %s not a number in :\n%s\n",a_args[2],OProcessGetCommand(a_process));
	  return EXIT_FAILURE;
	}
    }

  volume = 99;
  if(a_argn==4)
    {
      volume = (Position)CStringConvertToLong(a_args[3],&status);
      if(status==0) 
	{
	  CWarnF ("Argument %s not a number in :\n%s\n",a_args[3],OProcessGetCommand(a_process));
	  return EXIT_FAILURE;
	}
    }
  if( (volume<0) || (volume>100) ) volume = 99;

 {int count;
  for(count=0;count<number;count++) XBell (XtDisplay(widget),volume);}

  return   EXIT_SUCCESS;
}
/***************************************************************************/
int XxExecute_spawnShellScript (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> spawnShellScript <string:command>
  osh> spawnShellScript <string:command> <string:Widget>
 example : 
  osh> spawnShellScript ls
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget widget = NULL;
/*.........................................................................*/
  if( (a_argn!=2) && (a_argn!=3) )
    {
      CWarnF("One or two argument expected in :\n%s\n",OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  if(a_argn==3)
    {
      widget = XWidgetGetIdentifier (a_args[2]);
      if(widget==NULL)
	{
	  CWarnF ("Unknown widget %s in :\n%s\n",a_args[2],OProcessGetCommand(a_process));
	  return EXIT_FAILURE;
	}
    }

  XWidgetSpawnShellScript (widget,a_args[1],TreatInput);

  return   EXIT_SUCCESS;
}
/***************************************************************************/
static void TreatInput (
 Widget a_widget
,char*  a_string
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if( (a_widget==NULL) || (Class.treatInputFunction==NULL) ) 
    CInfo                    (a_string);
  else  
    Class.treatInputFunction (a_widget,a_string);
}
