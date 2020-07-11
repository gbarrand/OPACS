/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#define HAS_CO

#include <stdio.h>

#if defined(HAS_GL) && defined(HAS_X)

#include <math.h>
#include <stdlib.h>

#include <X11/Xlib.h>

#ifdef HAS_CO
#include <CCharacter.h>
#include <CMemory.h>
#include <CPrinter.h>
#else
#define CWarn  printf
#define CInfo  printf
#define CInfoF printf
#endif

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>

#ifdef __cplusplus
extern "C"{
#endif
static void   SetPickMatrix                 (GLdouble,GLdouble,GLdouble,GLdouble,GLint*);
static void   Initialize                    ();
static void   DumpHits                      (GLint,GLuint*);
static void   DrawWin1                      (GLenum);
static void   DrawBack                      (GLenum);
static void   DrawCube                      (GLenum);
static void   DrawString                    (char*);
static void   ConvertCharacterToVertices    (char);
static Window CreateWindow                  (char*,int,int,unsigned int,unsigned int);
static void   GetWindowSize                 (Display*,Window,int*,int*);
static Bool   WaitForNotify                 (Display*,XEvent*,char*);
static void   UniconifyWindow               (Display*,Window);
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
  Window win1,win2,win3;
  int    width,height;
  GLint  viewport[4];
  double aspect;
  GLuint list;
/*.........................................................................*/
  if(a_argn==2)
    {
      Class.privateColormap = 0;
    }

  Initialize     ();

/*Set window 1.*/
  win1           = CreateWindow("win 1",0    ,  0,400,200);
  if(glXMakeCurrent(Class.display,win1,Class.ctx)==False)
    {
      CWarn("glXMakeCurrent failed.\n");
    }
  GetWindowSize  (Class.display,win1,&width,&height);

/* Produce a white line on a black window
  glBegin     (GL_LINES);
  glVertex3d  (-0.9,-0.9,0.);
  glVertex3d  ( 0.9,0.9,0.);
  glEnd       ();
  glFinish    ();
  MainLoop    ();
*/
  glEnable       (GL_DEPTH_TEST);
  glEnable       (GL_SCISSOR_TEST);
  glShadeModel   (GL_FLAT);
  glViewport     (0,0,width,height);
  glScissor      (0,0,width,height);
  glClearColor   (0.8,0.8,0.8,0.);
/*glClear        (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);*/ /*C++ do not compile this !.*/
  glClear        (GL_COLOR_BUFFER_BIT);
  glClear        (GL_DEPTH_BUFFER_BIT);

  glViewport     (width/4,height/2,width/2,height/2);
  glScissor      (width/4,height/2,width/2,height/2);

  glGetIntegerv  (GL_VIEWPORT,viewport);
  aspect         = ((double)viewport[2])/((double)viewport[3]);

  glMatrixMode   (GL_PROJECTION); 
  glLoadIdentity ();
/*glFrustum      (-aspect,aspect,-1.,1.,1.,4.);*/
  glOrtho        (-aspect,aspect,-1.,1.,-10.,10.);
  glMatrixMode   (GL_MODELVIEW);

  glClearColor   (0.,0.,0.,0.);
  glClear        (GL_COLOR_BUFFER_BIT);
  glClear        (GL_DEPTH_BUFFER_BIT);
  DrawWin1       (GL_RENDER);

  glViewport     (0,0,width,height/4);
  glScissor      (0,0,width,height/4);
  glGetIntegerv  (GL_VIEWPORT,viewport);
  aspect         = ((double)viewport[2])/((double)viewport[3]);

  glMatrixMode   (GL_PROJECTION); 
  glLoadIdentity ();
  glOrtho        (0.,aspect * 1.1,-0.1,1.,-10.,10.);

  glClearColor   (0.4,0.4,0.4,0.);
  glClear        (GL_COLOR_BUFFER_BIT);
  glClear        (GL_DEPTH_BUFFER_BIT);

  glMatrixMode   (GL_MODELVIEW);
  glLoadIdentity ();
  glColor3d      (1.0,1.0,1.0);   
  glScaled       (0.3,1.,1.);
  DrawString     ("Pick cubes with button 1.");

  glFinish       ();
  glXSwapBuffers (Class.display,win1);

/*Set window 3.*/
  win3           = CreateWindow("win 3",0    ,450,200,400);
  if(glXMakeCurrent(Class.display,win3,Class.ctx)==False)
    {
      CWarn("glXMakeCurrent failed.\n");
    }
  GetWindowSize  (Class.display,win3,&width,&height);

  glEnable       (GL_DEPTH_TEST);
  glDisable      (GL_SCISSOR_TEST);
  glShadeModel   (GL_FLAT);
  glViewport     (0,0,width,height);
  glGetIntegerv  (GL_VIEWPORT,viewport);
  aspect         = ((double)viewport[2])/((double)viewport[3]);
  glMatrixMode   (GL_PROJECTION); 
  glLoadIdentity ();
  glFrustum      (-aspect,aspect,-1.,1.,1.,4.);
  glMatrixMode   (GL_MODELVIEW);

  glLoadIdentity ();
  gluLookAt      (3.,0.,0.,  /*eye*/
                  0.,0.,0.,  /*center*/
                  0.,-1.,0.); /*up*/

  glClearColor   (0.3,0.3,0.3,0.);
  glClear        (GL_COLOR_BUFFER_BIT);
  glClear        (GL_DEPTH_BUFFER_BIT);
  DrawBack       (GL_RENDER);
  DrawCube       (GL_RENDER);
  glFinish       ();
  glXSwapBuffers (Class.display,win3);

/*Set window 2.*/
  win2           = CreateWindow("win 2",450  ,  0,200,400);
  if(glXMakeCurrent(Class.display,win2,Class.ctx)==False)
    {
      CWarn("glXMakeCurrent failed.\n");
    }
  GetWindowSize  (Class.display,win2,&width,&height);

  glEnable       (GL_DEPTH_TEST);
  glDisable      (GL_SCISSOR_TEST);
  glShadeModel   (GL_FLAT);
  glViewport     (0,0,width,height);
  glGetIntegerv  (GL_VIEWPORT,viewport);
  aspect         = ((double)viewport[2])/((double)viewport[3]);
  glMatrixMode   (GL_PROJECTION); 
  glLoadIdentity ();
  glFrustum      (-aspect,aspect,-1.,1.,1.,4.);
  glMatrixMode   (GL_MODELVIEW);

  glClearColor   (0.5,0.5,0.5,0.0);

  list           = glGenLists (1);
  glNewList      (list,GL_COMPILE);
  DrawCube       (GL_RENDER);
  glEndList      ();

  {int count;for(count=0;count<20;count++) {
  glClear        (GL_COLOR_BUFFER_BIT);
  glClear        (GL_DEPTH_BUFFER_BIT);
  glLoadIdentity ();
  glTranslated   (0.,0.,-2.);
  DrawBack       (GL_RENDER);
  glRotated      (5. * count,0.,1.,0.);
  /*  DrawCube       (GL_RENDER);*/
  glCallList     (list);
  glFinish       ();
  glXSwapBuffers (Class.display,win2);
  glXWaitX       ();
  }}

  glDeleteLists  (list,1);

  glXWaitX       ();

  while(1)
    { XEvent      xevent;
      if(XPending(Class.display)!=0)
        {
	  XNextEvent  (Class.display,&xevent);
               if(xevent.type==ButtonPress && xevent.xbutton.button==1)
            {
#define BUFSIZE 512
              GLuint         selectBuffer[BUFSIZE];
              GLint          hitn;
              double         xpick,ypick;
/*Picking on window 1.*/
/*Set window 1.*/
	      if(glXMakeCurrent (Class.display,win1,Class.ctx)==False)
		{
		  CWarn("glXMakeCurrent failed.\n");
		}
	      GetWindowSize  (Class.display,win1,&width,&height);

              glSelectBuffer (BUFSIZE,selectBuffer);
              glRenderMode   (GL_SELECT);
              glInitNames    ();
              glPushName     (0);

	      glShadeModel   (GL_FLAT);
	      glViewport     (0,0,width,height);
	      glClearColor   (0.8,0.8,0.8,0.);
	      glClear        (GL_COLOR_BUFFER_BIT);
	      glClear        (GL_DEPTH_BUFFER_BIT);
	      
	      glViewport     (width/4,height/2,width/2,height/2);

/*
              glGetIntegerv  (GL_VIEWPORT,viewport);
	      aspect         = ((double)viewport[2])/((double)viewport[3]);

              xpick          = (GLdouble)xevent.xbutton.x;
              ypick          = (GLdouble)height - (GLdouble)xevent.xbutton.y; 

	      glMatrixMode   (GL_PROJECTION); 
	      glLoadIdentity ();
              gluPickMatrix  (xpick,ypick,10. * aspect ,10.,viewport);
 	      glOrtho        (-aspect,aspect,-1.,1.,1.,2.);
	      glMatrixMode   (GL_MODELVIEW);
*/

              glGetIntegerv  (GL_VIEWPORT,viewport);
              xpick          = (GLdouble)xevent.xbutton.x;
              ypick          = (GLdouble)height - (GLdouble)xevent.xbutton.y; 

	      glMatrixMode   (GL_PROJECTION); 
	      glLoadIdentity ();
              SetPickMatrix  (xpick,ypick,6.,6.,viewport);
	      glMatrixMode   (GL_MODELVIEW);

	      glClearColor   (0.,0.,0.,0.);
	      glClear        (GL_COLOR_BUFFER_BIT);
	      glClear        (GL_DEPTH_BUFFER_BIT);
	      DrawWin1       (GL_SELECT);

              hitn           = glRenderMode   (GL_RENDER);
              DumpHits       (hitn,selectBuffer);

	    }
          else if(xevent.type==ButtonPress && xevent.xbutton.button==2)
            {
              glXDestroyContext (Class.display,Class.ctx);
#ifdef HAS_CO
              CInfoF            ("%s\n",CMemoryGetStatus());
#endif
	      a_args            = NULL;
              exit              (EXIT_SUCCESS);
	    }
          else if(xevent.type==ButtonPress && xevent.xbutton.button==3)
            {
	      UniconifyWindow   (Class.display,win1);
	    }
	}
    }
}
/***************************************************************************/
static void SetPickMatrix (
 GLdouble a_x
,GLdouble a_y
,GLdouble a_width
,GLdouble a_height
,GLint*   a_viewport
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  double l,r,b,t;
  double aspect,x,y;
  double ln,rn,bn,tn;
/*.........................................................................*/
  aspect = ((GLdouble)a_viewport[2])/((GLdouble)a_viewport[3]);
 
  l = - aspect;
  r =   aspect;
  b = - 1.;
  t =   1.;
 
  x  = a_x - ((GLdouble)a_viewport[0]);
  y  = a_y - ((GLdouble)a_viewport[1]);

  ln = l + (x -  a_width/2.) * (r-l) / ((GLdouble)a_viewport[2]);
  rn = l + (x +  a_width/2.) * (r-l) / ((GLdouble)a_viewport[2]);
  bn = b + (y - a_height/2.) * (t-b) / ((GLdouble)a_viewport[3]);
  tn = b + (y + a_height/2.) * (t-b) / ((GLdouble)a_viewport[3]);

  glViewport     ((GLint)(a_x - a_width/2.),(GLint)(a_y - a_height/2.),(GLsizei)a_width,(GLsizei)a_height);
/*glFrustum      (ln,rn,bn,tn,1.,4.);*/
  glOrtho        (ln,rn,bn,tn,-10.,10.);
}
/***************************************************************************/
static void DumpHits (
 GLint   a_hitn
,GLuint* a_selectBuffer
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int count;
  GLuint* ptr;
/*.........................................................................*/
  ptr     = a_selectBuffer; 
  for(count=0;count<a_hitn;count++)
    { 
      GLuint       namen,namei;
      CInfo        ("hit:\n");
      namen        = *ptr;ptr++;
      CInfoF("  number of names:%d\n",namen);
      CInfoF("               z1:%d\n",*ptr);ptr++;
      CInfoF("               z2:%d\n",*ptr);ptr++;
      for(namei=0;namei<namen;namei++)
	{
	  CInfoF("            namei:%d\n",*ptr);ptr++;
	}
    }
}
/***************************************************************************/
static void DrawWin1 (
 GLenum a_mode
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  glLoadIdentity ();
  glRotated      (45.,1.,1.,0.);
  if(a_mode==GL_SELECT) glLoadName (100);
  DrawBack       (GL_RENDER);
  DrawCube       (a_mode);
  glLoadIdentity ();
  glTranslated   (1.5,0.,0.);
  glRotated      (45.,1.,1.,0.);
/*glScaled       (0.5,0.5,0.5);*/
  if(a_mode==GL_SELECT) glLoadName (200);
  DrawBack       (GL_RENDER);
  DrawCube       (a_mode);
}
/***************************************************************************/
static void DrawBack (
 GLenum a_mode
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_mode==GL_SELECT) glPushName (40);

  glColor3d      (0.7,0.7,0.7);   
  glBegin        (GL_POLYGON);
  glVertex3d     ( 1.0,-1.0, 1.0);
  glVertex3d     ( 1.0,-1.0,-1.0);
  glVertex3d     (-1.0,-1.0,-1.0);
  glVertex3d     (-1.0,-1.0, 1.0);
  glEnd          ();

  if(a_mode==GL_SELECT) glPopName  ();
}
/***************************************************************************/
static void DrawCube (
 GLenum a_mode
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  glLineWidth    (4.0);

  if(a_mode==GL_SELECT) glPushName (10);

  glColor3d      (1.0,0.0,0.);   
  glBegin        (GL_POLYGON);
  glVertex3d     ( 0.5,-0.5,0.5);
  glVertex3d     ( 0.5, 0.5,0.5);
  glVertex3d     (-0.5, 0.5,0.5);
  glVertex3d     (-0.5,-0.5,0.5);
  glEnd          ();

  if(a_mode==GL_SELECT) glPopName  ();

  if(a_mode==GL_SELECT) glPushName (20);

  glColor3d      (0.0,1.0,0.);   
  glBegin        (GL_POLYGON);
  glVertex3d     ( 0.5,-0.5,-0.5);
  glVertex3d     ( 0.5, 0.5,-0.5);
  glVertex3d     (-0.5, 0.5,-0.5);
  glVertex3d     (-0.5,-0.5,-0.5);
  glEnd          ();

  if(a_mode==GL_SELECT) glPopName  ();

  if(a_mode==GL_SELECT) glPushName (30);

  glColor3d      (1.0,1.0,0.);   
  glBegin        (GL_LINES);
  glVertex3d     ( 0.5,-0.5, 0.5);
  glVertex3d     ( 0.5,-0.5,-0.5);
  glVertex3d     ( 0.5, 0.5, 0.5);
  glVertex3d     ( 0.5, 0.5,-0.5);
  glVertex3d     (-0.5, 0.5, 0.5);
  glVertex3d     (-0.5, 0.5,-0.5);
  glVertex3d     (-0.5,-0.5, 0.5);
  glVertex3d     (-0.5,-0.5,-0.5);
  glEnd          ();

  if(a_mode==GL_SELECT) glPopName  ();

}
/***************************************************************************/
static void DrawString (
 char* a_string 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* itema;
/*.........................................................................*/
  if( (a_string==NULL) || (*a_string=='\0') ) return;
  glLineWidth    (1.0);
  for(itema=a_string;*itema!='\0';itema++)
    { 
      ConvertCharacterToVertices   (*itema);
      glTranslated (1.,0.,0.);
    }
}
/***************************************************************************/
static void ConvertCharacterToVertices (
 char a_char 
)
/***************************************************************************/
/*
   Points are in [0.,1.] x [0.,1.]
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int ipoly;
  int    number,ipoint;
#define HERSHEY__MAX_POLY   4
#define HERSHEY__MAX_POINT  160 /* 40 * 4 */
  int    max_point[HERSHEY__MAX_POLY];
  double xp[HERSHEY__MAX_POINT],yp[HERSHEY__MAX_POINT];
