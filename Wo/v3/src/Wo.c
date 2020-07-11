/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
static char what[] = "@(#)Wo v3";

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <CPrinter.h>
#include <CSystem.h>
#include <CString.h>
#include <CText.h>
#include <CFile.h>
#include <CPS.h>
#include <CList.h>
#include <CError.h>
#include <CTime.h>
#include <CMemory.h>
#include <CManager.h>
#include <CSystem.h>
#include <OType.h>
#include <OShell.h>
#include <OProcess.h>

#include <XMailbox.h>
#include <XSelection.h>

#include <XWidget.h>

#include <OWidget.h>
#include <OInterface.h>
#include <OResource.h>
#include <OInterpreter.h>
#include <OCyclic.h>

#include <OEvent.h>

#include <Wo.h>

#ifdef __cplusplus
extern "C"{
#endif
static void GetInterface            ();
static void Paste                   ();
static void MakeApplication         (char*,char*);
static void MakeApplicationMakefile (char*,char*);
#ifdef __cplusplus
}
#endif

#define NotFound (-1)

static struct 
{
  int                        init;
  XtAppContext               appContext;
  Widget                     top;
  OTrace                     verbose;
  int                        argn;
  char**                     args;
  char*                      sodb;
  WoInitializeClassFunction* initClassFunctions;
  int                        stopFlag;
  char*                      pasteScript;
  WoVoidFunction             exitFunction;
  OShell                     osh;
  WoVoidFunction*            clearClassFunctions;
  OProcess                   process;
} Class = {0,NULL,NULL,OTraceNone,0,NULL,NULL,NULL,0,NULL,NULL,NULL,NULL,NULL};
/***************************************************************************/
void WoSetXt (
 XtAppContext a_context
,Widget a_top
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  Class.appContext = a_context;
  Class.top        = a_top;
}
/***************************************************************************/
void WoStartup (
 int a_argn 
,char** a_args 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.init==1) return; /* already done */

  CTimeStart(); /*For trace mode OTraceStartup.*/

/*Do a copy of args that are corrupted by XtInitalize.*/
  Class.args = CTextDuplicate (a_argn,a_args,&Class.argn);

  if(Class.top==NULL)
    {
#if XtSpecificationRelease == 4
       Cardinal   argn;
       argn       = (Cardinal)a_argn;
#else
       int        argn;
       argn       = a_argn;
#endif
/* Init Xt first in any case. */
/* With this init, top shell will have a_args[0] has name. */
      Class.top = XtAppInitialize(&Class.appContext,"Wo",NULL,(Cardinal)0,&argn,a_args,NULL,NULL,(Cardinal)0);
      if(Class.top==NULL) 
	{
	  CWarn ("Unable to init Xt.\n");
	  exit  (EXIT_SUCCESS);
	}
      /*Restore a_args.*/
     {int argi;
      for(argi=0;argi<a_argn;argi++)
	{
	  strcpy(a_args[argi],Class.args[argi]);
	}}
    }
  
  XWidgetSetTop                (Class.top);
  XSelectionSetPasteFunction   (Paste);
  
  /*CWarn must be done here (after WoXxx that could redirect io).*/

  OWidgetInitializeClass ();
 {WoInitializeClassFunction* itema;
  for(itema = Class.initClassFunctions;(itema!=NULL) && (*itema!=NULL);itema++) (*itema)(XtDisplay(Class.top));}

  /*CWarn with console atbs loaded (from Xm.xrm) must be done here.*/

/*Fetch hierarchy.*/
  GetInterface                   ();
  OInterfaceLoadFile             (Class.sodb,NULL);
  OInterfaceSetExtentNotModified ();

  XtSetMappedWhenManaged    (Class.top,False);
 {Arg                       args[1];
  XtSetArg                  (args[0],"geometry","100x100+0+0");
  XtSetValues               (Class.top,args,1);}
  XtRealizeWidget           (Class.top);

