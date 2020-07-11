/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include <CPrinter.h>
#include <CMemory.h>
#include <CFile.h>
#include <CColor.h>
#include <CGIF.h>
#include <CPS.h>
#include <CString.h>
#include <CError.h>
#include <OColor.h>

#include <XDisplay.h>

#include <X11/Xutil.h>
#include <X11/Xatom.h>

#ifndef XlibSpecificationRelease
#define XlibSpecificationRelease 4
#endif

#ifdef HAS_XEXT
#include <shape.h>
/*
#define ShapeBounding 0
#define ShapeSet      0
*/
#endif

typedef unsigned long Pixel;
typedef unsigned long Ulong;
typedef unsigned int  Uint;
typedef unsigned char Uchar;


#ifdef __cplusplus
extern "C"{
#endif
int    _XGetBitsPerPixel (Display*,int); /*In X/XImUtil.c */

static char*         GetVisualName         (int);
static void          GetRectPart           (int,int,int,int,int,int,int*,int*,int*,int*);
static void          DrawBallon            (Display*,Drawable,GC,int,int,unsigned int,unsigned int,int);
static void          InitializeNamedColors (Display*);
static void          GetNamedColorRgb      (Display*,int,double*,double*,double*);
static Pixel*        GetImagePixels        (XImage*,Ulong*);
static Ulong         GetPixelIndex         (Ulong,XColor*,Pixel);
static XColor*       GetPixelsColors       (Display*,int,unsigned long*);
static int           GetRGB                (Uint,Uint,double*,double*,double*);
static Pixel         GetPixel              (int,int);
static unsigned long ConvertStringToMask   (char*);
static void          ReportError           (char*,int);
#ifdef __cplusplus
}
#endif

#if (XlibSpecificationRelease == 4)
#define XrmGetDatabase(This)          (This)->db
#define XrmSetDatabase(This,database) (This)->db = (database)
#endif

#define MINIMUM(a,b)  ((a)<(b)?a:b)
#define MAXIMUM(a,b)  ((a)>(b)?a:b)
#define IBIT(a_i,a_b) (((a_i)&(1<<a_b))>>a_b)
#define NotFound      (-1)

static struct 
{
  Display* display;
  int      number;
  int*     reds;
  int*     greens;
  int*     blues;
} NamedColor = {NULL,0,NULL,NULL,NULL};

static struct _VisualTable 
{
  char         *name;
  int          index;
} visual_table [] = {
       {"StaticGray",StaticGray},
       {"GrayScale",GrayScale},
       {"StaticColor",StaticColor},
       {"PseudoColor",PseudoColor},
       {"TrueColor",TrueColor},
       {"DirectColor",DirectColor}};

static struct {
  XImage* image;
  int     colorn;
  double* reds;
  double* greens;
  double* blues;
  int     warnShape;
} Class = {NULL,0,NULL,NULL,NULL,0};
/***************************************************************************/
static char* GetVisualName (
 int a_visual
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
#define XtNumber(arr)  (sizeof(arr) / sizeof(arr[0]))
  unsigned int count;
/*.........................................................................*/
  for(count=0;count<XtNumber(visual_table);count++)
    {if(a_visual==visual_table[count].index) return visual_table[count].name;}
  return NULL;
}
/***************************************************************************/
void XDisplayClearClass (
)
/******************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*............................................................................*/
  NamedColor.display = NULL;
  CMemoryFreeBlock   (NamedColor.reds);
  CMemoryFreeBlock   (NamedColor.greens);
  CMemoryFreeBlock   (NamedColor.blues);
  NamedColor.reds    = NULL;
  NamedColor.greens  = NULL;
  NamedColor.blues   = NULL;
  NamedColor.number  = 0;
  Class.warnShape    = 0;
}
/***************************************************************************/
void XDisplayDumpScreen (
 Display* This 
)
/***************************************************************************/
/* to have infos on Display, use also:
   UNIX> /usr/bin/X11/xdpyinfo
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  unsigned int       width,height;
  int                depth,vclass;
/*.........................................................................*/
  if(This==NULL) return;
  XDisplayGetDrawableSize  (This,DefaultRootWindow(This),&width,&height);  
#ifdef __cplusplus
  vclass             = DefaultVisualOfScreen(DefaultScreenOfDisplay(This))->c_class;
#else
  vclass             = DefaultVisualOfScreen(DefaultScreenOfDisplay(This))->class;
#endif
  depth              = DefaultDepthOfScreen(DefaultScreenOfDisplay(This));
  CDumpF             ("width : %d, height : %d\n",width,height);
  CDumpF             ("depth : %d\n",depth);
  CDumpF             ("bits/pixel : %d\n",XDisplayGetBitsPerPixel(This));
  CDumpF             ("visual class : %s\n",GetVisualName(vclass));
}
/***************************************************************************/
int XDisplayHasExtension (
 Display* This 
,char* a_name
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int major_opcode,first_event,first_error;
/*.........................................................................*/
  if(This==NULL)     return 0;
  return (int)XQueryExtension (This,a_name,&major_opcode,&first_event,&first_error);
}
/***************************************************************************/
int XDisplayGetBitsPerPixel (
 Display* This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 0;
  return _XGetBitsPerPixel (This,DefaultDepthOfScreen(DefaultScreenOfDisplay(This)));
}
/***************************************************************************/
void XDisplayGetSize (
 Display* This 
,Uint* a_width
,Uint* a_height
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_width!=NULL)        *a_width  = 0;
  if(a_height!=NULL)       *a_height = 0;
  if(This==NULL)          return;
  XDisplayGetDrawableSize  (This,DefaultRootWindow(This),a_width,a_height);  
}
/***************************************************************************/
int XDisplayGetDepth (
 Display* This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 0;
  return DefaultDepthOfScreen(DefaultScreenOfDisplay(This));
}
/***************************************************************************/
Pixel XDisplayGetWhitePixel (
 Display* This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 0L;
  return XWhitePixelOfScreen(DefaultScreenOfDisplay(This));
}
/***************************************************************************/
Pixel XDisplayGetBlackPixel (
 Display* This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 0L;
  return XBlackPixelOfScreen(DefaultScreenOfDisplay(This));
}
/***************************************************************************/
void XDisplayRingBell (
 Display* This 
,int a_number 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
/*.........................................................................*/
  if(This==NULL) return;
  for(count=0;count<a_number;count++) XBell (This,99);
}
/***************************************************************************/
void XDisplaySynchronize (
 Display* This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  XSync (This,False);
}
/***************************************************************************/
GC XDisplayGetRubberGC (
 Display* This 
,Drawable a_window 
)
/***************************************************************************/
/*
  Have to set correct colors if a_window is not root window.
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  XGCValues arggc;
/*.........................................................................*/
  if(This==NULL)       return NULL;
  if(a_window==0L)     return NULL;
  arggc.function       = GXxor;
  arggc.background     = XWhitePixelOfScreen(DefaultScreenOfDisplay(This));
  arggc.foreground     = 0xffffffff;
  arggc.plane_mask     = XWhitePixelOfScreen(DefaultScreenOfDisplay(This)) ^ 
                         XBlackPixelOfScreen(DefaultScreenOfDisplay(This));
  arggc.subwindow_mode = IncludeInferiors;
  return XCreateGC (This,a_window,GCFunction|GCForeground|GCBackground|GCPlaneMask|GCSubwindowMode,&arggc);
}
/***************************************************************************/
void XDisplayDrawRectangle (
 Display* This 
,Drawable a_window 
,GC       a_gc 
,XPoint*  a_begin 
,XPoint*  a_end 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  XPoint    pointx[5];
/*.........................................................................*/
  if(This==NULL)     return;
  if(a_window==0L)   return;
  if(a_gc==NULL)     return;
  if(a_begin==NULL)  return;
  if(a_end==NULL)    return;

  pointx[0]    = *a_begin;

  pointx[1].x  = a_end->x;
  pointx[1].y  = a_begin->y;

  pointx[2]    = *a_end;

  pointx[3].x  = a_begin->x;
  pointx[3].y  = a_end->y;

  pointx[4]    = *a_begin;
  XDrawLines   (This,a_window,a_gc,pointx,5,CoordModeOrigin);
}
/***************************************************************************/
void XDisplayGetPixelRGB (
 Display* This 
,Pixel a_pixel  
,double* a_red
,double* a_green
,double* a_blue
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  XColor color;
/*.........................................................................*/
  if(a_red!=NULL)   *a_red   = 0.;
  if(a_green!=NULL) *a_green = 0.;
  if(a_blue!=NULL)  *a_blue  = 0.;
  if(This==NULL)   return;
  color.pixel = a_pixel;
  XQueryColor (This,DefaultColormapOfScreen (DefaultScreenOfDisplay(This)),&color);
  XColorGetRGB   (&color,a_red,a_green,a_blue);
}
/***************************************************************************/
int XDisplayGetPixelFromRGB (
 Display* This 
,double a_red
,double a_green
,double a_blue
,Pixel* a_pixel
,int* a_alloc
)
/***************************************************************************/
/* read_write visual class: PseudoColor, GrayScale, DirectColor.           */
/* read_only  visual class: TrueColor, StaticColor, StaticGray.            */
/* Could allocate a read_only cell in a read_write, read_only visual class.*/
/* Could allocate a read_write cell only in read_write visual class.       */
/* StaticGray + (depth=1) is monochrome                                    */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Screen*       screen;
  int           vclass;
  Colormap      cmap;
  int           is_grey;
  double        grey;
  XColor        xcolor;
