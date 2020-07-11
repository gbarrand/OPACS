/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
static char what[] = "@(#)Xo v3";

#define HAS_XOPLOTTER

#include <string.h>
#include <stdio.h>
#include <math.h>
#ifndef M_PI
#define M_PI       3.1415926535897931160E0
#define M_PI_2     1.5707963267948965580E0  
#endif

#ifdef HAS_XM
#include <Xm/PushB.h>      
#include <Xm/List.h>      
#include <Xm/Form.h>      
#include <Xm/Label.h> 
#include <Xm/Frame.h> 
#include <Xm/DialogS.h> 
#endif /*HAS_XM*/

/*Co*/
#include <CString.h>
#include <CText.h>
#include <CMemory.h>
#include <CColor.h>
#include <CError.h>
#include <CVector.h>
#include <CPrinter.h>
#include <CList.h>  /*For XmFormBuildGrid*/

/*Go*/
#include <OCamera.h>
#include <Go.h>

/*Xx*/
#include <XWidget.h> /*For XmFormBuildGrid*/

/*Xo*/
#include <XoCamera.h>
#include <XoColormap.h>
#include <XoHelp.h>
#include <XoGraph.h>

#ifdef HAS_XOPLOTTER
#include <OScatter.h> /*For XmFormBuildGrid*/
#include <XoPlotter.h>
#else
#define XoNplottedObjects        "plottedObjects"
#endif /*HAS_XOPLOTTER*/

#include <Xo.h>

#ifdef __cplusplus
extern "C"{
#endif
static void HideCallback    (Widget,XtPointer,XtPointer);
static void ReportError     (char*,int);
static void Callback        (Widget,XtPointer,XtPointer);
static void UpdateFrame     (OCamera,OCamera);
static void UpdateRule      (OCamera,OCamera);

#ifdef HAS_XM
typedef Widget(*XmCreateWidgetFunction)(Widget,char*,ArgList,Cardinal);

static void XmSetCompoundStringResourceFromString (Widget,char*,char*);
static void XmFormBuildGrid                       (Widget,char*,XmCreateWidgetFunction,int,int);
static void XmAttachWidgetAtPosition              (Widget,int,int,int,int);
#endif /*HAS_XM*/
#ifdef __cplusplus
}
#endif

