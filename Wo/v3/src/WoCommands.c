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

#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>

/*Co*/
#include <CString.h>
#include <CPrinter.h>
#include <CFile.h>
#include <CMemory.h>
#include <CText.h>
#include <CList.h>
#include <OType.h>
#include <CoCommands.h>

/*Xo*/
#include <XWidget.h>

/*Wo*/
#include <OResource.h>
#include <OInterface.h>
#include <OWidget.h>
#include <OEvent.h>
#include <OCyclic.h>
#include <OPiece.h>
#include <WoTypes.h>
#include <Wo.h>

#include <WoCommands.h>

typedef unsigned long Ulong;
/***************************************************************************/
void WoAddCommands (
 OShell a_osh
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(OCommandGetIdentifier("thisValue")!=NULL) return; /*done*/

  WoSetTypes  ();

  OShellAddNewCommand (a_osh,"Wo/.osh"                     ,WoExecute_dot_osh);
  OShellAddNewCommand (a_osh,"Wo/postScript"               ,WoExecute_postScript);
  OShellAddNewCommand (a_osh,"Wo/realize"                  ,WoExecute_realize);
  OShellAddNewCommand (a_osh,"Wo/thisValue"                ,WoExecute_thisValue);
  OShellAddNewCommand (a_osh,"Wo/setWidgetResource"        ,WoExecute_setWidgetResource);
  OShellAddNewCommand (a_osh,"Wo/setAndMarkWidgetResource" ,WoExecute_setAndMarkWidgetResource);
  OShellAddNewCommand (a_osh,"Wo/getWidgetResource"        ,WoExecute_getWidgetResource);
  OShellAddNewCommand (a_osh,"Wo/findWidget"               ,WoExecute_findWidget);

}
/***************************************************************************/
int WoExecute_dot_osh (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> .osh <string:script>
 Warning, below line is not authorized :
  .osh> .osh <string:script>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_argn!=2) 
    {
      CWarnF ("One argument expected in :\n%s\n",OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return EXIT_FAILURE;
    }

  if(WoGetGlobalProcess()==a_process)
    {
      CWarnF (".osh command not authorized in .osh> mode in :\n%s\n",
	      OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return EXIT_FAILURE;
    }


  WoExecuteShellScriptInSameContext (OProcessGetString(a_process,a_args[1]));

  OProcessPutHandles (a_process,NULL);

  return EXIT_SUCCESS;
}
/***************************************************************************/
int WoExecute_postScript (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> postScript <widget> <string:script> 
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget widget = NULL;
/*.........................................................................*/
  if(a_argn!=3) 
    {
      CWarnF ("Two arguments expected in :\n%s\n",OProcessGetCommand(a_process));
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

  OWidgetSendScript (widget,a_args[2],0);

  OProcessPutHandles (a_process,NULL);

  return EXIT_SUCCESS;
}
/***************************************************************************/
int WoExecute_realize (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> realize -
  osh> realize <string:oid>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OHandle* hands;
/*.........................................................................*/
  if(a_argn!=2)
    {
      CWarnF("One argument expected in :\n%s\n",OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return EXIT_FAILURE;
    }

  if(OProcessPreparePipe(a_process,a_args[1])==0) return EXIT_FAILURE;

  hands              = OProcessGetHandles(a_process);
  OInterfaceRealize  ((CList)hands,NULL);
  OProcessPutString  (a_process,NULL);

  return EXIT_SUCCESS;
}
/***************************************************************************/
int WoExecute_thisValue (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> thisValue
  osh> thisValue <string:what>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if( (a_argn!=1) && (a_argn!=2) )
    {
      CWarnF("Zero or One argument expected in :\n%s\n",OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return EXIT_FAILURE;
    }

  if(a_argn==2)
    {
      XtPointer    data;
      int          type;
      char*        xstring;
      data         = XWidgetGetCallData (&type);
      /*type = 0;called from a        callback, data is the a_data  pointer.*/
      /*type = 1;called from an event handler,  data is the a_event pointer.*/
      if(type==0)
	{
	  xstring           = OResourceGetCallbackData (XWidgetGetThisWidget(),data,a_args[1]);
	  OProcessPutString (a_process,CStringDuplicate(xstring));
	  XtFree            (xstring);
	}
      else
	{
	  OProcessPutString (a_process,NULL);
	}
    }
  else
    {
      OProcessPutString (a_process,OWidgetGetThisValue());
    }

  a_args = NULL;
  return EXIT_SUCCESS;
}
/***************************************************************************/
int WoExecute_setWidgetResource (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> setWidgetResource -               <string:resource> <string:value>
  osh> setWidgetResource <string:Widget> <string:resource> -
  osh> setWidgetResource <string:Widget> <string:resource> <string:value>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget* widgets = NULL;
  char*   string;
/*.........................................................................*/
  if(a_argn!=4)
    {
      CWarnF("Three arguments expected in :\n%s\n",OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return EXIT_FAILURE;
    }

  widgets = (Widget*)OProcessGetTypedObjects (a_process,a_args[1],"Widget");
  string  = OProcessGetString(a_process,a_args[3]);

 {Widget*  itema;
  for(itema = widgets;(itema!=NULL) && (*itema!=NULL);itema++)
    {
      OWidgetSetResourceFromString (*itema,a_args[2],string,False);
    }}

  CListDelete         ((CList)widgets);

  OProcessPutHandles (a_process,NULL);

  return              EXIT_SUCCESS;
}
/***************************************************************************/
int WoExecute_setAndMarkWidgetResource (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> setAndMarkWidgetResource -               <string:resource> <string:value>
  osh> setAndMarkWidgetResource <string:Widget> <string:resource> -
  osh> setAndMarkWidgetResource <string:Widget> <string:resource> <string:value>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget* widgets = NULL;
  char*   string;
/*.........................................................................*/
  if(a_argn!=4)
    {
      CWarnF("Three arguments expected in :\n%s\n",OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return EXIT_FAILURE;
    }

  widgets = (Widget*)OProcessGetTypedObjects (a_process,a_args[1],"Widget");
  string  = OProcessGetString(a_process,a_args[3]);

 {Widget*  itema;
  for(itema = widgets;(itema!=NULL) && (*itema!=NULL);itema++)
    {
      OWidgetSetResourceFromString (*itema,a_args[2],string,True);
    }}

  CListDelete         ((CList)widgets);

  OProcessPutHandles (a_process,NULL);

  return EXIT_SUCCESS;
}
/***************************************************************************/
int WoExecute_getWidgetResource (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> getWidgetResource <string:Widget> <string:resource>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget widget = NULL;
/*.........................................................................*/
  if(a_argn!=3)
    {
      CWarnF("Two arguments expected in :\n%s\n",OProcessGetCommand(a_process));
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

  OProcessPutString (a_process,OWidgetGetAndConvertResourceToString (widget,a_args[2]));

  return EXIT_SUCCESS;
}
/***************************************************************************/
int WoExecute_findWidget (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> findWidget <string:resource> <string:value> <string:Widget parent> 
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget  parent;
  Widget* items;
  Widget* itema;
  Widget  widget = NULL;
/*.........................................................................*/
  if(a_argn!=4)
    {
      CWarnF("Three arguments expected in :\n%s\n",OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return EXIT_FAILURE;
    }

  parent = XWidgetGetIdentifier (a_args[3]);
  if(parent==NULL)
    {
      CWarnF ("Unknown widget %s in :\n%s\n",a_args[3],OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return EXIT_FAILURE;
    }

  items = XWidgetGetLeaves (parent);
  if(items==NULL)
    {
      OProcessPutHandles (a_process,NULL);
      return EXIT_FAILURE;
    }

  for(itema=items;(itema!=NULL) && (*itema!=NULL);itema++)
    {
      char*  string;
      string = OWidgetGetAndConvertResourceToString (*itema,a_args[1]);
      if( (string!=NULL) && (strcmp(string,a_args[2])==0) ) 
	{
	  CStringDelete (string);
	  widget        = *itema;
          break;
	}
      CStringDelete (string);
    }

  CListDelete ((CList)items);
  
  if(widget==NULL) 
    OProcessPutString (a_process,NULL);
  else
    OProcessPutString (a_process,CStringCreateF(128,"Widget/%lu",widget));

  return EXIT_SUCCESS;
}
