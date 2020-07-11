/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifdef HAS_X

#include <stdlib.h>
#include <string.h>

#include <CMemory.h>
#include <CPrinter.h>

#include <glDriver.h>
#include <glDriverZ.h>
#include <glDriverX.h>

#include <ZBuffer.h>

#include <glx.h>

typedef struct _GLXContextRecord
{
  int first;
  int buffering;
} GLXContextRecord;

#ifdef __cplusplus
extern "C"{
#endif
static void  ClearClass              ();
static void  PutZBufferToWindow      ();
static void  ChangeDoubleBufferSize  (int,int);
static void  PutDoubleBufferToWindow ();
static void  InitializeGC            ();
static void  FillBitmap              (Display*,Window,GC,unsigned int,unsigned int,int,int,char,GLubyte*);
#ifdef __cplusplus
}
#endif

#define BUFFERING_SIMPLE 0
#define BUFFERING_DOUBLE 1

static struct 
{
  GLXContextRecord glctx;
  Display*      display;
  Window        window;
  Drawable      drawable;
  GC            gc;
  int           ctxn;
  XImage*       zimage;
  Pixmap        pixmap;
  int           pixw,pixh;
} Class = {{0,BUFFERING_SIMPLE},NULL,0L,0L,NULL,0,NULL,0L,0,0};
/***************************************************************************/
static void ClearClass (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  glClearClass     ();

  glDriverX_ClearClass    (); /*To free colors (even in ZBuffer mode.*/

  if(Class.gc!=NULL)     XFreeGC (Class.display,Class.gc);
  Class.gc         = NULL;

  if(Class.zimage!=NULL) 
    {
      CMemoryFreeBlock (Class.zimage->data);
      XFree            ((char*)Class.zimage);
    }
  Class.zimage     = NULL;

  if(Class.pixmap!=0L) XFreePixmap (Class.display,Class.pixmap);
  Class.pixmap     = 0L;

  Class.ctxn       = 0;
  Class.display    = NULL;
  Class.drawable   = 0L;
}
/***************************************************************************/
Bool glXQueryExtension (
 Display* a_display
,int* a_errorBase
,int* a_eventBase
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_display==NULL) return False;
  a_errorBase = 0;
  a_eventBase = 0;
  return True;
}
/***************************************************************************/
Bool glXQueryVersion (
 Display* a_display
,int* a_major
,int* a_minor
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_major!=NULL) *a_major = 0;
  if(a_minor!=NULL) *a_minor = 0;
  if(a_display==NULL) return False;
  if(a_major!=NULL) *a_major = 1;
  if(a_minor!=NULL) *a_minor = 0;
  return True;
}
/***************************************************************************/
XVisualInfo* glXChooseVisual (
 Display* a_display
,int a_screen
,int* a_atbs
)
/***************************************************************************/
/*
  Use XGetVisualInfo to alloc the XVisualInfo. 
  A malloc(sizeof(XVisualInfo)) or Xmalloc is not freeable with the
 XFree of eXcursion on Window/NT.  
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  XVisualInfo*  vinfo;
/*.........................................................................*/
  if(a_display==NULL) return NULL;