/*.........................................................................*/
  if(a_pixel!=NULL) *a_pixel     = 0L;
  if(a_alloc!=NULL) *a_alloc     = 0;
  if(This==NULL)   return 0;

  screen        = DefaultScreenOfDisplay(This);
#ifdef __cplusplus
  vclass        = DefaultVisualOfScreen(screen)->c_class;
#else
  vclass        = DefaultVisualOfScreen(screen)->class;
#endif
  cmap          = DefaultColormapOfScreen(screen);

  is_grey       = ((vclass==GrayScale) || (vclass==StaticGray) ) ? 1 : 0;
  grey          = is_grey==1 ?  CColorConvertRGB_ToGrey(a_red,a_green,a_blue) : 0.;

  xcolor.pixel  = 0L;
  xcolor.red    = (unsigned short) ((is_grey==1 ? grey : a_red)   * 0xffff);
  xcolor.green  = (unsigned short) ((is_grey==1 ? grey : a_green) * 0xffff);
  xcolor.blue   = (unsigned short) ((is_grey==1 ? grey : a_blue)  * 0xffff);
  if(XAllocColor(This,cmap,&xcolor)==0) /*Could be done on read_only/read_write visual class.*/
    {                                 /*Cell is taken from a common pool. It is read only.*/
      /*Color not found. Try to allocate a private color cell.*/
      if( (vclass==TrueColor)   || 
          (vclass==StaticColor) || 
          (vclass==StaticGray)
        ) /*Viusal class is read_only.*/
        {
          return   0;
        }
      else
        {
	  if(XAllocColorCells (This,cmap,False,NULL,0,&(xcolor.pixel),1)==0) /*read/write cell*/
	    { /*Alloc fail.*/
	      return   0;
	    }
	  else
	    {
	      xcolor.flags = DoRed|DoGreen|DoBlue;
	      XStoreColor  (This,cmap,&xcolor);
	      if(a_pixel!=NULL)  *a_pixel     = xcolor.pixel;
	      if(a_alloc!=NULL)  *a_alloc     = 1;
	    }
	}
    }
  else
    {
      if(a_pixel!=NULL) *a_pixel = xcolor.pixel;
      if(a_alloc!=NULL) *a_alloc = 0;
    }
  return   1;
}
/***************************************************************************/
void XDisplayFreePixels (
 Display* This 
,int a_pixeln
,Pixel* a_pixels
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  XFreeColors   (This,DefaultColormapOfScreen(DefaultScreenOfDisplay(This)),a_pixels,a_pixeln,0);
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static void InitializeNamedColors (
 Display* This
)
/******************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int count;
/*............................................................................*/
  if(This==NULL) return;
  if(NamedColor.display!=NULL) return; /*done*/

  OColorGetNames  (&NamedColor.number);

  if(NamedColor.number<=0) 
    {
      NamedColor.number = 0;
      ReportError("InitializeNamedColors",CErrorValue);
      return;
    }
  
  NamedColor.reds   = (int*)CMemoryAllocateBlock(NamedColor.number * sizeof(int));
  NamedColor.greens = (int*)CMemoryAllocateBlock(NamedColor.number * sizeof(int));
  NamedColor.blues  = (int*)CMemoryAllocateBlock(NamedColor.number * sizeof(int));
  if( (NamedColor.reds==NULL) || (NamedColor.greens==NULL) || (NamedColor.blues==NULL) ) 
    {
      CMemoryFreeBlock  (NamedColor.reds);
      CMemoryFreeBlock  (NamedColor.greens);
      CMemoryFreeBlock  (NamedColor.blues);
      NamedColor.reds   = NULL;
      NamedColor.greens = NULL;
      NamedColor.blues  = NULL;
      NamedColor.number = 0;
      return;
    }

  for(count=0;count<NamedColor.number;count++)
    { char*  string;
      XColor xexact,xscreen;
      string = OColorGetName((OColor)count);
      if(XLookupColor(This,
                          DefaultColormapOfScreen (DefaultScreenOfDisplay(This)),
                          string,&xexact,&xscreen)==0) 
        {
          ReportError ("InitializeNamedColors",CErrorName);
          NamedColor.reds  [count] = 0;
          NamedColor.greens[count] = 0;
          NamedColor.blues [count] = 0;
        }
      else
        {
          NamedColor.reds  [count] = xscreen.red;
          NamedColor.greens[count] = xscreen.green;
          NamedColor.blues [count] = xscreen.blue;
        }
    }

  NamedColor.display = This;
}
/***************************************************************************/
char* XDisplayConvertPixelToColorName (
 Display* This
,Pixel a_pixel
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int      count;
  XColor   color;
  static   char string[32];
/*.........................................................................*/
  if(This==NULL) return NULL;
  InitializeNamedColors (This);

  if(This!=NamedColor.display) {ReportError("XDisplayConvertPixelToColorName",CErrorValue);return NULL;}

  color.pixel = a_pixel;
  XQueryColor (This,DefaultColormapOfScreen (DefaultScreenOfDisplay(This)),&color);
  for(count=0;count<NamedColor.number;count++)
    { 
      if (
           (color.red  ==NamedColor.reds  [count]) &&
           (color.green==NamedColor.greens[count]) &&
           (color.blue ==NamedColor.blues [count])
         )
        {
	  return OColorGetName((OColor)count);
	}
    }
  sprintf (string,"#%04x%04x%04x",
	   (unsigned int)color.red,
	   (unsigned int)color.green,
	   (unsigned int)color.blue);
  return  string;
}
/***************************************************************************/
void XDisplayGetRGBsOfColorNames (
 Display* This
,int* a_number
,double** a_reds
,double** a_greens
,double** a_blues
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  double* reds;
  double* greens;
  double* blues;
/*.........................................................................*/
  if(a_number!=NULL) *a_number = 0;
  if(a_reds!=NULL)   *a_reds   = NULL;
  if(a_greens!=NULL) *a_greens = NULL;
  if(a_blues!=NULL)  *a_blues  = NULL;  
  if(This==NULL)     return;

  InitializeNamedColors (This);

  if(NamedColor.number<=0) return;

  reds        = (double*)CMemoryAllocateBlock(NamedColor.number * sizeof(double));
  greens      = (double*)CMemoryAllocateBlock(NamedColor.number * sizeof(double));
  blues       = (double*)CMemoryAllocateBlock(NamedColor.number * sizeof(double));
  if( (reds==NULL) || (greens==NULL) || (blues==NULL) )
    {
      CMemoryFreeBlock (reds);
      CMemoryFreeBlock (greens);
      CMemoryFreeBlock (blues);
      return;
    }


  for(count=0;count<NamedColor.number;count++)
      GetNamedColorRgb  (This,count,reds+count,greens+count,blues+count);

  if(a_number!=NULL)  *a_number = NamedColor.number;
  if(a_reds!=NULL)    *a_reds   = reds;
  if(a_greens!=NULL)  *a_greens = greens;
  if(a_blues!=NULL)   *a_blues  = blues;

}
/***************************************************************************/
void XDisplayConvertColorNameToRGB (
 Display* This 
,char* a_name
,double* a_red
,double* a_green
,double* a_blue
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  GetNamedColorRgb  (This,OColorGetIdentifier(a_name),a_red,a_green,a_blue);
}
/******************************************************************************/
char* XDisplayConvertRGB_ToColorName (
 Display* This
,double a_red
,double a_green
,double a_blue
)
/******************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int count;
  int found;
/*............................................................................*/
  if(This==NULL) return NULL;
  InitializeNamedColors (This);
  found = 0;
  for(count=0;count<NamedColor.number;count++)
    { 
      double           red,green,blue;
      GetNamedColorRgb (This,count,&red,&green,&blue);
      if( 
	 (a_red   == red)   &&
	 (a_green == green) &&
	 (a_blue  == blue)
	 )
	{
	  found = 1;
	  break;  
	}
    }
  return (found==1 ? OColorGetName((OColor)count) : CStringNULL);
}
/***************************************************************************/
static void GetNamedColorRgb (
 Display* This
,int a_index
,double* a_red
,double* a_green
,double* a_blue
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_red!=NULL)   *a_red   = 0.;
  if(a_green!=NULL) *a_green = 0.;
  if(a_blue!=NULL)  *a_blue  = 0.;
  if(This==NULL)    return;

  InitializeNamedColors (This);

  if((a_index<0)||(a_index>=NamedColor.number)) return;
  if(This!=NamedColor.display) {ReportError("GetNamedColorRgb",CErrorValue);return;}

  if(a_red!=NULL)   *a_red   =  ((double)   NamedColor.reds  [a_index] )/0xffff;
  if(a_green!=NULL) *a_green =  ((double)   NamedColor.greens[a_index] )/0xffff;
  if(a_blue!=NULL)  *a_blue  =  ((double)   NamedColor.blues [a_index] )/0xffff;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
