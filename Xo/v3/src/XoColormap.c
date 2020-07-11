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

#include <X11/StringDefs.h>

/*Co*/
#include <CMemory.h>
#include <CString.h>

/*Go*/
#include <OColormap.h>
#include <OColormapX.h>
#include <GoTypes.h>

/*Private.*/
#include <X11/IntrinsicP.h>
#include <X11/CoreP.h>

#include <XoColormapP.h>

#ifdef __cplusplus
extern "C"{
#endif
static void             InitializeClass     (void);
static void             InitializeWidget    (Widget, Widget,ArgList,Cardinal*);
static void             DestroyWidget       (Widget);
static void             ChangeWidgetSize    (Widget);
static void             DrawWidget          (Widget, XEvent*, Region);
static Boolean          SetValues           (Widget,Widget,Widget,ArgList,Cardinal *);
static void             SelectAction        (Widget,XEvent*, String* ,Cardinal*);
static void             RepresentColormap   (Widget);
static void             GetParameters       (Widget,OColormap*,int*,int**,int*,int*,int*,int*,int*,int*,int*,double**);
#ifdef __cplusplus
}
#endif

#define athis   ((XoColormapWidget)This)->cmap
#define acur    ((XoColormapWidget)a_current)->cmap
#define IFMOD(a_field)  if(athis.a_field != acur.a_field)
#define IFSMOD(a_field) if(CStringCompare(athis.a_field,acur.a_field)==0)
#define NotFound           (-1)

static char translations[] =
            "\
                        <Btn1Down>: CmapSelect()";

static XtActionsRec actions[] = 
{
  { "CmapSelect"    , SelectAction}
};


static XtResource resources [] ={
  {XoNvirtualColormap, "VirtualColormap", XtRString , sizeof(String) ,
      XtOffset(XoColormapWidget, cmap.virtualColormap),XtRString,  "ocolormap_X"},
  {XoNvalueColorMapping ,"ValueColorMapping"   , XtRString   , sizeof(String)        ,
      XtOffset(XoColormapWidget, cmap.valueColorMapping)   ,XtRString , NULL},
  {XoNtitleFont      , "TitleFont"      , XtRFont   , sizeof(Font)    ,
      XtOffset(XoColormapWidget, cmap.titleFont),XtRString,XtDefaultFont},
  {XoNselectedColor  , "SelectedColor"          , XtRString  , sizeof(String)    ,
      XtOffset(XoColormapWidget, cmap.selectedColor),XtRString,"-1"},
  {XoNselectCallback,XtCCallback  ,XtRCallback,sizeof(XtCallbackList),
    XtOffset(XoColormapWidget, cmap.selectCallback),XtRImmediate,(XtPointer)NULL}
};

XoColormapClassRec  xoColormapClassRec = {
/* Core Class Part */
{
   (WidgetClass)&coreClassRec,  /* superclass            */
    "XoColormap",         	/* widget resource class name       */
    sizeof(XoColormapRec),	/* size in bytes of widget record   */
    InitializeClass,            /* class_initialize                 */
    NULL, 			/* dynamic initialization           */
    FALSE,       		/* has class been initialized?      */
    InitializeWidget,           /* initialize                       */
    NULL,			/* notify that initialize called    */
    XtInheritRealize,           /* XCreateWindow for widget         */
    actions,     		/* widget semantics name to proc map*/
    XtNumber(actions),          /* number of entries in actions     */
    resources,                  /* resources for subclass fields    */
    XtNumber(resources),        /* number of entries in resources   */
    NULLQUARK,		        /* resource class quarkified        */
    TRUE,			/* compress MotionNotify for widget */
    TRUE,			/* compress Expose events for widget*/
    TRUE,			/* compress enter and leave events  */
    TRUE,			/* select for VisibilityNotify      */
    DestroyWidget,	        /* free data for subclass pointers  */
    ChangeWidgetSize,           /* geom manager changed widget size */
    DrawWidget,              	/* rediplay window                  */
    SetValues,		        /* set subclass resource values     */
    NULL,			/* notify that SetValues called     */
    XtInheritSetValuesAlmost,   /* SetValues got "Almost" geo reply */
    NULL,		        /* notify that get_values called    */
    XtInheritAcceptFocus,       /* assign input focus to widget     */
    XtVersion,	                /* version of intrinsics used       */
    NULL,		        /* list of callback offsets         */
    translations,	        /* state machine                    */
    XtInheritQueryGeometry,     /* return preferred geometry        */
    XtInheritDisplayAccelerator,/* display your accelerator         */
    NULL		        /* pointer to extension record      */
},
/* XoColormap */
{
   NULL
}
   
};

WidgetClass xoColormapWidgetClass = (WidgetClass) &xoColormapClassRec;