/*printf ("debug:GLo\n");*/

 {XVisualInfo temp;
  long        mask;
  int         n;
  mask        = VisualScreenMask;
  temp.screen = a_screen;
  vinfo       = XGetVisualInfo(a_display, mask, &temp, &n );}
  if(vinfo==NULL) return NULL;

  Class.glctx.buffering = BUFFERING_SIMPLE;

  if(a_atbs!=NULL)
    { 
      int* ptr;
      ptr  = a_atbs;
      while(1)
        { int value;
          value = *ptr;
               if(value==None) break;
               if(value==GLX_DOUBLEBUFFER)
            {
              Class.glctx.buffering = BUFFERING_DOUBLE;
	    }
          ptr++;
	}
    }


  vinfo->screen = a_screen;
  vinfo->depth  = DefaultDepth  (a_display,a_screen);
  vinfo->visual = DefaultVisual (a_display,a_screen);

  return        vinfo;
}
/***************************************************************************/
GLXContext glXCreateContext (
 Display* a_display
,XVisualInfo* a_vis
,GLXContext a_sharelist
,Bool a_direct
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  GLXContext This;
/*.........................................................................*/
  if(a_display==NULL) return NULL;

  This                = (GLXContext)CMemoryAllocateBlock(sizeof(GLXContextRecord));
  if(This==NULL)      return NULL;

  This->buffering    = Class.glctx.buffering;

  This->first        = 1;

  a_display          = NULL;
  a_vis              = NULL;
  a_sharelist        = NULL;
  a_direct           = 0; 
  Class.ctxn++;
  return             This;
}
/***************************************************************************/
void glXDestroyContext (
 Display* a_display
,GLXContext a_ctx
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CMemoryFreeBlock  (a_ctx);
  Class.ctxn        --;
  if(Class.ctxn<=0) ClearClass ();
  a_display         = NULL;
}
/***************************************************************************/
Bool glXMakeCurrent (
 Display*    a_display
,GLXDrawable a_window
,GLXContext  a_ctx
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  XWindowAttributes watbs;
  int               vclass;
/*.........................................................................*/
  if(a_display==NULL)        return 0;
  if(a_window==0L)           return 0;
  if(a_ctx==NULL)            return 0;

  Class.glctx           = *a_ctx;

  Class.display         = a_display;

  XGetWindowAttributes  (Class.display,a_window,&watbs);

  glSetDriver        (glDriverX_ClearClass,
		      glDriverX_ConvertRGB_ToPixel,
		      glDriverX_ConvertPixelToRGB,
		      glDriverX_SetViewport,
		      glDriverX_ClearColorBuffer,
		      glDriverX_DrawPrimitive,
		      glDriverX_DrawBuffer);
  glDriverX_SetWindow       (watbs.width,watbs.height);
  glDriverZ_SetWindow       (watbs.width,watbs.height);
#ifdef __cplusplus
  vclass                    = watbs.visual->c_class;
#else
  vclass                    = watbs.visual->class;
#endif
  glDriverX_SetColormap     (watbs.colormap,vclass);

  if(Class.glctx.buffering==BUFFERING_DOUBLE)
    {
      if(  (Class.pixmap==0L) ||
           ((watbs.width>Class.pixw) || (watbs.height>Class.pixh))
         )
	ChangeDoubleBufferSize         (watbs.width,watbs.height);

      Class.drawable        = Class.pixmap;
      Class.window          = a_window;

      glDriverX_SetCurrentWindow  ((void*)Class.display,Class.pixmap);
    }
  else
    {
      Class.drawable             = a_window;
      Class.window               = a_window;
      glDriverX_SetCurrentWindow ((void*)Class.display,a_window); /*For glDriverX_ConvertRGB_ToPixel.*/
    }

  if(a_ctx->first==1)
    {
      glViewport   (0,0,watbs.width,watbs.height); 
      a_ctx->first = 0;
    }

  return 1;
}
/***************************************************************************/
Bool glXIsDirect (
 Display* a_display
,GLXContext a_context
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  a_display = NULL;
  a_context = NULL;
  return False;
}
/***************************************************************************/
void glXSwapBuffers (
 Display*    a_display
,GLXDrawable a_window
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int mode;
/*.........................................................................*/
  if(a_display==NULL) return;
  if(a_window==0L)    return;
  glGetIntegerv (GL_RENDER_MODE,&mode);
  if(mode==GL_RENDER)
    {
      if(glIsEnabled(GL_DEPTH_TEST)==GL_TRUE)
	{
	  PutZBufferToWindow();
	}
      else
	{
	  if(Class.display==NULL)                     return;
	  if(Class.glctx.buffering==BUFFERING_DOUBLE) PutDoubleBufferToWindow ();
	  XSync (Class.display,False);
	}
    }
}
/***************************************************************************/
void glXWaitX (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.display==NULL) return;
  XSync (Class.display,False);
}
/***************************************************************************/
void glXWaitGL (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
}
/***************************************************************************/
void glXUseXFont (
 Font font
,int first
,int count
,int listbase
)
/***************************************************************************/
/*
  From Mesa-1.2.5/src/xfonts.c
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Display*      dpy;
  Window        win;
  Pixmap        pixmap;
  GC            gc;
  XGCValues     values;
  unsigned long valuemask;
  XFontStruct*  fs;
  GLint         swapbytes, lsbfirst, rowlength;
  GLint         skiprows, skippixels, alignment;
  unsigned int  max_width, max_height, max_bm_width, max_bm_height;
  GLubyte*      bm;
  int           i;
/*.........................................................................*/
  if(Class.display==NULL) return;
  if(Class.window ==0L)   return;
  dpy = Class.display;
  win = Class.window;

  fs = XQueryFont (dpy, font);  
  if (fs==NULL)
    {
      CWarn ("Couldn't get font structure information.\n");
      return;
    }

  /* Allocate a bitmap that can fit all characters.  */
  max_width     = fs->max_bounds.rbearing - fs->min_bounds.lbearing;
  max_height    = fs->max_bounds.ascent + fs->max_bounds.descent;
  max_bm_width  = (max_width + 7) / 8;
  max_bm_height = max_height;

  bm = (GLubyte *) CMemoryAllocateBlock ((max_bm_width * max_bm_height) * sizeof (GLubyte));
  if (bm==NULL)
    {
      CWarn ("Couldn't allocate bitmap in glXUseXFont().\n");
      return;
    }

  /* Save the current packing mode for bitmaps.  */
  glGetIntegerv	(GL_UNPACK_SWAP_BYTES,  &swapbytes);
  glGetIntegerv	(GL_UNPACK_LSB_FIRST,   &lsbfirst);
  glGetIntegerv	(GL_UNPACK_ROW_LENGTH,  &rowlength);
  glGetIntegerv	(GL_UNPACK_SKIP_ROWS,   &skiprows);
  glGetIntegerv	(GL_UNPACK_SKIP_PIXELS, &skippixels);
  glGetIntegerv	(GL_UNPACK_ALIGNMENT,   &alignment);

  /* Enforce a standard packing mode which is compatible with
     FillBitmap() from above.  This is actually the default mode,
     except for the (non)alignment.  */
  glPixelStorei	(GL_UNPACK_SWAP_BYTES,  GL_FALSE);
  glPixelStorei	(GL_UNPACK_LSB_FIRST,   GL_FALSE);
  glPixelStorei	(GL_UNPACK_ROW_LENGTH,  0);
  glPixelStorei	(GL_UNPACK_SKIP_ROWS,   0);
  glPixelStorei	(GL_UNPACK_SKIP_PIXELS, 0);
  glPixelStorei	(GL_UNPACK_ALIGNMENT,   1);

  pixmap      = XCreatePixmap (dpy, win, 10, 10, 1);
  values.foreground = BlackPixel (dpy, DefaultScreen (dpy));
  values.background = WhitePixel (dpy, DefaultScreen (dpy));
  values.font = fs->fid;
  valuemask   = GCForeground | GCBackground | GCFont;
  gc          = XCreateGC (dpy, pixmap, valuemask, &values);
  XFreePixmap (dpy, pixmap);

  for (i = 0; i < count; i++)
    {
      unsigned int width, height, bm_width, bm_height;
      GLfloat      x0, y0, dx, dy;
      XCharStruct* ch;
      int          x, y;
      int          c = first + i;
      int          list = listbase + i;

      if (fs->per_char && (c >= (int)fs->min_char_or_byte2) && (c <= (int)fs->max_char_or_byte2))
	ch = &fs->per_char[c-fs->min_char_or_byte2];
      else
	ch = &fs->max_bounds;

      width  = ch->rbearing - ch->lbearing;
      height = ch->ascent + ch->descent;
      x0     = - ch->lbearing;
      y0     = ch->descent - 1;
      dx     = ch->width;
      dy     = 0;

      /* X11's starting point.  */
      x      = - ch->lbearing;
      y      = ch->ascent;
      
      /* Round the width to a multiple of eight.  We will use this also
	 for the pixmap for capturing the X11 font.  This is slightly
	 inefficient, but it makes the OpenGL part real easy.  */
      bm_width = (width + 7) / 8;
      bm_height = height;

      glNewList (list, GL_COMPILE);
      if ((c >= (int)fs->min_char_or_byte2) && (c <= (int)fs->max_char_or_byte2) && (bm_width > 0) && (bm_height > 0))
	  {
	    memset      ((void*)bm,(int) '\0', (size_t)(bm_width * bm_height));
	    FillBitmap  (dpy, win, gc, bm_width, bm_height, x, y, (char)c, bm);
	    glBitmap    (width, height, x0, y0, dx, dy, bm);
	  }
	else
	  glBitmap (0, 0, 0.0, 0.0, dx, dy, NULL);
      glEndList ();
    }

  CMemoryFreeBlock (bm);
  XFreeFontInfo    ( NULL, fs, 0 );
  XFreeGC          (dpy, gc);

  /* Restore saved packing modes.  */    
  glPixelStorei(GL_UNPACK_SWAP_BYTES, swapbytes);
  glPixelStorei(GL_UNPACK_LSB_FIRST, lsbfirst);
  glPixelStorei(GL_UNPACK_ROW_LENGTH, rowlength);
  glPixelStorei(GL_UNPACK_SKIP_ROWS, skiprows);
  glPixelStorei(GL_UNPACK_SKIP_PIXELS, skippixels);
  glPixelStorei(GL_UNPACK_ALIGNMENT, alignment);
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static void PutZBufferToWindow (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  unsigned int width,height;
/*.........................................................................*/
  if(Class.display==NULL)  return;
  if(Class.drawable==0L)   return;
  InitializeGC                ();
  if(Class.gc==NULL)       return;

 {Window        root;
  int           cx,cy;
  unsigned int  border,depth;
  if(XGetGeometry(Class.display,Class.drawable,
		  &root,&cx,&cy,&width,&height,&border,&depth)==0) return;}

  if(Class.zimage!=NULL) 
    {
      CMemoryFreeBlock (Class.zimage->data);
      XFree    ((char*)Class.zimage);
    }
  Class.zimage = NULL;

 {ZPixel*      zdata;
  int          zbw,zbh;

  zdata        = ZBufferGetColorBuffer(&zbw,&zbh);

/*  printf ("debug:%d %d\n",zbw,zbh);*/
  if(zdata==NULL) return;

/*ZPixel is unsigned int (always 32 bits, long could be 64 on some machine).
  Screen bits_per_pixel could be <32 (1, 8 plane screen).
  Create an image that has bits_per_pixel = 32.
  Could we have problem on screen with bits_per_pixel <32 ????
*/
  Class.zimage                  = XCreateImage  (Class.display,
						 XDefaultVisualOfScreen (XDefaultScreenOfDisplay(Class.display)),
						 XDefaultDepthOfScreen  (XDefaultScreenOfDisplay(Class.display)),
						 ZPixmap,0,
						 NULL,
						 (unsigned int)zbw,(unsigned int)zbh,8,0);
  if(Class.zimage==NULL)        return;
  Class.zimage->data            = (char*) CMemoryAllocateBlock ( Class.zimage->height * Class.zimage->bytes_per_line * sizeof(char));
  if(Class.zimage->data==NULL) 
    {
      XFree        ((char*)Class.zimage);
      Class.zimage = NULL;
    }
  if(Class.zimage==NULL)             return;

 {int          row,col;
  for(row=0;row<zbh;row++)
    for(col=0;col<zbw;col++)
      {
        XPutPixel (Class.zimage,col,row,*zdata);
        zdata++;
      }}
  }

  XPutImage     (Class.display,Class.drawable,Class.gc,Class.zimage,0,0,0,0,width,height);

  if(Class.glctx.buffering==BUFFERING_DOUBLE) PutDoubleBufferToWindow ();
}
/***************************************************************************/
/********** Double buffer **************************************************/
/***************************************************************************/
/***************************************************************************/
static void ChangeDoubleBufferSize (
 int a_width
,int a_height
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if( (Class.pixmap!=0L) && (Class.pixw==a_width)  && (Class.pixh==a_height)  ) return;

  if(Class.display==NULL) return;

  if(Class.pixmap!=0L) XFreePixmap (Class.display,Class.pixmap);
  Class.pixmap  = 0L;
  Class.pixw    = 0;
  Class.pixh    = 0;

  if( (a_width<=0) || (a_height<=0) ) return;

  Class.pixmap        = XCreatePixmap  (Class.display,
					XDefaultRootWindow(Class.display),
					a_width,a_height,
					XDefaultDepthOfScreen(XDefaultScreenOfDisplay(Class.display)));
  if(Class.pixmap==0L) return;

 {Window        root;
  int           cx,cy;
  unsigned int  width,height,border,depth;
  if( (Class.pixmap!=0L) && 
      (XGetGeometry(Class.display,Class.pixmap,&root,&cx,&cy,&width,&height,&border,&depth)==0) 
     )
    {
      CWarn        ("Pixmap allocation failed.\n");
      Class.pixmap = 0L;
      return;
    }}
  Class.pixw = a_width;
  Class.pixh = a_height;

/*printf("debug:create pixmap:%d %d\n",Class.pixw,Class.pixh);*/

}
/***************************************************************************/
static void PutDoubleBufferToWindow (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  XWindowAttributes     watbs;
/*.........................................................................*/
  if(Class.display==NULL)   return;
  if(Class.window==0L)    return;
  if(Class.pixmap==0L)    return;

  InitializeGC               ();
  if(Class.gc==NULL)        return;

  XGetWindowAttributes (Class.display,Class.window,&watbs);

  XCopyArea            (Class.display,Class.pixmap,Class.window,Class.gc,0,0,watbs.width,watbs.height,0,0);
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static void InitializeGC (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  unsigned long flags = 0L;
  XGCValues     args;
/*.........................................................................*/
  if(Class.gc!=NULL)       return;

  if(Class.display==NULL) return;

  Class.gc           = (GC) XCreateGC (Class.display,XDefaultRootWindow(Class.display),flags,&args);
}
/***************************************************************************/
static void FillBitmap (
 Display* dpy
,Window win
,GC gc
,unsigned int width
,unsigned int height
,int x0
,int y0
,char c
,GLubyte *bitmap
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  XImage *image;
  unsigned int x, y;
  Pixmap pixmap;
/*.........................................................................*/
  pixmap = XCreatePixmap (dpy, win, 8*width, height, 1);
  XSetForeground(dpy, gc, 0);
  XFillRectangle (dpy, pixmap, gc, 0, 0, 8*width, height);
  XSetForeground(dpy, gc, 1);
  XDrawString (dpy, pixmap, gc, x0, y0, &c, 1);

  image = XGetImage (dpy, pixmap, 0, 0, 8*width, height, 1, XYPixmap);

  /* Fill the bitmap (X11 and OpenGL are upside down wrt each other).  */
  for (y = 0; y < height; y++)
    for (x = 0; x < 8*width; x++)
      if (XGetPixel (image, x, y))
	bitmap[width*(height - y - 1) + x/8] |= (1 << (7 - (x % 8)));
  
  XFreePixmap (dpy, pixmap);
  XDestroyImage (image);
}

#else
#include <stddef.h>
#include <CPrinter.h>

#define NO_X_H
typedef struct _XDisplay    Display;
typedef struct _XVisualInfo XVisualInfo;
#define Bool int

#include <glx.h>
Bool glXQueryExtension (Display* a_display,int* a_errorBase,int* a_eventBase) 
{
  CWarn       ("glXQueryExtension : dummy.\n");
  a_display   = NULL;
  a_errorBase = 0;
  a_eventBase = 0;
  return      0;
}
Bool glXQueryVersion (Display* a_display,int* a_major,int* a_minor)
{
  CWarn     ("glXQueryVersion : dummy.\n");
  a_display = NULL;
  a_major   = 0;
  a_minor   = 0;
  return    0;
}
XVisualInfo* glXChooseVisual (Display* a_display,int a_screen,int* a_atbs)
{
  CWarn       ("glXChooseVisual : dummy.\n");
  a_display   = NULL;
  a_screen    = 0;
  a_atbs      = NULL;
  return      NULL;
}
GLXContext glXCreateContext (Display* a_display,XVisualInfo* a_vis,GLXContext a_sharelist,Bool a_direct) 
{
  CWarn       ("glXCreateContext : dummy.\n");
  a_display   = NULL;
  a_vis       = NULL;
  a_sharelist = NULL;
  a_direct    = 0; 
  return      NULL;
}
void glXDestroyContext (Display* a_display,GLXContext a_ctx) 
{
  CWarn       ("glXDestroyContext : dummy.\n");
  a_display   = NULL;
  a_ctx       = NULL;
}
Bool glXMakeCurrent (Display*    a_display,GLXDrawable a_window,GLXContext  a_ctx)
{
  CWarn       ("glXMakeCurrent : dummy.\n");
  a_display   = NULL;
  a_window    = 0L;
  a_ctx       = NULL;
  return      0;
}
void glXWaitX       () 
{
  CWarn ("glXWaitX : dummy.\n");
}
void glXWaitGL      ()                     
{
  CWarn ("glXWaitGL : dummy.\n");
}
Bool glXIsDirect    (Display* a_display,GLXContext a_ctx)  
{
  CWarn       ("glXIsDirect : dummy.\n");
  a_display   = NULL;
  a_ctx       = NULL;
  return      0;
}
void glXSwapBuffers (Display* a_display,GLXDrawable a_window) 
{
  CWarn       ("glXSwapBuffers : dummy.\n");
  a_display   = NULL;
  a_window    = 0L;
}
void glXUseXFont (Font font,int first,int count,int listbase)
{
  CWarn       ("glXUseXFont : dummy.\n");
  font        = 0L;
  first       = 0;
  count       = 0;
  listbase    = 0;
}
#endif /*HAS_X*/