void XDisplayGetDrawableSize (
 Display* This 
,Drawable a_drawable 
,Uint* a_width 
,Uint* a_height 
)
/***************************************************************************/
{
  Window       root;
  int          cx,cy;
  unsigned int width,height,border,depth;
/*.........................................................................*/
  if(a_width!=NULL)     *a_width  = 0;
  if(a_height!=NULL)    *a_height = 0;
  if(This==NULL)       return;
  if(a_drawable==0L) return;
  XGetGeometry    (This,a_drawable,&root,&cx,&cy,&width,&height,&border,&depth);
  if(a_width!=NULL)     *a_width  = width;
  if(a_height!=NULL)    *a_height = height;
}
/***************************************************************************/
Uint XDisplayGetDrawableDepth (
 Display* This 
,Drawable a_drawable 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int             cx,cy;
  unsigned int    width,height,border,depth;
  Window          root;
/*.........................................................................*/
  if(This==NULL)       return 0;
  if(a_drawable==0L) return 0;
  depth           = 0;
  XGetGeometry    (This,a_drawable,&root,&cx,&cy,&width,&height,&border,&depth);
  return          depth;
}
/***************************************************************************/
/********* Pixmap **********************************************************/
/***************************************************************************/
/***************************************************************************/
Pixmap XDisplayLoadPixmapFile (
 Display* This 
,char* a_fname
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Pixmap          pixmap;  
  int             status,x_hot,y_hot;
  unsigned int    width,height;
/*.........................................................................*/
  if(This==NULL)       return 0L;
  if(a_fname==NULL)    return 0L;
  status          = XReadBitmapFile (This,DefaultRootWindow(This),a_fname,&width,&height,&pixmap,&x_hot,&y_hot);  
       if(status==BitmapOpenFailed)
    {
      ReportError        ("XDisplayLoadPixmapFile",CErrorFailure);
      return       0L;
    }
  else if(status==BitmapFileInvalid)
    {
      ReportError        ("XDisplayLoadPixmapFile",CErrorFormat);
      return       0L;
    }
  else if(status==BitmapNoMemory)
    {
      ReportError        ("XDisplayLoadPixmapFile",CErrorFailure);
      return       0L;
    }
  return pixmap;
}
/***************************************************************************/
XImage* XDisplayGetPixmapImage (
 Display* This 
,Pixmap a_drawable
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  unsigned int      width  = 0;
  unsigned int      height = 0;
  XImage*           image;
/*.........................................................................*/
  if(This==NULL)         return NULL;
  if(a_drawable==0L)   return NULL;
  XDisplayGetDrawableSize (This,a_drawable,&width,&height);
  if((width==0)||(height==0)) return NULL;
  image             = XGetImage (This,a_drawable,0,0,width,height,(unsigned long)-1,ZPixmap);
  if(image==NULL)   return NULL;
  if(((unsigned int)image->width!=width)||((unsigned int)image->height!=height))
    {
      XDestroyImage (image);
      return        NULL;
    }
  return            image;
}
/***************************************************************************/
void XDisplayPutPixmapInPS_Stream (
 Display* This
,Pixmap a_drawable
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Uint    width,height;
  XImage* image;
/*.........................................................................*/
  if(This==NULL)       return;
  if(a_drawable==0L) return;
  if(CPS_GetStream()==NULL)  CPS_OpenFileForWriting (NULL);
  if(CPS_GetStream()==NULL)  return;

  XDisplayGetDrawableSize       (This,a_drawable,&width,&height);  

  image                   = XDisplayGetPixmapImage (This,a_drawable);
  if(image==NULL)              return;

  CPS_PutBeginPageInStream            ();
  CPS_PutPageScaleInStream            ((double)width,(double)height);
  XDisplayPutImageInPS_Stream         (This,image);
  CPS_PutEndPageInStream              ();


  XDestroyImage                       (image);
}
/***************************************************************************/
void XDisplayPutPixmapInRootWindow (
 Display* This 
,Pixmap a_pixmap
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)             return;
  if(a_pixmap==0L)           return;
  XSetWindowBackgroundPixmap (This,DefaultRootWindow(This),a_pixmap);
  XClearWindow               (This,DefaultRootWindow(This));
}
/***************************************************************************/
Pixmap XDisplayConvertPixmapToBitmap (
 Display* This 
,Pixmap a_pixmap
,Ulong a_back
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int row,col;
  XImage*      image;
  Pixmap       pixmap = 0L;
  XImage*      bitmap;
  GC           gc;
  XGCValues    xgcv;
  size_t       size;
/*.........................................................................*/
  if(This==NULL)   return 0L;
  if(a_pixmap==0L) return 0L;

  image           = XDisplayGetPixmapImage (This,a_pixmap);
  if(image==NULL) return 0L;

  pixmap          = XCreatePixmap (This, DefaultRootWindow(This), (Uint)(image->width),(Uint)(image->height), 1);
  if(pixmap==0L)  
    {
      XDestroyImage (image);
      return        0L;
    }
  
  bitmap          = XCreateImage  (This,
                                   DefaultVisualOfScreen (DefaultScreenOfDisplay(This)),
                                   1,   /*depth*/
                                   ZPixmap,0,
                                   NULL,(Uint)(image->width),(Uint)(image->height),8,0);
  if(bitmap==0L) 
    {
      XFreePixmap   (This,pixmap);
      XDestroyImage (image);
      return        0L;
    }
  size                    = image->height * image->bytes_per_line * sizeof(char);
  bitmap->data            = (char*) CMemoryAllocateBlock (size);
  memset                  (bitmap->data,0,size);  
  bitmap->f.destroy_image = XImageDelete;
  bitmap->obdata          = CStringDuplicate("bitmap");
  if(bitmap->data==NULL)       
    {
      XFree         ((char*)bitmap);
      XFreePixmap   (This,pixmap);
      XDestroyImage (image);
      return        0L;
    }

  for (row=0;row<image->height;row++) 
    { 
      for (col=0;col<image->width;col++) 
	{ Ulong pixel;
	  pixel = XGetPixel(image,col,row);
	  XPutPixel (bitmap,col,row,(pixel==a_back)  ? 0L : 1L );
	}
    }

  gc              = XCreateGC (This,pixmap, 0, &xgcv);
  if(gc!=NULL) 
    {
      XPutImage       (This,pixmap,gc,bitmap,0,0,0,0,(Uint)(image->width),(Uint)(image->height));
      XFreeGC         (This,gc);
    }

  XDestroyImage   (image);

  XDestroyImage   (bitmap);

  return          pixmap;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
Pixmap XDisplayGetDrawablePixmap (
 Display* This 
,Drawable a_drawable 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int               cx,cy;
  unsigned int      width = 0,height = 0,border = 0,depth = 0;
  Window            root;
  Pixmap            pixmap;
  XImage            *image;
  GC                gc;
  XGCValues         xgcv;
/*.........................................................................*/
  if(This==NULL)       return 0;
  if(a_drawable==0L) return 0;
  XGetGeometry    (This,a_drawable,&root,&cx,&cy,&width,&height,&border,&depth);
  if((width==0)||(height==0)) return 0;

  image           = XGetImage (This,a_drawable,0,0,width,height,(unsigned long)-1,ZPixmap);
  if(image==NULL)      return 0;
  if(((Uint)image->width!=width)||((Uint)image->height!=height))
    {
      XDestroyImage (image);
      return        0;
    }

  pixmap              = XCreatePixmap (This, DefaultRootWindow(This), width, height, depth);
  if(pixmap==0L)         {XDestroyImage (image);return 0;}

  gc                  = XCreateGC (This,pixmap, 0, &xgcv);
  if(gc!=NULL) 
    {
      XPutImage           (This,pixmap,gc,image,0,0,0,0,width,height);
      XFreeGC             (This,gc);
    } /*endif gc*/

  XDestroyImage       (image);

  return              pixmap;
}
/***************************************************************************/
Pixmap XDisplayGetWindowPixmap (
 Display* This 
,Window  a_drawable 
,int a_beg_x
,int a_beg_y
,int a_end_x
,int a_end_y
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int               a_wx,a_wy,a_ww,a_wh;
  int               cx,cy,px,py;
  unsigned int      sw,sh,width = 0,height = 0,border = 0,depth = 0;
  Window            root;
  Window            child;
  XImage            *image;
  int               partx,party,partw,parth;
  int               wantx,wanty,wantw,wanth;
  Pixmap            pixmap;
  GC                gc;
  XGCValues         xgcv;
/*.........................................................................*/
  if(This==NULL)       return 0;
  if(a_drawable==0L) return 0;

  a_wx      = MINIMUM (a_beg_x,a_end_x);
  a_wy      = MINIMUM (a_beg_y,a_end_y);
  a_ww      = MAXIMUM (a_beg_x,a_end_x) - a_wx;
  a_wh      = MAXIMUM (a_beg_y,a_end_y) - a_wy;

  if((a_ww<=0)||(a_wh<=0)) return 0;

  XGetGeometry    (This,a_drawable,&root,&cx,&cy,&width,&height,&border,&depth);
  if((width==0)||(height==0)) return 0;

/* get pixmap size */
  partx           = 0;
  party           = 0;
  partw           = width;
  parth           = height;
  if( (a_ww!=0) && (a_wh!=0) ) 
    GetRectPart  (width,height,a_wx,a_wy,a_ww,a_wh,&partx,&party,&partw,&parth);

/*check that window has not a part out of screen */
  XDisplayGetDrawableSize      (This,DefaultRootWindow(This),&sw,&sh);
  XTranslateCoordinates  (This,a_drawable,DefaultRootWindow(This),partx,party,&px,&py,&child);
  wantx                  = px;
  wanty                  = py;
  wantw                  = partw;
  wanth                  = parth;
  GetRectPart              ((int)sw,(int)sh,wantx,wanty,wantw,wanth,&partx,&party,&partw,&parth);
  XTranslateCoordinates  (This,DefaultRootWindow(This),a_drawable,partx,party,&px,&py,&child);
  width                  = partw;
  height                 = parth;

  if((width==0)||(height==0)) return 0;

  image         = XGetImage (This,a_drawable,px,py,width,height,(unsigned long)-1,ZPixmap);
  if(image==NULL)    return 0;
  if(((Uint)image->width!=width)||((Uint)image->height!=height))
    {
      XDestroyImage (image);
      return        0;
    }

  pixmap              = XCreatePixmap (This, DefaultRootWindow(This), width, height, depth);
  if(pixmap==0L)         {XDestroyImage (image);return 0;}

  gc                  = XCreateGC (This,pixmap, 0, &xgcv);
  if(gc!=NULL) 
    {
      XPutImage           (This,pixmap,gc,image,0,0,0,0,width,height);
      XFreeGC             (This,gc);
     } /*endif gc*/

  XDestroyImage       (image);

  return              pixmap;
}
/***************************************************************************/
static void GetRectPart (
 int   a_width  
,int   a_height 
,int a_wx
,int a_wy
,int a_ww
,int a_wh
,int* a_px
,int* a_py
,int* a_pwidth
,int* a_pheight
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int wbrx,wbry;
  int pulx,puly,pbrx,pbry;
/*.........................................................................*/
  wbrx           = a_wx+a_ww;
  wbry           = a_wy+a_wh;
  pulx           =  (a_wx<0)||(a_wx>=a_width)  ? 0 : a_wx;
  puly           =  (a_wy<0)||(a_wy>=a_height) ? 0 : a_wy;
  pbrx           =  (wbrx>=a_width )||(wbrx<0) ? a_width  : wbrx;
  pbry           =  (wbry>=a_height)||(wbry<0) ? a_height : wbry;
  if(a_px!=NULL)       *a_px          = pulx;
  if(a_py!=NULL)       *a_py          = puly;
  if(a_pwidth!=NULL)   *a_pwidth      = pbrx-pulx;
  if(a_pheight!=NULL)  *a_pheight     = pbry-puly;
}
/***************************************************************************/
void XDisplaySetWindowShapeE (
 Display* This 
,Window   a_window
,Window   a_parent
,XShape a_shape
,...
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  va_list                 args;
/*.........................................................................*/
  va_start                (args,a_shape);
  XDisplaySetWindowShapeV    (This,a_window,a_parent,a_shape,args);
  va_end                  (args);
}
/***************************************************************************/
void XDisplayIconifyWindow (
 Display* This
,Window a_window
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)   return;
  if(a_window==0L) return;
  XIconifyWindow (This,a_window,XScreenNumberOfScreen(DefaultScreenOfDisplay(This)));
}
/***************************************************************************/
int XDisplaySendMessage (
 Display* This
,Window a_window
,long a_1
,long a_2
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  XClientMessageEvent event;
/*.........................................................................*/
  if (This==NULL)      return 0;
  if (a_window==0L)    return 0;
  event.type           = ClientMessage;
  event.display        = This;
  event.window         = a_window;
  event.message_type   = XA_INTEGER;
  event.format         = 8;    /* put 8 because bug with 32 on VMCMS */
  event.data.l[0]      = a_1;
  event.data.l[1]      = a_2;
  event.data.l[2]      = 0L;
  event.data.l[3]      = 0L;
  event.data.l[4]      = 0L;
  if(XSendEvent        (event.display,event.window,False,0L,(XEvent*)&event)==0) return 0;
  XFlush               (event.display);
  return               1;
}
/***************************************************************************/
void XDisplaySetWindowToNormalState (
 Display* This
,Window a_window
)
/***************************************************************************/
/*
  Used to deiconify a window.
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  XWMHints         wh;
/*.........................................................................*/
  if(This==NULL)   return;
  if(a_window==0L) return;
  wh.initial_state = NormalState; 
  wh.flags         = StateHint;
  XSetWMHints      (This,a_window,&wh);
  XMapWindow       (This,a_window);
}
/***************************************************************************/
void XDisplaySetWindowShapeV (
 Display* This 
,Window   a_window
,Window   a_parent
,XShape a_shape
,va_list a_args
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)     return;
  if(a_window==0L) return;
  if(a_parent==0L) return;

  if(XDisplayHasExtension(This,"SHAPE")==0)
    {
      if(Class.warnShape==0)
	{
	  CWarn ("X display has not X shape extensions.\n");
	  Class.warnShape = 1;
	}
      return;
    }

