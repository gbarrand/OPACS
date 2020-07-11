/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
/*
#undef  HAS_XM
#define HAS_XAW
*/

#include <stdlib.h>
#include <math.h>
#include <string.h>

#include <X11/StringDefs.h>
#include <X11/cursorfont.h>
#include <X11/Intrinsic.h>
#include <X11/Shell.h>

#ifdef HAS_XM
#include <Xm/PushB.h>           /*IMAGE_PIXEL*/
#include <Xm/RowColumn.h>       /*XmMenuPosition*/
#include <Xm/PushBG.h>          /*for popup menu*/
#endif /*HAS_XM*/

#ifdef HAS_XAW
#include <Xaw/Command.h>        /*IMAGE_PIXEL*/
#include <Xaw/SimpleMenu.h>   
#include <Xaw/SmeBSB.h>   
#include <Xaw/SmeLine.h>  
#endif

/*Co*/
#include <CMemory.h>
#include <CString.h>
#include <CList.h>
#include <CPS.h>
#include <CGIF.h>
#include <CImageshort.h>
#include <CError.h>
#include <CPrinter.h>
#include <CSystem.h>
#include <OMatrix33.h>
#include <OLUT.h>

/*Go*/
#include <OColormap.h>

/*Xx*/
#include <XWidget.h>

/*Xo*/
#include <OImage.h>
#include <OColormapX.h>

/*Private.*/
#include <X11/IntrinsicP.h>
#include <X11/CoreP.h>
#include <X11/CompositeP.h>

#include <XoShapeP.h>
#include <XoImageP.h>

#ifdef __cplusplus
extern "C"{
#endif
static void     InitializeClass          (void);
static void     InitializeWidget         (Widget, Widget,ArgList,Cardinal*);
static void     RealizeWidget            (Widget, XtValueMask*, XSetWindowAttributes*);
static void     DestroyWidget            (Widget);
static void     ChangeWidgetSize         (Widget);
static void     DrawWidget               (Widget, XEvent*, Region);
static Boolean  SetValues                (Widget,Widget,Widget,ArgList,Cardinal *);

static void     GetImagePart             (Widget,int,int);
static int      GetCompressFactor        (double);
static void     MoveAction               (Widget,XEvent*, String* ,Cardinal*);
static void     CollectAction            (Widget,XEvent*, String* ,Cardinal*);
static void     ShowAction               (Widget,XEvent*, String* ,Cardinal*);
static void     PopupAction              (Widget,XEvent*,String*,Cardinal*);
static void     CreatePopup              (Widget);
static void     ActivateCallback         (Widget,XtPointer,XtPointer);
static void     CreateWidget             (Widget);
static int      ConvoluteImage           (short*,unsigned int,unsigned int,int,unsigned char*);
static void     SetRectangleWithPart     (ORect,int,int,ORect);
static void     SetRectangleWithBeginEnd (ORect,int,int,int,int);
static void     LoadData                 (Widget);
static void     AttachImage              (Widget,void*,void*,void*);
static void     ReportError              (char*,int);
static Widget   FindPopupEntry           (char*);
#ifdef __cplusplus
}
#endif

typedef unsigned char Uchar;

typedef struct
{
    int     reason;
    XEvent  *event;
} XoAnyCallbackStruct;

#define DBL_INT       (int)(float)
#define MINIMUM(a,b)  ((a)<(b)?a:b)
#define MAXIMUM(a,b)  ((a)>(b)?a:b)

#define athis   ((XoImageWidget)This)->image
#define acur    ((XoImageWidget)a_current)->image

#define IFMOD(a_field)  if(athis.a_field != acur.a_field)
#define IFSMOD(a_field) if(CStringCompare(athis.a_field,acur.a_field)==0)

static struct 
{
  Cursor cursor_fleur;
  Cursor cursor_top_left_arrow;
  Widget This;
  Widget popup;
  XEvent This_event;
} Class = {0L,0L,NULL,NULL};

static char translations[] =
            "\
           Shift Ctrl <Btn1Down>: ImageCollect(None)\n\
   Shift Ctrl Button1 <PtrMoved>: ImageCollect(None)\n\
             Shift Ctrl <Btn1Up>: ImageCollect(None)\n\
                 Ctrl <Btn1Down>: ImageMove()\n\
         Ctrl Button1 <PtrMoved>: ImageMove()\n\
                   Ctrl <Btn1Up>: ImageMove()\n\
                Shift <Btn1Down>: ImageShowPixel()\n\
        Shift Button1 <PtrMoved>: ImageShowPixel()\n\
                  Shift <Btn1Up>: ImageShowPixel()\n\
                      <Btn1Down>: ImageCollect(Zoom)\n\
              Button1 <PtrMoved>: ImageCollect(Zoom)\n\
                        <Btn1Up>: ImageCollect(Zoom)\n\
                      <Btn3Down>: ImagePopup()";

static XtActionsRec actions[] = 
{
  { "ImageMove"      , MoveAction},
  { "ImageCollect"   , CollectAction},
  { "ImageShowPixel" , ShowAction},
  { "ImagePopup"     , PopupAction}
};

static XtResource resources [] ={
  {XoNvirtualColormap, "VirtualColormap", XtRString    , sizeof(String) ,
                    XtOffset(XoImageWidget,image.virtualColormap),XtRImmediate,(XtPointer)NULL},
  {XoNpostScriptFile , "PostScriptFile" , XtRString    , sizeof(String) ,
                    XtOffset(XoImageWidget, image.postScriptFile)     ,XtRString   ,"out.ps"},
  {XoNgifFile        , "GifFile"        , XtRString    , sizeof(String) ,
                    XtOffset(XoImageWidget, image.gifFile)     ,XtRString   ,"out.gif"},
  {XoNdataFile        , "DataFile"        , XtRString    , sizeof(String) ,
                    XtOffset(XoImageWidget, image.dataFile)     ,XtRImmediate,(XtPointer)NULL},
  {XoNpixelPrecision  , "PixelPrecision"  , XtRInt       , sizeof(int) ,
                    XtOffset(XoImageWidget, image.pixelPrecision) ,XtRImmediate,(XtPointer)10},
  {XoNcollectCallback,XtCCallback  ,XtRCallback,sizeof(XtCallbackList),
                               XtOffset(XoImageWidget,image.collectCallback),XtRImmediate,(XtPointer)NULL}
};

