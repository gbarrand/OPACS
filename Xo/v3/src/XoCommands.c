/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#define HAS_XOPLOTTER
#define HAS_XOMOSAIC

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <CPrinter.h>
#include <CFile.h>
#include <CMemory.h>
#include <CString.h>
#include <CGIF.h>
#include <CText.h>
#include <OType.h>
#include <OShell.h>

#include <XWidget.h>

#include <OImage.h>
#include <Xo.h>
#include <XoTypes.h>
#include <XoCamera.h>
#include <XoHelp.h>

#ifdef HAS_XOMOSAIC
#include <XoMosaic.h>
#endif /*HAS_XOMOSAIC*/

#ifdef HAS_XOPLOTTER
#include <XoPlotter.h>
#endif /*HAS_XOPLOTTER*/

#ifdef HAS_HTML
#include <mowww.h>
#endif /*HAS_HTML*/

#include <XoCommands.h>
/***************************************************************************/
void XoAddCommands (
 OShell a_osh
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(OCommandGetIdentifier("thisCamera")!=NULL) return; /*done*/

  XoSetTypes();

  OShellAddNewCommand (a_osh,"Xo/targetCamera"       ,XoExecute_targetCamera);
  OShellAddNewCommand (a_osh,"Xo/thisCamera"         ,XoExecute_thisCamera);
  OShellAddNewCommand (a_osh,"Xo/getWidgetCamera"    ,XoExecute_getWidgetCamera);

  OShellAddNewCommand (a_osh,"Xo/showTopic"          ,XoExecute_showTopic);
  OShellAddNewCommand (a_osh,"Xo/createRotatedImage" ,XoExecute_createRotatedImage);
  OShellAddNewCommand (a_osh,"Xo/saveImage"          ,XoExecute_saveImage);

  OShellAddNewCommand (a_osh,"Xo/cancelGoRefresh"    ,XoExecute_cancelGoRefresh);
  OShellAddNewCommand (a_osh,"Xo/saveContent"        ,XoExecute_saveContent);

#ifdef HAS_XOPLOTTER
  OShellAddNewCommand (a_osh,"Xo/getWidgetPlotter"   ,XoExecute_getWidgetPlotter);
  OShellAddNewCommand (a_osh,"Xo/plotCurve"          ,XoExecute_plotCurve);
  OShellAddNewCommand (a_osh,"Xo/magnifyPlot"        ,XoExecute_magnifyPlot);
  OShellAddNewCommand (a_osh,"Xo/setPlotLayout"      ,XoExecute_setPlotLayout);
  OShellAddNewCommand (a_osh,"Xo/assignToPlotters"   ,XoExecute_assignToPlotters);
  OShellAddNewCommand (a_osh,"Xo/attachToPlotters"   ,XoExecute_attachToPlotters);
  OShellAddNewCommand (a_osh,"Xo/updatePlot"         ,XoExecute_updatePlot);
#endif /*HAS_XOPLOTTER*/

#ifdef HAS_HTML
  OShellAddNewCommand (a_osh,"Xo/getWWW"             ,XoExecute_getWWW);
#endif /*HAS_HTML*/
}
/***************************************************************************/
int XoExecute_targetCamera (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> targetCamera
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget  widget;
  void*   camera;
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

  if( (widget!=NULL) && !XtIsSubclass(widget,xoCameraWidgetClass) ) 
    {
      CWarnF ("Widget %s is not an XoCamera in :\n%s\n",XtName(widget),OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return EXIT_FAILURE;
    }

  camera = XoCameraGetCamera (widget);
  if(camera==NULL)
    {
      CWarnF ("No OCamera associated to %s in :\n%s\n",XtName(widget),OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return EXIT_FAILURE;
    }
  
  OProcessPutString (a_process,CStringCreateF(128,"OCamera/%lu",camera));

  a_args            = NULL;
  return            EXIT_SUCCESS;
}
/***************************************************************************/
int XoExecute_thisCamera (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> thisCamera
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget  widget;
  void*   camera;
/*.........................................................................*/
  if(a_argn!=1) 
    {
      CWarnF("Zero argument expected in :\n%s\n",OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return EXIT_FAILURE;
    }

  widget = XWidgetGetThisWidget();
  if(widget==NULL)
    {
      OProcessPutHandles (a_process,NULL);
      return       EXIT_FAILURE;
    }

  if( (widget!=NULL) && !XtIsSubclass(widget,xoCameraWidgetClass) ) 
    {
      CWarnF ("Widget %s is not an XoCamera in :\n%s\n",XtName(widget),OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return EXIT_FAILURE;
    }

  camera = XoCameraGetCamera (widget);
  if(camera==NULL)
    {
      CWarnF ("No OCamera associated to %s in :\n%s\n",XtName(widget),OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return EXIT_FAILURE;
    }
  
  OProcessPutString (a_process,CStringCreateF(128,"OCamera/%lu",camera));

  a_args            = NULL;
  return            EXIT_SUCCESS;
}
/***************************************************************************/
int XoExecute_getWidgetCamera (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> getWidgetCamera <string:Widget>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget  widget;
  void*   camera;
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

  if( (widget!=NULL) && !XtIsSubclass(widget,xoCameraWidgetClass) ) 
    {
      CWarnF ("Widget %s is not an XoCamera in :\n%s\n",a_args[1],OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return EXIT_FAILURE;
    }

  camera = XoCameraGetCamera (widget);
  if(camera==NULL)
    {
      CWarnF ("No OCamera associated to %s in :\n%s\n",a_args[1],OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return EXIT_FAILURE;
    }
  
  OProcessPutString (a_process,CStringCreateF(128,"OCamera/%lu",camera));

  a_args            = NULL;
  return            EXIT_SUCCESS;
}
/***************************************************************************/
int XoExecute_showTopic (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> showTopic <string:Widget> <string:topic> 
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget widget;
  char*  topic;
/*.........................................................................*/
  if(a_argn!=3)
    {
      CWarnF  ("Two arguments expected in :\n%s\n",OProcessGetCommand(a_process));
      return  EXIT_FAILURE;
    }

  widget = XWidgetGetIdentifier (a_args[1]);
  if(widget==NULL)
    {
      CWarnF ("Unknown widget %s in :\n%s\n",a_args[1],OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  topic  = OProcessGetString(a_process,a_args[2]);

       if(XtIsSubclass(widget,xoHelpWidgetClass)) 
    {
      XoHelpShowTopic   (widget,topic);
    }
#ifdef HAS_XOMOSAIC
  else if(XtIsSubclass(widget,xoMosaicWidgetClass)) 
    {
      XoMosaicShowTopic (widget,topic);
    }
#endif /*HAS_XOMOSAIC*/
  else
    {
      CWarnF ("Widget %s is not an XoMosaic or an XoHelp widget in :\n%s\n",a_args[1],OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  return        EXIT_SUCCESS;
}
/***************************************************************************/
int XoExecute_saveContent (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> saveContent <string:Widget> <string:file>
  osh> saveContent <string:Widget> <string:file> [plain_text, formatted_text, postscript] [times, large_helvetica, newcentury, lucidabright]
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget widget;
/*.........................................................................*/
  if( (a_argn!=3) && (a_argn!=5) )
    {
      CWarnF  ("Two or three arguments expected in :\n%s\n",OProcessGetCommand(a_process));
      return  EXIT_FAILURE;
    }

  widget = XWidgetGetIdentifier (a_args[1]);
  if(widget==NULL)
    {
      CWarnF ("Unknown widget %s in :\n%s\n",a_args[1],OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

#ifdef HAS_XOMOSAIC
  if(XtIsSubclass(widget,xoMosaicWidgetClass)) 
    {
      if(a_argn==3)
	XoMosaicSave (widget,a_args[2],"postscript","times");
      else
	XoMosaicSave (widget,a_args[2],a_args[3],a_args[4]);
      return EXIT_SUCCESS;
    }
  else
    {
      CWarnF ("Widget %s is not an XoMosaic widget in :\n%s\n",a_args[1],OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }
#else  /*HAS_XOMOSAIC*/
  CWarnF ("XoMosaic widget unknown for :\n%s\n",OProcessGetCommand(a_process));
  return EXIT_FAILURE;
#endif /*HAS_XOMOSAIC*/
}
/***************************************************************************/
int XoExecute_createRotatedImage (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> createRotatedImage <string:name> <string:OImage>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OImage oimage;
/*.........................................................................*/
  if(a_argn!=3) 
    {
      CWarnF  ("Two arguments expected in :\n%s\n",OProcessGetCommand(a_process));
      return  EXIT_FAILURE;
    }

  oimage = OImageGetIdentifier (a_args[2]);
  if(oimage==NULL)
    {
      CWarnF ("Unknown OImage %s in :\n%s\n",a_args[2],OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  OImageCreateRotated (OImageGetDisplay(oimage),a_args[1],oimage);

  return              EXIT_SUCCESS;
}
/***************************************************************************/
int XoExecute_saveImage (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> saveImage <string:OImage> <string:file name>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OImage oimage;
  FILE*  file;
/*.........................................................................*/
  if(a_argn!=3)
    {
      CWarnF  ("Two arguments expected in :\n%s\n",OProcessGetCommand(a_process));
      return  EXIT_FAILURE;
    }

  oimage = OImageGetIdentifier (a_args[1]);
  if(oimage==NULL)
    {
      CWarnF ("Unknown OImage %s in :\n%s\n",a_args[1],OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  file     = CFileOpenForWriting (a_args[2]);
  if(file==NULL) 
    {
      CWarnF ("Can't open file %s in :\n%s\n",a_args[2],OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }
  else
    {
      unsigned int   width;
      unsigned int   height;
      unsigned char* data;
      int            colorn;
      double*        reds;
      double*        greens;
      double*        blues;
      data                  = (unsigned char*)OImageGetSizeAndBytes (oimage,&width,&height);
      OImageGetRGBs         (oimage,&colorn,&reds,&greens,&blues);
      CGIF_PutBytesInStream (file,data,width,height,colorn,reds,greens,blues);
      fclose                (file);
    }

  return        EXIT_SUCCESS;
}
/***************************************************************************/
int XoExecute_cancelGoRefresh (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> cancelGoRefresh
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  XoCameraCancelGoRefresh ();
  a_argn    = 0;
  a_args    = NULL;
  a_process = NULL;
  return    EXIT_SUCCESS;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
#ifdef HAS_XOPLOTTER
/***************************************************************************/
int XoExecute_getWidgetPlotter (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> getWidgetPlotter <string:Widget>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget  widget;
  void*   plotter;
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

  if( (widget!=NULL) && !XtIsSubclass(widget,xoPlotterWidgetClass) ) 
    {
      CWarnF ("Widget %s is not an XoPlotter in :\n%s\n",a_args[1],OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return EXIT_FAILURE;
    }

  plotter = XoPlotterGetPlotter (widget);
  if(plotter==NULL)
    {
      CWarnF ("No OPlotter associated to %s in :\n%s\n",a_args[1],OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return EXIT_FAILURE;
    }
  
  OProcessPutString (a_process,CStringCreateF(128,"OPlotter/%lu",plotter));

  a_args            = NULL;
  return            EXIT_SUCCESS;
}
/***************************************************************************/
int XoExecute_plotCurve (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> plotCurve <string:Widget> {<x> <y>}
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget    widget;
  int       pointn,valn;
  double*   xs;
  double*   ys;
  int       argi;
/*.........................................................................*/
  if(a_argn<4) 
    {
      CWarnF              ("More than two arguments expected in :\n%s\n",OProcessGetCommand(a_process));
      return              EXIT_FAILURE;
    }

  widget = XWidgetGetIdentifier (a_args[1]);
  if(widget==NULL)
    {
      CWarnF ("Unknown widget %s in :\n%s\n",a_args[1],OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  if( (widget!=NULL) && !XtIsSubclass(widget,xoPlotterWidgetClass) ) 
    {
      CWarnF ("Widget %s is not an XoPlotter in :\n%s\n",a_args[1],OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  valn   = a_argn-2;
  pointn = valn/2;
  if(2*pointn!=valn) 
    {
      CWarnF("An even number of values is requested (here %d) in :\n%s\n",valn,OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  xs     = (double*)CMemoryAllocateBlock (pointn * sizeof(double));
  ys     = (double*)CMemoryAllocateBlock (pointn * sizeof(double));
  argi   = 2;
  if( (xs!=NULL) && (ys!=NULL) )
    {
       int count;
      for(count=0;count<pointn;count++)
	{
	  int        status;
	  xs[count]  = CStringConvertToDouble(a_args[argi],&status);
	  if(status==0) 
	    {
	      CWarnF           ("Argument %s not a number in :\n%s\n",a_args[argi],OProcessGetCommand(a_process));
	      CMemoryFreeBlock (xs);
	      CMemoryFreeBlock (ys);
	      return           EXIT_FAILURE;
	    }
	  argi++;
	  ys[count]  = CStringConvertToDouble(a_args[argi],&status);
	  if(status==0) 
	    {
	      CWarnF           ("Argument %s not a number in :\n%s\n",a_args[argi],OProcessGetCommand(a_process));
	      CMemoryFreeBlock (xs);
	      CMemoryFreeBlock (ys);
	      return           EXIT_FAILURE;
	    }
	  argi++;
	  /*
	  printf ("debug:%d:%g %g\n",count,xs[count],ys[count]);
	  */
	}
      XoPlotCurve   (widget,pointn,xs,ys);
    }
  CMemoryFreeBlock     (xs);
  CMemoryFreeBlock     (ys);

  return        EXIT_SUCCESS;
}
/***************************************************************************/
int XoExecute_magnifyPlot (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> magnifyPlot <Widget>
  osh> magnifyPlot <Widget> back
  osh> magnifyPlot <Widget> reset
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget    widget;
/*.........................................................................*/
  if( (a_argn!=2) && (a_argn!=3)) 
    {
      CWarnF              ("One or two arguments expected in :\n%s\n",OProcessGetCommand(a_process));
      return              EXIT_FAILURE;
    }

  widget = XWidgetGetIdentifier (a_args[1]);
  if(widget==NULL)
    {
      CWarnF ("Unknown widget %s in :\n%s\n",a_args[1],OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  if( (widget!=NULL) && !XtIsSubclass(widget,xoPlotterWidgetClass) ) 
    {
      CWarnF ("Widget %s is not an XoPlotter in :\n%s\n",a_args[1],OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  if(a_argn==2)  
    {
      XoPlotterMagnifyCollectedBins (widget);
    }
  else if(strcmp(a_args[2],"back")==0)
    {
      XoPlotterUndoMagnification (widget);
    }
  else if(strcmp(a_args[2],"reset")==0)
    {
      XoPlotterRestoreFirstMagnification (widget);
    }
  else
    {
      CWarnF ("Bad option %s (back, reset expected) in :\n%s\n",a_args[2],OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  return        EXIT_SUCCESS;
}
/***************************************************************************/
int XoExecute_setPlotLayout (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> setPlotLayout <string:Widget>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget    widget;
/*.........................................................................*/
  if(a_argn!=2)
    {
      CWarnF  ("One argument expected in :\n%s\n",OProcessGetCommand(a_process));
      return  EXIT_FAILURE;
    }

  widget      = XWidgetGetIdentifier (a_args[1]);
  if(widget==NULL)
    {
      CWarnF ("Unknown widget %s in :\n%s\n",a_args[1],OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  if( (widget!=NULL) && !XtIsSubclass(widget,xoPlotterWidgetClass) ) 
    {
      CWarnF ("Widget %s is not an XoPlotter in :\n%s\n",a_args[1],OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  XoPlotterSetLayout   (widget);

  return        EXIT_SUCCESS;
}
/***************************************************************************/
int XoExecute_assignToPlotters (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> assignToPlotters - <string:Widget> 
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget widget;
  int    linen = 0;
  char** lines = NULL;
/*.........................................................................*/
  if(a_argn!=3)
    {
      CWarnF              ("Two or four arguments expected in :\n%s\n",OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return              EXIT_FAILURE;
    }

  widget = XWidgetGetIdentifier (a_args[2]);
  if(widget==NULL)
    {
      CWarnF              ("Unknown widget %s in :\n%s\n",a_args[2],OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return              EXIT_FAILURE;
    }

  if(OProcessPreparePipe(a_process,a_args[1])!=0) 
    {
      OHandle*  hands;
      OHandle*  handa;
      hands  = OProcessGetHandles    (a_process);
      for(handa=hands;(handa!=NULL)&&(*handa!=NULL);handa++) 
	{ 
	  OType       otype;
	  OIdentifier oid;
	  otype       = (OType)OHandleGetType(*handa);
	  oid         = (OIdentifier)OHandleGetIdentifier(*handa);
	  if(OTypeIsObjectValid (otype,oid)==1)
	    {
	      char* name;
	      name  = OTypeGetObjectName (otype,oid);
	      if(name!=NULL)
		{
		  CTextAddLine (&linen,&lines,name);
		}
	    }
	}
    }
  
  XoAttachObjectsToGridOfPlotters (widget,linen,lines);

  CTextDelete         (linen,lines);

  OProcessPutHandles (a_process,NULL);

  return              EXIT_SUCCESS;
}
/***************************************************************************/
int XoExecute_attachToPlotters (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> attachToPlotters <list of strings> <string:Widget> 
  osh> attachToPlotters -                 <string:Widget> 
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget widget;
  int    linen = 0;
  char** lines = NULL;
/*.........................................................................*/
  if(a_argn!=3)
    {
      CWarnF              ("Two or four arguments expected in :\n%s\n",OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return              EXIT_FAILURE;
    }

  widget = XWidgetGetIdentifier (a_args[2]);
  if(widget==NULL)
    {
      CWarnF              ("Unknown widget %s in :\n%s\n",a_args[2],OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return              EXIT_FAILURE;
    }

  lines = CTextCreateFromString (OProcessGetString(a_process,a_args[1]),&linen);

  XoAttachObjectsToGridOfPlotters (widget,linen,lines);

  CTextDelete        (linen,lines); 

  OProcessPutHandles (a_process,NULL);

  return              EXIT_SUCCESS;
}
/***************************************************************************/
int XoExecute_updatePlot (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> updatePlot -
  osh> updatePlot <Widget>
  osh> updatePlot -        now
  osh> updatePlot <Widget> now
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget* widgets = NULL;
/*.........................................................................*/
  if( (a_argn!=2) && (a_argn!=3) )
    {
      CWarnF              ("One or two arguments expected in :\n%s\n",OProcessGetCommand(a_process));
      return              EXIT_FAILURE;
    }

  widgets = (Widget*)OProcessGetTypedObjects (a_process,a_args[1],"Widget");

 {Widget*  itema;
  for(itema = widgets;(itema!=NULL) && (*itema!=NULL);itema++) {
    Widget widget = *itema;
    if(widget==NULL) {
    } else if(!XtIsSubclass(widget,xoPlotterWidgetClass)) {
      CWarnF ("Widget %s is not an XoPlotter in :\n%s\n",XtName(widget),OProcessGetCommand(a_process));
    } else {
      if(a_argn==2)
	XoPlotterRequireRepresentation (widget);
      else
	XoPlotterUpdateNow             (widget);
    }
  }}

  CListDelete         ((CList)widgets);

  OProcessPutHandles  (a_process,NULL);

  return              EXIT_SUCCESS;
}
#endif /*HAS_XOPLOTTER*/
#ifdef HAS_HTML
/***************************************************************************/
int XoExecute_getWWW (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> getWWW <string:url>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* text;
  char* head;
/*.........................................................................*/
  if(a_argn!=2)
    {
      CWarnF              ("One arguments expected in :\n%s\n",OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return EXIT_FAILURE;
    }
  
  text = mo_pull_er_over (a_args[1],&head);

  OProcessPutString (a_process,CStringDuplicate(text));

  if(head!=NULL) free(head);

  return        EXIT_SUCCESS;
}
#endif /*HAS_HTML*/