/*A load file could be done also in WoT.c for -show option.*/
  if(OWidgetHasMappedShell()==0) XWidgetMap (OWidgetGetFirstShell());

 {char*     WOVERSION;
  WOVERSION = getenv("WOVERSION");
  if( (WOVERSION!=NULL) && (strcmp(WOVERSION ,OInterfaceGetVersion())!=0) ) /*No warning if WOVERSION not set.*/
    {
      CWarnF ("It is better to execute Wo %s setup script.\n",OInterfaceGetVersion());
    }}

  Class.init = 1;
}
/***************************************************************************/
void WoClearClass (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.init==0) return;

  CTextDelete             (Class.argn,Class.args);
  Class.argn              = 0;
  Class.args              = NULL;

  CStringDelete           (Class.pasteScript);
  Class.pasteScript       = NULL;

  CListDelete              ((CList)Class.initClassFunctions);
  Class.initClassFunctions = NULL;

  CStringDelete           (Class.sodb);
  Class.sodb              = NULL;

  CPrinterSetPutFunction   (NULL);
  CPrinterSetPutV_Function (NULL);

  if(Class.clearClassFunctions!=NULL)
    {
      WoVoidFunction* itema;
      for(itema = Class.clearClassFunctions;(itema!=NULL) && (*itema!=NULL);itema++)
	{
	  (*itema)();
	}
    }

  if(Class.top!=NULL)     OWidgetDelete(Class.top,False); 