XoImageClassRec  xoImageClassRec = {
/* Core Class Part */
{
    (WidgetClass) &xoShapeClassRec,         
                                /* pointer to superclass ClassRec   */
    "XoImage",         	        /* widget resource class name       */
    sizeof(XoImageRec),	        /* size in bytes of widget record   */
    InitializeClass,                  /* class_initialize                 */
    NULL, 			/* dynamic initialization           */
    FALSE,       		/* has class been initialized?      */
    InitializeWidget,                 /* initialize                       */
    NULL,			/* notify that initialize called    */
    RealizeWidget,              /* XCreateWindow for widget         */
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
    ChangeWidgetSize,               /* geom manager changed widget size */
    DrawWidget,	        /* rediplay window                  */
    SetValues,	                /* set subclass resource values     */
    NULL,			/* notify that SetValues called    */
    XtInheritSetValuesAlmost,   /* SetValues got "Almost" geo reply*/
    NULL,		        /* notify that get_values called    */
    XtInheritAcceptFocus,       /* assign input focus to widget     */
    XtVersion,	                /* version of intrinsics used       */
    NULL,		        /* list of callback offsets         */
    translations,	        /* state machine                    */
    XtInheritQueryGeometry,     /* return preferred geometry        */
    XtInheritDisplayAccelerator,/* display your accelerator         */
    NULL 		        /* pointer to extension record      */
},
/* Composite Class Part */
{
    XtInheritGeometryManager,   /* geometry manager for children   */
    XtInheritChangeManaged,     /* change managed state of child   */
    XtInheritInsertChild,       /* physically add child to parent  */
    XtInheritDeleteChild,       /* physically remove child         */
    NULL 		        /* pointer to extension record     */
},
/* XoShape */
{
   NULL 
},
/* XoImage */
{
   NULL 
}
   
};

