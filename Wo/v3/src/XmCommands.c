/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifdef HAS_XM

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/*Xt*/
#include <X11/Intrinsic.h>

/*Xm*/
#include <Xm/List.h>
#include <Xm/Text.h>
#include <Xm/Form.h>

/*Co*/
#include <CString.h>
#include <CPrinter.h>
#include <CMemory.h>
#include <CText.h>
#include <CFile.h>
#include <CoCommands.h>

/*Xx*/
#include <XWidget.h>

/*Xo*/
/*#include <XoColormap.h>*/ /*Do not put an explicit dependence over Xo.*/
#define XoNselectedColor   "selectedColor"
#define XoNvirtualColormap "virtualColormap" 

/*Wo*/
#include <OClass.h>
#include <OPart.h>
#include <OWidget.h>
#include <OInterface.h>
#include <OEvent.h>

#include <OResource.h>

/*Else*/
#include <XmTools.h>

#include <XmCommands.h>

#ifdef __cplusplus
extern "C"{
#endif
static Widget CreateFormChild             (Widget,String,ArgList,Cardinal);
static void   InitializePrivatePart       (Widget,char*);
static void   SetOptionScript             (Widget,char*);
static void   SetCallBack                 (Widget);
/*Resource editor*/
static void   SelectResource              (Widget);
static void   AddModification             (Widget);
static void   ApplyModification           (Widget);
static void   ClearResourceEditor         ();
/*Attachment editor*/
static void   UpdateAttachmentPanel       (Widget,char*); 
static void   ApplyAttachment             (Widget);
/*Create compound*/
static Widget CreateRowColumn             (Widget,char*,int,int);
static Widget CreateForm                  (Widget,char*,int,int);
static Widget CreateRowsOfLabelTextScale  (Widget,int);
static Widget CreateRowsOfLabelText       (Widget,int);
#ifdef __cplusplus
}
#endif

#define NotFound (-1)