#ifdef HAS_XEXT
 {unsigned  int width,height;
  Pixmap    shape_mask;
  GC        gc;
  XGCValues xgcv;
  XDisplayGetDrawableSize (This,a_window,&width,&height);
  if( (width==0) || (height==0)) return;

  shape_mask        = XCreatePixmap (This,a_window,width,height, 1);
  if(shape_mask==0L)   return;
  gc                = XCreateGC     (This,shape_mask, 0, &xgcv);
  if(gc==NULL)           return;

  XSetForeground    (This,gc, 0);
  XFillRectangle    (This,shape_mask,gc, 0, 0,width,height);
  XSetForeground    (This,gc, 1);

  switch(a_shape)
    { 
   	  case XShapeRectangular:
            XFillRectangle  (This,shape_mask,gc,0,0,width,height);
            break;
          case XShapeOval:
            XFillArc        (This,shape_mask,gc,0,0,width,height,90 * 64,360 * 64);
            break;
          case XShapeDiamond:
           {XPoint          *points;
            points          = (XPoint*)CMemoryAllocateBlock(4*sizeof(XPoint));
            if(points!=NULL)      
	      {
		points[0].x     = width/2;
		points[0].y     = 0;
		points[1].x     = width;
		points[1].y     = height/2;
		points[2].x     = width/2;
		points[2].y     = height;
		points[3].x     = 0;
		points[3].y     = height/2;
		XFillPolygon    (This,shape_mask,gc,points,4,Convex,CoordModeOrigin);
	      }
            CMemoryFreeBlock        (points);
	    }
            break;
          case XShapeBallonArcRight:
            DrawBallon (This,shape_mask,gc,0,height,width,height,0);
            break;
          case XShapeBallonArcLeft:
            DrawBallon (This,shape_mask,gc,width,height,width,height,0);
            break;
          case XShapeBallonArcXY:
	    {int         x,y;
             x           = va_arg (a_args,int);
             y           = va_arg (a_args,int);
             DrawBallon (This,shape_mask,gc,x,y,width,height,0);
            }
            break;
          case XShapeBallonRectRight:
            DrawBallon (This,shape_mask,gc,0,height,width,height,1);
            break;
          case XShapeBallonRectLeft:
            DrawBallon (This,shape_mask,gc,width,height,width,height,1);
            break;
          case XShapeBallonRectXY:
	    {int         x,y;
             x           = va_arg (a_args,int);
             y           = va_arg (a_args,int);
             DrawBallon (This,shape_mask,gc,x,y,width,height,1);
            }
            break;
   	  case XShapeFunction:
            { XDisplayShapeFunction proc;
              char*                 tag;
              proc                  = va_arg(a_args,XDisplayShapeFunction);
              tag                   = va_arg(a_args,char*);
              if(proc!=NULL)        proc (This,shape_mask,gc,tag);
  	    } break;
   	  case XShapePixmap:
            { Pixmap             pixmap = 0;
              Ulong              pixel  = 0;
              Pixmap             bitmap;
              pixmap             = va_arg(a_args,Pixmap);
              pixel              = va_arg(a_args,Ulong);
	      bitmap             = XDisplayConvertPixmapToBitmap(This,pixmap,pixel);
	      if(bitmap!=0L)
		{
		  XCopyArea        (This,bitmap,shape_mask,gc,0,0,width,height, 0, 0 );
		  XFreePixmap      (This,bitmap);
		}
  	    } break;
    }

  XShapeCombineMask (This,a_parent, ShapeBounding,0, 0,shape_mask, ShapeSet);
  XFreePixmap       (This, shape_mask);
  XFreeGC           (This,gc);}