WidgetClass xoImageWidgetClass = (WidgetClass) &xoImageClassRec;
/***************************************************************************/
static void InitializeClass (
 void
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
#ifdef HAS_XAW
  XtRegisterGrabAction(PopupAction,True,ButtonPressMask | ButtonReleaseMask,GrabModeAsync,GrabModeAsync);
#endif /*HAS_XAW*/
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

 {XGCValues             values;
  XtGCMask              mask;
  mask                  = GCForeground;
  values.foreground     = This->core.background_pixel;
  athis.gc              = XtGetGC((Widget)This, mask, &values);}

/* init image part */
  athis.oimage          = NULL;
  athis.ocmap           = NULL;
  athis.olut            = NULL;
  athis.tx              = 0;
  athis.ty              = 0;
  athis.scale           = 1.;
  athis.part.x          = 0;
  athis.part.y          = 0;
  athis.part.width      = 0;
  athis.part.height     = 0;
  athis.beg_x           = 0;
  athis.beg_y           = 0;
  athis.end_x           = 0;
  athis.end_y           = 0;

  athis.rubberGC        = NULL;

  athis.showPixelWidget = NULL;
  CreateWidget          (This);

  athis.virtualColormap = CStringDuplicate(athis.virtualColormap);
  athis.postScriptFile  = CStringDuplicate(athis.postScriptFile);
  athis.gifFile         = CStringDuplicate(athis.gifFile);
  athis.dataFile        = CStringDuplicate(athis.dataFile);

  LoadData              (This); /*If some dataFile, will set oimage and ocmap.*/

  if(athis.ocmap==NULL)
    {
      athis.ocmap = OColormapGetIdentifierFromString (athis.virtualColormap);
    }
  else  /*A colormap has been given through dataFile. */
    {
      if(athis.virtualColormap!=NULL)
        { /*A colormap has also been given in resource, take it.*/
	  athis.ocmap = OColormapGetIdentifierFromString (athis.virtualColormap);
        }
    }

  a_request = NULL;
  a_args    = NULL;
  a_argn    = 0; /*avoid C++ warnings*/
}
/***************************************************************************/
static void RealizeWidget (
 Widget This
,XtValueMask* a_mask
,XSetWindowAttributes* a_watbs
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
/* Call the Realize procedure (XtInheritRealize) */
  if(xoImageWidgetClass->core_class.superclass->core_class.realize!=NULL)
    (xoImageWidgetClass->core_class.superclass->core_class.realize)(This, a_mask, a_watbs);

  athis.rubberGC = XDisplayGetRubberGC(XtDisplay(This),XtWindow(This));
}
/***************************************************************************/
static void DestroyWidget (
 Widget This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(athis.rubberGC!=NULL)
    {
      XFreeGC        (XtDisplay(This),athis.rubberGC);
      athis.rubberGC = NULL;
    }

  XtRemoveAllCallbacks  (This, XoNcollectCallback);

  CStringDelete         (athis.virtualColormap);
  CStringDelete         (athis.postScriptFile);
  CStringDelete         (athis.gifFile);
  CStringDelete         (athis.dataFile);

  XtReleaseGC           (This,athis.gc);
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
  int DoRedraw = False;
/*.........................................................................*/
  if(This->core.background_pixel!=a_current->core.background_pixel)
    {
      XSetForeground (XtDisplay(This),athis.gc,This->core.background_pixel);
      DoRedraw       = True;
    }

  IFSMOD(virtualColormap) 
    {
      CStringDelete         (acur.virtualColormap);
      athis.virtualColormap = CStringDuplicate (athis.virtualColormap);
      athis.ocmap           = OColormapGetIdentifierFromString (athis.virtualColormap);
      DoRedraw              = True;
    }
  else {athis.virtualColormap = acur.virtualColormap;}

  IFSMOD(postScriptFile)
    { 
      CStringDelete         (acur.postScriptFile);
      athis.postScriptFile  = CStringDuplicate (athis.postScriptFile);
    }
  else {athis.postScriptFile = acur.postScriptFile;}

  IFSMOD(gifFile)
    { 
      CStringDelete         (acur.gifFile);
      athis.gifFile         = CStringDuplicate (athis.gifFile);
    }
  else {athis.gifFile = acur.gifFile;}

  IFSMOD(dataFile)
    { 
      CStringDelete         (acur.dataFile);
      athis.dataFile        = CStringDuplicate (athis.dataFile);
      LoadData              (This);
      DoRedraw              = True;
    }
  else {athis.dataFile = acur.dataFile;}

  a_request = NULL;
  a_args    = NULL;
  a_argn    = 0; /*avoid C++ warnings*/

  return    DoRedraw;
}
/***************************************************************************/
static void ChangeWidgetSize (
 Widget This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
/* Call the Resize procedure (XtInheritResize) */
  if(xoImageWidgetClass->core_class.superclass->core_class.resize!=NULL)
    (xoImageWidgetClass->core_class.superclass->core_class.resize)(This);

  DrawWidget (This,NULL,NULL);
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
  XImage*    ximage;
  XImage*    simage;
  int        compress,ulx,uly;
  unsigned   int width,height;
  XImage*    pimage;
  int        fits = 0;
/*.........................................................................*/
  if(xoImageWidgetClass->core_class.superclass->core_class.expose!=NULL)
    (xoImageWidgetClass->core_class.superclass->core_class.expose)(This,a_event,a_region);

  if(OColormapIsValid(athis.ocmap)==0)   
    {
      CWarnF("Widget %s has no OColormap.\n",XtName(This));
      return;
    }
  if(OImageIsValid(athis.oimage)==0) return;

  GetImagePart          (This,athis.part.x,athis.part.y);

  ximage                = (XImage*)OImageGetX11_Image(athis.oimage);
  if(ximage==NULL)      return;
  simage                = XSubImage (ximage,athis.part.x,
                                            athis.part.y,
                                            (unsigned int)athis.part.width,
                                            (unsigned int)athis.part.height);
  if(simage==NULL)      return;

/*For fits images convol with athis.olut.*/
  fits = 0;
  if( (simage->format == ZPixmap)  && 
      (simage->bits_per_pixel==16) && 
      (OLUT_IsValid(athis.olut)==1) 
     )
    {
      fits = 1;
      if(ConvoluteImage ((short*)(simage->data),
			 (unsigned int)(simage->width),
			 (unsigned int)(simage->height),
			 OLUT_GetSize(athis.olut),OLUT_GetList(athis.olut))==0)
       ReportError ("DrawWidget",CErrorFlow);
/*
  printf ("debug:pix  sub:%d %d(14:16:20482)(must be 15)\n",((short*)simage->data)[16 * simage->width + 14],
                                                            XGetPixel(simage,14,16)); */
    }
/*At this step, image contains unsigned data.*/
/*Adapt image to screen.
  Could loose color info if screen_bits_per_pixel>image_bits_per_pixel.
  This is the case of a 16 bit image on a 24 planes screen (32 bits/pixel).
  Loose info when putting an unsigned int on a short.
*/
  if(     (simage->depth         !=XDisplayGetDepth(XtDisplay(This))) ||
          (simage->bits_per_pixel <XDisplayGetBitsPerPixel(XtDisplay(This)))  
    )
    { XImage*        image;
      image          = XDisplayAdaptImageToScreen (XtDisplay(This),simage);
      XDestroyImage  (simage);
      simage         = image;
    }

/*ocmap index ->pixel*/
 {int      pixeln;
  Pixel*   pixels;
  pixeln   = OColormapGetSize(athis.ocmap);
  pixels   = OColormapXGetPixels(athis.ocmap,XtDisplay(This),fits==1 ? 0:1,(double)athis.pixelPrecision);
  if(XImageConvolute(simage,pixeln,pixels)==1) ReportError ("DrawWidget",CErrorFlow);
  CMemoryFreeBlock (pixels);
  }

/*Now we have X pixels in simage; compress and expand will work.*/
  ulx                   = athis.tx;
  uly                   = athis.ty;
  compress              = GetCompressFactor (athis.scale);
       if(compress<0)       
    {
      pimage        = XDisplayCompressImage (XtDisplay(This),simage,-compress);
      XDestroyImage (simage);
      ulx           = (ulx>=0  ? 0:-ulx * -compress);
      uly           = (uly>=0  ? 0:-uly * -compress);
    }
  else if(compress>0) 
    {
      pimage        = XDisplayExpandImage   (XtDisplay(This),simage, compress);
      XDestroyImage (simage);
      ulx           = (ulx>=0  ? 0:-ulx * compress);
      uly           = (uly>=0  ? 0:-uly * compress);
    }
  else   
    { 
      pimage       = simage;
      ulx          = (ulx>=0  ? 0:-ulx);
      uly          = (uly>=0  ? 0:-uly);
    }
  XImageGetSize (pimage,&width,&height);
/* clear discovered parts */
  if(ulx>0)
    {
      XFillRectangle     (XtDisplay(This),XtWindow(This),athis.gc,0,0,ulx,This->core.height);
    }
  if(uly>0)
    {
      XFillRectangle     (XtDisplay(This),XtWindow(This),athis.gc,0,0,This->core.width,uly);
    }
  if(ulx+width <This->core.width)  
    {
      XFillRectangle  (XtDisplay(This),XtWindow(This),athis.gc,ulx+width,0,This->core.width-(ulx+width),This->core.height);
    }
  if(uly+height <This->core.height)
    {
      XFillRectangle (XtDisplay(This),XtWindow(This),athis.gc,0,uly+height,This->core.width,This->core.height-(uly+height));
    }

  if(pimage!=NULL) 
    { 
      XPutImage     (XtDisplay(This),XtWindow(This),athis.gc,pimage,0,0,ulx,uly,width,height);
      XDestroyImage (pimage);
    }

  a_event          = NULL;
  a_region         = NULL;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
void XoImageClearClass (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  OColormapXClearClass     ();
  OColormapClearClass      ();
  OImageClearClass     ();
  OMatrix33ClearClass    ();
  OLUT_ClearClass       ();
}
/***************************************************************************/
void XoImageDraw (
 Widget This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if( (This==NULL) || !XtIsSubclass(This,xoImageWidgetClass) ) return;
  DrawWidget (This,NULL,NULL);
}
/***************************************************************************/
void XoImageRestoreDefaultTransformation (
 Widget This 
)
/***************************************************************************/
{
/*.........................................................................*/
  if( (This==NULL) || !XtIsSubclass(This,xoImageWidgetClass) ) return;
  athis.tx     = 0;
  athis.ty     = 0;
  athis.scale  = 1.;
  GetImagePart    (This,0,0);
  DrawWidget (This,NULL,NULL);
}
/***************************************************************************/
void XoImageTranslate (
 Widget This  
,int    a_x  
,int    a_y  
)
/***************************************************************************/
/*
  translate 1 ,on an expanded by n image, implies on screen 
  translate pixmap by n pixels
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if( (This==NULL) || !XtIsSubclass(This,xoImageWidgetClass) ) return;
  athis.tx    += a_x;
  athis.ty    += a_y;
  GetImagePart    (This,athis.tx,athis.ty);
  DrawWidget (This,NULL,NULL);
}
/***************************************************************************/
void XoImageScale (
 Widget This  
,double a_scale 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if( (This==NULL) || !XtIsSubclass(This,xoImageWidgetClass) ) return;
  if(a_scale<=0.)    return;
  athis.scale       *= a_scale;
  GetImagePart          (This,athis.part.x,athis.part.y);
  DrawWidget       (This,NULL,NULL);
}
/***************************************************************************/
void XoImageTranslateScreen (
 Widget This  
,int  a_x  
,int  a_y  
)
/***************************************************************************/
/*
  translate n ,on an expanded by n, image implies 
  translate pixmap by 1 pixels
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int compress,tx,ty;
/*.........................................................................*/
  if( (This==NULL) || !XtIsSubclass(This,xoImageWidgetClass) ) return;
  tx                     = a_x;
  ty                     = a_y;
  compress               = GetCompressFactor (athis.scale);
       if (compress<0)   {tx *= (-compress);ty *= (-compress);}
  else if (compress>0)   {tx /=  compress;  ty /=  compress;}
  XoImageTranslate       (This,tx,ty);
}
/***************************************************************************/
void XoImageCenter (
 Widget This 
,int a_x 
,int a_y 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if( (This==NULL) || !XtIsSubclass(This,xoImageWidgetClass) ) return;
  XoImageTranslateScreen (This,a_x - (This->core.width/2),a_y - (This->core.height/2));
}
/***************************************************************************/
void XoImageMagnify (
 Widget This   
,int a_beg_x
,int a_beg_y
,int a_end_x
,int a_end_y
)
/***************************************************************************/
{
  ORectRecord  box;
  double    s_x,s_y;
/*.........................................................................*/
  if( (This==NULL) || !XtIsSubclass(This,xoImageWidgetClass) ) return;
  if( (a_beg_x==a_end_x) ||  (a_beg_y==a_end_y)) return;
  SetRectangleWithBeginEnd           (&box,a_beg_x,a_beg_y,a_end_x,a_end_y);
  s_x                   = This->core.width  / box.width;
  s_y                   = This->core.height / box.height;
  XoImageTranslateScreen       (This,box.x,box.y);
  XoImageScale          (This,MINIMUM(s_x,s_y));
}
/***************************************************************************/
XImage* XoImageGetPart (
 Widget This   
,int a_beg_x
,int a_beg_y
,int a_end_x
,int a_end_y
)
/***************************************************************************/
{
  ORectRecord box;
  XImage*  ximage;
  XImage*  simage;
  int      begx,begy,endx,endy;
/*.........................................................................*/
  if( (This==NULL) || !XtIsSubclass(This,xoImageWidgetClass) ) return NULL;
  if( (a_beg_x==a_end_x) ||  (a_beg_y==a_end_y) ) return NULL;
  if(OImageIsValid(athis.oimage)==0) return NULL;

  ximage                  = (XImage*)OImageGetX11_Image(athis.oimage);
  if(ximage==NULL)             return NULL;

  begx                    = a_beg_x;
  begy                    = a_beg_y;
  endx                    = a_end_x;
  endy                    = a_end_y;

  XoImageGetLocalPosition            (This,&begx,&begy);
  XoImageGetLocalPosition            (This,&endx,&endy);

  SetRectangleWithBeginEnd             (&box,begx,begy,endx,endy);
  simage                  = XSubImage (ximage,box.x,
                                              box.y,
                                              (unsigned int)box.width,
                                              (unsigned int)box.height);

  return                  simage;
}
/***************************************************************************/
void XoImageGetCollectedRegion (
 Widget This   
,int* a_beg_x
,int* a_beg_y
,int* a_end_x
,int* a_end_y
)
/***************************************************************************/
{
/*.........................................................................*/
  if(a_beg_x!=NULL) *a_beg_x = 0;
  if(a_beg_y!=NULL) *a_beg_y = 0;
  if(a_end_x!=NULL) *a_end_x = 0;
  if(a_end_y!=NULL) *a_end_y = 0;
  if( (This==NULL) || !XtIsSubclass(This,xoImageWidgetClass) ) return;
  if(a_beg_x!=NULL) *a_beg_x = athis.beg_x;
  if(a_beg_y!=NULL) *a_beg_y = athis.beg_y;
  if(a_end_x!=NULL) *a_end_x = athis.end_x;
  if(a_end_y!=NULL) *a_end_y = athis.end_y;
}
/***************************************************************************/
void XoImageGetLocalPosition (
 Widget This 
,int* a_x
,int* a_y
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int       compress;
  int       ulx,uly;
  int       px,py;
/*.........................................................................*/
  if(a_x!=NULL)      *a_x = 0;
  if(a_y!=NULL)      *a_y = 0;
  if( (This==NULL) || !XtIsSubclass(This,xoImageWidgetClass) ) return;
  compress         = GetCompressFactor (athis.scale);
  ulx              = athis.tx;
  uly              = athis.ty;
  if (compress<0)       
    {
      ulx                    = (ulx>=0  ? 0:-ulx * -compress);
      uly                    = (uly>=0  ? 0:-uly * -compress);
    }
  else if (compress>0) 
    {
      ulx                    = (ulx>=0  ? 0:-ulx * compress);
      uly                    = (uly>=0  ? 0:-uly * compress);
    }
  else   
    {
      ulx                   = (ulx>=0  ? 0:-ulx);
      uly                   = (uly>=0  ? 0:-uly);
    }
  px           = *a_x;
  py           = *a_y;
  px          -= ulx;
  py          -= uly;
       if (compress<0)  {px  *= (-compress);py *= (-compress);}
  else if (compress>0)  {px  /=  compress;  py /=  compress;}
  px           += athis.part.x;
  py           += athis.part.y;
  if(a_x!=NULL)      *a_x = px;
  if(a_y!=NULL)      *a_y = py;
}
/***************************************************************************/
void XoImageGetLocalBox (
 Widget This 
,int a_beg_x
,int a_beg_y
,int a_end_x
,int a_end_y
,int* a_x
,int* a_y
,int* a_w
,int* a_h
)
/***************************************************************************/
/* Input are in window coordinate. Output are in image coordinate.         */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int            begx,begy,endx,endy;
  ORectRecord       want,part;
  unsigned int   iwidth,iheight;