/***************************************************************************/
static void InitializeClass (
 void
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  GoLoadColormapX ();
}
/***************************************************************************/
static void InitializeWidget (
 Widget a_request
,Widget This
,ArgList a_args
,Cardinal* a_argn
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_request->core.width<=0)  This->core.width  = 100;
  if(a_request->core.height<=0) This->core.height = 100;

  athis.virtualColormap   = (char*)XtNewString(athis.virtualColormap);
  athis.valueColorMapping = (char*)XtNewString(athis.valueColorMapping);
  athis.selectedColor     = (char*)XtNewString(athis.selectedColor);

  a_request   = NULL;
  a_args      = NULL;
  a_argn      = 0;
}
/***************************************************************************/
static void DestroyWidget (
 Widget This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  XtRemoveAllCallbacks    (This, XoNselectCallback);
  XtFree                  (athis.virtualColormap);
  XtFree                  (athis.valueColorMapping);
  XtFree                  (athis.selectedColor);
  athis.virtualColormap   = NULL;
  athis.valueColorMapping = NULL;
  athis.selectedColor     = NULL;
}
/***************************************************************************/
static void ChangeWidgetSize (
 Widget This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  RepresentColormap (This);
}
/***************************************************************************/
static void DrawWidget (
 Widget  This
,XEvent* a_event
,Region a_region
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  RepresentColormap (This);
  a_event  = NULL;
  a_region = NULL;
}
/***************************************************************************/
static Boolean SetValues (
 Widget a_current
,Widget a_request
,Widget This
,ArgList a_args
,Cardinal* a_argn
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  IFSMOD(virtualColormap)
    { 
      XtFree                (acur.virtualColormap);
      athis.virtualColormap = (char*)XtNewString(athis.virtualColormap);
      return                True;
    }
  else {athis.virtualColormap = acur.virtualColormap;}

  IFSMOD(valueColorMapping)
    { 
      XtFree                  (acur.valueColorMapping);
      athis.valueColorMapping = (char*)XtNewString(athis.valueColorMapping);
      return                  True;
    }
  else {athis.valueColorMapping = acur.valueColorMapping;}

  IFSMOD(selectedColor)
    { 
      XtFree                (acur.selectedColor);
      athis.selectedColor   = (char*)XtNewString(athis.selectedColor);
      return                True;
    }
  else {athis.selectedColor = acur.selectedColor;}

  IFMOD(titleFont)
    {
      return                True;
    }

  a_request = NULL;
  a_args    = NULL;
  a_argn    = 0;
  return    False;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static void SelectAction (
 Widget This
,XEvent* a_event
,String* a_args
,Cardinal* a_argn
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OColormap     cmap;
  int           colorn,coln,rown,width,height,margin,text,start;
  int*          colors = NULL;
  double*       values = NULL;
/*.........................................................................*/
  if(a_event->type!=ButtonPress) return;

  GetParameters       (This,&cmap,&colorn,&colors,&coln,&rown,&width,&height,&margin,&text,&start,&values);
  if(colorn==0)       return;

  XtFree              (athis.selectedColor);
  athis.selectedColor = (char*)XtNewString ("-1");

 {int                 count,row,col;
  int                 x,y;
  count               = 0;
  y                   = start + margin;
  for(row=0;row<rown;row++)
    {
      x       = margin;
      for(col=0;col<coln;col++) 
        {
          if(count>=colorn) break;
          if( (x<a_event->xbutton.x)&&(a_event->xbutton.x<x+width)  &&
              (y<a_event->xbutton.y)&&(a_event->xbutton.y<y+height) 
            ) 
            { 
	      XoColormapCallbackStruct     callData;
	      XtFree                       (athis.selectedColor);
	      athis.selectedColor          = (char*)XtNewString(OColormapGetIthColorName(cmap,colors[count]));

              callData.reason              = 0L;
              callData.event               = a_event;
              callData.selectedColor       = athis.selectedColor;

              RepresentColormap            (This);
              XtCallCallbacks              (This, XoNselectCallback,(XtPointer)&callData);
	      CMemoryFreeBlock             (values);
	      CMemoryFreeBlock             (colors);
              return;
            }
          x += width + margin;
          count++;
        }
      y += height + margin;
    }}
  RepresentColormap (This);
  CMemoryFreeBlock  (values);
  CMemoryFreeBlock  (colors);
  a_args = NULL;
  a_argn = 0;
}
/***************************************************************************/
static void RepresentColormap (
 Widget This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  GC             xgc;
  Display*       xdisp;
  Window         xwindow;
  OColormap      cmap;
  char*          name;
  int            found,index,colorn,coln,rown,width,height,margin,text,start;
  int*           colors = NULL;
  double*        values = NULL;
  int            x,y;
  int            count,row,col;
/*.........................................................................*/
  if(!XtIsRealized(This)) return;

  xdisp             = XtDisplay (This);
  xwindow           = XtWindow  (This);

  XClearWindow      (xdisp,xwindow);

  xgc               = (GC) XCreateGC (xdisp,XDefaultRootWindow(xdisp),0L,NULL);

  GetParameters     (This,&cmap,&colorn,&colors,&coln,&rown,&width,&height,&margin,&text,&start,&values);

  name              = OColormapGetName(cmap);
  name              = name!=NULL ? CStringDuplicate(name) : CStringCreateF (10+64,"OColormap/%lu",cmap);
  XSetFont          (xdisp,xgc,athis.titleFont);
  XSetForeground    (xdisp,xgc,XWhitePixelOfScreen(XDefaultScreenOfDisplay(xdisp)));
  XDrawString       (xdisp,xwindow,xgc,margin,text,name,strlen(name));
  CStringDelete     (name);

  index             = OColormapGetColorNameIndex (cmap,athis.selectedColor);
  found             = 0;
  for(count=0;count<colorn;count++) 
    {
      if(colors[count]==index)
	{
	  index = count;
	  found = 1;
          break;
	}
    }
  if(found==0) index = NotFound;

  if(colorn!=0)
    {
      count             = 0;
      y                 = start + margin;
      for(row=0;row<rown;row++)
	{
	  x       = margin;
	  for(col=0;col<coln;col++) 
	    {
	      if(count>=colorn) break;
	      if(count==index)
		{ 
		  int            border = 3;
		  XSetForeground (xdisp,xgc,XWhitePixelOfScreen(XDefaultScreenOfDisplay(xdisp)));
		  XFillRectangle (xdisp,xwindow,xgc,x-border,y-border,width+2*border,height+2*border);
		}
	      XSetForeground   (xdisp,xgc,OColormapXGetPixel (cmap,colors[count],xdisp));
	      XFillRectangle   (xdisp,xwindow,xgc,x,y,width,height);
	      if(values!=NULL)
		{
		  char*          string = NULL;
		  XSetForeground (xdisp,xgc,XWhitePixelOfScreen(XDefaultScreenOfDisplay(xdisp)));
		  if(count==0)
		    {
		      string = CStringCreateF (2+64,"< %g",values[1]);
		    }
		  else if(count==colorn-1)
		    {
		      string = CStringCreateF (3+2*64,"%g <=",values[count]);
		    }
		  else
		    {
		      string = CStringCreateF (10+2*64,"%g <= and < %g",values[count],values[count+1]);
		    }
		  if(string!=NULL) XDrawString (xdisp,xwindow,xgc,x+width/2,y+height/2,string,strlen(string));
		  CStringDelete  (string);
		}
	      x                += width + margin;
	      count++;
	    }
	  y += height + margin;
	}
    }
  
  CMemoryFreeBlock (values);
  CMemoryFreeBlock (colors);
  XFreeGC          (xdisp,xgc);
}
/***************************************************************************/
static void GetParameters (
 Widget This
,OColormap* a_cmap
,int* a_colorn
,int** a_colors
,int* a_coln
,int* a_rown
,int* a_width
,int* a_height
,int* a_margin
,int* a_text
,int* a_start 
,double** a_values
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*            name;
  int              direct,ascent,descent;
  XCharStruct      charStruct;
/*.........................................................................*/
  *a_colorn           = 0;
  *a_colors           = NULL;
  *a_values           = NULL;
  *a_cmap             = OColormapGetIdentifier (athis.virtualColormap);
  if(*a_cmap==NULL)   return;

  if(athis.valueColorMapping!=NULL)
    {
      *a_colorn        = OColormapGetValueColorMapping (*a_cmap,athis.valueColorMapping,a_values,a_colors);
      if(*a_colorn==0) return;
      *a_coln           = 1;
      *a_rown           = (*a_colorn) / (*a_coln);
      (*a_rown)++;
    }
  else
    {
      int                 count;
      *a_colorn           = OColormapGetSize (*a_cmap);
      if(*a_colorn==0)    return;
      *a_colors           = (int*) CMemoryAllocateBlock ((*a_colorn) * sizeof(int));
      if(*a_colors==NULL) 
	{
	  *a_colorn = 0;
	  return;
	}
      for(count=0;count<*a_colorn;count++) (*a_colors)[count] = count;
      *a_coln           = 6;
      *a_rown           = (*a_colorn) / (*a_coln);
      (*a_rown)++;
    }


  *a_margin         = 10;

  name              = OColormapGetName(*a_cmap);
  name              = name!=NULL ? CStringDuplicate(name) : CStringCreateF(10+64,"OColormap/%lu",*a_cmap);
  XQueryTextExtents (XtDisplay(This),athis.titleFont,name,strlen(name),&direct,&ascent,&descent,&charStruct);
  CStringDelete     (name);

  *a_text           = *a_margin + ascent;
  *a_start          = *a_margin + ascent + descent;

  *a_width          = ((int)This->core.width               - (*a_margin) * ((*a_coln)+1))/(*a_coln);
  *a_height         = ((int)This->core.height - (*a_start) - (*a_margin) * ((*a_rown)+1))/(*a_rown);

}
/***************************************************************************/
void XoColormapClearClass (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  OColormapXClearClass ();
  OColormapClearClass  ();
}