static struct 
{
  WidgetClass wclass;
/*SetOptionScript*/
  Widget      option;
  char*       script;
/*oresed*/
  Widget      widget;
  int         namen;
  char**      names;
  int         valuen;
  char**      values;
} Class = {NULL,NULL,NULL,NULL,0,NULL,0,NULL};
/***************************************************************************/
void XmAddCommands (
 OShell a_osh
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(OCommandGetIdentifier("buildWidgetGrid")!=NULL) return; /*done*/

  OShellAddNewCommand (a_osh,"Wo/buildWidgetGrid"       ,XmExecute_buildWidgetGrid);
  OShellAddNewCommand (a_osh,"Wo/setWidgetOptions"      ,XmExecute_setWidgetOptions);
  OShellAddNewCommand (a_osh,"Wo/setWidgetOption"       ,XmExecute_setWidgetOption);
  OShellAddNewCommand (a_osh,"Wo/setWidgetOptionScript" ,XmExecute_setWidgetOptionScript);
  OShellAddNewCommand (a_osh,"Wo/getWidgetOption"       ,XmExecute_getWidgetOption);
  OShellAddNewCommand (a_osh,"Wo/popWidgetAside"        ,XmExecute_popWidgetAside);
  OShellAddNewCommand (a_osh,"Wo/setWidgetTextSize"     ,XmExecute_setWidgetTextSize);
  OShellAddNewCommand (a_osh,"Wo/popWidgetBalloon"      ,XmExecute_popWidgetBalloon);
  OShellAddNewCommand (a_osh,"Wo/attachWidget"          ,XmExecute_attachWidget);
  OShellAddNewCommand (a_osh,"Wo/createCompoundWidget"  ,XmExecute_createCompoundWidget);
  OShellAddNewCommand (a_osh,"Wo/editWidgetResource"    ,XmExecute_editWidgetResource);

  OShellAddNewCommand (a_osh,"Wo/appendText"            ,XmExecute_appendText);
  OShellAddNewCommand (a_osh,"Wo/warn"                  ,XmExecute_warn);
  OShellAddNewCommand (a_osh,"Wo/ask"                   ,XmExecute_ask);
  OShellAddNewCommand (a_osh,"Wo/askPassword"           ,XmExecute_askPassword);
  OShellAddNewCommand (a_osh,"Wo/askText"               ,XmExecute_askText);
  OShellAddNewCommand (a_osh,"Wo/askList"               ,XmExecute_askList);
  OShellAddNewCommand (a_osh,"Wo/askFile"               ,XmExecute_askFile);

  OShellAddNewCommand (a_osh,"Wo/buildZones"            ,XmExecute_buildZones);
  OShellAddNewCommand (a_osh,"Wo/createGridFile"        ,XmExecute_createGridFile);
}
/***************************************************************************/
int XmExecute_buildWidgetGrid (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  &osh> buildWidgetGrid <string:Widget> <string:WidgetType> <integer:rows> <integer:columns>  
  Due to some widget destructions, it is better to pass this command in a backgroud script.
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget  parent;
  char*   type;
  OClass  oclass;
  int     rows,cols,status;
  OClassCreateWidgetFunction createProc;
/*.........................................................................*/
  if(a_argn!=5)
    {
      CWarnF("Four arguments expected in :\n%s\n",OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  parent = XWidgetGetIdentifier (a_args[1]);
  if(parent==NULL)
    {
      CWarnF ("Unknown widget %s in :\n%s\n",a_args[1],OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  type    = a_args[2];
  oclass  = OClassGetIdentifier (type);
  if(oclass==NULL)
    {
      CWarnF("Unknown Widget type %s in :\n%s\n",type,OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  rows    = (int)CStringConvertToLong(a_args[3],&status);
  if(status==0)  
    {
      CWarnF ("Argument %s not a number in :\n%s\n",a_args[3],OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  cols    = (int)CStringConvertToLong(a_args[4],&status);
  if(status==0)  
    {
      CWarnF ("Argument %s not a number in :\n%s\n",a_args[4],OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  createProc   = OClassGetCreateFunction (oclass);
  if(createProc==NULL)  /*True class (not compound).*/
    {
      Class.wclass = (WidgetClass)OClassGetWidgetClass (oclass);
      createProc   = CreateFormChild;
    }

  if(createProc==NULL) 
    {
      Class.wclass = NULL;
      CWarnF ("Can't find a create procedure for widget type %s in :\n%s\n",type,OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  XmFormBuildGrid       (parent,NULL,createProc,rows,cols,False);
  InitializePrivatePart (parent,type);
  Class.wclass          = NULL;

  a_process = NULL;

  return EXIT_SUCCESS;
}
/***************************************************************************/
int XmExecute_buildZones (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  &osh> buildZones <string:Widget> <string:WidgetType> <integer:rows> <integer:columns>  
 For XoPlotter or XzPlotter widgets.
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget  widget;
  char*   type;
  OClass  oclass;
  int     rows,cols,status;
  char*                      cname;
  Boolean                    adapt;
  OClassCreateWidgetFunction createProc;
/*.........................................................................*/
  if(a_argn!=5)
    {
      CWarnF("Four arguments expected in :\n%s\n",OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  widget = XWidgetGetIdentifier (a_args[1]);
  if(widget==NULL)
    {
      CWarnF ("Unknown widget %s in :\n%s\n",a_args[1],OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  type    = a_args[2];
  oclass  = OClassGetIdentifier (type);
  if(oclass==NULL)
    {
      CWarnF("Unknown Widget type %s in :\n%s\n",type,OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  rows    = (int)CStringConvertToLong(a_args[3],&status);
  if(status==0)  
    {
      CWarnF ("Argument %s not a number in :\n%s\n",a_args[3],OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  cols    = (int)CStringConvertToLong(a_args[4],&status);
  if(status==0)  
    {
      CWarnF ("Argument %s not a number in :\n%s\n",a_args[4],OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  if(strcmp(type,"XoPlotter")==0)
    {
      createProc   = OClassGetCreateFunction (oclass);
      if(createProc==NULL)  /*True class (not compound).*/
	{
	  Class.wclass = (WidgetClass)OClassGetWidgetClass (oclass);
	  createProc   = CreateFormChild;
	}
      if(createProc==NULL) 
	{
	  Class.wclass = NULL;
	  CWarnF ("Can't find a create procedure for widget type %s in :\n%s\n",type,OProcessGetCommand(a_process));
	  return EXIT_FAILURE;
	}
      cname                 = XWidgetGetClassName(XWidgetGetFirstChild(widget));
      adapt                 = ( (cname!=NULL) && (strcmp(cname,"XzPlotter")==0) ) ? False : True;
      XmFormBuildGrid       (widget,NULL,createProc,rows,cols,adapt);
      InitializePrivatePart (widget,type);
      Class.wclass          = NULL;
    }
  else if(strcmp(type,"XzPlotter")==0) /*Create one XzPlotter.*/
    {
      createProc   = OClassGetCreateFunction (oclass);
      if(createProc==NULL)  /*True class (not compound).*/
	{
	  Class.wclass = (WidgetClass)OClassGetWidgetClass (oclass);
	  createProc   = CreateFormChild;
	}
      if(createProc==NULL) 
	{
	  Class.wclass = NULL;
	  CWarnF ("Can't find a create procedure for widget type %s in :\n%s\n",type,OProcessGetCommand(a_process));
	  return EXIT_FAILURE;
	}
      cname                 = XWidgetGetClassName(XWidgetGetFirstChild(widget));
      adapt                 = ( (cname!=NULL) && (strcmp(cname,"XoPlotter")==0) ) ? False : True;
      XmFormBuildGrid       (widget,NULL,createProc,1,1,adapt);
      InitializePrivatePart (widget,type);
      Class.wclass          = NULL;
      widget                = XWidgetGetFirstChild (widget);
      if(widget!=NULL)      /*Set XzPlotter zone resource.*/
	{
	  Arg           args[1];
	  char*         string;
	  string        = CStringCreateF (2 * 64,"%d,%d",cols,rows);
	  XtSetArg      (args[0],"zones",string);
	  XtSetValues   (widget,args,1);
	  CStringDelete (string);
	}
    }
  else
    {
      CWarnF ("Argument %s not a XoPlotter or XzPlotter in :\n%s\n",type,OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  a_process = NULL;

  return EXIT_SUCCESS;
}
/***************************************************************************/
int XmExecute_createGridFile (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> createGridFile <file name> <parent> <widget type> <integer:rows> <integer:columns> <plottedObjects> <userCallback>
 Example :
  osh> createGridFile gauss.odb Visualizer_form XoPlotter 2 1
  osh> createGridFile gauss.odb Visualizer_form XoPlotter 2 1 Ho/gauss\nHo/gauss_2d 'osh> echo hello'
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*  afileName;
  char*  aparent;
  char*  atype;
  char*  arows;
  char*  acols;
  char*  aobjs = NULL;
  char*  auser = NULL;
  FILE*  file;
  char*  name;
  int    status,rows,cols,rowi,coli;
  int    width,height;
  int    top, bottom, left, right;
  int    border = 2;
  int    objn = 0;
  char** objs = NULL;
  int    obji;
/*.........................................................................*/
  if( (a_argn!=6) && (a_argn!=7) && (a_argn!=8) ) {
    CWarnF   ("Five, six, seven arguments expected in :\n%s\n",OProcessGetCommand(a_process));
    OProcessPutString (a_process,NULL);
    return            EXIT_FAILURE;
  }

  afileName = a_args[1];
  aparent   = a_args[2];
  atype     = a_args[3];
  arows     = a_args[4];
  acols     = a_args[5];
  if(a_argn>=7) aobjs = a_args[6];
  if(a_argn>=8) auser = a_args[7];

  rows    = (int)CStringConvertToLong(arows,&status);
  if(status==0) {
    CWarnF ("Argument %s not a number in :\n%s\n",arows,OProcessGetCommand(a_process));
    return EXIT_FAILURE;
  }

  cols    = (int)CStringConvertToLong(acols,&status);
  if(status==0) {
    CWarnF ("Argument %s not a number in :\n%s\n",acols,OProcessGetCommand(a_process));
    return EXIT_FAILURE;
  }
  if((rows<=0) || (cols<=0) ) {
    CWarnF ("Bad row or column number in :\n%s\n",OProcessGetCommand(a_process));
    return EXIT_FAILURE;
  }

  objs = CTextCreateFromString(aobjs,&objn);

  file = CFileOpenForWriting (afileName);
  if(file==NULL) return 0;

  name = CFileGetName(afileName);

  fprintf (file,"\
!----------------------------\n\
begin OInterface\n\
 prefix = %s_\n\
 parent = %s\n\
end\n\
!----------------------------\n\
begin OWidget\n\
 name = %s_grid\n\
 type = XmForm\n\
 background = dimgrey\n\
 topAttachment = attach_form\n\
 bottomAttachment = attach_form\n\
 leftAttachment = attach_form\n\
 rightAttachment = attach_form\n\
 fractionBase = 1000\n\
 children =\
",name,aparent,name);
  /*In reverse order so that ctrl+button2 permit to resize plots easily.*/
  for(rowi=rows-1;rowi>=0;rowi--) {
    for(coli=cols-1;coli>=0;coli--) {
      fprintf (file," %s_%d_%d",name,rowi+1,coli+1);
    }
  }
  fprintf (file,"\n\
end\n");

  height = (1000 / rows) - 2 * border;
  width  = (1000 / cols) - 2 * border;
  top    = border;
  bottom = top + height;

  obji   = 0;
  for(rowi=0;rowi<rows;rowi++) {
    left  = border;
    right = left + width;
    for(coli=0;coli<cols;coli++) {
      fprintf (file,"\
!----------------------------\n\
begin OWidget\n\
 name = %s_%d_%d\n\
 type = %s\n\
 topAttachment = attach_position\n\
 topPosition = %d\n\
 bottomAttachment = attach_position\n\
 bottomPosition = %d\n\
 leftAttachment = attach_position\n\
 leftPosition = %d\n\
 rightAttachment = attach_position\n\
 rightPosition = %d\n\
",name,rowi+1,coli+1,atype,top,bottom,left,right);

      if(obji<objn) {
	fprintf (file," plottedObjects = %s\n",objs[obji]);
	obji++;
      }
      if(auser!=NULL) {
	fprintf (file," userCallback = %s\n",auser);
      }

      fprintf (file,"end\n");

      left                 = right + 2 * border;
      right                = left  + width;
    }
    top     = bottom + 2 * border;
    bottom  = top    + height;
  }

  fclose        (file);

  CStringDelete (name);
  CTextDelete   (objn,objs);

  return        EXIT_SUCCESS;
}
/***************************************************************************/
int XmExecute_setWidgetOptions (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> setWidgetOptions <string:Widget> <strings:values> 
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget widget = NULL;
  char** list;
  int    nitem;
/*.........................................................................*/
  if(a_argn!=3)
    {
      CWarnF("Two arguments expected in :\n%s\n",OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  widget = XWidgetGetIdentifier (a_args[1]);
  if(widget==NULL)
    {
      CWarnF ("Unknown widget %s in :\n%s\n",a_args[1],OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }


  list               = CTextCreateFromString(a_args[2],&nitem);
  XmOptionInitialize (widget,nitem,list,"");
  CTextDelete        (nitem,list);

  return             EXIT_SUCCESS;
}
/***************************************************************************/
int XmExecute_setWidgetOption (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> setWidgetOption <string:Widget> -
  osh> setWidgetOption <string:Widget> <string:value> 
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget               widget = NULL;
/*.........................................................................*/
  if(a_argn!=3)
    {
      CWarnF("Two arguments expected in :\n%s\n",OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  widget = XWidgetGetIdentifier (a_args[1]);
  if(widget==NULL)
    {
      CWarnF ("Unknown widget %s in :\n%s\n",a_args[1],OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  XmOptionSetOption   (widget,OProcessGetString(a_process,a_args[2]));

  return               EXIT_SUCCESS;
}
/***************************************************************************/
int XmExecute_getWidgetOption (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> getWidgetOption <string:Widget>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget widget = NULL;
  char*  xstring;
/*.........................................................................*/
  if(a_argn!=2)
    {
      CWarnF("One argument expected in :\n%s\n",OProcessGetCommand(a_process));
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

  xstring           = XmOptionGetOption (widget);
  OProcessPutString (a_process,CStringDuplicate(xstring));
  XtFree            (xstring);

  return            EXIT_SUCCESS;
}
/***************************************************************************/
int XmExecute_setWidgetOptionScript (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> setWidgetOptionScript <string:Widget> -
  osh> setWidgetOptionScript <string:Widget> <string:script>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget               widget = NULL;
/*.........................................................................*/
  if(a_argn!=3)
    {
      CWarnF("Two arguments expected in :\n%s\n",OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  widget = XWidgetGetIdentifier (a_args[1]);
  if(widget==NULL)
    {
      CWarnF ("Unknown widget %s in :\n%s\n",a_args[1],OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }


  SetOptionScript      (widget,OProcessGetString(a_process,a_args[2]));

  return               EXIT_SUCCESS;
}
/***************************************************************************/
int XmExecute_popWidgetAside (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> popWidgetAside <string:Widget> <string:Widget>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget    popup,widget;
  XEvent*   xevent = NULL;
/*.........................................................................*/
  if(a_argn!=3)
    {
      CWarnF("Two arguments expected in :\n%s\n",OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  popup = XWidgetGetIdentifier (a_args[1]);
  if(popup==NULL)
    {
      CWarnF ("Unknown widget %s in :\n%s\n",a_args[1],OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  widget = XWidgetGetIdentifier (a_args[2]);
  if(widget==NULL)
    {
      CWarnF ("Unknown widget %s in :\n%s\n",a_args[2],OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

 {XtPointer data;
  int       type;
  data      = XWidgetGetCallData (&type);
  if(data==NULL) return EXIT_FAILURE;
/*type = 0;called from a        callback XThiCallData return the a_data  pointer.*/
/*type = 1;called from an event handler  XThisCallDat return the a_event pointer.*/
  if(type==1)
    {
      xevent = (XEvent*)data;
    }
  else
    {
      XWidgetCallbackEventFunction cbkProc;
      cbkProc                      = XWidgetSetCallbackEventFunction(NULL);      
      xevent                       = cbkProc!=NULL ? cbkProc(data) : (XEvent*)NULL;
      XWidgetSetCallbackEventFunction (cbkProc);      
    }}

  if(xevent==NULL) return EXIT_FAILURE;

  XmMenuPopAside (popup,widget,xevent);

  return EXIT_SUCCESS;
}
/***************************************************************************/
int XmExecute_setWidgetTextSize (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> setWidgetTextSize <string:Widget> <integer:rows> <integer:columns>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget widget;
  int    rows,cols,status;
/*.........................................................................*/
  if(a_argn!=4)
    {
      CWarnF("Three arguments expected in :\n%s\n",OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  widget = XWidgetGetIdentifier (a_args[1]);
  if(widget==NULL)
    {
      CWarnF ("Unknown widget %s in :\n%s\n",a_args[1],OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  rows         = (int)CStringConvertToLong(a_args[2],&status);
  if(status==0)
    {
      CWarnF ("Argument %s not a number in :\n%s\n",a_args[2],OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  cols         = (int)CStringConvertToLong(a_args[3],&status);
  if(status==0)
    {
      CWarnF ("Argument %s not a number in :\n%s\n",a_args[3],OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  XmTextSetNumberOfRowsAndColumns (widget,rows,cols);

  return EXIT_SUCCESS;
}
/***************************************************************************/
int XmExecute_appendText (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> appendText <string:Widget> <strings>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int    count;
  Widget widget;
  int    linen = 0;
  char** lines = NULL;
/*.........................................................................*/
  if(a_argn!=3)
    {
      CWarnF ("Two arguments expected in :\n%s\n",OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  widget = XWidgetGetIdentifier (a_args[1]);
  if(widget==NULL)
    {
      CWarnF ("Unknown widget %s in :\n%s\n",a_args[1],OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  lines = CTextCreateFromString (a_args[2],&linen);
  for(count=0;count<linen;count++) 
    {
      if(XtIsSubclass(widget,xmTextWidgetClass))
	{ 
	  XmTextAppendString (widget,lines[count]);
	}
      else if(XtIsSubclass(widget,xmListWidgetClass))
	{ 
	  XmListAppendString (widget,lines[count]);
	}
    }
  CTextDelete       (linen,lines);

  return EXIT_SUCCESS;
}
/***************************************************************************/
int XmExecute_popWidgetBalloon (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> popWidgetBalloon <widget> -
  osh> popWidgetBalloon <widget> <string>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget widget;
/*.........................................................................*/
  if(a_argn!=3)
    {
      CWarnF("Two arguments expected in :\n%s\n",OProcessGetCommand(a_process));
      OProcessPutHandles   (a_process,NULL);
      return EXIT_FAILURE;
    }

  widget = XWidgetGetIdentifier (a_args[1]);
  if(widget==NULL)
    {
      CWarnF ("Unknown widget %s in :\n%s\n",a_args[1],OProcessGetCommand(a_process));
      OProcessPutHandles   (a_process,NULL);
      return EXIT_FAILURE;
    }


  XmPopBalloon          (widget,OProcessGetString(a_process,a_args[2]));
  OProcessPutHandles   (a_process,NULL);

  return         EXIT_SUCCESS;
}
/***************************************************************************/
int XmExecute_attachWidget (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> attachWidget -               [left,right,top,bottom,apply,glue]
  osh> attachWidget <string:widget> [left,right,top,bottom,apply,glue]
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget widget = NULL;
  Widget parent;
/*.........................................................................*/
  if(a_argn!=3)
    {
      CWarnF ("Two arguments expected in :\n%s\n",OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return EXIT_FAILURE;
    }

  widget = XWidgetGetIdentifier(OProcessGetString(a_process,a_args[1]));
  if(widget==NULL)
    {
      CWarnF ("Unknown widget %s in :\n%s\n",a_args[1],OProcessGetCommand(a_process));
      OProcessPutHandles   (a_process,NULL);
      return EXIT_FAILURE;
    }

  parent = XtParent(widget); 
  if( (parent!=NULL) &&  (!XtIsSubclass(parent,xmFormWidgetClass)) )
    { 
      CWarnF ("Parent (%s) of widget %s is not an XmForm in :\n%s\n",XtName(parent),XtName(widget),OProcessGetCommand(a_process));
      OProcessPutHandles   (a_process,NULL);
      return EXIT_FAILURE;
    }

       if(strcmp(a_args[2],"glue")==0)
    {
      XmGlueWidget (widget,OWidgetMarkResourceToBeSaved);
    }
  else if(strcmp(a_args[2],"apply")==0)
    {
      ApplyAttachment (widget);
    }
  else if(
	  (strcmp(a_args[2],"left")==0)   ||
	  (strcmp(a_args[2],"right")==0)  ||
	  (strcmp(a_args[2],"top")==0)    ||
	  (strcmp(a_args[2],"bottom")==0) 
	  )
    {
      UpdateAttachmentPanel (widget,a_args[2]);
    }
  else
    {
      CWarnF("Unknown option %s (left,right,top,bottom,apply,glue expected) in :\n%s\n",a_args[2],OProcessGetCommand(a_process));
      OProcessPutHandles   (a_process,NULL);
      return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}
/***************************************************************************/
int XmExecute_createCompoundWidget (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> createCompoundWidget <string:parent> rc               <type> <rows> <cols>
  osh> createCompoundWidget <string:parent> form             <type> <rows> <cols>
  osh> createCompoundWidget <string:parent> label_text_scale <rows>
  osh> createCompoundWidget <string:parent> label_text       <rows>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget parent = NULL;
  int   status;
  int   rows,cols = 0;
  Widget widget = NULL;
/*.........................................................................*/
  if((a_argn!=4) && (a_argn!=6))
    {
      CWarnF ("Three or five arguments expected in :\n%s\n",OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return EXIT_FAILURE;
    }

  parent = XWidgetGetIdentifier(OProcessGetString(a_process,a_args[1]));
  if(parent==NULL)
    {
      CWarnF ("Unknown widget %s in :\n%s\n",a_args[1],OProcessGetCommand(a_process));
      OProcessPutHandles   (a_process,NULL);
      return EXIT_FAILURE;
    }

  if(a_argn==6)
    {
      rows   = (int)CStringConvertToLong(a_args[4],&status);
      if(status==0) 
	{
	  CWarnF ("Argument %s not a number in :\n%s\n",a_args[4],OProcessGetCommand(a_process));
	  OProcessPutHandles   (a_process,NULL);
	  return EXIT_FAILURE;
	}
      cols   = (int)CStringConvertToLong(a_args[5],&status);
      if(status==0) 
	{
	  CWarnF ("Argument %s not a number in :\n%s\n",a_args[5],OProcessGetCommand(a_process));
	  OProcessPutHandles   (a_process,NULL);
	  return EXIT_FAILURE;
	}
    }
  else
    {
      rows   = (int)CStringConvertToLong(a_args[3],&status);
      if(status==0) 
	{
	  CWarnF ("Argument %s not a number in :\n%s\n",a_args[3],OProcessGetCommand(a_process));
	  OProcessPutHandles   (a_process,NULL);
	  return EXIT_FAILURE;
	}
    }

       if(strcmp(a_args[2],"rc")==0)
    {
      widget = CreateRowColumn (parent,a_args[3],rows,cols);
    }
  else if(strcmp(a_args[2],"form")==0)
    {
      widget = CreateForm (parent,a_args[3],rows,cols);
    }
  else if(strcmp(a_args[2],"label_text_scale")==0)
    {
      widget = CreateRowsOfLabelTextScale (parent,rows);
    }
  else if(strcmp(a_args[2],"label_text")==0)
    {
      widget = CreateRowsOfLabelText (parent,rows);
    }
  else
    {
      CWarnF
      ("Unknown option %s (rc,form,label_text_scale,label_scale expected) in :\n%s\n",a_args[2],OProcessGetCommand(a_process));
      OProcessPutHandles   (a_process,NULL);
      return EXIT_FAILURE;
    }

  if(widget!=NULL)
    {
      OProcessPutString (a_process,CStringCreateF(7+64,"Widget/%lu",widget));
    }
  else
    {
      OProcessPutHandles   (a_process,NULL);
    }
  

  return EXIT_SUCCESS;
}
/***************************************************************************/
int XmExecute_editWidgetResource (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> editWidgetResource -               [reset, add, select, apply]
  osh> editWidgetResource <string:widget> [reset, add, select, apply]
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

  widget = XWidgetGetIdentifier(OProcessGetString(a_process,a_args[1]));

       if(strcmp(a_args[2],"reset")==0)
    {
      if( (widget!=NULL) && (widget!=Class.widget)) ClearResourceEditor ();
    }
  else if(strcmp(a_args[2],"select")==0)
    {
      SelectResource (widget);
    }
  else if(strcmp(a_args[2],"add")==0)
    {
      AddModification   (widget);
    }
  else if(strcmp(a_args[2],"apply")==0)
    {
      ApplyModification (widget);
    }
  else
    {
      CWarnF("Unknown option %s (reset,add,select,apply expected) in :\n%s\n",a_args[2],OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return EXIT_FAILURE;
    }

  OProcessPutHandles (a_process,NULL);

  return EXIT_SUCCESS;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
int XmExecute_warn (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> warn -
  osh> warn <string:message>
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
  XmDialogWarn         (OProcessGetString(a_process,a_args[1]));
  OProcessPutHandles  (a_process,NULL);
  return               EXIT_SUCCESS;
}
/***************************************************************************/
int XmExecute_ask (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> ask -               <string:default>
  osh> ask <string:prompt> <string:default>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*          xstring;
/*.........................................................................*/
  if(a_argn!=3)
    {
      CWarnF              ("Two arguments expected in :\n%s\n",OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return              EXIT_FAILURE;
    }
  xstring           = XmDialogAskForString(OProcessGetString(a_process,a_args[1]),a_args[2]);
  OProcessPutString (a_process,CStringDuplicate(xstring));
  XtFree            (xstring);
  return            EXIT_SUCCESS;
}
/***************************************************************************/
int XmExecute_askPassword (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> askPassword -
  osh> askPassword <string:prompt>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*          xstring;
/*.........................................................................*/
  if(a_argn!=2)
    {
      CWarnF              ("One argument expected in :\n%s\n",OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return              EXIT_FAILURE;
    }
  xstring           = XmDialogAskForPassword(OProcessGetString(a_process,a_args[1]));
  OProcessPutString (a_process,CStringDuplicate(xstring));
  XtFree            (xstring);
  return            EXIT_SUCCESS;
}
/***************************************************************************/
int XmExecute_askText (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> askText -               <string:default> [<integer:rows> <integer:cols>]
  osh> askText <string:prompt> <string:default> [<integer:rows> <integer:cols>]
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* xstring;
  int   rows,cols;
  char* string;
/*.........................................................................*/
  if( (a_argn!=3) && (a_argn!=5) )
    {
      CWarnF            ("Two or four arguments expected in :\n%s\n",OProcessGetCommand(a_process));
      OProcessPutString (a_process,NULL);
      return            EXIT_FAILURE;
    }

  rows = 0;
  cols = 0;
  if(a_argn==5)
    {
      int          status;
      rows         = (int)CStringConvertToLong(a_args[3],&status);
      if(status==0)
	{
	  CWarnF ("Argument %s not a number in :\n%s\n",a_args[3],OProcessGetCommand(a_process));
	  OProcessPutString   (a_process,NULL);
	  return EXIT_FAILURE;
	}
      cols         = (int)CStringConvertToLong(a_args[4],&status);
      if(status==0)  
	{
	  CWarnF ("Argument %s not a number in :\n%s\n",a_args[4],OProcessGetCommand(a_process));
	  OProcessPutString   (a_process,NULL);
	  return EXIT_FAILURE;
	}
    }

  string              = CStringDuplicate(a_args[2]);
  CStringReplacePart  (&string,"\\n","\n");
  xstring             = XmDialogAskForText(OProcessGetString(a_process,a_args[1]),string,rows,cols);
  CStringDelete       (string);
  OProcessPutString   (a_process,CStringDuplicate(xstring));
  XtFree              (xstring);

  return              EXIT_SUCCESS;
}
/***************************************************************************/
int XmExecute_askList (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> askList -               <string:default> [<boolean:sort>]
  osh> askList <string:prompt> <string:default> [<boolean:sort>]
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*   xstring;
  Boolean sort  = 0;
  int     linen = 0;
  char**  lines = NULL;
/*.........................................................................*/
  if( (a_argn!=3) && (a_argn!=4) ) 
    {
      CWarnF              ("Two or three arguments expected in :\n%s\n",OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return              EXIT_FAILURE;
    }

  if(a_argn==4)
    {
      int  status;
      sort = (Boolean)CStringConvertToBoolean (a_args[3],&status);
      if(status==0)  
	{
	  CWarnF ("Argument %s not a boolean in :\n%s\n",a_args[3],OProcessGetCommand(a_process));
	  OProcessPutHandles (a_process,NULL);
	  return EXIT_FAILURE;
	}
    }
  lines             = CTextCreateFromString (a_args[2],&linen);
  xstring           = XmDialogAskForList(OProcessGetString(a_process,a_args[1]),linen,lines,sort);
  OProcessPutString (a_process,CStringDuplicate(xstring));
  XtFree            (xstring);
  CTextDelete       (linen,lines);
  return            EXIT_SUCCESS;
}
/***************************************************************************/
int XmExecute_askFile (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> askFile
  osh> askFile <string:prompt>
  osh> askFile <string:prompt> <string:dirMask>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* xstring;
  char* prompt  = NULL;
  char* dirMask = NULL;
/*.........................................................................*/
  if( (a_argn!=1) && (a_argn!=2) && (a_argn!=3) ) 
    {
      CWarnF              ("Zero or one arguments expected in :\n%s\n",OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return              EXIT_FAILURE;
    }

  if(a_argn>=2) prompt  = OProcessGetString(a_process,a_args[1]);
  if(a_argn==3) dirMask = a_args[2];

  xstring           = XmDialogAskForFile(prompt,dirMask);
  OProcessPutString (a_process,CStringDuplicate(xstring));
  XtFree            (xstring);

  return            EXIT_SUCCESS;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static void SetOptionScript (
 Widget This 
,char* a_script 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  Class.option         = This;
  Class.script         = a_script;
  XmOptionVisit        (This,SetCallBack);
  Class.option         = NULL;
  Class.script         = NULL;
}
/***************************************************************************/
static void SetCallBack (
 Widget a_child
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
/* option could have been inited by XmOptionInitialize and then OPart of buttons are not created.*/
  OPartInitialize ((OPart)OWidgetBuildPrivatePart(a_child),NULL,OWidgetGetFile(Class.option),"XmPushButton",False,False);
  OWidgetSetResourceFromString   (a_child,"activateCallback",Class.script,False);
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static Widget CreateFormChild (
 Widget a_parent
,String a_name
,ArgList a_list
,Cardinal a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_parent==NULL)     return NULL;
  if(a_name==NULL)       return NULL;
  if(Class.wclass==NULL) return NULL;
  return        XtCreateManagedWidget(a_name,Class.wclass,a_parent,a_list,a_number);
}
/***************************************************************************/
static void InitializePrivatePart (
 Widget This 
,char*  a_type
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  WidgetList list;
  int        number;
/*.........................................................................*/
  if(This==NULL)           return;
  XWidgetVisitHierarchyForward  (This,(XWidgetVisitFunction)OWidgetBuildPrivatePart);
  list                = XWidgetGetChildren (This,&number);
  if( (number!=0) && (list!=NULL) )
   {int count;
    for(count=0;count<number;count++) 
      {
	OPart    opart;
	opart    = OPartGetIdentifierFromWidget(list[count]);
	OPartInitialize (opart,NULL,(XtPointer)OWidgetGetFile(This),a_type,False,False);
      }
   }
  CMemoryFreeBlock (list);
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static void SelectResource (
  Widget This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*  selection;
  char*  word1;
  char*  word2;
  char*  word3;
  char*  value;
  int    number;
  char** list;
/*.........................................................................*/
  if(This==NULL) return;

/* get infos */
  selection     = OWidgetGetAndConvertResourceToString(XWidgetGetIdentifier("ResEd_list"),"selectedItems");
  if( (selection==NULL) || (*selection=='\0') )
    {
      CStringDelete(selection);
      return;
    }
  if( (selection!=NULL) && (strncmp(selection,"-----",5)==0) ) 
    {
      CStringDelete(selection);
      return;
    }

  word1  = CStringGetFirstWord  (selection);
  word2  = CStringGetSecondWord (selection);
  word3  = CStringGetThirdWord  (selection);

  CStringDelete (selection);

  value      = OWidgetGetAndConvertResourceToString (This,word1);
  list       = OResourceGetOptions                  (word3,&number);
  
/* set up */
  XmTextSetString(XWidgetGetIdentifier("ResEd_value"),value);

       if( (word3!=NULL) && 
	   ( (strcmp(word3,XtRPixel)==0) || (strcmp(word3,"OColorIndex")==0) )
         )
    { 
      XmTextSetString              (XWidgetGetIdentifier("ResEd_colormap_widget_name")  ,XtName(This));
      XmTextSetString              (XWidgetGetIdentifier("ResEd_colormap_resource")  ,word1);
      if(OWidgetHasResource(This,XoNvirtualColormap)==1)
	{
	  char*                        string;
	  string                       = OWidgetGetAndConvertResourceToString (This,XoNvirtualColormap);
	  OWidgetSetResourceFromString (XWidgetGetIdentifier("ResEd_colormap"),XoNvirtualColormap   ,string,False);
	  CStringDelete                (string);
	}
      else
	{
	  OWidgetSetResourceFromString (XWidgetGetIdentifier("ResEd_colormap"),XoNvirtualColormap,"ocolormap_X",False);
	}
      OWidgetSetResourceFromString (XWidgetGetIdentifier("ResEd_colormap"),XoNselectedColor,value ,False);
      XtManageChild                (XWidgetGetIdentifier("ResEd_colormap_dialog"));
      XWidgetRaise                 (XWidgetGetIdentifier("ResEd_colormap_dialog_popup"));
      list                         = NULL;
/*Could give color directly.*/
      XWidgetSetSensitive  (XWidgetGetIdentifier("ResEd_to_modify"),True);
      XWidgetSetSensitive  (XWidgetGetIdentifier("ResEd_apply"),True);
      XmTextSetEditable    (XWidgetGetIdentifier("ResEd_value"),True);
    }
  else if( (OResourceIsFont(word3,NULL)==1)     || 
	   (OResourceIsFontList(word3,NULL)==1) || 
	   (OResourceIsFontStruct(word3,NULL)==1) 
	  )
    {
      XtManageChild         (XWidgetGetIdentifier("ResEd_font"));
      XWidgetRaise          (XWidgetGetIdentifier("ResEd_font_popup"));

      XWidgetSetSensitive  (XWidgetGetIdentifier("ResEd_to_modify"),False);
      XWidgetSetSensitive  (XWidgetGetIdentifier("ResEd_apply"),False);
      XmTextSetEditable    (XWidgetGetIdentifier("ResEd_value"),False);
    }
  else if( (word3!=NULL) && 
	   ( (strcmp(word3,XtRFile)==0) || (OResourceIsPixmap(word3,NULL)==1) )
	  )
    {
      XtManageChild         (XWidgetGetIdentifier("ResEd_file"));
      XWidgetRaise          (XWidgetGetIdentifier("ResEd_file_popup"));

      XWidgetSetSensitive  (XWidgetGetIdentifier("ResEd_to_modify"),False);
      XWidgetSetSensitive  (XWidgetGetIdentifier("ResEd_apply"),False);
      XmTextSetEditable    (XWidgetGetIdentifier("ResEd_value"),False);
    }
  else if(OResourceIsMultiLine(word3,word1)==1)
    { 
      XmTextSetString       (XWidgetGetIdentifier("ResEd_edit_widget_name"),XtName(This));
      XmTextSetString       (XWidgetGetIdentifier("ResEd_edit_resource"),word1);
      XmTextSetString       (XWidgetGetIdentifier("ResEd_edit"),value);
      XtManageChild         (XWidgetGetIdentifier("ResEd_edit_dialog"));
      XWidgetRaise          (XWidgetGetIdentifier("ResEd_edit_dialog_popup"));

      XWidgetSetSensitive  (XWidgetGetIdentifier("ResEd_to_modify"),False);
      XWidgetSetSensitive  (XWidgetGetIdentifier("ResEd_apply"),False);
      XmTextSetEditable    (XWidgetGetIdentifier("ResEd_value"),False);
    }
  else if(number!=0)
    {
      XmOptionInitialize    (XWidgetGetIdentifier("ResEd_options"),number,list,value);

      XWidgetSetSensitive   (XWidgetGetIdentifier("ResEd_to_modify"),True);
      XWidgetSetSensitive   (XWidgetGetIdentifier("ResEd_apply"),True);
      XmTextSetEditable     (XWidgetGetIdentifier("ResEd_value"),False);
    }  
  else
    {
      XWidgetSetSensitive  (XWidgetGetIdentifier("ResEd_to_modify"),True);
      XWidgetSetSensitive  (XWidgetGetIdentifier("ResEd_apply"),True);
      XmTextSetEditable    (XWidgetGetIdentifier("ResEd_value"),True);
    }  

  CStringDelete   (value);
  CStringDelete   (word1);
  CStringDelete   (word2);
  CStringDelete   (word3);
}
/***************************************************************************/
static void AddModification (
  Widget This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*   selection;
  char*   word1;
  char*   word2;
  char*   word3;
  char*   old;
  char*   value  = NULL;
  int     number = 0;
/*.........................................................................*/
  if(This==NULL) return;

  selection     = OWidgetGetAndConvertResourceToString         (XWidgetGetIdentifier("ResEd_list"),"selectedItems");
  if( (selection==NULL) || (*selection=='\0') )                {CStringDelete(selection);return;}
  if( (selection!=NULL) && (strncmp(selection,"-----",5)==0) ) {CStringDelete(selection);return;}

  word1  = CStringGetFirstWord  (selection);
  word2  = CStringGetSecondWord (selection);
  word3  = CStringGetThirdWord  (selection);

  CStringDelete (selection);

  old       = OWidgetGetAndConvertResourceToString      (This,word1);
  OResourceGetOptions (word3,&number);
  
       if( (word3!=NULL) && 
	   ( (strcmp(word3,XtRPixel)==0) || (strcmp(word3,"OColorIndex")==0) )
	   )
    { 
      value = OWidgetGetAndConvertResourceToString (XWidgetGetIdentifier("ResEd_colormap"),XoNselectedColor);
      if( (value==NULL) || (strcmp(value,"")==0)  )
	{ 
	  CStringDelete (value);
          value = XmTextGetString (XWidgetGetIdentifier("ResEd_value"));
        }
    }
  else if(number!=0)
    { 
      value = XmOptionGetOption  (XWidgetGetIdentifier("ResEd_options"));
    }
  else if(OResourceIsMultiLine(word3,word1)==1)
    {
      value = XmTextGetString (XWidgetGetIdentifier("ResEd_edit"));
    }
  else
    {
      value = XmTextGetString (XWidgetGetIdentifier("ResEd_value"));
    }
/*CPrinterPutF("debug:%s %s\n",name,value);*/
  if(CStringCompare(value,old)==0)
    {
      if(Class.widget==NULL) Class.widget = This;
      CTextAddLine (&(Class.namen) ,&(Class.names) ,word1);
      CTextAddLine (&(Class.valuen),&(Class.values),value);
    }

  XtFree          (value);

  CStringDelete   (old);
  CStringDelete   (word1);
  CStringDelete   (word2);
  CStringDelete   (word3);
}
/***************************************************************************/
static void ApplyModification (
  Widget This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
/*.........................................................................*/
  if(Class.namen==0)  AddModification (This);
  if(Class.widget==NULL) return;
  XResourceChangesInitialize    ();
  for(count=0;count<Class.namen;count++)
    {
      OWidgetSetResourceFromString (Class.widget,Class.names[count],Class.values[count],True);
    }
  XResourceChangesCommit       ();
  if(Class.namen==1)  XmTextSetString(XWidgetGetIdentifier("ResEd_value"),Class.values[0]);
  ClearResourceEditor         ();
}
/***************************************************************************/
static void ClearResourceEditor (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  Class.widget   = NULL;
  CTextDelete      (Class.namen ,Class.names); 
  CTextDelete      (Class.valuen,Class.values); 
  Class.names    = NULL;
  Class.namen    = 0;
  Class.values   = NULL;
  Class.valuen   = 0;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static void UpdateAttachmentPanel (
 Widget This
,char* a_side
) 
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget parent;
  Widget w17;
  char   str[17];
  char*  mode;
/*.........................................................................*/
  if(This==NULL) return;
  parent      = XtParent(This);
  if(parent==NULL) return;
  if(!XtIsSubclass(parent,xmFormWidgetClass))
    { 
      CPrinterPutF         ("widget %s not in an XmForm",XtName (This));
      return;
    }
  sprintf    (str,"%sAttachment",a_side);
  mode       = OWidgetGetAndConvertResourceToString   (This,str);
  if(mode==NULL)  return;
  w17        = XWidgetGetIdentifier("Attach_17");
       if(strcmp(mode,"attach_none")==0)
    XWidgetSetWidgetResource (w17,XmNmenuHistory,XWidgetGetIdentifier("Attach_23"));
  else if(strcmp(mode,"attach_self")==0)
    XWidgetSetWidgetResource (w17,XmNmenuHistory,XWidgetGetIdentifier("Attach_29"));
  else if(strcmp(mode,"attach_form")==0)
    XWidgetSetWidgetResource (w17,XmNmenuHistory,XWidgetGetIdentifier("Attach_24"));
  else if(strcmp(mode,"attach_opposite_form")==0)
    XWidgetSetWidgetResource (w17,XmNmenuHistory,XWidgetGetIdentifier("Attach_27"));
  else if(strcmp(mode,"attach_widget")==0)
    XWidgetSetWidgetResource (w17,XmNmenuHistory,XWidgetGetIdentifier("Attach_25"));
  else if(strcmp(mode,"attach_opposite_widget")==0)
    XWidgetSetWidgetResource (w17,XmNmenuHistory,XWidgetGetIdentifier("Attach_28"));
  else if(strcmp(mode,"attach_position")==0)
    XWidgetSetWidgetResource (w17,XmNmenuHistory,XWidgetGetIdentifier("Attach_26"));

  if( (strcmp(mode,"attach_widget")==0)            ||
      (strcmp(mode,"attach_opposite_widget")==0)
     )
    { Widget          wglue;
      sprintf         (str,"%sWidget",a_side);
      wglue           = XWidgetGetWidgetResource (This,str);
      if(wglue!=NULL) XmTextSetString       (XWidgetGetIdentifier("Attach_34"),XtName(wglue));
      else            XmTextSetString       (XWidgetGetIdentifier("Attach_34"),"");
    }
  else
    {
      XmTextSetString (XWidgetGetIdentifier("Attach_34"),"");
    }
  if(strcmp(mode,"attach_position")==0)
    { int                  pos;
      sprintf              (str,"%sPosition",a_side);
      pos                  = XWidgetGetIntegerResource (This,str);
      sprintf              (str,"%d",pos);
      XmTextSetString      (XWidgetGetIdentifier("Attach_35"),str);
    }
  else
    {
      XmTextSetString      (XWidgetGetIdentifier("Attach_35"),"");
    }
 {int offset;
  sprintf              (str,"%sOffset",a_side);
  offset               = XWidgetGetIntegerResource (This,str);
  sprintf              (str,"%d",offset);
  XmTextSetString      (XWidgetGetIdentifier("Attach_44"),str);}
  CStringDelete        (mode);
}
/***************************************************************************/
static void ApplyAttachment (
  Widget This
) 
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget  glued_to;
  char*   mode;
  char*   side;
  int     pos,offset;
/*.........................................................................*/
  if(This==NULL) return;
  mode      = XmOptionGetOption    (XWidgetGetIdentifier("Attach_17"));
  if(mode==NULL) return;
  side      = XmOptionGetOption    (XWidgetGetIdentifier("Attach_12"));
  glued_to  = XWidgetGetIdentifier (XmTextGetValue(XWidgetGetIdentifier("Attach_34")));
  pos       = XmTextConvertValueToInteger       (XWidgetGetIdentifier("Attach_35"));
  offset    = XmTextConvertValueToInteger       (XWidgetGetIdentifier("Attach_44"));
       if(strcmp(mode,"attach_none")==0)            
    XmSetSideAttachment (This,side,mode,offset,NULL,OWidgetMarkResourceToBeSaved);
  else if(strcmp(mode,"attach_self")==0)            
    XmSetSideAttachment (This,side,mode,offset,NULL,OWidgetMarkResourceToBeSaved);
  else if(strcmp(mode,"attach_form")==0)            
    XmSetSideAttachment (This,side,mode,offset,NULL,OWidgetMarkResourceToBeSaved);
  else if(strcmp(mode,"attach_opposite_form")==0)   
    XmSetSideAttachment (This,side,mode,offset,NULL,OWidgetMarkResourceToBeSaved);
  else if(strcmp(mode,"attach_widget")==0)          
    XmSetSideAttachment (This,side,mode,offset,&glued_to,OWidgetMarkResourceToBeSaved);
  else if(strcmp(mode,"attach_opposite_widget")==0) 
    XmSetSideAttachment (This,side,mode,offset,&glued_to,OWidgetMarkResourceToBeSaved);
  else if(strcmp(mode,"attach_position")==0)        
    XmSetSideAttachment (This,side,mode,offset,&pos,OWidgetMarkResourceToBeSaved);
  XtFree   (side);
  XtFree   (mode);
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static Widget CreateRowColumn (
 Widget a_parent
,char* a_type
,int a_rows
,int a_cols
) 
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget rc;
  int    nwidget,count;
/*.........................................................................*/
  if(a_parent==NULL)               return NULL;
  if( (a_rows<=0) || (a_cols<=0) ) return NULL;
  if(OClassGetIdentifier(a_type)==NULL) return NULL;

  rc                              = OWidgetCreate (NULL,a_parent,"XmRowColumn",True);
  XResourceChangesInitialize      ();
  OWidgetSetResourceFromString    (rc,XmNpacking,"pack_column",True);
  OWidgetSetResourceFromInteger   (rc,XmNnumColumns,a_cols,True);
  XResourceChangesCommit          ();

  nwidget               = a_rows * a_cols;
  for(count=0;count<nwidget;count++) OWidgetCreate(NULL,rc,a_type,True); 

  return rc;
}
/***************************************************************************/
static Widget CreateForm (
 Widget a_parent
,char* a_type
,int a_rows
,int a_cols
) 
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int    row,col;
  Widget wform;
  int    width,height;
  int    top, bottom;
  int    wsep = 4;
/*.........................................................................*/
  if(a_parent==NULL)               return NULL;
  if( (a_rows<=0) || (a_cols<=0) ) return NULL;
  if(OClassGetIdentifier(a_type)==NULL) return NULL;

  wform                = OWidgetCreate(NULL,a_parent,"XmForm",True);
  XResourceChangesInitialize       ();
  OWidgetSetResourceFromString (wform, XmNfractionBase     , "1000" , True);
  OWidgetSetResourceFromString (wform, XtNborderWidth      , "3"    , True);
  OWidgetSetResourceFromString (wform, XtNborderColor      , "grey" , True);
  XResourceChangesCommit        ();
  height               = (1000 / a_rows) - wsep;
  width                = (1000 / a_cols) - wsep;
  top    = 1;
  bottom = height;
  for (row=0;row<a_rows;row++)
    { 
      int    left, right;
      left  = 1;
      right = width;
      for (col=0;col<a_cols;col++)
	{ 
	  Widget                child;
	  child                 = OWidgetCreate(NULL,wform,a_type,True);
	  XmAttachWidgetAtPositionAndMark   (child , top, bottom, left, right,OWidgetMarkResourceToBeSaved);
	  left                 += width + wsep ;
	  right                += width + wsep ;
	}
      top    += height + wsep ;
      bottom += height + wsep ;
    }
  return wform;
}
/***************************************************************************/
static Widget CreateRowsOfLabelTextScale (
 Widget a_parent
,int a_rows
) 
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int    count;
  Widget wform;
  int    height,top,bottom,slabel,sscale;
/*.........................................................................*/
  if(a_parent==NULL) return NULL;
  if(a_rows==0)   return NULL;

  wform  = OWidgetCreate(NULL,a_parent,"XmForm",True);
  height = 99 / a_rows;
  slabel = 20;
  sscale = 50;
  top    = 1;
  bottom = height;
  for(count=0;count<a_rows;count++)
    { 
      Widget wlabel,wtext,wscale;
      wlabel  = OWidgetCreate (NULL,wform,"XmLabel",True);
      wtext   = OWidgetCreate (NULL,wform,"XmText" ,True);
      wscale  = OWidgetCreate (NULL,wform,"XmScale",True);
      XmAttachWidgetAtPositionAndMark  (wlabel , top, bottom, 1, slabel,OWidgetMarkResourceToBeSaved);
      XmAttachWidgetAtPositionAndMark  (wtext  , top, bottom, slabel+1, sscale,OWidgetMarkResourceToBeSaved);
      XmAttachWidgetAtPositionAndMark  (wscale , top, bottom, sscale+1, 99,OWidgetMarkResourceToBeSaved);
      OWidgetSetResourceFromString(wscale ,XtNorientation,"horizontal",True);
      OWidgetSetResourceFromString(wlabel ,XtNbackground ,"grey",True);
      top    += height;
      bottom += height;
    }

  return wform;
}
/***************************************************************************/
static Widget CreateRowsOfLabelText (
 Widget a_parent
,int a_rows
) 
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int    count;
  Widget wform;
  int    height,top,bottom,slabel;
/*.........................................................................*/
  if(a_parent==NULL) return NULL;
  if(a_rows==0)   return NULL;

  wform  = OWidgetCreate(NULL,a_parent,"XmForm",True);
  height = 99 / a_rows;
  slabel = 20;
  top    = 1;
  bottom = height;
  for(count=0;count<a_rows;count++)
    { 
      Widget wlabel,wtext;
      wlabel  = OWidgetCreate (NULL,wform,"XmLabel",True);
      wtext   = OWidgetCreate (NULL,wform,"XmText" ,True);
      XmAttachWidgetAtPositionAndMark  (wlabel, top, bottom, 1, slabel,OWidgetMarkResourceToBeSaved);
      XmAttachWidgetAtPositionAndMark  (wtext , top, bottom, slabel+1, 99,OWidgetMarkResourceToBeSaved);
      OWidgetSetResourceFromString(wlabel ,XtNbackground ,"grey",True);
      top    += height;
      bottom += height;
    }

  return wform;
}




#else /*HAS_XM*/
#include <stddef.h>
#endif /*HAS_XM*/