/*.........................................................................*/
#ifdef HAS_CO
  CCharacterGetASCII_FontPoints (a_char,&number,max_point,xp,yp,NULL);
/*CCharacterGetGreekFontPoints  (a_char,&number,max_point,xp,yp,NULL);*/
#else
  number = 0;
#endif
  ipoint = 0;
  for (ipoly=0;ipoly<number;ipoly++)
    { int    pointn;
      pointn = max_point[ipoly];
      if(pointn>0) 
	{
	   int count;
	  glBegin  (GL_LINE_STRIP);
	  for(count=0;count<pointn;count++)
            {
	      glVertex3d (xp[ipoint],yp[ipoint],0.);
	      ipoint     ++;
	    }
	  glEnd    ();
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
  if(Class.display==NULL) 
    {
      CWarn("Can't open display.\n");
      exit(EXIT_SUCCESS);
    }

  Class.vinfo    = glXChooseVisual(Class.display,DefaultScreen(Class.display),attributeList);
  if(Class.vinfo==NULL)   
    {
      CWarn("Can't choose a visual.\n");
      exit(EXIT_SUCCESS);
    }

  Class.ctx      = glXCreateContext(Class.display,Class.vinfo,NULL,GL_FALSE);
  if(Class.ctx==NULL) 
    {
      CWarn("Can't create a GLX context.\n");
      exit(EXIT_SUCCESS);
    }

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
  if(Class.colormap==0L) 
    {
      CWarn("Can't create X colormap.\n");
      exit(EXIT_SUCCESS);
    }
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
  This             = XCreateWindow (Class.display, 
                                    XDefaultRootWindow(Class.display),
                                    a_x,a_y,a_width,a_height,
				    0,
                                    Class.vinfo->depth,
				    InputOutput,
                                    Class.vinfo->visual,
				    CWBorderPixel|CWColormap|CWEventMask,&swa);
  if(This==0L) 
    {
      CWarn ("Can't create an X window.\n");
      exit  (EXIT_SUCCESS);
    }

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
static void UniconifyWindow (
 Display* This
,Window a_window
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  XWMHints          wh;
/*.........................................................................*/
  wh.initial_state = NormalState; 
  wh.flags          = StateHint;
  XSetWMHints       (This,a_window,&wh);
  XMapWindow        (This,a_window);
}
/***************************************************************************/
static void GetWindowSize (
 Display* a_display
,Window a_window
,int* a_width
,int* a_height
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  XWindowAttributes     watbs;
/*.........................................................................*/
  if(a_width)     *a_width  = 0;
  if(a_height)    *a_height = 0;
  if(!a_display)  return;
  if(!a_window)   return;
  XGetWindowAttributes  (a_display,a_window,&watbs);
  if(a_width)     *a_width  = watbs.width;
  if(a_height)    *a_height = watbs.height;
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