/*.........................................................................*/
  if(a_x!=NULL)        *a_x = 0;
  if(a_y!=NULL)        *a_y = 0;
  if(a_w!=NULL)        *a_w = 0;
  if(a_h!=NULL)        *a_h = 0;
  if( (This==NULL) || !XtIsSubclass(This,xoImageWidgetClass) ) return;
  begx           = a_beg_x;
  begy           = a_beg_y;
  endx           = a_end_x;
  endy           = a_end_y;
  XoImageGetLocalPosition   (This,&begx,&begy);
  XoImageGetLocalPosition   (This,&endx,&endy);
  SetRectangleWithBeginEnd    (&want,begx,begy,endx,endy);
  OImageGetSizeAndBytes (athis.oimage,&iwidth,&iheight);
  SetRectangleWithPart      (&part,iwidth,iheight,&want);
  if(a_x!=NULL)        *a_x = part.x;
  if(a_y!=NULL)        *a_y = part.y;
  if(a_w!=NULL)        *a_w = part.width;
  if(a_h!=NULL)        *a_h = part.height;
}
/***************************************************************************/
String XoImageGetInformations (
 Widget This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* iname;
  char* lname;
  char* cname;
  String string;
/*.........................................................................*/
  if( (This==NULL) || !XtIsSubclass(This,xoImageWidgetClass) ) return NULL;

  string = (char*)CStringCreate (512);
  if(string==NULL) return NULL;

  cname  = OColormapGetName       (athis.ocmap);
  iname  = OImageGetName      (athis.oimage);
  lname  = OLUT_GetName        (athis.olut);
  sprintf(string,
"\
    widget:%s\n\
  colormap:%s\n\
     image:%s\n\
       lut:%s\n\
",This->core.name,cname!=NULL ? cname : "(nil)",
                  iname!=NULL ? iname : "(nil)",
                  lname!=NULL ? lname : "(nil)");

  return string;
}
/***************************************************************************/
void XoImagePutPixmapInPS_Stream (
 Widget This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if( (This==NULL) || !XtIsSubclass(This,xoImageWidgetClass) ) return;
  XWidgetPutPixmapInPS_Stream (This);
}
/***************************************************************************/
void* XoImageGetImage (
 Widget This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if( (This==NULL) || !XtIsSubclass(This,xoImageWidgetClass) ) return NULL;
  return athis.oimage;
}
/***************************************************************************/
void* XoImageGetLUT (
 Widget This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if( (This==NULL) || !XtIsSubclass(This,xoImageWidgetClass) ) return NULL;
  return athis.olut;
}
/***************************************************************************/
void* XoImageGetColormap (
 Widget This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if( (This==NULL) || !XtIsSubclass(This,xoImageWidgetClass) ) return NULL;
  return athis.ocmap;
}
/***************************************************************************/
void XoImageSetColormap (
 Widget This
,void* a_ocmap 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if( (This==NULL) || !XtIsSubclass(This,xoImageWidgetClass) ) return;
  if(a_ocmap==athis.ocmap) return;
  athis.ocmap   = (OColormap)a_ocmap;
  XoImageDraw (This);
}
/***************************************************************************/
void XoImageAttachImageColormapLUT (
 Widget  This  
,void*   a_image 
,void*   a_cmap  
,void*   a_lut   
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if( (This==NULL) || !XtIsSubclass(This,xoImageWidgetClass) ) return;
  AttachImage  (This,a_image,a_lut,a_cmap);
  XoImageDraw (This);
}
/***************************************************************************/
void XoImageAddPopupEntry (
 Widget This
,char* a_label
,XtCallbackProc a_proc
,XtPointer a_data
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*             name;
  Arg               args[1];
  Widget            widget = NULL;
/*.........................................................................*/
  if(This==NULL)    return;
  if(a_label==NULL) return;
  if(a_proc==NULL)  return;
  CreatePopup       (This);
  if(Class.popup==NULL) return;
  if(FindPopupEntry(a_label)!=NULL) return;
  name             = CStringCreateF (14+strlen(a_label),"xoimage_popup_%s",a_label);
#ifdef HAS_XM
  widget           = XmCreatePushButtonGadget (Class.popup,name,args,0);
 {XmString         cps;
  cps              = XmStringLtoRCreate(a_label,XmSTRING_DEFAULT_CHARSET);
  XtSetArg         (args[0],XmNlabelString,cps);
  XtSetValues      (widget,args,1);
  XmStringFree     (cps);}
  XtAddCallback    (widget,XmNactivateCallback,a_proc,(XtPointer)a_data);
#endif /*HAS_XM*/
#ifdef HAS_XAW
  widget           = XtCreateManagedWidget(name,smeBSBObjectClass,Class.popup,args,0);
  XtSetArg         (args[0],XtNlabel,a_label);
  XtSetValues      (widget,args,1);
  XtAddCallback    (widget,XtNcallback,a_proc,(XtPointer)a_data);
#endif /*HAS_XM*/
  if(widget!=NULL) XtManageChild    (widget);
  CStringDelete    (name);
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static void GetImagePart (
 Widget This
,int a_ulx
,int a_uly 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int      iw,ih,compress;
/*.........................................................................*/
  compress = GetCompressFactor (athis.scale);
  iw       = This->core.width;
  ih       = This->core.height;
  if(compress<0)
    {
      iw    *= -compress;
      ih    *= -compress;
    }
  else if(compress>0)
    {
      iw    = iw / compress + 1;
      ih    = ih / compress + 1;
    }
  if((iw>0)&&(ih>0)) 
    { ORectRecord     want;
      want.x       = a_ulx;
      want.y       = a_uly;
      want.width   = iw;
      want.height  = ih;
      SetRectangleWithPart    (&(athis.part),athis.iwidth,athis.iheight,&want);
    }
}
/***************************************************************************/
static int GetCompressFactor (
 double a_scale  
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
       if(a_scale<=0.)    return 0;
  else if(a_scale==1.)    return 0;
  else if(a_scale<1.)     return - DBL_INT (1./a_scale);
  else                    return   DBL_INT a_scale;
}
/***************************************************************************/
static void MoveAction (
 Widget This
,XEvent* a_event
,String* a_args
,Cardinal* a_argn
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  static  struct  
  {
    int      ox,oy;
  } pointer = {0,0};
/*.........................................................................*/
  if(This==NULL) return;
  switch(a_event->type)
    {
      case ButtonPress:
            if(Class.cursor_fleur==0L)   
	      Class.cursor_fleur          = XCreateFontCursor(XtDisplay(This),XC_fleur);
            if(Class.cursor_top_left_arrow==0L) 
	      Class.cursor_top_left_arrow = XCreateFontCursor(XtDisplay(This),XC_top_left_arrow);
            XDefineCursor     (XtDisplay(This),XtWindow(This),Class.cursor_fleur) ;
            pointer.ox        = a_event->xbutton.x;
            pointer.oy        = a_event->xbutton.y;
            break;
      case ButtonRelease:
            XDefineCursor     (XtDisplay(This),XtWindow(This),Class.cursor_top_left_arrow) ;
            break;
      case MotionNotify:
            XoImageTranslate  (This,pointer.ox-a_event->xmotion.x,
                                    pointer.oy-a_event->xmotion.y);
            pointer.ox       = a_event->xmotion.x;
            pointer.oy       = a_event->xmotion.y;
            break;
    }
  a_args = NULL; /* to remove warning of c++ compiler */
  a_argn = 0; /* to remove warning of c++ compiler */
}
/***************************************************************************/
static void CollectAction (
 Widget This
,XEvent* a_event
,String* a_args
,Cardinal* a_argn
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  static  struct  
  {
    XPoint  beg,end;
  } pointer = {{0,0},{0,0}};
/*.........................................................................*/
  switch(a_event->type)
    {
      case ButtonPress:
            pointer.end.x    = pointer.beg.x      = a_event->xbutton.x;
            pointer.end.y    = pointer.beg.y      = a_event->xbutton.y;
            XDisplayDrawRectangle (XtDisplay(This),XtWindow(This),athis.rubberGC,&pointer.beg,&pointer.end);
            break;
      case ButtonRelease:
            XDisplayDrawRectangle (XtDisplay(This),XtWindow(This),athis.rubberGC,&pointer.beg,&pointer.end);

	    athis.beg_x          = pointer.beg.x;
            athis.beg_y          = pointer.beg.y;
            athis.end_x          = pointer.end.x;
            athis.end_y          = pointer.end.y;

	    if( (a_args[0]!=NULL) &&(strcmp(a_args[0],"Zoom")==0) )
  	      XoImageMagnify          (This,athis.beg_x,athis.beg_y,athis.end_x,athis.end_y);

           {XoAnyCallbackStruct  value;
            value.reason         = 1;
            value.event          = a_event;
            XtCallCallbacks      (This, XoNcollectCallback,(XtPointer)&value);}

            break;
      case MotionNotify:
/* Destroy previous box by redrawing it !!! (in mode COMP this is equivallent to a deletion ) */
            XDisplayDrawRectangle (XtDisplay(This),XtWindow(This),athis.rubberGC,&pointer.beg,&pointer.end);
            pointer.end.x = a_event->xmotion.x;
            pointer.end.y = a_event->xmotion.y;
            XDisplayDrawRectangle (XtDisplay(This),XtWindow(This),athis.rubberGC,&pointer.beg,&pointer.end);
            break;
    }
  a_argn = 0; /* to remove warning of c++ compiler */
}
/***************************************************************************/
static void ShowAction (
 Widget This
,XEvent* a_event
,String* a_args
,Cardinal* a_argn
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Arg  args[1];
/*.........................................................................*/
  if(This==NULL) return;
  switch(a_event->type)
    {
      case ButtonPress:
            if(athis.showPixelWidget!=NULL)
              { 
#ifdef HAS_XM
		XmString       cps;
                cps            = XmStringLtoRCreate("000 000: 000: 000 000 000",XmSTRING_DEFAULT_CHARSET);
                XtSetArg       (args[0],"labelString",cps);
                XtSetValues    (athis.showPixelWidget,args,1);
                XmStringFree   (cps);
                XtMapWidget    (athis.showPixelWidget);
#endif /*HAS_XM*/
#ifdef HAS_XAW
                XtSetArg       (args[0],"label","000 000: 000: 000 000 000");
                XtSetValues    (athis.showPixelWidget,args,1);
                XtMapWidget    (athis.showPixelWidget);
#endif /*HAS_XAW*/
              }
            break;
      case ButtonRelease:
            break;
      case MotionNotify:
           {int    x,y;
            static char string[64];
            x                        = a_event->xmotion.x;
            y                        = a_event->xmotion.y;
            XoImageGetLocalPosition             (This,&x,&y);
           {long                     v_long;
            unsigned long            v_ulong;
            double                   red,green,blue;
            if(OImageGetPixel(athis.oimage,x,y,&v_long,&v_ulong,&red,&green,&blue)==1)
              {
		Uchar         ured,ugreen,ublue;
                ured          = (Uchar)(red   * 255.);
                ugreen        = (Uchar)(green * 255.);
                ublue         = (Uchar)(blue  * 255.);
		sprintf       (string,"%d %d: %ld: %d %d %d",x,y,v_long,ured,ugreen,ublue);
	      }
            else
	      {
		Uchar         ured,ugreen,ublue;
                ured          = (Uchar)(red   * 255.);
                ugreen        = (Uchar)(green * 255.);
                ublue         = (Uchar)(blue  * 255.);
	        sprintf       (string,"%d %d: %lu: %d %d %d",x,y,v_ulong,ured,ugreen,ublue);
	      }} 
            if(athis.showPixelWidget!=NULL)
              { 
#ifdef HAS_XM
		XmString       cps;
                cps            = XmStringLtoRCreate(string,XmSTRING_DEFAULT_CHARSET);
                XtSetArg       (args[0],"labelString",cps);
                XtSetValues    (athis.showPixelWidget,args,1);
                XmStringFree   (cps);
#endif /*HAS_XM*/
#ifdef HAS_XAW
                XtSetArg       (args[0],"label",string);
                XtSetValues    (athis.showPixelWidget,args,1);
#endif /*HAS_XAW*/
              }
            }
            break;
    }
  a_args = NULL; /* to remove warning of c++ compiler */
  a_argn = 0; /* to remove warning of c++ compiler */
}
/***************************************************************************/
static void PopupAction (
 Widget    This 
,XEvent*   a_event 
,String*   a_args
,Cardinal* a_argn
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_event==NULL)                    return;
  if(a_event->type!=ButtonPress)       return;
  if(a_event->xbutton.button!=Button3) return;

  CreatePopup (This);

  if(Class.popup!=NULL)
    { 
#ifdef HAS_XM
      XmMenuPosition    (Class.popup,(XButtonPressedEvent*)a_event);
      XtManageChild     (Class.popup);
#endif /*HAS_XM*/
#ifdef HAS_XAW
     {Arg                 args[2];
      XtSetArg            (args[0], XtNx, ((XButtonPressedEvent*)a_event)->x_root);
      XtSetArg            (args[1], XtNy, ((XButtonPressedEvent*)a_event)->y_root);
      XtSetValues         (Class.popup, args,2);
      XtPopupSpringLoaded (Class.popup);}
#endif /*HAS_XM*/
      Class.This        = This;
      Class.This_event  = *a_event;
    }

  a_args  = NULL; 
  a_argn  = 0;
}
/***************************************************************************/
static void CreatePopup (
 Widget This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)        return;
  if(Class.popup!=NULL) return; /*Done.*/