/* 
  upper line will clear OPart of all widget hierarchy, 
  will destroy all child of TopShell,
  but will not destroy TopShell 
*/
  XMailboxClearClass      ();
  XSelectionClearClass    ();

  OCyclicClearClass       (); /*Done here for XtRemoveTimeOut.*/
  OInterfaceClearClass    ();
  OEventClearClass        ();
  OWidgetClearClass       ();
  OResourceClearClass     ();
  CPS_ClearClass          ();

  OShellDelete              (Class.osh);
  Class.osh                 = NULL;
  OProcessDelete            (Class.process);
  Class.process             = NULL;
  CListDelete               ((CList)Class.clearClassFunctions);
  Class.clearClassFunctions = NULL;

  /*  
  XtDestroyApplicationContext  (Class.appContext);
  Class.appContext             = NULL;
  */

  OTypeClearClass          ();

  Class.init               = 0;
  what[0] = '@'; /*c++ no warning.*/
}
/***************************************************************************/
int WoIsInitialized (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return Class.init;
}
/***************************************************************************/
void WoSetInterfaceFile (
 char* a_fname 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CStringDelete (Class.sodb);
  Class.sodb    = CStringDuplicate(a_fname);
}
/***************************************************************************/
char* WoGetInterfaceFile (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return Class.sodb;
}
/***************************************************************************/
char* WoGetVersion (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return OInterfaceGetVersion();
}
/***************************************************************************/
char** WoGetArguments (
 int* a_argn
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_argn!=NULL) *a_argn = Class.argn;
  return Class.args;
}
/***************************************************************************/
void WoSetInitializeClassFunction (
 WoInitializeClassFunction a_proc
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_proc==NULL)      return;
  CListAddEntryFunction ((CList*)&Class.initClassFunctions,(CEntryFunction)a_proc);
  CListRemoveDuplicates ((CList)Class.initClassFunctions,NULL,0);
}
/***************************************************************************/
void WoSetExitFunction (
 WoVoidFunction a_proc
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  Class.exitFunction = a_proc;
}
/***************************************************************************/
void WoSetTrace (
  OTrace a_level 
)
/***************************************************************************/
{
/*.........................................................................*/
  Class.verbose      = a_level;
  OInterfaceSetTrace (a_level);
  OWidgetSetTrace    (a_level);
}
/***************************************************************************/
OTrace WoGetTrace (
)
/***************************************************************************/
{
/*.........................................................................*/
  return Class.verbose;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static void GetInterface (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*     fname;
  char*     oenv;
  char*     aname;
  char*     name;
  int       fexist;
/*.........................................................................*/
  fname  = NULL;

/* search name of some .odb file to load */
  oenv          = getenv("WOENVIRONMENT");

  aname         = (Class.argn!=0) && (Class.args!=NULL) ? CFileGetName(Class.args[0]) : CStringNULL;
  name          = aname!=NULL ? CStringCreateF (strlen(aname)+4,"%s.odb",aname) : CStringNULL;
  CStringDelete (aname);

  if( (Class.sodb!=NULL) && (strcmp(Class.sodb,"nil")==0) ) 
    {

    } 
  else if( (Class.sodb==NULL) && (oenv==NULL) && ((fexist=CFileIsValidInPathList(name,NULL))==0) ) 
    {

      if(name!=NULL)
	{
	  CWarnF("No resource file specified.\n\
 Create an %s.odb file in current directory or\n\
 use environment variable WOENVIRONMENT to specifie \n\
 an existing one:\n\
  ksh> WOENVIRONMENT=\"my_odb.odb\";export WOENVIRONMENT\n\
  csh> setenv WOENVIRONMENT \"my_odb.odb\"\n\
  NT>  set    WOENVIRONMENT=my_odb.odb\n\
  VMS$ define WOENVIRONMENT my_odb.odb\n",name);
	}
      else
	{
	  CWarn("No resource file specified.\n\
 Use environment variable WOENVIRONMENT to do it:\n\
  ksh> WOENVIRONMENT=\"my_odb.odb\";export WOENVIRONMENT\n\
  csh> setenv WOENVIRONMENT \"my_odb.odb\"\n\
  NT>  set    WOENVIRONMENT=my_odb.odb\n\
  VMS$ define WOENVIRONMENT my_odb.odb\n");
	}
    } 
  else 
    {
      if(Class.sodb!=NULL)
	fname = CStringDuplicate (Class.sodb);
      else if(oenv!=NULL)
	fname = CStringDuplicate (oenv);
      else
	fname = CStringDuplicate (name);
    }
  CStringDelete  (name);
  WoSetInterfaceFile       (fname);
  CStringDelete  (fname);
}
/***************************************************************************/
/***************************************************************************/
/******** Event loop *******************************************************/
/***************************************************************************/
/***************************************************************************/
int WoProcessEvents (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.top==NULL) 
    {
      CWarn("No interface here.\n");
      return 0;
    } /* no interface here. */
  if(Class.appContext==NULL) return 0;

  if(Class.verbose==OTraceEvent) CInfo ("Entering main loop.\n");

  while(1)
    { 
      XEvent          xevent;
      XtAppNextEvent  (Class.appContext,&xevent);  /*XtAppNextEvent does more than XNextEvent !!!*/
      if ( (xevent.xany.type==ClientMessage) && 
           (xevent.xclient.data.l[0]==OEventExit) )   
        { 
          if(Class.verbose==OTraceEvent)  CInfo ("Exit main loop.\n");
          WoDispatchEvent (&xevent);
          return (int)(xevent.xclient.data.l[1]);
         }
      else if(WoDispatchEvent(&xevent)==0) 
	XtDispatchEvent(&xevent);        
    }

}
/***************************************************************************/
void WoFlushEvents (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.top==NULL) 
    {
      CWarn("No interface here.\n");
      return;
    }
  if(Class.appContext==NULL) return;

  XSync (XtDisplay(Class.top),0);
  while(1)
    { 
      XEvent          xevent;
      if(XtAppPending(Class.appContext)==0) break; /* if no event exit loop */
      XtAppNextEvent  (Class.appContext,&xevent);
      if ( (xevent.xany.type==ClientMessage) && 
           (xevent.xclient.data.l[0]==OEventExit) )   
        { 
          WoDispatchEvent (&xevent);
          return;
         }
      else if(WoDispatchEvent(&xevent)==0) 
	XtDispatchEvent(&xevent);        
    }
}
/***************************************************************************/
int WoWaitClientMessage (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.appContext==NULL) return 0;
  while(1)
    { 
      XEvent          xevent;
      XtAppNextEvent  (Class.appContext,&xevent);

      XtDispatchEvent (&xevent);        

           if (xevent.xany.type==ClientMessage) 
	{
	  if(OEventIsPrivateEvent(&xevent)==1) 
            {
	      OEventDelete ((OEvent)(xevent.xclient.data.l[1]));
	    }
	  else if (xevent.xclient.data.l[0]==XMSG_CALLBACK)
            {
	      return xevent.xclient.data.l[1];
	    }
	}
    }
}
/***************************************************************************/
void WoSendExit (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.top==NULL) return;
  OEventSendExit (XtDisplay(Class.top),0);
}
/***************************************************************************/
int WoDispatchEvent (
 void* a_event
)
/***************************************************************************/
/*
  return 0 : not a Wo event.
  return 1 : Wo event.
  return 2 : Wo exit event.
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  XEvent* xevent;
/*.........................................................................*/
  if(a_event==NULL) return 0;

  xevent = (XEvent*)a_event;

  if (xevent->xany.type==PropertyNotify) 
    { 
      int            length;
      char*          message;
      if(XSelectionIsEndOfCopyEvent(xevent)==1) return 1; /* could come from an end of copy selection */
      message        = XMailboxGetMessage (xevent,&length);
      if(message!=NULL) 
        {  
	  /*get an external message:transform it to C string and send it to local message system */
          char*              string;
          string             = (char*)CMemoryAllocateBlock( (length+1) * sizeof(char));
          if(string!=NULL) 
            {
              string[length]      = '\0';
              strncpy             (string,message,(size_t)length);
              string[length]      = '\0';
              OWidgetRequireScriptExecution (Class.top,string,NULL,0);
              CMemoryFreeBlock              (string);
            }            
          XFree              (message);
        }
      return 1;
    }
  else if ( (xevent->xany.type==SelectionNotify) &&
            (XSelectionIsNotificationEvent(xevent)==1) ) /*Else could be a Motif cut/paste.*/
    { 
      XSelectionPasteFunction pasteProc;
      pasteProc               = XSelectionGetPasteFunction();
      if(pasteProc!=NULL)     pasteProc();
      return                  1;
    }
  else if ( (xevent->xany.type==SelectionRequest) && 
	    (XSelectionIsRequestEvent(xevent)==1) )      /*Else could be a Motif cut/paste.*/
    { 
      return               1;
    }
  else if ( (xevent->xany.type==ClientMessage)  && 
            (xevent->xclient.data.l[0]==OEventExecuteScript) )   
    {
      OEvent               event;
      event                = (OEvent)(xevent->xclient.data.l[1]);
      XWidgetSetThisWidget ((Widget)OEventGetWidget(event));
      XWidgetSetCallValue  (OEventGetValue(event));
      XWidgetSetCallData   (OEventGetEvent(event),1);
      if(Class.verbose==OTraceEvent) 
	{
	  CInfoF("Execute:%s\n",OEventGetString(event));
	}
      OScriptExecute       (OEventGetString(event));
      XWidgetSetThisWidget (NULL);
      XWidgetSetCallValue  (NULL);
      XWidgetSetCallData   (NULL,0);
      OEventDelete         (event);
      return               1;
    }
  else if ( (xevent->xany.type==ClientMessage)  && 
            (xevent->xclient.data.l[0]==OEventExit) )   
    {
      OEvent                     event;
      char*                      string;
      long                       retval = 0L;
      event                      = (OEvent)(xevent->xclient.data.l[1]);
      string                     = OEventGetString(event);
      retval                     = CStringConvertToLong (string,NULL);
      OEventDelete               (event);
      xevent->xclient.data.l[1] = retval;
      return                     2;
    }
  return 0;
}
/***************************************************************************/
static void Paste (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  OWidgetRequireScriptExecution   (Class.top,Class.pasteScript,NULL,0);
}
/***************************************************************************/
void WoSetPasteScript (
 char* a_script
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CStringDelete     (Class.pasteScript);
  Class.pasteScript = CStringDuplicate (a_script);
}
/***************************************************************************/
int WoGetStopFlag (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.appContext==NULL) return 0;
  while(1)
    { 
      XEvent         xevent;
      if(XtAppPending(Class.appContext)==0) break; /* if no event exit loop */
      XtAppNextEvent (Class.appContext,&xevent);
/*    if ( (xevent.xany.type==ClientMessage) && 
           (xevent.xclient.data.l[0]==OEventExit) )   
        { 
          if(Class.verbose==OTraceEvent)  CInfo ("Exit main loop.\n");
          WoDispatchEvent (&xevent);
          return (int)(xevent.xclient.data.l[1]);
         }
      else */ 
      if(WoDispatchEvent(&xevent)==0) 
	XtDispatchEvent(&xevent);        
    }
  if(Class.stopFlag==1) 
    {
      Class.stopFlag = 0;
      return 1;
    }
  return             0;
}
/***************************************************************************/
void WoSetStopFlag (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  Class.stopFlag = 1;
}
/***************************************************************************/
OShell WoGetShellInterpreter (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.osh!=NULL) return Class.osh;
  Class.osh = OShellCreate("osh");
  return    Class.osh;
}
/***************************************************************************/
void WoExecuteSystemScript (
 char* a_string
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(CSystemIsSecured()==1)
    {
      CWarn ("System is secured.\n");
      return;
    }
  CSystemExecute (a_string);
}
/***************************************************************************/
void WoExecuteShellScript (
 char* a_string
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  OShellExecute (WoGetShellInterpreter(),a_string);
}
/***************************************************************************/
void WoExecuteShellScriptInSameContext (
 char* a_string
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  OProcessExecuteLines (WoGetGlobalProcess(),a_string);
}
/***************************************************************************/
void WoExecuteShellFileInSameContext (
 char* a_string
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  OProcessExecuteFile (WoGetGlobalProcess(),a_string);
}
/***************************************************************************/
OProcess WoGetGlobalProcess (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.process==NULL) Class.process = OProcessCreate (WoGetShellInterpreter());
  return Class.process;
}
/***************************************************************************/
void WoAddClearClassFunction (
 WoVoidFunction a_proc
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_proc==NULL)      return;
  CListAddEntryFunction ((CList*)&Class.clearClassFunctions,(CEntryFunction)a_proc);
  CListRemoveDuplicates ((CList)Class.clearClassFunctions,NULL,0);
}
/***************************************************************************/
int WoSetAttribute (
 void*  This
,char*  a_name
,void*  a_user
,void*  a_addr 
,int    a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_name==NULL)  return 0;
  if(a_addr==NULL)  return 0; 

       if(strcmp(a_name,"packages")==0) {}
  else if(strcmp(a_name,"version")==0)  {}
  else if(strcmp(a_name,"traces")==0)   {}
  else if(strcmp(a_name,"shells")==0)   {}
  else if(strcmp(a_name,"overflyHelp")==0)   
    {
      OWidgetSetHelp ((Boolean)(*((int*)a_addr)));
    }
  else if(strcmp(a_name,"stopFlag")==0)   
    {
      WoSetStopFlag ();
    }
  else if(strcmp(a_name,"trace")==0)   
    {
      WoSetTrace   (OTraceGetIdentifier (*((char**)a_addr)));
    }
  else if(strcmp(a_name,"interfaceFile")==0)   
    {
      WoSetInterfaceFile (*((char**)a_addr));
    }
  else if(strcmp(a_name,"pasteScript")==0)   
    {
      WoSetPasteScript (*((char**)a_addr));
    }
  else 
    {
      CInfoF ("WoSetAttribute: unknown property %s.\n",a_name);
      return 0;
    }

  a_user   = NULL;
  a_number = 0;
  This     = NULL;
  return   1;
}
/***************************************************************************/
int WoGetAttribute (
 void*  This
,char*  a_name
,void*  a_user
,void*  a_addr 
,int*   a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_number!=NULL) *a_number = 0;
  if(a_name==NULL)  return 0;
  if(a_addr==NULL)  return 0; 

       if(strcmp(a_name,"packages")==0)  
    {
      int           linen;
      char**        lines;
      lines         = OInterfaceGetPackages (&linen);
      if(a_number!=NULL) *a_number = linen;
      *((char***)a_addr) = lines;
    }
  else if(strcmp(a_name,"traces")==0)  
    {
      int           linen;
      char**        lines;
      lines         = OTraceGetNames (&linen);
      if(a_number!=NULL) *a_number = linen;
      *((char***)a_addr) = lines;
    }
  else if(strcmp(a_name,"shells")==0)  
    {
      int           stringn;
      char**        strings;
      strings       = OWidgetGetShells (&stringn);
      if(a_number!=NULL) *a_number = stringn;
      *((char***)a_addr) = strings;
      return        FREE_BLOCKS;
    }
  else if(strcmp(a_name,"overflyHelp")==0)   
    {
      *((int*)a_addr) = OWidgetGetHelp();
    }
  else if(strcmp(a_name,"stopFlag")==0)   
    {
      *((int*)a_addr) = WoGetStopFlag();
    }
  else if(strcmp(a_name,"version")==0)
    {
      *((char**)a_addr)    = WoGetVersion();
    }
  else if(strcmp(a_name,"trace")==0)
    {
      *((char**)a_addr)    = OTraceGetName(WoGetTrace());
    }
  else if(strcmp(a_name,"interfaceFile")==0)   
    {
      *((char**)a_addr)    = WoGetInterfaceFile();
    }
  else if(strcmp(a_name,"pasteScript")==0)   
    {
      *((char**)a_addr)    = Class.pasteScript;
    }
  else 
    {
      CInfoF("WoGetAttribute: unknown property %s.\n",a_name);
      return 0;
    }

  a_user = NULL;
  This   = NULL;
  return 1;
}
/***************************************************************************/
int WoDoMethod (
 void*  This
,char*  a_name
,void*  a_user
,int    a_argn
,char** a_args
,void*  a_addr 
,int*   a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_number!=NULL) *a_number = 0;
  if(a_name==NULL)  return 0;

       if(strcmp(a_name,"sendExit")==0) 
    {
      int   value = 0;
      if(a_argn==1) 
	{
	  int   status;
	  value = (int)CStringConvertToLong(a_args[0],&status);
	  if(status==0) 
	    {
	      CWarnF ("Argument %s not a number.\n",a_args[0]);
	      return 0;
	    }
	}
      if(Class.exitFunction!=NULL) Class.exitFunction ();
      if(Class.top!=NULL) OEventSendExit  (XtDisplay(Class.top),value);
    }
  else if(strcmp(a_name,"sendExitOnly")==0) 
    {
      int   value = 0;
      if(a_argn==1) 
	{
	  int   status;
	  value = (int)CStringConvertToLong(a_args[0],&status);
	  if(status==0) 
	    {
	      CWarnF ("Argument %s not a number.\n",a_args[0]);
	      return 0;
	    }
	}
      if(Class.top!=NULL) OEventSendExit (XtDisplay(Class.top),value);
    }
  else if(strcmp(a_name,"processEvents")==0)   
    {
      WoProcessEvents    ();
    }
  else if(strcmp(a_name,"flushEvents")==0)   
    {
      WoFlushEvents    ();
    }
  else if(strcmp(a_name,"loadInterface")==0)   
    {
      if(a_argn!=1) 
	{
	  CWarnF("One argument expected for %s routine property.\n",a_name);
	  return 0;
	}
      WoSetInterfaceFile             (a_args[0]);
      OInterfaceLoadFile             (a_args[0],NULL);
      OInterfaceSetExtentNotModified ();
      if(OWidgetHasMappedShell()==0) XWidgetMap (OWidgetGetFirstShell());
      WoProcessEvents                ();
    }
  else if(strcmp(a_name,"removeInterface")==0)   
    {
      if(a_argn!=1) 
	{
	  CWarnF("One argument expected for %s routine property.\n",a_name);
	  return 0;
	}
      OInterfaceDestroy              (OInterfaceGetIdentifier(a_args[0]));
    }
  else if(strcmp(a_name,"saveInterface")==0)   
    {
      if(a_argn==0)
	{
	  OInterfaceSaveExtent ();
	}
      else if(a_argn==1)
	{
	  OInterface            oui;
	  oui                   = OInterfaceGetIdentifier(a_args[0]);
	  OInterfaceMark        (oui,1);
	  OInterfaceSave        (oui);
	}
    }
  else if(strcmp(a_name,"makeApplication")==0)   
    {
      if( (a_argn!=1) && (a_argn!=2) ) 
	{
	  CWarnF("One or two argument expected for %s routine property.\n",a_name);
	  return 0;
	}
      if(a_argn==2)
	{
	  MakeApplicationMakefile      (a_args[0],a_args[1]);
	  MakeApplication              (a_args[0],a_args[1]);
	}
      else
	{
	  MakeApplicationMakefile      (a_args[0],"");
	  MakeApplication              (a_args[0],"");
	}
    }
  else if(strcmp(a_name,"makeApplicationMakefile")==0)   
    {
      if( (a_argn!=1) && (a_argn!=2) ) 
	{
	  CWarnF("One or two argument expected for %s routine property.\n",a_name);
	  return 0;
	}
      if(a_argn==2)
	{
	  MakeApplicationMakefile      (a_args[0],a_args[1]);
	}
      else
	{
	  MakeApplicationMakefile      (a_args[0],"");
	}
    }
  else 
    {
      CInfoF("WoDoMethod: %s is not a routine property.\n",a_name);
      return 0;
    }

  a_user = NULL;
  a_argn = 0;
  a_args = NULL;
  This   = NULL;
  a_addr = NULL;
  return 1;
}
/******************************************************************************/
static void MakeApplication (
 char* a_name 
,char* a_dir
)
/***************************************************************************/
/*
  For UNIX a_dir must be a string ended with '/'. Exa : /tmp/ .
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int    status = 1;
  char*  fname;
  char*  tname;
/*.........................................................................*/
  if(a_dir==NULL) return;
  if(CStringIsWord(a_name)==0) 
    {
      CWarnF ("\"%s\" is not a valid application name.\n",a_name);
      return;
    }

  fname           = CStringCreateF (strlen(a_name)+2,"%s.c",a_name);
  tname           = CStringCreateF (strlen(a_dir) +6,"%sOApp.c" ,a_dir);
  if(CFileProduceFromTemplate (fname,tname,a_name,"OApp")==0) status = 0;
  CStringDelete   (fname);
  CStringDelete   (tname);

  fname           = CStringCreateF (strlen(a_name)+4,"%s.odb",a_name);
  tname           = CStringCreateF (strlen(a_dir) +8,"%sOApp.odb",a_dir);
  if(CFileProduceFromTemplate (fname,tname,a_name,"OApp")==0) status = 0;
  CStringDelete   (fname);
  CStringDelete   (tname);

  fname           = CStringCreateF (strlen(a_name)+5,"%s.htm",a_name);
  tname           = CStringCreateF (strlen(a_dir) +9,"%sOApp.htm",a_dir);
  if(CFileProduceFromTemplate (fname,tname,a_name,"OApp")==0) status = 0;
  CStringDelete   (fname);
  CStringDelete   (tname);

  if(status==0)
    {
      CWarnF ("Code creation from template \"%s\" partially failed.\n",a_dir);
    }
  else
    {
      if(CSystemIsKindOf("UNIX",NULL)==1)
	{
	  CPrinterPutF("\
UNIX> [g]make `uname`\n\
      to compile and link.\n\
UNIX> ./%s.exe &\n\
      to run.\n\
  Note that you can also use :\n\
UNIX> oshow.exe %s.odb &\n\
  to see and modify interface of %s.\n",a_name,a_name,a_name);
	}
      else if(CSystemIsKindOf("NT",NULL)==1)
	{
	  CPrinterPutF("\
NT> nmake /f NMake NT\n\
      to compile and link.\n\
NT> %s.exe\n\
      to run.\n\
  Note that you can also use :\n\
NT> oshow.exe %s.odb\n\
  to see and modify interface of %s.\n",a_name,a_name,a_name);
	}
      else if(CSystemIsKindOf("VMS",NULL)==1)
	{
	  CPrinterPutF("\
VMS$ mms [ALPHA or VAX]\n\
      to compile and link.\n\
VMS$ run %s\n\
      to run.\n\
  Note that you can also use :\n\
VMS$ oshow == \"$WOROOT:[ALPHA or VAX]oshow.exe\"\n\
VMS$ oshow %s.odb\n\
  to see and modify interface of %s.\n",a_name,a_name,a_name);
	}
    }
}
/******************************************************************************/
static void MakeApplicationMakefile (
 char* a_name 
,char* a_dir
)
/***************************************************************************/
/*
  For UNIX a_dir must be a string ended with '/'. Exa : /tmp/ .
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int   status = 1;
  char* tname;
/*.........................................................................*/
  if(a_dir==NULL) return;
  if(CStringIsWord(a_name)==0) 
    {
      CWarnF ("\"%s\" is not a valid application name.\n",a_name);
      return;
    }

  tname           = CStringCreateF (strlen(a_dir) + 8,"%sOApp.mk",a_dir);
  if(CFileProduceFromTemplate ("Makefile",tname,a_name,"OApp")==0) status = 0;
  CStringDelete   (tname);

  tname           = CStringCreateF (strlen(a_dir)+12,"%sOAppMake.odb",a_dir);
  if(CFileProduceFromTemplate ("Make.odb",tname,a_name,"OApp")==0) status = 0;
  CStringDelete   (tname);

  tname           = CStringCreateF (strlen(a_dir) + 9,"%sOApp.nmk",a_dir);
  if(CFileProduceFromTemplate ("NMake",tname,a_name,"OApp")==0) status = 0;
  CStringDelete   (tname);

  tname           = CStringCreateF (strlen(a_dir) + 10,"%smacros.nmk",a_dir);
  if(CFileProduceFromTemplate ("macros.nmk",tname,a_name,"OApp")==0) status = 0;
  CStringDelete   (tname);

  tname           = CStringCreateF (strlen(a_dir) + 8,"%sOApp.mms",a_dir);
  if(CFileProduceFromTemplate ("descrip.mms",tname,a_name,"OApp")==0) status = 0;
  CStringDelete   (tname);

  tname           = CStringCreateF (strlen(a_dir) + 10,"%smacros.mms",a_dir);
  if(CFileProduceFromTemplate ("macros.mms",tname,a_name,"OApp")==0) status = 0;
  CStringDelete   (tname);

  if(status==0)
    {
      CWarnF ("Code creation from template \"%s\" partially failed.\n",a_dir);
    }
  else
    {
      CManagerProduceMakefile      ("Make.odb",0,NULL); /*Produce This.mk*/
      CManagerProduceNMakefile     ("Make.odb",0,NULL); /*Produce This.nmk*/
      CManagerProduceMMS_File      ("Make.odb",0,NULL); /*Produce This.mms*/
    }

}
