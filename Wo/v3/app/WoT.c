/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
/* The below cpp flags could be use to connect/disconnect the use of a package. 
   They are set/unset through the Wo.make file.
  HAS_XM HAS_CI HAS_XO HAS_XZ HAS_TCL HAS_XAW HAS_G3O HAS_DEX
*/
#include <stdlib.h>
#include <string.h>

/*
#define TWICE
*/
#ifdef TWICE   /*To test twice startup. Ci is not today able to have a second startup.*/
#undef HAS_CI
#endif

/*begin Want_h*/
#include <WoWo.h>
#include <WoXtw.h>
#include <WoXm.h>
#include <WoXo.h>
#include <WoHTML.h>
#include <WoXz.h>
#include <WoDeX.h>
#include <WoCi.h>
#include <WoPacksCi.h>
#include <Wotcl.h>
#include <WoHoXz.h>
#include <WoXaw.h>
#include <WoG3o.h>
#include <WoFNAL.h>
#include <WoPAW.h> /*Need HAS_XZ and HAS_PAW flags.*/
/*end Want_h*/

#ifdef HAS_G3O
#include <G3oEXPO.h>
#endif /*HAS_G3O*/

/*Co*/
#include <CPrinter.h>
#include <CMemory.h>
#include <CFile.h>
#include <CText.h>
#include <COption.h>
#include <CString.h>
#include <CSystem.h>
#include <CError.h>
#include <OHTML.h>

/*Xx*/
#include <XDisplay.h>
#include <XMailbox.h>
#include <XWidget.h>

/*Wo*/
#include <OEvent.h>
#include <OWidget.h>
#include <Wo.h>

#ifdef __cplusplus
extern "C"{
#endif
static void   ProcessOptions  (int);
static void   ReportError     (char*,int);
#ifdef __cplusplus
}
#endif

static char what[] = "@(#)WoT v3";
/***************************************************************************/
int main (
 int a_argn
,char** a_args
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
#ifdef TWICE
  {int count;for(count=0;count<2;count++) {
#endif

/*begin Want_c*/
#include <WoWo.ic>
#include <WoXtw.ic>
#include <WoXm.ic>
#include <WoXo.ic>
#include <WoHTML.ic>
#include <WoXz.ic>
#include <WoDeX.ic>
#include <WoCi.ic>
#include <WoPacksCi.ic>
#include <Wotcl.ic>
#include <WoHoXz.ic>
#include <WoXaw.ic>
#include <WoG3o.ic>
#include <WoFNAL.ic>
#include <WoPAW.ic>
/*end Want_c*/

#ifdef HAS_G3O  
  G3oEXPO_Initialize (WoGetShellInterpreter());  /*Define env for G3o example of a GEANT simulation.*/
#endif /*HAS_G3O*/

  WoSetInterfaceFile ("nil");
  WoStartup          (a_argn,a_args);

#define NotFound        (-1)
#define OPT_VERSION     1
#define OPT_LATEX       2
#define OPT_TOPIC       3
#define OPT_AUTHOR      5
#define OPT_APPLICATION 7
#define OPT_SHOW        9
#define OPT_TRACE      11
#define OPT_XRM        15
#define OPT_OSH        16
#define OPT_PAW        17
  COptionInitialize  (ProcessOptions);
  COptionAddEntry    ("-vers"    ,OPT_VERSION);
  COptionAddEntry    ("-latex"   ,OPT_LATEX);
  COptionAddEntry    ("-topic"   ,OPT_TOPIC);
  COptionAddEntry    ("-author"  ,OPT_AUTHOR);
  COptionAddEntry    ("-app"     ,OPT_APPLICATION);
  COptionAddEntry    ("-show"    ,OPT_SHOW);
  COptionAddEntry    ("-trace"   ,OPT_TRACE);
  COptionAddEntry    ("-xrm"     ,OPT_XRM);
  COptionAddEntry    ("-display" ,OPT_XRM);
  COptionAddEntry    ("-cmds"    ,OPT_OSH);
  COptionAddEntry    ("-w"       ,OPT_PAW);
 {int                argn;
  char**             args;
  args               = WoGetArguments (&argn); /*Get an original copy of args.*/
  COptionVisit       (argn,args);}
  COptionClearClass  ();

  WoProcessEvents    ();
  WoClearClass       ();
#ifdef TWICE
  }}
#endif

  return             EXIT_SUCCESS;
}
/***************************************************************************/
static void ProcessOptions (
 int   a_opt
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* arg1;
  char* arg2;
/*.........................................................................*/
  switch(a_opt)
    {
      case 0:
        OHTML_DumpTopic ("WoT.htm#The_Wo_tool");
        break;
/*Infos.*/
      case OPT_VERSION:
        CInfoF          ("Wo version %s.\n",WoGetVersion());
        break;
      case OPT_LATEX:
        arg1                    = COptionGoForth();
        OHTML_ProduceLATEX_File (arg1);
        break;
      case OPT_TOPIC:
        arg1               = COptionGoForth();
        OHTML_DumpTopic    (arg1);
        break;
      case OPT_AUTHOR:
        OHTML_DumpTopic    ("WoT.htm#Author_addresses");
        break;
      case OPT_OSH:
        arg1               = COptionGoForth();
        if(arg1!=NULL) 
	  {
	    OShellProduceHTML_File ((OShell)WoGetShellInterpreter(),arg1);
	  } 
	break;
/*Else.*/
      case OPT_APPLICATION:
        arg1                          = COptionGoForth();
        OShellExecuteF                ((OShell)WoGetShellInterpreter(),strlen(arg1)+22,"do Wo makeApplication %s",arg1);
        break;
      case OPT_SHOW:
        arg1                           = COptionGoForth();
        WoSetInterfaceFile             (arg1);
        OInterfaceLoadFile             (arg1,NULL);
	OInterfaceSetExtentNotModified ();
        if(OWidgetHasMappedShell()==0) XWidgetMap (OWidgetGetFirstShell());
        break;
      case OPT_XRM:
        arg1           = COptionGoForth();
        /* to avoid warnings */
        break;
      case OPT_TRACE:
        arg1           = COptionGoForth();
        WoSetTrace (OTraceGetIdentifier (arg1));
        break;
      case OPT_PAW:
        arg1           = COptionGoForth();
        /* to avoid warnings */
        break;
    }

}
/***************************************************************************/
static void ReportError (
 char* a_name
,int a_code
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CErrorHandle ("Wo","WoT",a_name,a_code);
}