#else  /*HAS_XEXT*/
  a_shape = XShapePixmap;
  va_arg(a_args,Pixmap);
#endif /*HAS_XEXT*/
}
/***************************************************************************/
static void DrawBallon (
 Display* This
,Drawable a_drawable
,GC       a_gc
,int      a_x
,int      a_y
,unsigned int a_width
,unsigned int a_height
,int a_type
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  XPoint*           points;
/*.........................................................................*/
  if(a_gc==NULL)    return;
  if(a_x<0)         a_x = 0;
  if(a_x>(int)a_width)   a_x = a_width;
  if(a_y<0)         a_y = 0;
  if(a_y>(int)a_height)  a_y = a_height;
  points            = (XPoint*)CMemoryAllocateBlock(3*sizeof(XPoint));
  if(points==NULL)  return;
  if(a_type==0)
    XFillArc          (This,a_drawable,a_gc,0,0,a_width,(unsigned int)(a_height * 0.8),90 * 64,360 * 64);
  else
    XFillRectangle    (This,a_drawable,a_gc,0,0,a_width,(unsigned int)(a_height * 0.8));
  points[0].x       = a_width/2;
  points[0].y       = (short)(a_height * 0.3);
  points[1].x       = a_x;
  points[1].y       = a_y;
  points[2].x       = a_width/2;
  points[2].y       = (short)(a_height * 0.7);
  XFillPolygon      (This,a_drawable,a_gc,points,3,Convex,CoordModeOrigin);
  CMemoryFreeBlock  (points);
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
void XDisplayPutFileInResourceDatabase (
 Display* This 
,char* a_fname 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  FILE*       file;
  char*       fname;
  XrmDatabase dbres = NULL;
/*.........................................................................*/
  if(This==NULL)      return;
  if(a_fname==NULL)   return;
  file                = CFileOpenForReading (a_fname,&fname); 
  if(file==NULL)      return;
  fclose              (file);
  dbres               = XrmGetFileDatabase (fname);
  CMemoryFreeBlock    (fname);
  if(dbres==NULL)     ReportError ("XDisplayPutFileInResourceDatabase",CErrorFailure);
 {XrmDatabase         database;
  database            = XrmGetDatabase (This);
  if(database!=NULL)  
    { 
      XrmMergeDatabases  (dbres,&database);
    }
  else 
    {
      XrmSetDatabase    (This,dbres);
    }}
}
/***************************************************************************/
void XDisplayPutStringInResourceDatabase (
 Display* This 
,char* a_string 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  XrmDatabase dbres = 0;
/*.........................................................................*/
  if(This==NULL)      return;
  if(a_string==NULL)  return;
  dbres               = XrmGetStringDatabase (a_string);
  if(dbres==NULL)     ReportError ("XDisplayPutStringInResourceDatabase",CErrorFailure);
 {XrmDatabase         database;
  database            = XrmGetDatabase (This);
  if(database!=NULL)  
    {
      XrmMergeDatabases  (dbres,&database);
    }
  else
    {
      XrmSetDatabase     (This,dbres);
    }}
}
/***************************************************************************/
void XDisplayDumpResource (
 Display* This 
,char* a_name  
,char* a_class
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*       type;
  XrmValue    value;
  XrmDatabase database;
/*.........................................................................*/
  if(This==NULL)     return;
  if(a_name==NULL)   return;
  if(a_class==NULL)  return;
  database           = XrmGetDatabase (This);
  if(database==NULL) return;
  if(XrmGetResource(database,a_name,a_class,&type,&value)==True)
    CDumpF("%s|%s\n",type,value.addr);
  else
    CDump ("not found\n");
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
void XDisplayGetImageColormap (
 Display* This
,XImage*  a_image
,int*     a_colorn
,double** a_reds
,double** a_greens
,double** a_blues
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Ulong    colorn;
  XColor*  colors;
  double*  reds   = 0;
  double*  greens = 0;
  double*  blues  = 0;
/*.........................................................................*/
  if(a_colorn!=NULL)  *a_colorn = 0;
  if(a_reds!=NULL)    *a_reds   = 0;
  if(a_greens!=NULL)  *a_greens = 0;
  if(a_blues!=NULL)   *a_blues  = 0;
  if(This==NULL)     return;
  if(a_image==NULL)  return;

 {Pixel*      pixels;
  pixels      = GetImagePixels  (a_image,&colorn);
  colors      = GetPixelsColors (This,(int)colorn,pixels);
  CMemoryFreeBlock    (pixels);}
  if(colors==NULL) return;

  reds   = (double*)CMemoryAllocateBlock((size_t)colorn * sizeof(double));  
  greens = (double*)CMemoryAllocateBlock((size_t)colorn * sizeof(double));  
  blues  = (double*)CMemoryAllocateBlock((size_t)colorn * sizeof(double));  
  if( (reds==NULL) || (greens==NULL) || (blues==NULL) ) 
    {     
      CMemoryFreeBlock  (reds);
      CMemoryFreeBlock  (greens);
      CMemoryFreeBlock  (blues);
      colorn    = 0;
      reds      = greens = blues = 0;
    }
  else
    {
       int row,col;
       Ulong count;
      GetPixelIndex (0,0,0);
      for(row=0;row<a_image->height;row++)
	for(col=0;col<a_image->width;col++)
	  { 
	    XPutPixel(a_image,col,row,GetPixelIndex(colorn,colors,XGetPixel(a_image,col,row)));
	  }
      for(count=0;count<colorn;count++)
	{ 
	  XColorGetRGB (&(colors[count]),&(reds[count]),&(greens[count]),&(blues[count]));
	}
    }

  CMemoryFreeBlock  (colors);

  if(a_colorn!=NULL)  *a_colorn = (int)colorn;
  if(a_reds!=NULL)    *a_reds   = reds;
  if(a_greens!=NULL)  *a_greens = greens;
  if(a_blues!=NULL)   *a_blues  = blues;
}
/***************************************************************************/
static Pixel* GetImagePixels (
 XImage* This
,Ulong* a_pixeln
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int row,col;
  size_t size;
  Ulong  pixeln;
  Pixel* pixels;
  Pixel  pixelPrev;
/*.........................................................................*/
  if(a_pixeln!=NULL) *a_pixeln    = 0;
  if(This==NULL)    return 0;
  size         = 256;
  pixels       = (Pixel*)CMemoryAllocateBlock(size * sizeof(Pixel));
  if(pixels==NULL)  return 0;
  pixelPrev    = 0L;
  pixeln       = 0;
  for(row=0;row<This->height;row++)
    for(col=0;col<This->width;col++)
      { 
        Pixel pixel;
        pixel = XGetPixel(This,col,row);
        if( (pixeln==0) || (pixel!=pixelPrev))
          {
	    unsigned int count;
	    int   found;
	    found = 0;
	    for(count=0;count<pixeln;count++) 
	      {
		if(pixel==pixels[count]) 
		  {
		    found=1;
		    break;
		  }
	      }
	    if(found==0)
	      {
		if(pixeln>=size) 
		  {
		    size   += 256;
		    pixels  = (Pixel*)CMemoryChangeBlockSize(pixels,size * sizeof(Pixel));
		  } 
		pixels[pixeln] = pixel;
		pixeln++;
	      }
	  }
	pixelPrev = pixel;
      }
  if(a_pixeln!=NULL) *a_pixeln = pixeln;
  return       pixels;
}
/***************************************************************************/
static XColor* GetPixelsColors (
 Display* This 
,int    a_pixeln
,Pixel* a_pixels
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  XColor* colors;
/*.........................................................................*/
  if(This==NULL)       return 0;
  if(a_pixeln<=0) return 0;
  if(a_pixels==NULL)   return 0;
  colors          = (XColor*)CMemoryAllocateBlock(a_pixeln * sizeof(XColor));
  if(colors==NULL)     return 0;
  for(count=0;count<a_pixeln;count++) colors[count].pixel = a_pixels[count];
  XQueryColors    (This,DefaultColormapOfScreen(DefaultScreenOfDisplay(This)),colors,a_pixeln);
  return          colors;
}
/***************************************************************************/
static Ulong GetPixelIndex (
 Ulong   a_colorn
,XColor* a_colors
,Pixel   a_pixel  
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  static int    init  = 0;
  static Pixel  pixel;
  static Ulong  index = 0L;
/*.........................................................................*/
  if(a_colorn==0) 
    {
      init = 0;
      return 0;
    }
  if( (init==0) || (a_pixel!=pixel))
    { 
       Ulong count;
      for(count=0;count<a_colorn;count++)
	{
	  if(a_pixel==a_colors[count].pixel)
	    {
	      index     = count;
	      pixel     = a_pixel;
	      init      = 1;
	      break;
	    }
	}
    }
  return index;
}
/***************************************************************************/
void XDisplayPutImageInGIF_Stream (
 Display* This
,XImage* a_image
,void* a_file
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int      colorn;
  double*  reds;
  double*  greens;
  double*  blues;
/*.........................................................................*/
  if(This==NULL)    return;
  if(a_image==NULL) return;
  if(a_file==NULL)  return;

  XDisplayGetImageColormap     (This,a_image,&colorn,&reds,&greens,&blues);
  if(colorn>256)    
    CWarn ("More than 256 colors in image.\n Some colors will be badly rendered in GIF file.\n");

  Class.image       = a_image;
  CGIF_PutImageInStream ((FILE*)a_file,GetPixel,(Uint)a_image->width,(Uint)a_image->height,colorn,reds,greens,blues);

  CMemoryFreeBlock          (reds);
  CMemoryFreeBlock          (greens);
  CMemoryFreeBlock          (blues);
}
/***************************************************************************/
static Pixel GetPixel (
 int a_x
,int a_y
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return XGetPixel(Class.image,a_x,a_y);
}
/***************************************************************************/
void XDisplayPutImageInPS_Stream (
 Display* This
,XImage* a_image
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)    return;
  if(a_image==NULL) return;
  XDisplayGetImageColormap     (This,a_image,&Class.colorn,&Class.reds,&Class.greens,&Class.blues);
  Class.image          = a_image;
  CPS_PutImageInStream ((Uint)(a_image->width),(Uint)(a_image->height),GetRGB);
  CMemoryFreeBlock     (Class.reds);
  CMemoryFreeBlock     (Class.greens);
  CMemoryFreeBlock     (Class.blues);
  Class.colorn         = 0;
  Class.reds           = NULL;
  Class.greens         = NULL;
  Class.blues          = NULL;
}
/***************************************************************************/
static int GetRGB (
 Uint a_col
,Uint a_row
,double* a_red
,double* a_green
,double* a_blue
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Ulong index;
/*.........................................................................*/
  index = (Ulong)XGetPixel (Class.image,a_col,a_row);
  if(index>=(Ulong)Class.colorn) 
    {
      *a_red     = 0.;
      *a_green   = 0.;
      *a_blue    = 0.;
      return     0;
    }
  else
    {
      *a_red     = Class.reds  [index];
      *a_green   = Class.greens[index];
      *a_blue    = Class.blues [index];
      return     1;
    }
}
/***************************************************************************/
XImage* XDisplayCompressImage (
 Display* This
,XImage* a_from
,int a_factor 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  unsigned int row,col;
  Uint     width,height;
  XImage*  image;
  int      frow;
  size_t   size;
/*.........................................................................*/
  if(This==NULL)                                   return 0;
  if(a_from==NULL)                                 return 0;
  if(a_from->data==NULL)                           return 0;
  if((a_from->width<=0)||(a_from->height<=0)) return 0;
  if(a_factor<=1) a_factor = 1;

  width      = a_from->width /a_factor;
  height     = a_from->height/a_factor;
  image      = XCreateImage (This,
					    DefaultVisualOfScreen (DefaultScreenOfDisplay(This)),
					    a_from->depth,
					    a_from->format,
                                            a_from->xoffset,
					    0,
                                            width,
                                            height,
                                            a_from->bitmap_pad,0);

  if(image==NULL)  return 0;
  size                   = image->height * image->bytes_per_line * sizeof(char);
  image->data            = (char*) CMemoryAllocateBlock (size);
  memset                 (image->data,0,size);  
  image->f.destroy_image = XImageDelete;
  image->obdata          = CStringDuplicate("compress");
  if(image->data==NULL) 
    {
      XFree((char*)image);
      return 0;
    }

  frow = 0;
  for (row=0;row<height;row++)
    { int  fcol;
      fcol = 0;
      for (col=0;col<width;col++) 
        {
          XPutPixel(image,col,row,XGetPixel(a_from,fcol,frow));
          fcol += a_factor;
        }
      frow += a_factor;
    }

  return     image;
}
/***************************************************************************/
XImage* XDisplayExpandImage (
 Display* This
,XImage* a_from
,int a_factor 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  unsigned int row,col;
  Uint     width,height;
  XImage*  image;
  int      frow,irow;
  size_t   size;
/*.........................................................................*/
  if(This==NULL)                                   return 0;
  if(a_from==NULL)                                 return 0;
  if(a_from->data==NULL)                           return 0;
  if((a_from->width<=0)||(a_from->height<=0)) return 0;
  if(a_factor<=1) a_factor = 1;

  width      = a_from->width  * a_factor;
  height     = a_from->height * a_factor;
  image      = XCreateImage (This,
					    DefaultVisualOfScreen (DefaultScreenOfDisplay(This)),
					    a_from->depth,
					    a_from->format,
                                            a_from->xoffset,
					    0,
                                            width,
                                            height,
                                            a_from->bitmap_pad,0);
  if(image==NULL)  return 0;
  size                   = image->height * image->bytes_per_line * sizeof(char);
  image->data            = (char*) CMemoryAllocateBlock (size);
  memset                 (image->data,0,size);  
  image->f.destroy_image = XImageDelete;
  image->obdata          = CStringDuplicate("expand");
  if(image->data==NULL)     
    {
      XFree((char*)image);
      return 0;
    }

  frow = 0;
  irow = 1;
  for (row=0;row<height;row++)
    { int   fcol,icol;
      Pixel pixel;
      fcol  = 0;
      icol  = 1;
      pixel = XGetPixel (a_from,fcol,frow);
      for (col=0;col<width;col++) 
        { 
          XPutPixel          (image,col,row,pixel);
          if(icol>=a_factor) {icol = 1;fcol++;pixel = XGetPixel (a_from,fcol,frow);}
          else                icol++;
        }
      if(irow>=a_factor) {irow = 1;frow++;}
      else               irow++;
    }

  return     image;
}
/***************************************************************************/
XImage* XDisplayAdaptImageToScreen (
 Display* This
,XImage* a_from
)
/***************************************************************************/
/*
  Adapt an image to the screen.
  Usefull if depth of image != depth of screen or
  disp_bits_per_pixel > image_bits_per_pixel.
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int row,col;
  XImage*  image;
  size_t   size;
/*.........................................................................*/
  if(This==NULL)                                   return 0;
  if(a_from==NULL)                                 return 0;
  if(a_from->data==NULL)                           return 0;
  if((a_from->width<=0)||(a_from->height<=0)) return 0;
  image      = XCreateImage (This,
			     DefaultVisualOfScreen (DefaultScreenOfDisplay(This)),
			     XDisplayGetDepth(This),
			     a_from->format,
			     a_from->xoffset,
			     0,
			     a_from->width,
			     a_from->height,
			     a_from->bitmap_pad,0);
  if(image==NULL)  return 0;
  size                   = image->height * image->bytes_per_line * sizeof(char);
  image->data            = (char*) CMemoryAllocateBlock (size);
  memset                 (image->data,0,size);
  image->f.destroy_image = XImageDelete;
  image->obdata          = CStringDuplicate("screen");
  if(image->data==NULL)       
    {
      XFree((char*)image);
      return 0;
    }

  for (row=0;row<image->height;row++)
      for (col=0;col<image->width;col++) 
        { 
          XPutPixel (image,col,row,XGetPixel(a_from,col,row));
        }

  return image;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/*
  bits_per_pixel =  8       >>3 = 1
  bits_per_pixel = 16       >>3 = 2
  bits_per_pixel = 32       >>3 = 4
  XGetPixel use:
      This->data[ZINDEX(x,y,This)) 

#define ZINDEX(x, y,This) ((y) * This->bytes_per_line) + \
                           (((x) * This->bits_per_pixel) >> 3)

  XCreateImage(depth,ZPixmap,...,0);
     This->bits_per_pixel = XDisplayGetBitsPerPixel(depth)  (depth 1 plane -> 1, 8 planes -> 8, 24 planes ->32)
     This0>bytes_per_line = computed.
  XCreateImage(depth,ZPixmap,...,bytes_per_line);
     This->bits_per_pixel = XDisplayGetBitsPerPixel(depth)  (depth 1 plane -> 1, 8 planes -> 8, 24 planes ->32)
     This->bytes_per_line = bytes_per_line;
 
*/
/***************************************************************************/
int XImageDelete (
 XImage* This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)   return 0;
  CStringDelete    (This->obdata);
  CMemoryFreeBlock (This->data);
  This->obdata     = NULL;
  This->data       = NULL;
  XFree            ((char*)This);
  return           1;
}
/***************************************************************************/
int XImageDestroy (
 XImage* This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 0;
  return XDestroyImage (This);
}
/***************************************************************************/
void XImageGetSize (
 XImage* This
,unsigned int* a_width
,unsigned int* a_height
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_width!=NULL)  *a_width  = 0;
  if(a_height!=NULL) *a_height = 0;
  if(This==NULL)    return;
  if(a_width!=NULL)  *a_width  = This->width;
  if(a_height!=NULL) *a_height = This->height;
}
/***************************************************************************/
char* XImageGetBytes (
 XImage* This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 0;
  return This->data;
}
/***************************************************************************/
Pixel XImageGetPixel (
 XImage* This
,int a_x
,int a_y
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 0;
  return XGetPixel (This,a_x,a_y);
}
/***************************************************************************/
void XImagePutPixel (
 XImage* This
,int a_x
,int a_y
,Pixel a_pixel
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  XPutPixel (This,a_x,a_y,a_pixel);
}
/***************************************************************************/
void XImageAddPixel (
 XImage* This
,long a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  XAddPixel (This,a_value);
}
/***************************************************************************/
int XImageConvolute (
 XImage*  This
,int      a_pixeln
,Pixel*   a_pixels
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int row,col;
  int pb;
/*.........................................................................*/
  if(This==NULL)       return 0;
  if(a_pixeln<=0) return 0;
  if(a_pixels==NULL)   return 0;
  pb = 0;
  for(row=0;row<This->height;row++)
    for(col=0;col<This->width;col++)
      { Pixel pixel;
	pixel = XGetPixel(This,col,row);
	if(pixel>=(Pixel)a_pixeln)
	  {
	    if(pb==0) 
	      CWarnF("XImageConvolute: some pixels (%lu) are out of array range (%d).\n",pixel,a_pixeln);
	    pb = 1;
	  }
	else
	  {
	    XPutPixel(This,col,row,a_pixels[pixel]);
	  }
      }
  return pb;
}
/***************************************************************************/
void XImagePutBitmapInXBM_Stream  (
 XImage* This
,void* a_file
,char* a_name
,Pixel a_back
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int row,col;
  int           ibyte;
/*.........................................................................*/
  if(This==NULL)                 return;
  if( (a_name==NULL) || (*a_name=='\0')) return;
  if(a_file==NULL)               return;

/*      Ouverture du fichier :            */
  fprintf ((FILE*)a_file,"\
#define %s_width %d\n\
#define %s_height %d\n\
static char %s_bits[] = {\n\
",a_name,This->width,
  a_name,This->height,
  a_name);
  
  ibyte        = 0;
  for(row=0;row<This->height;row++)
    { 
       int   count;
      unsigned char  byte;
      count = 0;      
      byte  = 0;
      for(col=0;col<This->width;col++)
        {
	  Pixel pixel;
          pixel = XGetPixel(This,col,row);
	  pixel = (pixel==a_back)  ? 0 : 1;
          byte  = pixel==0 ? byte : byte + (1<<count);
          if(count==7) 
            {
              fprintf ((FILE*)a_file," 0x%2.2x,",(unsigned int)byte);
              count = 0;
              byte  = 0;
              if(ibyte==11) {fprintf((FILE*)a_file,"\n");ibyte=0;}
              else          {ibyte++;}
            } 
          else 
            {count++;}

        }
      if(count!=0) /* end of row: a not written byte remain; write it */
        {
          fprintf ((FILE*)a_file," 0x%2.2x,",(unsigned int)byte);
          count = 0;
          byte  = 0;
          if(ibyte==11) {fprintf((FILE*)a_file,"\n");ibyte=0;}
          else          {ibyte++;}
        } 
    }

/* last char is ',' go back to overwrite it*/
  fseek   ((FILE*)a_file,(long int)-1,SEEK_CUR);
  fprintf ((FILE*)a_file,"};\n");
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
void XColorGetRGB (
 XColor* a_color
,double* a_red
,double* a_green
,double* a_blue
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_red!=NULL)    *a_red   = 0.;
  if(a_green!=NULL)  *a_green = 0.;
  if(a_blue!=NULL)   *a_blue  = 0.;
  if(a_color==NULL) return;
  if(a_red!=NULL)    *a_red   =  ((double)   a_color->red   )/0xffff;
  if(a_green!=NULL)  *a_green =  ((double)   a_color->green )/0xffff;
  if(a_blue!=NULL)   *a_blue  =  ((double)   a_color->blue  )/0xffff;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
Ulong XStringGetMask (
 char* a_string
)
/***************************************************************************/
/*
   symtax of a_string is: aaa|bbb|ccc.
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* string;
  unsigned long mask;
  char* pos;
/*.........................................................................*/
  if( (a_string==NULL) || (*a_string=='\0')) return 0;
  string = CStringDuplicate(a_string);
  if(string==NULL) return 0;
  mask = 0;
  pos  = string;
  while(1)
    { char* found;
      found = strchr (pos,'|');
      if(found==NULL) 
        {
          CStringTrim (pos);
	  mask = mask | ConvertStringToMask(pos);
	  break;
	}
      if((*found)=='|')
        {
          *found = '\0';
          CStringTrim (pos);
	  mask = mask | ConvertStringToMask(pos);
	}
      pos = found+1;
    }
  CStringDelete (string);
  return        mask;
}
/***************************************************************************/
static unsigned long ConvertStringToMask (
 char* a_name
)
/***************************************************************************/
/*
   symtax of This is: aaa|bbb|ccc.
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_name==NULL) return 0;
       if(strcmp(a_name,"NoEventMask")==0) return NoEventMask;
  else if(strcmp(a_name,"KeyPressMask")==0) return KeyPressMask;
  else if(strcmp(a_name,"KeyReleaseMask")==0) return KeyReleaseMask;
  else if(strcmp(a_name,"ButtonPressMask")==0) return ButtonPressMask;
  else if(strcmp(a_name,"ButtonReleaseMask")==0) return ButtonReleaseMask;
  else if(strcmp(a_name,"EnterWindowMask")==0) return EnterWindowMask;
  else if(strcmp(a_name,"LeaveWindowMask")==0) return LeaveWindowMask;
  else if(strcmp(a_name,"PointerMotionMask")==0) return PointerMotionMask;
  else if(strcmp(a_name,"PointerMotionHintMask")==0) return PointerMotionHintMask;
  else if(strcmp(a_name,"Button1MotionMask")==0) return Button1MotionMask;
  else if(strcmp(a_name,"Button2MotionMask")==0) return Button2MotionMask;
  else if(strcmp(a_name,"Button3MotionMask")==0) return Button3MotionMask;
  else if(strcmp(a_name,"Button4MotionMask")==0) return Button4MotionMask;
  else if(strcmp(a_name,"Button5MotionMask")==0) return Button5MotionMask;
  else if(strcmp(a_name,"ButtonMotionMask")==0) return ButtonMotionMask;
  else if(strcmp(a_name,"KeymapStateMask")==0) return KeymapStateMask;
  else if(strcmp(a_name,"ExposureMask")==0) return ExposureMask;
  else if(strcmp(a_name,"VisibilityChangeMask")==0) return VisibilityChangeMask;
  else if(strcmp(a_name,"StructureNotifyMask")==0) return StructureNotifyMask;
  else if(strcmp(a_name,"ResizeRedirectMask")==0) return ResizeRedirectMask;
  else if(strcmp(a_name,"SubstructureNotifyMask")==0) return SubstructureNotifyMask;
  else if(strcmp(a_name,"SubstructureRedirectMask")==0) return SubstructureRedirectMask;
  else if(strcmp(a_name,"FocusChangeMask")==0) return FocusChangeMask;
  else if(strcmp(a_name,"PropertyChangeMask")==0) return PropertyChangeMask;
  else if(strcmp(a_name,"ColormapChangeMask")==0) return ColormapChangeMask;
  else if(strcmp(a_name,"OwnerGrabButtonMask")==0) return OwnerGrabButtonMask;
  else {ReportError("ConvertStringToMask",CErrorName);return 0;}
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
  CErrorHandle ("Xx","XDisplay",a_name,a_code);
}