#define MINIMUM(a,b)            ((a)<(b)?a:b)
#define MAXIMUM(a,b)            ((a)>(b)?a:b)
#define AROUND(a_prec,a_around,a_number) if(fabs((a_number)-(a_around))<a_prec) {a_number = a_around;}
#define NotFound                (-1)
/***************************************************************************/
void XoInitializeClass (
 Display* a_display
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  XDisplayPutFileInResourceDatabase (a_display,"Xo.xrm");
}
/******************************************************************************/
void XoClearClass (
)
/******************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*............................................................................*/
  what[0] = '@'; /*c++ no warning.*/
}
/***************************************************************************/
Widget XoCreateFrame (
 Widget a_parent
,String a_name
,ArgList a_list
,Cardinal a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget  widget;
  OCamera ocamera;
  char*   name;
  ONode   onode;
/*.........................................................................*/
  if(a_parent==NULL) return NULL;
  if(a_name==NULL)   return NULL;  

  widget              = XtCreateManagedWidget(a_name,xoCameraWidgetClass,a_parent,a_list,a_number);

  XtAddCallback       (widget,XoNreceiveTransformCallback,Callback,(XtPointer)1);

  ocamera               = (OCamera)XoCameraGetCamera (widget);
  OContextInitialize    (OContextGetStaticInstance());
  OContextSetColorIndex (OContextGetStaticInstance(),OColorWhite);
  name                  = OCameraGetName(ocamera);
  if(name==NULL)
    onode               = ONodeCreateF (strlen(name)+6,"OCamera/%lu_frame",ocamera);
  else 
    onode               = ONodeCreateF (strlen(name)+6,"%s_frame",name);
  GoAddFrameToNode      (onode,1.);
  OCameraViewNode       (ocamera,onode);

  return                widget;
}
/***************************************************************************/
Widget XoCreateRule (
 Widget a_parent
,String a_name
,ArgList a_list
,Cardinal a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget widget;
  Arg    args[1];
/*.........................................................................*/
  if(a_parent==NULL) return NULL;
  if(a_name==NULL)   return NULL;  

  widget              = XtCreateManagedWidget(a_name,xoCameraWidgetClass,a_parent,a_list,a_number);

  XtSetArg            (args[0],XoNviewLimits,"0 1");
  XtSetValues         (widget,args,1);

  XtAddCallback       (widget,XoNreceiveTransformCallback,Callback,(XtPointer)2);

  return              widget;
}
/***************************************************************************/
static void Callback (
 Widget    This 
,XtPointer a_tag
,XtPointer a_data
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OCamera othis;
/*.........................................................................*/
  othis = (OCamera)XoCameraGetCamera (This);
       if((long)a_tag==1) 
    {
      Widget  sender;
      OCamera csender;
      sender  = (Widget) ((XoCameraCallbackStruct*)a_data)->widget;
      csender = (OCamera)XoCameraGetCamera(sender);
      if(csender!=NULL) UpdateFrame (othis,csender);
    }
  else if((long)a_tag==2) 
    {
      Widget  sender;
      OCamera csender;
      sender  = (Widget) ((XoCameraCallbackStruct*)a_data)->widget;
      csender = (OCamera)XoCameraGetCamera(sender);
      if(csender!=NULL) UpdateRule (othis,csender);
    }
}
/***************************************************************************/
static void UpdateFrame (
 OCamera a_camera    
,OCamera a_target    
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OMatrix  matrix,mctm,mscale;
  double   scale;
  double   vrpx,vrpy,vrpz;
  double   upx,upy,upz;
/*.........................................................................*/
  scale               = OCameraGetScale  (a_target);
  mctm                = OCameraGetMatrix (a_target);
  OMatrixRemoveTranslations (mctm);
  mscale              = scale<=0. ? OMatrixCreate (OMatrixIdentity) : OMatrixCreate (OMatrixScaleIsometric,1./scale);
  matrix              = OMatrixMultiply (mscale,mctm);
  OMatrixDelete       (mscale);
  OCameraGetVRP       (a_target,&vrpx,&vrpy,&vrpz);
  OCameraGetUpVector  (a_target,&upx,&upy,&upz);
  CVectorNormalize    (&vrpx,&vrpy,&vrpz);
  CVectorNormalize    (&upx,&upy,&upz);

  OCameraSetMatrix    (a_camera,matrix);
  OCameraSetVRP       (a_camera,vrpx,vrpy,vrpz);
  OCameraSetUpVector  (a_camera,upx,upy,upz);

  OMatrixDelete       (matrix);
}
/***************************************************************************/
static void UpdateRule (
 OCamera a_rule 
,OCamera a_target 
)
/***************************************************************************/
/* a_rule : camera that contain rule                                       */
/* a_target : camera to measure target                                     */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*     name;
  char*     string;
  int       dw,dh;
  int       horizontal;

  double    rule_height,rule_scale,rscale;
  int       fdigit,nbzero,nbtick;
  ONode*    list;
  char*     oname;
  ONode     onode;

  double    min,max;

  OMatrix   matrix,smatrix;

#define REP__RULE_FORMAT "%g"
/*.........................................................................*/
/* set rule camera */
  name           = OCameraGetName (a_rule);
  if(name==NULL) return;
  string         = CStringCreateF (strlen(name)+7,"%s_rule_*",name);
  list           = OCameraCollect (a_rule,OCollectMatchingName,string);
  CStringDelete  (string);
  oname          = (list!=NULL) && (list[0]!=NULL) ?  ONodeGetName   (list[0]) : CStringNULL;
  CListDelete    ((CList)list);

  rscale       = -1.;
  if( (oname==NULL) || (sscanf(oname+strlen(name)+1,"rule_%le",&rscale)!=1)) rscale = -1.;