#ifdef HAS_XM
 {Arg         args[1];
  XtSetArg    (args[0],XmNwhichButton,Button3);
  Class.popup = XmCreatePopupMenu (This,"xoimage_popup",args,1);}
#endif /*HAS_XM*/
#ifdef HAS_XAW
 {Arg         args[1];
  Class.popup = XtCreatePopupShell ("xoimage_popup",simpleMenuWidgetClass,This,args,0);}
#endif /*HAS_XAW*/

  if(Class.popup==NULL) return;

  XtAddCallback  (Class.popup   ,XtNdestroyCallback ,ActivateCallback,(XtPointer)115);

/*Default menu.*/
  XoImageAddPopupEntry     (This,"Zoom in"   ,ActivateCallback,(XtPointer)111);
  XoImageAddPopupEntry     (This,"Zoom out"  ,ActivateCallback,(XtPointer)222);
  XoImageAddPopupEntry     (This,"Reset"     ,ActivateCallback,(XtPointer)777);
  XoImageAddPopupEntry     (This,"Center"    ,ActivateCallback,(XtPointer)666);
  XoImageAddPopupEntry     (This,"Target"    ,ActivateCallback,(XtPointer)999);
  if(CSystemIsSecured()==0)
    {
  XoImageAddPopupEntry     (This,"GIF"       ,ActivateCallback,(XtPointer)333);
  XoImageAddPopupEntry     (This,"PostScript",ActivateCallback,(XtPointer)888);
    }
}
/***************************************************************************/
static void ActivateCallback (
 Widget    This 
,XtPointer a_tag
,XtPointer a_data
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
       if((long)a_tag==111) XoImageScale        (Class.This,2.);
  else if((long)a_tag==222) XoImageScale        (Class.This,0.5);
  else if((long)a_tag==999) XWidgetSetTarget    (Class.This);
  else if((long)a_tag==777) XoImageRestoreDefaultTransformation        (Class.This);
  else if((long)a_tag==888)
    { 
      XWidgetSetCursor                (Class.This,"watch");
      CPS_OpenFileForWriting          (((XoImageWidget)Class.This)->image.postScriptFile);
      CPS_SetBitsPerPixel             (8);
      CPS_SetColorScheme              (CPS_Color);
      XWidgetPutPixmapPageInPS_Stream (Class.This);
      CPS_CloseStream                 ();
      XWidgetSetCursor                (Class.This,NULL);
    }
  else if((long)a_tag==333)
    { 
      XWidgetSetCursor            (Class.This,"watch");
      XWidgetSavePixmapInGIF_File (Class.This,((XoImageWidget)Class.This)->image.gifFile);
      XWidgetSetCursor            (Class.This,NULL);
    }
  else if((long)a_tag==666) 
    {
      XoImageCenter (Class.This,Class.This_event.xbutton.x,Class.This_event.xbutton.y);
    }
  else if((long)a_tag==444) /*Unmap show pixel button.*/
    {
       XtUnmapWidget (This);
    }
  else if((long)a_tag==115)
    { 
      Class.popup = NULL;
    }

  This   = NULL;
  a_tag  = NULL;
  a_data = NULL;
}
/***************************************************************************/
static int ConvoluteImage (
 short*  a_items
,unsigned int   a_width
,unsigned int   a_height
,int    a_celln
,unsigned char*  a_cells
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  unsigned int row,col;
  int ok;
/*.........................................................................*/
  if(a_items==NULL)                   return 0;
  if( (a_width==0) || (a_height==0) ) return 0;   
  ok = 1;
  for (row=0;row<a_height;row++)
    { 
#define short_max 32767
       short* itema;
      itema = a_items + row * a_width;
      for (col=0;col<a_width;col++)
        { unsigned short value;
	  value = (*itema) + short_max;  /*short -> unsigned short*/
	  if(value>=a_celln)
	    {
	      ok = 0;
	    }
	  else
	    {
	      *itema = (short)a_cells[value]; /*unsigned short -> ocmap index*/
	    }
          itema++;
        }
    }
  return ok;
}
/***************************************************************************/
static void CreateWidget (
 Widget This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Arg                   args[3];
/*.........................................................................*/
#ifdef HAS_XM
  athis.showPixelWidget = XmCreatePushButton (This,"IMAGE_PIXEL",args,0);
  XtManageChild         (athis.showPixelWidget);
  XtAddCallback         (athis.showPixelWidget,XmNactivateCallback,ActivateCallback,(XtPointer)444);
#endif /*HAS_XM*/
#ifdef HAS_XAW
  athis.showPixelWidget = XtCreateWidget ("IMAGE_PIXEL",commandWidgetClass,This,args,0);
  XtManageChild         (athis.showPixelWidget);
  XtAddCallback         (athis.showPixelWidget,XtNcallback,ActivateCallback,(XtPointer)444);
#endif /*HAS_XAW*/
  This = NULL;
}
/***************************************************************************/
static void SetRectangleWithBeginEnd (
 ORect This  
,int a_beg_x
,int a_beg_y
,int a_end_x
,int a_end_y
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  This->x      = MINIMUM (a_beg_x,a_end_x);
  This->y      = MINIMUM (a_beg_y,a_end_y);
  This->width  = MAXIMUM (a_beg_x,a_end_x) - This->x;
  This->height = MAXIMUM (a_beg_y,a_end_y) - This->y;
}
/***************************************************************************/
static void SetRectangleWithPart (
 ORect This
,int   a_width  
,int   a_height 
,ORect a_want   
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int   wbrx,wbry;
  int   pbrx,pbry;
/*.........................................................................*/
  if(This==NULL)     return;
  wbrx           = a_want->x + a_want->width;
  wbry           = a_want->y + a_want->height;
  This->x        = (a_want->x<0)||(a_want->x>=a_width)  ? 0 : a_want->x;
  This->y        = (a_want->y<0)||(a_want->y>=a_height) ? 0 : a_want->y;
  pbrx           = (wbrx>=a_width )||(wbrx<0) ? a_width  : wbrx;
  pbry           = (wbry>=a_height)||(wbry<0) ? a_height : wbry;
  This->width    = pbrx-This->x;
  This->height   = pbry-This->y;
}
/***************************************************************************/
static void AttachImage (
 Widget  This  
,void*   a_image 
,void*   a_cmap  
,void*   a_lut   
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  ORectRecord     want,part;
  int          dw,dh;
  unsigned int width,height;
/*.........................................................................*/
  if(a_image==NULL) return;
  if(OImageIsValid((OImage)a_image)==0) return;
  OImageGetSizeAndBytes    ((OImage)a_image,&width,&height);
  dw                = This->core.width;
  dh                = This->core.height;
  want.x            = 0;
  want.y            = 0;
  want.width        = dw;
  want.height       = dh;
  SetRectangleWithPart         (&part,width,height,&want);
  athis.oimage      = (OImage)a_image;
  athis.iwidth      = width;
  athis.iheight     = height;
  athis.tx          = 0;
  athis.ty          = 0;
  athis.scale       = 1.;
  athis.part        = part;
  athis.olut        = (OLUT)a_lut;

  if(a_cmap==NULL)
    {
      int           ncolor;
      double*       reds;
      double*       greens;
      double*       blues;
      OImageGetRGBs (athis.oimage,&ncolor,&reds,&greens,&blues);
      athis.ocmap   = OColormapGetIdentifierFromRGBs(ncolor,reds,greens,blues);
      if(athis.ocmap==NULL)  
	athis.ocmap = OColormapCreate(NULL,ncolor,reds,greens,blues);
    }
  else
    {
      athis.ocmap = (OColormap)a_cmap;
    }
}
/***************************************************************************/
static void LoadData (
 Widget This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*  line;
  char*  fname;
  OImage oimage;
/*.........................................................................*/
  if(This==NULL)   return;
  line        = athis.dataFile;
  if( (line==NULL) || (*line=='\0') ) return;

  fname       = strstr(line,">");
  fname       = fname!=NULL ? fname+1: line;

       if ( (strncmp(line,"gif>",4)==0) || 
	    (strstr(line,".gif")!=NULL) ||
	    (strstr(line,".GIF")!=NULL)   
	   )
    { 
      oimage      = OImageCreateFromGIF_File(XtDisplay(This),fname);
      if(oimage==NULL)
	CWarnF("Can't read GIF file %s.\n",fname);
      else
	AttachImage (This,oimage,NULL,NULL);
    }
  else if ( (strncmp(line,"xbm>",4)==0) || 
	    (strstr(line,".xbm")!=NULL) ||
	    (strstr(line,".XBM")!=NULL)   
	   )
    { 
      oimage      = OImageCreateFromXBM_File(XtDisplay(This),fname);
      if(oimage==NULL)
	CWarnF("Can't read XBM file %s.\n",fname);
      else
	AttachImage (This,oimage,NULL,NULL);
    }
  else if ( (strncmp(line,"tiff>",5)==0) || 
	    (strstr(line,".tiff")!=NULL) ||
	    (strstr(line,".TIFF")!=NULL)   
	   )
    { 
      oimage      = OImageCreateFromTIFF_File(XtDisplay(This),fname);
      if(oimage==NULL)
	CWarnF("Can't read TIFF file %s.\n",fname);
      else
	AttachImage (This,oimage,NULL,NULL);
    }
  else if ( 
 	    (strncmp(line,"fits>",5)==0) || 
 	    (strncmp(line,"fit>",4)==0) || 
	    (strstr(line,".fit")!=NULL) ||
	    (strstr(line,".FIT")!=NULL)   
	   )
    { 
      char*                senv;
      int                  swap = 0;
      OImage               oimage;
      senv                 = getenv ("XOFITSSWAP");
      swap                 = ( (senv!=NULL) && (strcmp(senv,"1")==0) ) ? 1 : 0;
      oimage               = OImageCreateFromFITS_File(XtDisplay(This),fname,0,0,0,0,swap);
      if(oimage!=NULL)
        {
	  OLUT                 olut;
	  unsigned int         width,height;
	  int                  min,max,nbin;
	  OColormap            ocmap;
	  short*               data;
	  data                 = (short*)OImageGetSizeAndBytes(oimage,&width,&height);
	  CImageshortGetLUT    (data,width,height,&min,&max,&nbin);
	  olut                 = OLUT_Create (NULL,65536);
	  OLUT_Initialize      (olut,OLUT_Linear,min+short_max,max+short_max,nbin);
	  ocmap                = OColormapGetIdentifierFromString("ocolormap_16_color");
	  AttachImage         (This,oimage,ocmap,olut);
	}
      else
	{
	  CWarnF("Can't read FITS file %s.\n",fname);
	}
    }
  

}
/***************************************************************************/
static Widget FindPopupEntry (
 char* a_label
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  WidgetList items,itema;
  char*      name;
  Widget     entry = NULL;
/*.........................................................................*/
  if(a_label==NULL)     return NULL;
  if(Class.popup==NULL) return NULL;
  name  = CStringCreateF (14+strlen(a_label),"xoimage_popup_%s",a_label);
  items = XWidgetGetChildren (Class.popup,NULL);
  entry = NULL;
  for(itema=items;(itema!=NULL) && (*itema!=NULL);itema++) 
    {
      char*  string;
      string = XtName(*itema);
      if( (string!=NULL) && (strcmp(name,string)==0) ) /*Already exists*/
	{
	  entry = *itema;
          break;
	}
    }
  CListDelete   ((CList)items);
  CStringDelete (name);
  return        entry;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static void ReportError (
 char* a_name
,int a_code
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CErrorHandle ("Xo","XoImage",a_name,a_code);
}
