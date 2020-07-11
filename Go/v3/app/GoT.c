/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#define DEBUG

#include <stdio.h>
#include <math.h>
#ifndef M_PI
#define M_PI       3.1415926535897931160E0
#define M_PI_2     1.5707963267948965580E0  
#endif

#if defined(HAS_GL) && defined(HAS_X)

#include <math.h>
#include <stdlib.h>

#include <X11/Xlib.h>

#include <CPrinter.h>
#include <OShell.h>
#include <OType.h>
#include <CoCommands.h>

#include <GL/glx.h>

#include <OCamera.h>
#include <OCamVRML.h>
#include <Go.h>
#include <GoCommands.h>
#include <GoTypes.h>

#ifdef __cplusplus
extern "C"{
#endif
static void   Initialize                    ();
static Window CreateWindow                  (char*,int,int,unsigned int,unsigned int);
static Bool   WaitForNotify                 (Display*,XEvent*,char*);
#ifdef __cplusplus
}
#endif

static struct 
{
  Display*      display;
  Colormap      colormap;
  XVisualInfo*  vinfo;
  GLXContext    ctx;
  int           privateColormap;
} Class = {NULL,0L,NULL,NULL,1};
/***************************************************************************/
int main (
 int    a_argn
,char*  a_args[]
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OShell              osh;
  Window              win1,win2;
  OCamera             camera1,camera2;
  ONode               node;
  OMatrix             matrix;
  double              angle;
/*.........................................................................*/
  if(a_argn==2)
    {
      Class.privateColormap = 0;
    }

  CWarn("\
  Click on button 2 in any GoT window to stop.\n\
  Click on button 1 in any GoT window to produce :\n\
   - a postscript file out.ps.\n\
   - an Inventor file out.iv.\n\
   - a VRML file out.wrl.\n\
");

  Initialize          ();

  GoLoadColormapX     ();

  win1                = CreateWindow("win 1",0    ,  0,400,200);

  /*First window attached to an osh example.*/
  osh                 = OShellCreate ("osh");
  CoAddCommands       (osh);
  GoAddCommands       (osh);
  OShellExecute       (osh,"\
load GoT.odb\n\
dump OColormap/ocolormap_X\n\
set OCamera/my_camera colormap ocolormap_X\n\
load atpc.odb | viewNode - my_camera\n\
#collect OCamera | dump -;collect ONode | dump -;collect OPrimitive| dump -\n\
getCameraNodes my_camera | normalizeNode - my_camera\n\
");
  camera1             = OCameraGetIdentifier ("my_camera");
  if(camera1==NULL)   CWarn ("OCamera my_camera does not exist.\n");

  /*Add a cube.*/
  node                  = ONodeCreate ("Cube");
  OContextSetColorIndex (OContextGetStaticInstance(),OColorBlue); 
  /*Go* modeling routines use the "static" OContext to retrieve primitives attributes.*/
  GoAddBoxToNode        (node,0.,0.,0.,200.,200.,200.);  
  OCameraViewNode       (camera1,node);

  glXMakeCurrent        (Class.display,win1,Class.ctx);
  {int count;
   OShellExecute       (osh,"rotateCamera my_camera x 45;rotateCamera my_camera y 45");
   for(count=0;count<10;count++) 
     {
       matrix                = OMatrixCreate (OMatrixRotationY,0.314);
       OCameraMultiplyMatrix (camera1,matrix);
       OMatrixDelete         (matrix);
       OCameraRefresh        (camera1);
       glXSwapBuffers        (Class.display,win1);
     }}

  /*Second window attached to a full C example.*/
  win2                           = CreateWindow("win 2",450  ,  0,200,200);
  camera2                        = OCameraCreate("camera2",200,200);
  OCameraSetColormap             (camera2,OColormapGetIdentifier("ocolormap_X"));
  OCameraSetBackgroundColorIndex (camera2,OColorBlack);

  node                           = ONodeCreate ("Line");
  OContextSetColorIndex          (OContextGetStaticInstance(),OColorGreen);
  OCameraSetProjection           (camera2,OProjectionPerspective);
  angle                          = M_PI/2.;
  OCameraSetProjectionAngle      (camera2,angle);
  GoAddBoxToNode                 (node,-1.,-1.,0.,2.,2.,1.);
  OCameraViewNode                (camera2,node);

  glXMakeCurrent                 (Class.display,win2,Class.ctx);
  {int count;
   for(count=0;count<10;count++) 
     {
       OCameraSetProjectionAngle (camera2,angle - count * 0.07);
       OCameraRefresh            (camera2);
       glXSwapBuffers            (Class.display,win2);
     }}

  CWarn  ("Main loop.\n");	      

  while(1)
    { XEvent      xevent;
      if(XPending(Class.display)!=0)
        {
	  XNextEvent  (Class.display,&xevent);
#ifdef DEBUG
	  printf ("XEvent : %d\n",xevent.type);
#endif
               if( (xevent.type==Expose) && (xevent.xany.window==win1) )
            {
	      glXMakeCurrent      (Class.display,win1,Class.ctx);
	      OCameraRefresh      (camera1);
	      glXSwapBuffers      (Class.display,win1);
	    }
          else if( (xevent.type==Expose) && (xevent.xany.window==win2) )
            {
	      glXMakeCurrent      (Class.display,win2,Class.ctx);
	      OCameraRefresh      (camera2);
	      glXSwapBuffers      (Class.display,win2);
	    }
          else if( (xevent.type==ButtonPress) && (xevent.xbutton.button==1) )
            {
	      OCamera camera;
	      if(xevent.xany.window==win2)   
		{
		  camera = camera2;
		  CWarn  ("Produce out.ps, out.iv, out.wrl for window 2.\n");	      
		}
	      else  
		{
		  camera = camera1;
		  CWarn  ("Produce out.ps, out.iv, out.wrl for window 2.\n");	      
		}
	      /*Produce a PostScript file.*/
	      OCameraOpenPS_FileForWriting   ("out.ps");
	      OCameraPutScenePageInPS_Stream (camera); 
	      OCameraClosePS_Stream          ();
	      /*Produce an Inventor file.*/
	      OCameraOpenVRML_FileForWriting   ("out.iv");
	      OCameraPutSceneInVRML_Stream     (camera); 
	      OCameraCloseVRML_Stream          ();
	      /*Produce a VRML file.*/
	      OCameraOpenVRML_FileForWriting   ("out.wrl");
	      OCameraPutSceneInVRML_Stream     (camera); 
	      OCameraCloseVRML_Stream          ();
#ifdef DEBUG
	      printf ("End production.\n");
#endif
	    }
          else if( (xevent.type==ButtonPress) && (xevent.xbutton.button==2) )
            {
              glXDestroyContext (Class.display,Class.ctx);
	      OTypeClearClass   ();
	      a_args            = NULL;
              exit              (EXIT_SUCCESS);
	    }
	}
    }
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static void Initialize (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  static int attributeList[] = { GLX_RGBA,
		    GLX_RED_SIZE, 2,
		    GLX_GREEN_SIZE, 2,
		    GLX_BLUE_SIZE, 2,
 		    GLX_DOUBLEBUFFER,
		    None };
/*.........................................................................*/
  Class.display  = XOpenDisplay(NULL);                                                         
  if(Class.display==NULL) {CWarn("Can't open display.\n");exit(EXIT_SUCCESS);}

  Class.vinfo    = glXChooseVisual(Class.display,DefaultScreen(Class.display),attributeList);
  if(Class.vinfo==NULL)   {CWarn("Can't choose a visual.\n");exit(EXIT_SUCCESS);}

  Class.ctx      = glXCreateContext(Class.display,Class.vinfo,NULL,GL_FALSE);
  if(Class.ctx==NULL) {CWarn("Can't create a GLX context.\n");exit(EXIT_SUCCESS);}

  if(Class.privateColormap==1)
    {
      /* It is better to create a colormap adapted to the visual.*/
      Class.colormap     = XCreateColormap  (Class.display,XDefaultRootWindow(Class.display),Class.vinfo->visual, AllocNone); 
    }
  else
    {
      /* Default colormap does not work on an SGI with SGI libGL.*/
      Class.colormap     = XDefaultColormap (Class.display,DefaultScreen(Class.display));
    }
  if(Class.colormap==0L) {CWarn("Can't create X colormap.\n");exit(EXIT_SUCCESS);}
}
/***************************************************************************/
static Window CreateWindow (
 char* title
,int a_x
,int a_y
,unsigned int a_width
,unsigned int a_height
)
/***************************************************************************/
/* From:
   UNIX>  man glXIntro
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Window               This;
  XSetWindowAttributes swa;
  XSizeHints           sh;
  XTextProperty        tp;
/*.........................................................................*/
  if(Class.display==NULL) return 0L;
  if(Class.vinfo==NULL)   return 0L;

  swa.colormap     = Class.colormap;
  swa.border_pixel = 0L;
  swa.event_mask   = StructureNotifyMask | ExposureMask | ButtonPressMask;
  This             = XCreateWindow(Class.display, 
                                   XDefaultRootWindow(Class.display),
                                   a_x,a_y,a_width,a_height,
				   0,
                                   Class.vinfo->depth,
				   InputOutput,
                                   Class.vinfo->visual,
				   CWBorderPixel|CWColormap|CWEventMask,&swa);
  if(This==0L) {CWarn("Can't create X window.\n");exit(EXIT_SUCCESS);}

  XStringListToTextProperty (&title, 1, &tp);
  sh.flags         = USPosition | USSize;
  XSetWMProperties (Class.display, This, &tp, &tp, 0, 0, &sh, 0, 0);
  XFree            (tp.value);

  XMapWindow       (Class.display, This);
  XRaiseWindow     (Class.display, This);
 {XEvent           event;
  XIfEvent         (Class.display, &event, WaitForNotify, (char*)This);}

  return           This;
}
/***************************************************************************/
static Bool WaitForNotify (
 Display* d
,XEvent*  e
,char*    arg
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  d = NULL;
  return (e->type == MapNotify) && (e->xmap.window == (Window)arg);
}

#else
#define CWarn printf
int main() {CWarn ("C macro HAS_GL or HAS_X not defined.\n");return 0;}
#endif