/* set rule camera */
  rule_scale             = OCameraGetRuleScale (a_target,a_rule,&fdigit,&nbzero);
  if(fdigit==NotFound)   return;
  AROUND                 (1.e-3,rule_scale,rscale)
  if(rule_scale==rscale) return;

  OCameraGetSize     (a_rule,&dw,&dh);

  horizontal         = dw>dh ? 1 : 0; 

  rule_height        = (2./3.) * .9;
  min                = 0.;
  max                = fdigit;
  nbtick             = fdigit * 100 + 10;

  OContextInitialize    (OContextGetStaticInstance());
  OContextSetColorIndex (OContextGetStaticInstance(),OColorWhite);

  OCameraErase        (a_rule);

  onode               = ONodeCreateF (strlen(name)+6+64,"%s_rule_%g",name,rule_scale);

  if(horizontal==1)
    { 
      matrix             = OMatrixCreate(OMatrixScaleTranslation,rule_scale,1.,1.,0.,1.,0.);
      smatrix            = OMatrixCreate(OMatrixScale,.3,.3,1.);
      GoAddRuleToNode    (onode,0.,1./fdigit,min,max,nbtick,REP__RULE_FORMAT,-rule_height,matrix,smatrix);

      OMatrixDelete        (smatrix);
      OMatrixDelete        (matrix);
/* write units */
      if(nbzero!=0)
	{
	  smatrix            = OMatrixCreate(OMatrixScale,.29,.29,1.);
	  string             = CStringCreateF (64,"x 10e%d",nbzero);
	  GoAddStringToNode  (onode,string,.5 * dw/dh,0.1,0.,smatrix);
	  CStringDelete      (string);
	  OMatrixDelete      (smatrix);
	}
    }    
  else
    { 
      OMatrix            rotz,roty,scale;

      rotz               = OMatrixCreate(OMatrixRotationZ,M_PI_2);
      roty               = OMatrixCreate(OMatrixRotationY,M_PI);
      scale              = OMatrixCreate(OMatrixScaleTranslation,1.,rule_scale,1.,1.,0.,0.);
      matrix             = OMatrixMultiplyMatrices (3,scale,roty,rotz);

      smatrix            = OMatrixCreate(OMatrixScaleTranslation,.2,.3,1.,0.,0.,0.);
      GoAddRuleToNode    (onode,0.,1./fdigit,min,max,nbtick,REP__RULE_FORMAT,-rule_height,matrix,smatrix);


      OMatrixDelete        (smatrix);
      OMatrixDelete        (roty);
      OMatrixDelete        (rotz);
      OMatrixDelete        (scale);
      OMatrixDelete        (matrix);

/* write units */
      if(nbzero!=0)
	{
	  scale              = OMatrixCreate(OMatrixScale,.29,.29,1.);
	  rotz               = OMatrixCreate(OMatrixRotationZ,M_PI_2);
	  smatrix            = OMatrixMultiply     (rotz,scale);
	  string             = CStringCreateF (64,"x 10e%d",nbzero);
	  GoAddStringToNode  (onode,string,.3,.5 * dh/dw,0.,smatrix);
	  OMatrixDelete      (smatrix);
	  OMatrixDelete      (rotz);
	  OMatrixDelete      (scale);
	  CStringDelete      (string);
	}
    }

  OCameraViewNode    (a_rule,onode);

}
/***************************************************************************/
Widget XoCreateGraph (
 Widget a_parent
,String a_name
,ArgList a_list
,Cardinal a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return XtCreateManagedWidget(a_name,xoGraphWidgetClass,a_parent,a_list,a_number);
}
/***************************************************************************/
Widget XoCreateColormapDialog (
 Widget a_parent
,String a_name
,ArgList a_list
,Cardinal a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget     cmap = NULL;
/*.........................................................................*/
  if(a_parent==NULL)     return NULL;
  if(a_name==NULL)       return NULL;

#ifdef HAS_XM
 {String     name;
  Widget     form,hide,label,frame,cmap;
  Arg        args[6];

  name              = CStringDuplicate           (a_name);
  CStringConcatenate        (&name,"FORM");
  form              = XmCreateFormDialog   (a_parent,name,args,0);
  CStringDelete     (name);
  XtRealizeWidget   (form);

  hide              = XmCreatePushButton   (form,"CMAPD_HIDE",args,0);
  XtManageChild     (hide);

  label             = XmCreateLabel        (form,"CMAPD_LABEL",args,0);
  XtManageChild     (label);

  frame             = XmCreateFrame        (form,"CMAPD_FRAME",args,0);
  XtManageChild     (frame);

/* put XoColormap in a frame so that we can set XoColormap resources in OKit.oui */
  cmap              = XtCreateWidget (a_name,xoColormapWidgetClass,frame,a_list,a_number);
  XtManageChild     (cmap);

  XtSetArg          (args[0],XmNleftAttachment    ,XmATTACH_FORM);
  XtSetArg          (args[1],XmNrightAttachment   ,XmATTACH_FORM);
  XtSetArg          (args[2],XmNtopAttachment     ,XmATTACH_FORM);
  XtSetValues       (hide,args,3);

  XtSetArg          (args[0],XmNleftAttachment    ,XmATTACH_FORM);
  XtSetArg          (args[1],XmNrightAttachment   ,XmATTACH_FORM);
  XtSetArg          (args[2],XmNtopAttachment     ,XmATTACH_WIDGET);
  XtSetArg          (args[3],XmNtopWidget         ,hide);
  XtSetValues       (label,args,4);

  XtSetArg          (args[0],XmNleftAttachment    ,XmATTACH_FORM);
  XtSetArg          (args[1],XmNrightAttachment   ,XmATTACH_FORM);
  XtSetArg          (args[2],XmNtopAttachment     ,XmATTACH_WIDGET);
  XtSetArg          (args[3],XmNtopWidget         ,label);
  XtSetArg          (args[4],XmNbottomAttachment  ,XmATTACH_FORM);
  XtSetValues       (frame,args,5);

  XtAddCallback     (hide ,XmNactivateCallback    ,HideCallback, NULL);

  XmSetCompoundStringResourceFromString   (form,XmNdialogTitle,"Cmap");
  }
#else /*HAS_XM*/
  CWarn ("Xo package has been compiled without the -DHAS_XM option.\n");
#endif /*HAS_XM*/
  a_list   = NULL;/*To avoid C++ warning.*/
  a_number = 0;/*To avoid C++ warning.*/

  return              cmap;
}
/***************************************************************************/
static void HideCallback (
 Widget This 
,XtPointer a_tag
,XtPointer a_data
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  XtUnmanageChild (XtParent(This));
  a_tag  = NULL;/*To avoid C++ warning.*/
  a_data = NULL;/*To avoid C++ warning.*/
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
void XoPlotCurve (
 Widget This  
,int a_number
,double* a_xs
,double* a_ys
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
#ifdef HAS_XOPLOTTER
   int count;
  double xmin,xmax;
  double ymin,ymax;
  OScatter  oscat;
  char*  name;
/*.........................................................................*/
  if(This==NULL) return;
  if(!XtIsSubclass(This,xoPlotterWidgetClass)) return;
  if(a_number<=0) return;
  xmin = a_xs[0];
  xmax = a_xs[0];
  ymin = a_ys[0];
  ymax = a_ys[0];
  for(count=1;count<a_number;count++)
    {
      xmin = MINIMUM(xmin,a_xs[count]);
      xmax = MAXIMUM(xmax,a_xs[count]);
      ymin = MINIMUM(ymin,a_ys[count]);
      ymax = MAXIMUM(ymax,a_ys[count]);
    }
  name         = XtName(This);
  name         = CStringCreateF (strlen(name)+6,"%s_curve",name);
  OScatterDelete  (OScatterGetIdentifier(name));
  oscat        = OScatterCreate (name,2,"x",xmin,xmax,"y",ymin,ymax);  /*Points must be in limits.*/
  if(oscat!=NULL)
    {
      OScatterAllocatePoints (oscat,a_number);
      for(count=0;count<a_number;count++)
	{
	  OScatterFillTwoDimensional (oscat,a_xs[count],a_ys[count]);
	}
      XoPlotterSetPlottedObjects    (This,name);
    }
  CStringDelete           (name);
#else  /*HAS_XOPLOTTER*/
  This     = NULL;
  a_number = 0;
  a_xs     = NULL;
  a_ys     = NULL;
#endif /*HAS_XOPLOTTER*/
}
/***************************************************************************/
Widget XoCreatePlotter (
 Widget a_parent
,String a_name
,ArgList a_list
,Cardinal a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget widget;
/*.........................................................................*/
  if(a_parent==NULL) return NULL;
  if(a_name==NULL)   return NULL;
#ifdef HAS_XOPLOTTER
  widget = XtCreateManagedWidget(a_name,xoPlotterWidgetClass,a_parent,a_list,a_number);
#else  /*HAS_XOPLOTTER*/
  widget   = NULL;
  a_number = 0;
  a_list   = NULL;
#endif /*HAS_XOPLOTTER*/
  return widget;
}
/***************************************************************************/
void XoBuildGridOfPlotters (
 Widget This
,char* a_name
,int a_rows
,int a_cols
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
#if defined(HAS_XM) && defined (HAS_XOPLOTTER)
  XmFormBuildGrid (This,a_name,XoCreatePlotter,a_rows,a_cols);
#else /*HAS_XM*/
  CWarn ("Xo package has been compiled without the -DHAS_XM option.\n");
  This   = NULL;
  a_name = NULL;
  a_rows = 0;
  a_cols = 0;
#endif /*HAS_XM*/
}
/***************************************************************************/
void XoAttachObjectsToGridOfPlotters (
 Widget a_widget
,int    a_namen
,char** a_names
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* cname;
  Arg   args[1];
/*.........................................................................*/
  if(a_widget==NULL) return;
  cname = XWidgetGetClassName (a_widget);
  if( (cname!=NULL) && ( (strcmp(cname,"XoPlotter")==0)  || (strcmp(cname,"XzPlotter")==0) ) )
    {
      if(a_namen==1)
	{
	  XtSetArg    (args[0],XoNplottedObjects,NULL);
	  XtSetValues (a_widget,args,1);
	  XtSetArg    (args[0],XoNplottedObjects,a_names[0]);
	  XtSetValues (a_widget,args,1);
	}
      else if(strcmp(cname,"XoPlotter")==0)
	{
	  CWarnF      ("Zones not supported by XoPlotter %s.n",XtName(a_widget));
	}
      else if(strcmp(cname,"XzPlotter")==0)
	{
	  char*         string;
	  string        = CTextConvertToString (a_namen,a_names,"\\n");
	  XtSetArg      (args[0],XoNplottedObjects,NULL);
	  XtSetValues   (a_widget,args,1);
	  XtSetArg      (args[0],XoNplottedObjects,string);
	  XtSetValues   (a_widget,args,1);
	  CStringDelete (string);
	}
    }
  else  /*Assume a container with some plotters.*/
    {
      int           number;
      WidgetList    list;
      list          = XWidgetGetChildren (a_widget,&number);
      cname         = number==0 ? CStringNULL : XWidgetGetClassName (list[0]);
      if( (number==1) && (strcmp(cname,"XzPlotter")==0) )  /*Use HPLOT zones.*/
	{
	  char*         string;
	  string        = CTextConvertToString (a_namen,a_names,"\\n");
	  XtSetArg      (args[0],XoNplottedObjects,NULL);
	  XtSetValues   (list[0],args,1);
	  XtSetArg      (args[0],XoNplottedObjects,string);
	  XtSetValues   (list[0],args,1);
	  CStringDelete (string);
	}
      else
	{
	  int           count;
	  int           iname;
	  iname         = 0;
	  for(count=number-1;count>=0;count--)
	    {
	      Widget  plotter;
	      plotter = list[count];
	      if( (iname<a_namen) && (XtIsManaged(plotter)==True) )
		{
#ifdef HAS_XOPLOTTER
		  XoPlotterSetPlottedObjects (plotter,a_names[iname]);
#endif /*HAS_XOPLOTTER*/
		  /*XoPlotterUpdateNow(plotter); Not really better.*/
		  iname++;
		}
	      else
		{
#ifdef HAS_XOPLOTTER
		  if(XtIsManaged(plotter)==True)
  		    XoPlotterSetPlottedObjects (plotter,NULL);
#endif /*HAS_XOPLOTTER*/
		  /*XoPlotterUpdateNow(plotter); Not really better.*/
		}
	    }
	}
      CMemoryFreeBlock      (list);
    }
}
#ifdef HAS_XM
/***************************************************************************/
/******* From XmWidget *****************************************************/
/***************************************************************************/
/***************************************************************************/
static void XmSetCompoundStringResourceFromString (
 Widget This
,char* a_name
,char* a_string
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Arg       args[1];
  XmString  cps;
/*.........................................................................*/
  if(This==NULL)     return;
  if(a_name==NULL)   return;
  if(a_string==NULL) return;
  cps          = XmStringLtoRCreate(a_string,XmSTRING_DEFAULT_CHARSET);
  XtSetArg     (args[0],a_name,cps);
  XtSetValues  (This,args,1);
  XmStringFree (cps);
}
/***************************************************************************/
static void XmFormBuildGrid (
 Widget This
,char* a_name
,XmCreateWidgetFunction a_proc
,int  a_rows
,int  a_cols
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count,row,col;
  int    number;
  Arg    args[1];
  int    width,height;
  int    top, bottom, left, right;
  int    border = 0;
  char** names;
  int*   tops;
  int*   bottoms;
  int*   lefts;
  int*   rights;
/*.........................................................................*/
  if(This==NULL)                                 return;
  if(!XtIsSubclass(This,xmFormWidgetClass)) 
    {
      CWarnF("XmFormBuildGrid: %s is not an XmForm.\n",XtName(This));
      return;
    }
  if(a_proc==NULL) return;
  if(a_rows<=0)    return;
  if(a_cols<=0)    return;

/* kill existing children */
 {WidgetList items,itema;
  items = XWidgetGetChildren (This,NULL);
  for(itema=items;(itema!=NULL) && (*itema!=NULL);itema++) XtDestroyWidget (*itema);
  CListDelete((CList)items);}

  XtSetArg    (args[0], XmNfractionBase     ,1000);
  XtSetValues (This, args,1);

  number      = a_rows * a_cols;

  height      = (1000 / a_rows) - 2 * border;
  width       = (1000 / a_cols) - 2 * border;

  top    = border;
  bottom = top + height;

  names   = (char**)CMemoryAllocateBlock(number * sizeof(char*));
  tops    = (int*)  CMemoryAllocateBlock(number * sizeof(int));
  bottoms = (int*)  CMemoryAllocateBlock(number * sizeof(int));
  lefts   = (int*)  CMemoryAllocateBlock(number * sizeof(int));
  rights  = (int*)  CMemoryAllocateBlock(number * sizeof(int));
  if( (names==NULL) || (tops==NULL) || (bottoms==NULL) || (lefts==NULL) || (rights==NULL) )
    {
      CTextDelete        (number,names);
      CMemoryFreeBlock   (tops);
      CMemoryFreeBlock   (bottoms);
      CMemoryFreeBlock   (lefts);
      CMemoryFreeBlock   (rights);
      return;
    }

  count  = 0;  
  for (row=0;row<a_rows;row++)
    {
      left  = border;
      right = left + width;
      for (col=0;col<a_cols;col++)
        { 
          char*                name;
          if(count>=number)    return;
	  if(a_name!=NULL)
	    {
	      name = CStringCreateF(strlen(a_name)+16+16+2,"%s_%d_%d",a_name,row+1,col+1);
	    }
	  else if(XtName(This)!=NULL)
	    {
	      name = CStringCreateF(strlen(XtName(This))+16+16+2,"%s_%d_%d",XtName(This),row+1,col+1);
	    }
	  else
	    {
	      name = CStringCreateF(5+16+16+2,"xxxx_%d_%d",row+1,col+1);
	    }
          names[count]         = CStringDuplicate(name);
          CStringDelete        (name);
          tops[count]          = top;
          bottoms[count]       = bottom;
          lefts[count]         = left;
          rights[count]        = right;
          count               ++;
          left                 = right + 2 * border;
          right                = left  + width;
        }
      top     = bottom + 2 * border;
      bottom  = top    + height;
    }

  for(count=number-1;count>=0;count--)
    {
      Widget                   child;
      child                    = a_proc(This,names[count],args,0);
      XmAttachWidgetAtPosition (child , tops[count], bottoms[count], lefts[count], rights[count]);
    }

  CTextDelete        (number,names);
  CMemoryFreeBlock   (tops);
  CMemoryFreeBlock   (bottoms);
  CMemoryFreeBlock   (lefts);
  CMemoryFreeBlock   (rights);

}
/***************************************************************************/
static void XmAttachWidgetAtPosition (
 Widget This 
,int  a_top    
,int  a_bottom 
,int  a_left   
,int  a_right  
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Arg args[13];
  int offset = 2; 
/*.........................................................................*/
  if(This==NULL)           return;
  if(XtParent(This)==NULL) return;
  if(!XtIsSubclass(XtParent(This),xmFormWidgetClass)) return;
  XtSetArg    (args[0]  , XmNresizable        ,True); 
  XtSetArg    (args[1]  , XmNtopAttachment    ,XmATTACH_POSITION); 
  XtSetArg    (args[2]  , XmNbottomAttachment ,XmATTACH_POSITION); 
  XtSetArg    (args[3]  , XmNleftAttachment   ,XmATTACH_POSITION); 
  XtSetArg    (args[4]  , XmNrightAttachment  ,XmATTACH_POSITION); 
  XtSetArg    (args[5]  , XmNtopPosition      ,a_top); 
  XtSetArg    (args[6]  , XmNbottomPosition   ,a_bottom);
  XtSetArg    (args[7]  , XmNleftPosition     ,a_left); 
  XtSetArg    (args[8]  , XmNrightPosition    ,a_right);

  XtSetArg    (args[9]  , XmNtopOffset        ,offset); 
  XtSetArg    (args[10] , XmNbottomOffset     ,offset);
  XtSetArg    (args[11] , XmNleftOffset       ,offset); 
  XtSetArg    (args[12] , XmNrightOffset      ,offset);
  XtSetValues (This,args,13);
}
#endif /*HAS_XM*/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
static void ReportError (
 char* a_name
,int a_code
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CErrorHandle ("Xo","Xo",a_name,a_code);
}
