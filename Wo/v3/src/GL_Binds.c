/*--------------------------------------------
  This file has been generated by cigen       
  using the following command :               

  $CIBIN/cigen.exe -p -f GL/gl.h -n CiBindgl -o ../gen/gl_B.c

--------------------------------------------*/

#include <GL/gl.h>

#ifndef __CI_SWITCH_DEFS__
#define __CI_SWITCH_DEFS__

#ifdef __cplusplus
#define SWITCH_ELLIPSIS ...
#else
#define SWITCH_ELLIPSIS 

#endif

#ifndef __CiDefs__

typedef void            (*CiRoutine)(SWITCH_ELLIPSIS);
typedef char            (*CiCRoutine)(SWITCH_ELLIPSIS);
typedef short           (*CiSRoutine)(SWITCH_ELLIPSIS);
typedef int             (*CiIRoutine)(SWITCH_ELLIPSIS);
typedef long            (*CiLRoutine)(SWITCH_ELLIPSIS);
typedef unsigned char   (*CiUCRoutine)(SWITCH_ELLIPSIS);
typedef unsigned short  (*CiUSRoutine)(SWITCH_ELLIPSIS);
typedef unsigned int    (*CiUIRoutine)(SWITCH_ELLIPSIS);
typedef unsigned long   (*CiULRoutine)(SWITCH_ELLIPSIS);
typedef float           (*CiFRoutine)(SWITCH_ELLIPSIS);
typedef double          (*CiDRoutine)(SWITCH_ELLIPSIS);
typedef void*           (*CiPRoutine)(SWITCH_ELLIPSIS);

#endif

#ifndef __CiType__

typedef union {
  char c;
  short s;
  int i;
  long l;
  char* p;
  float f;
  double d;
} CiPrimValueRec, *CiPrimValue;

#endif

#endif

#ifdef __cplusplus
extern "C" {
#endif

CiRoutine CiBindgl (char* library);

static void CiBindglInitFunction (CiRoutine compileFunction,
                            CiRoutine pathFunction,
                            CiRoutine bindFunction,
                            CiRoutine bindVariable,
                            CiRoutine switchNew);

static char* CiBindglSetupFunction (CiRoutine* switchFunction);

static void CiBindglSwitchFunction (CiRoutine f,
                              int code,
                              int position,
                              CiPrimValue args,
                              CiPrimValue retValue);
void CiBindglCleanup ();

static CiRoutine CiBindglFileCompile       = 0;
static CiRoutine CiBindglFunctionBind      = 0;
static CiRoutine CiBindglVariableBind      = 0;
static CiRoutine CiBindglFunctionSwitchNew = 0;

#ifdef __cplusplus
}
#endif

/*- Procedure ------------------------------------------*/
CiRoutine CiBindgl (char* library)
/*------------------------------------------------------*/
{
  static int first = 1;

  if (first)
    {
      first = 0;
    }
  else if (CiBindglFunctionSwitchNew)
    {
      CiBindglFunctionSwitchNew ((CiRoutine) CiBindglSetupFunction);
    }

  library = 0;

  return ((CiRoutine) CiBindglInitFunction);
}

/*- Procedure ------------------------------------------*/
static void CiBindglInitFunction (CiRoutine compileFunction,
                            CiRoutine pathFunction,
                            CiRoutine bindFunction,
                            CiRoutine bindVariable,
                            CiRoutine switchNew)
/*------------------------------------------------------*/
{
  CiBindglFileCompile       = compileFunction;
  pathFunction = 0;
  CiBindglFunctionBind      = bindFunction;
  CiBindglVariableBind      = bindVariable;
  CiBindglFunctionSwitchNew = switchNew;
  CiBindglVariableBind (0, 0);
  CiBindglFunctionBind (0, 0);
}

/*- Procedure ------------------------------------------*/
static char* CiBindglSetupFunction (CiRoutine* switchFunction)
/*------------------------------------------------------*/
{
  static char cases[] = {
    1, 8, 8, 8, 0,
    1, 7, 7, 7, 7, 0,
    2, 4, 0,
    1, 4, 4, 4, 4, 0,
    1, 0,
    1, 4, 3, 0,
    1, 4, 4, 0,
    1, 4, 6, 0,
    1, 7, 7, 7, 0,
    1, 8, 8, 8, 8, 8, 8, 0,
    1, 8, 8, 8, 8, 0,
    1, 4, 4, 6, 0,
    1, 1, 0,
    1, 2, 0,
    4, 4, 0,
    1, 4, 0,
    1, 6, 0,
    1, 7, 0,
    1, 8, 0,
    0
  };
  static int first = 1;
  
  if (first)
    {
      first = 0;
  
      CiBindglFileCompile ("GL/gl.h");
      CiBindglFunctionBind ("glFinish", (CiRoutine) glFinish);
      CiBindglFunctionBind ("glRenderMode", (CiRoutine) glRenderMode);
      CiBindglFunctionBind ("glRotated", (CiRoutine) glRotated);
      CiBindglFunctionBind ("glDepthFunc", (CiRoutine) glDepthFunc);
      CiBindglFunctionBind ("glLineStipple", (CiRoutine) glLineStipple);
      CiBindglFunctionBind ("glPushName", (CiRoutine) glPushName);
      CiBindglFunctionBind ("glEnable", (CiRoutine) glEnable);
      CiBindglFunctionBind ("glLoadIdentity", (CiRoutine) glLoadIdentity);
      CiBindglFunctionBind ("glClearColor", (CiRoutine) glClearColor);
      CiBindglFunctionBind ("glGetIntegerv", (CiRoutine) glGetIntegerv);
      CiBindglFunctionBind ("glBegin", (CiRoutine) glBegin);
      CiBindglFunctionBind ("glInitNames", (CiRoutine) glInitNames);
      CiBindglFunctionBind ("glEdgeFlag", (CiRoutine) glEdgeFlag);
      CiBindglFunctionBind ("glFlush", (CiRoutine) glFlush);
      CiBindglFunctionBind ("glSelectBuffer", (CiRoutine) glSelectBuffer);
      CiBindglFunctionBind ("glPolygonMode", (CiRoutine) glPolygonMode);
      CiBindglFunctionBind ("glViewport", (CiRoutine) glViewport);
      CiBindglFunctionBind ("glDisable", (CiRoutine) glDisable);
      CiBindglFunctionBind ("glScissor", (CiRoutine) glScissor);
      CiBindglFunctionBind ("glPushMatrix", (CiRoutine) glPushMatrix);
      CiBindglFunctionBind ("glClear", (CiRoutine) glClear);
      CiBindglFunctionBind ("glShadeModel", (CiRoutine) glShadeModel);
      CiBindglFunctionBind ("glTranslated", (CiRoutine) glTranslated);
      CiBindglFunctionBind ("glTranslatef", (CiRoutine) glTranslatef);
      CiBindglFunctionBind ("glFrustum", (CiRoutine) glFrustum);
      CiBindglFunctionBind ("glEndList", (CiRoutine) glEndList);
      CiBindglFunctionBind ("glLineWidth", (CiRoutine) glLineWidth);
      CiBindglFunctionBind ("glLoadMatrixd", (CiRoutine) glLoadMatrixd);
      CiBindglFunctionBind ("glMultMatrixd", (CiRoutine) glMultMatrixd);
      CiBindglFunctionBind ("glScaled", (CiRoutine) glScaled);
      CiBindglFunctionBind ("glLoadName", (CiRoutine) glLoadName);
      CiBindglFunctionBind ("glPointSize", (CiRoutine) glPointSize);
      CiBindglFunctionBind ("glIsEnabled", (CiRoutine) glIsEnabled);
      CiBindglFunctionBind ("glPopMatrix", (CiRoutine) glPopMatrix);
      CiBindglFunctionBind ("glOrtho", (CiRoutine) glOrtho);
      CiBindglFunctionBind ("glGenLists", (CiRoutine) glGenLists);
      CiBindglFunctionBind ("glColor3d", (CiRoutine) glColor3d);
      CiBindglFunctionBind ("glColor3f", (CiRoutine) glColor3f);
      CiBindglFunctionBind ("glCullFace", (CiRoutine) glCullFace);
      CiBindglFunctionBind ("glMatrixMode", (CiRoutine) glMatrixMode);
      CiBindglFunctionBind ("glNormal3d", (CiRoutine) glNormal3d);
      CiBindglFunctionBind ("glDeleteLists", (CiRoutine) glDeleteLists);
      CiBindglFunctionBind ("glDrawBuffer", (CiRoutine) glDrawBuffer);
      CiBindglFunctionBind ("glNewList", (CiRoutine) glNewList);
      CiBindglFunctionBind ("glVertex3d", (CiRoutine) glVertex3d);
      CiBindglFunctionBind ("glVertex3f", (CiRoutine) glVertex3f);
      CiBindglFunctionBind ("glPopName", (CiRoutine) glPopName);
      CiBindglFunctionBind ("glLightfv", (CiRoutine) glLightfv);
      CiBindglFunctionBind ("glClearDepth", (CiRoutine) glClearDepth);
      CiBindglFunctionBind ("glGetDoublev", (CiRoutine) glGetDoublev);
      CiBindglFunctionBind ("glCallList", (CiRoutine) glCallList);
      CiBindglFunctionBind ("glEnd", (CiRoutine) glEnd);
    }
  *switchFunction = (CiRoutine) CiBindglSwitchFunction;
  return ((char*) cases);
}

/*- Procedure ------------------------------------------*/
static void CiBindglSwitchFunction (CiRoutine f,
                              int code,
                              int position,
                              CiPrimValue args,
                              CiPrimValue retValue)
/*------------------------------------------------------*/
{
  CiPrimValueRec prim;
  
  prim.l = 0;
  
  switch (code)
  {
    case 1 :
      switch (position)
      {
        case 1 :
          {
            CiRoutine g = f;

            g (args[0].d, args[1].d, args[2].d);
          }
          break;
      }
      break;
    case 6 :
      switch (position)
      {
        case 1 :
          {
            CiRoutine g = f;

            g (args[0].f, args[1].f, args[2].f, args[3].f);
          }
          break;
      }
      break;
    case 8 :
      switch (position)
      {
        case 1 :
          {
            CiCRoutine g = (CiCRoutine) f;

            prim.c = g (args[0].i);
          }
          break;
      }
      break;
    case 10 :
      switch (position)
      {
        case 1 :
          {
            CiRoutine g = f;

            g (args[0].i, args[1].i, args[2].i, args[3].i);
          }
          break;
      }
      break;
    case 14 :
      switch (position)
      {
        case 1 :
          {
            CiRoutine g = f;

            g ();
          }
          break;
      }
      break;
    case 15 :
      switch (position)
      {
        case 1 :
          {
            CiRoutine g = f;

            g (args[0].i, args[1].s);
          }
          break;
      }
      break;
    case 16 :
      switch (position)
      {
        case 1 :
          {
            CiRoutine g = f;

            g (args[0].i, args[1].i);
          }
          break;
      }
      break;
    case 18 :
      switch (position)
      {
        case 1 :
          {
            CiRoutine g = f;

            g (args[0].i, args[1].p);
          }
          break;
        case 2 :
          {
            CiRoutine g = f;

            g (args[0].f, args[1].f, args[2].f);
          }
          break;
      }
      break;
    case 20 :
      switch (position)
      {
        case 1 :
          {
            CiRoutine g = f;

            g (args[0].d, args[1].d, args[2].d, args[3].d, 
               args[4].d, args[5].d);
          }
          break;
      }
      break;
    case 26 :
      switch (position)
      {
        case 1 :
          {
            CiRoutine g = f;

            g (args[0].d, args[1].d, args[2].d, args[3].d);
          }
          break;
      }
      break;
    case 31 :
      switch (position)
      {
        case 1 :
          {
            CiRoutine g = f;

            g (args[0].i, args[1].i, args[2].p);
          }
          break;
      }
      break;
    case 32 :
      switch (position)
      {
        case 1 :
          {
            CiRoutine g = f;

            g ();
          }
          break;
      }
      break;
    case 33 :
      switch (position)
      {
        case 1 :
          {
            CiRoutine g = f;

            g (args[0].c);
          }
          break;
      }
      break;
    case 34 :
      switch (position)
      {
        case 1 :
          {
            CiIRoutine g = (CiIRoutine) f;

            prim.i = g (args[0].i);
          }
          break;
      }
      break;
    case 35 :
      switch (position)
      {
        case 1 :
          {
            CiRoutine g = f;

            g (args[0].i);
          }
          break;
      }
      break;
    case 37 :
      switch (position)
      {
        case 1 :
          {
            CiRoutine g = f;

            g (args[0].p);
          }
          break;
      }
      break;
    case 38 :
      switch (position)
      {
        case 1 :
          {
            CiRoutine g = f;

            g (args[0].f);
          }
          break;
      }
      break;
    case 39 :
      switch (position)
      {
        case 1 :
          {
            CiRoutine g = f;

            g (args[0].d);
          }
          break;
      }
      break;
  }
  
  args = 0;
  position = 0;
  
  if (retValue) *retValue = prim;
}

/*--------------------------------------------
  This file has been generated by cigen       
  using the following command :               

  $CIBIN/cigen.exe -p -f GL/glu.h -n CiBindglu -o ../gen/glu_B.c

--------------------------------------------*/

#include <GL/glu.h>

#ifndef __CI_SWITCH_DEFS__
#define __CI_SWITCH_DEFS__

#ifdef __cplusplus
#define SWITCH_ELLIPSIS ...
#else
#define SWITCH_ELLIPSIS 

#endif

#ifndef __CiDefs__

typedef void            (*CiRoutine)(SWITCH_ELLIPSIS);
typedef char            (*CiCRoutine)(SWITCH_ELLIPSIS);
typedef short           (*CiSRoutine)(SWITCH_ELLIPSIS);
typedef int             (*CiIRoutine)(SWITCH_ELLIPSIS);
typedef long            (*CiLRoutine)(SWITCH_ELLIPSIS);
typedef unsigned char   (*CiUCRoutine)(SWITCH_ELLIPSIS);
typedef unsigned short  (*CiUSRoutine)(SWITCH_ELLIPSIS);
typedef unsigned int    (*CiUIRoutine)(SWITCH_ELLIPSIS);
typedef unsigned long   (*CiULRoutine)(SWITCH_ELLIPSIS);
typedef float           (*CiFRoutine)(SWITCH_ELLIPSIS);
typedef double          (*CiDRoutine)(SWITCH_ELLIPSIS);
typedef void*           (*CiPRoutine)(SWITCH_ELLIPSIS);

#endif

#ifndef __CiType__

typedef union {
  char c;
  short s;
  int i;
  long l;
  char* p;
  float f;
  double d;
} CiPrimValueRec, *CiPrimValue;

#endif

#endif

#ifdef __cplusplus
extern "C" {
#endif

CiRoutine CiBindglu (char* library);

static void CiBindgluInitFunction (CiRoutine compileFunction,
                            CiRoutine pathFunction,
                            CiRoutine bindFunction,
                            CiRoutine bindVariable,
                            CiRoutine switchNew);

static char* CiBindgluSetupFunction (CiRoutine* switchFunction);

static void CiBindgluSwitchFunction (CiRoutine f,
                              int code,
                              int position,
                              CiPrimValue args,
                              CiPrimValue retValue);
void CiBindgluCleanup ();

static CiRoutine CiBindgluFileCompile       = 0;
static CiRoutine CiBindgluFunctionBind      = 0;
static CiRoutine CiBindgluVariableBind      = 0;
static CiRoutine CiBindgluFunctionSwitchNew = 0;

#ifdef __cplusplus
}
#endif

/*- Procedure ------------------------------------------*/
CiRoutine CiBindglu (char* library)
/*------------------------------------------------------*/
{
  static int first = 1;

  if (first)
    {
      first = 0;
    }
  else if (CiBindgluFunctionSwitchNew)
    {
      CiBindgluFunctionSwitchNew ((CiRoutine) CiBindgluSetupFunction);
    }

  library = 0;

  return ((CiRoutine) CiBindgluInitFunction);
}

/*- Procedure ------------------------------------------*/
static void CiBindgluInitFunction (CiRoutine compileFunction,
                            CiRoutine pathFunction,
                            CiRoutine bindFunction,
                            CiRoutine bindVariable,
                            CiRoutine switchNew)
/*------------------------------------------------------*/
{
  CiBindgluFileCompile       = compileFunction;
  pathFunction = 0;
  CiBindgluFunctionBind      = bindFunction;
  CiBindgluVariableBind      = bindVariable;
  CiBindgluFunctionSwitchNew = switchNew;
  CiBindgluVariableBind (0, 0);
  CiBindgluFunctionBind (0, 0);
}

/*- Procedure ------------------------------------------*/
static char* CiBindgluSetupFunction (CiRoutine* switchFunction)
/*------------------------------------------------------*/
{
  static char cases[] = {
    1, 6, 4, 6, 4, 6, 4, 4, 6, 4, 4, 4, 0,
    1, 6, 4, 7, 0,
    6, 1, 0,
    1, 8, 8, 8, 8, 8, 8, 8, 8, 8, 0,
    1, 8, 8, 8, 8, 0,
    1, 6, 0,
    0
  };
  static int first = 1;
  
  if (first)
    {
      first = 0;
  
      CiBindgluFileCompile ("GL/glu.h");
      CiBindgluFunctionBind ("gluLookAt", (CiRoutine) gluLookAt);
      CiBindgluFunctionBind ("gluBeginSurface", (CiRoutine) gluBeginSurface);
      CiBindgluFunctionBind ("gluNurbsProperty", (CiRoutine) gluNurbsProperty);
      CiBindgluFunctionBind ("gluNurbsSurface", (CiRoutine) gluNurbsSurface);
      CiBindgluFunctionBind ("gluEndSurface", (CiRoutine) gluEndSurface);
      CiBindgluFunctionBind ("gluNewNurbsRenderer", (CiRoutine) gluNewNurbsRenderer);
      CiBindgluFunctionBind ("gluPerspective", (CiRoutine) gluPerspective);
    }
  *switchFunction = (CiRoutine) CiBindgluSwitchFunction;
  return ((char*) cases);
}

/*- Procedure ------------------------------------------*/
static void CiBindgluSwitchFunction (CiRoutine f,
                              int code,
                              int position,
                              CiPrimValue args,
                              CiPrimValue retValue)
/*------------------------------------------------------*/
{
  CiPrimValueRec prim;
  
  prim.l = 0;
  
  switch (code)
  {
    case 5 :
      switch (position)
      {
        case 1 :
          {
            CiRoutine g = f;

            g (args[0].p, args[1].i, args[2].p, args[3].i, 
               args[4].p, args[5].i, args[6].i, args[7].p, 
               args[8].i, args[9].i, args[10].i);
          }
          break;
      }
      break;
    case 10 :
      switch (position)
      {
        case 1 :
          {
            CiRoutine g = f;

            g (args[0].p, args[1].i, args[2].f);
          }
          break;
      }
      break;
    case 17 :
      switch (position)
      {
        case 1 :
          {
            CiPRoutine g = (CiPRoutine) f;

            prim.p = (char*) g ();
          }
          break;
      }
      break;
    case 23 :
      switch (position)
      {
        case 1 :
          {
            CiRoutine g = f;

            g (args[0].d, args[1].d, args[2].d, args[3].d, 
               args[4].d, args[5].d, args[6].d, args[7].d, 
               args[8].d);
          }
          break;
      }
      break;
    case 26 :
      switch (position)
      {
        case 1 :
          {
            CiRoutine g = f;

            g (args[0].d, args[1].d, args[2].d, args[3].d);
          }
          break;
      }
      break;
    case 37 :
      switch (position)
      {
        case 1 :
          {
            CiRoutine g = f;

            g (args[0].p);
          }
          break;
      }
      break;
  }
  
  args = 0;
  position = 0;
  
  if (retValue) *retValue = prim;
}

/*--------------------------------------------
  This file has been generated by cigen       
  using the following command :               

  $CIBIN/cigen.exe -p -h X11/Xlib.h -h X11/Xutil.h -h GL/gl.h -f GL/glx.h -n CiBindglx -o ../gen/glx_B.c

--------------------------------------------*/

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <GL/gl.h>
#include <GL/glx.h>

#ifndef __CI_SWITCH_DEFS__
#define __CI_SWITCH_DEFS__

#ifdef __cplusplus
#define SWITCH_ELLIPSIS ...
#else
#define SWITCH_ELLIPSIS 

#endif

#ifndef __CiDefs__

typedef void            (*CiRoutine)(SWITCH_ELLIPSIS);
typedef char            (*CiCRoutine)(SWITCH_ELLIPSIS);
typedef short           (*CiSRoutine)(SWITCH_ELLIPSIS);
typedef int             (*CiIRoutine)(SWITCH_ELLIPSIS);
typedef long            (*CiLRoutine)(SWITCH_ELLIPSIS);
typedef unsigned char   (*CiUCRoutine)(SWITCH_ELLIPSIS);
typedef unsigned short  (*CiUSRoutine)(SWITCH_ELLIPSIS);
typedef unsigned int    (*CiUIRoutine)(SWITCH_ELLIPSIS);
typedef unsigned long   (*CiULRoutine)(SWITCH_ELLIPSIS);
typedef float           (*CiFRoutine)(SWITCH_ELLIPSIS);
typedef double          (*CiDRoutine)(SWITCH_ELLIPSIS);
typedef void*           (*CiPRoutine)(SWITCH_ELLIPSIS);

#endif

#ifndef __CiType__

typedef union {
  char c;
  short s;
  int i;
  long l;
  char* p;
  float f;
  double d;
} CiPrimValueRec, *CiPrimValue;

#endif

#endif

#ifdef __cplusplus
extern "C" {
#endif

CiRoutine CiBindglx (char* library);

static void CiBindglxInitFunction (CiRoutine compileFunction,
                            CiRoutine pathFunction,
                            CiRoutine bindFunction,
                            CiRoutine bindVariable,
                            CiRoutine switchNew);

static char* CiBindglxSetupFunction (CiRoutine* switchFunction);

static void CiBindglxSwitchFunction (CiRoutine f,
                              int code,
                              int position,
                              CiPrimValue args,
                              CiPrimValue retValue);
void CiBindglxCleanup ();

static CiRoutine CiBindglxFileCompile       = 0;
static CiRoutine CiBindglxFunctionBind      = 0;
static CiRoutine CiBindglxVariableBind      = 0;
static CiRoutine CiBindglxFunctionSwitchNew = 0;

#ifdef __cplusplus
}
#endif

/*- Procedure ------------------------------------------*/
CiRoutine CiBindglx (char* library)
/*------------------------------------------------------*/
{
  static int first = 1;

  if (first)
    {
      first = 0;
    }
  else if (CiBindglxFunctionSwitchNew)
    {
      CiBindglxFunctionSwitchNew ((CiRoutine) CiBindglxSetupFunction);
    }

  library = 0;

  return ((CiRoutine) CiBindglxInitFunction);
}

/*- Procedure ------------------------------------------*/
static void CiBindglxInitFunction (CiRoutine compileFunction,
                            CiRoutine pathFunction,
                            CiRoutine bindFunction,
                            CiRoutine bindVariable,
                            CiRoutine switchNew)
/*------------------------------------------------------*/
{
  CiBindglxFileCompile       = compileFunction;
  pathFunction = 0;
  CiBindglxFunctionBind      = bindFunction;
  CiBindglxVariableBind      = bindVariable;
  CiBindglxFunctionSwitchNew = switchNew;
  CiBindglxVariableBind (0, 0);
  CiBindglxFunctionBind (0, 0);
}

/*- Procedure ------------------------------------------*/
static char* CiBindglxSetupFunction (CiRoutine* switchFunction)
/*------------------------------------------------------*/
{
  static char cases[] = {
    1, 6, 5, 0,
    1, 6, 6, 0,
    6, 6, 4, 6, 0,
    4, 6, 5, 6, 0,
    1, 0,
    6, 6, 6, 6, 4, 0,
    4, 6, 6, 6, 0,
    4, 6, 6, 0,
    0
  };
  static int first = 1;
  
  if (first)
    {
      first = 0;
  
      CiBindglxFileCompile ("X11/Xlib.h");
      CiBindglxFileCompile ("X11/Xutil.h");
      CiBindglxFileCompile ("GL/gl.h");
      CiBindglxFileCompile ("GL/glx.h");
      CiBindglxFunctionBind ("glXSwapBuffers", (CiRoutine) glXSwapBuffers);
      CiBindglxFunctionBind ("glXQueryVersion", (CiRoutine) glXQueryVersion);
      CiBindglxFunctionBind ("glXCreateContext", (CiRoutine) glXCreateContext);
      CiBindglxFunctionBind ("glXWaitGL", (CiRoutine) glXWaitGL);
      CiBindglxFunctionBind ("glXQueryExtension", (CiRoutine) glXQueryExtension);
      CiBindglxFunctionBind ("glXDestroyContext", (CiRoutine) glXDestroyContext);
      CiBindglxFunctionBind ("glXChooseVisual", (CiRoutine) glXChooseVisual);
      CiBindglxFunctionBind ("glXIsDirect", (CiRoutine) glXIsDirect);
      CiBindglxFunctionBind ("glXMakeCurrent", (CiRoutine) glXMakeCurrent);
      CiBindglxFunctionBind ("glXWaitX", (CiRoutine) glXWaitX);
    }
  *switchFunction = (CiRoutine) CiBindglxSwitchFunction;
  return ((char*) cases);
}

/*- Procedure ------------------------------------------*/
static void CiBindglxSwitchFunction (CiRoutine f,
                              int code,
                              int position,
                              CiPrimValue args,
                              CiPrimValue retValue)
/*------------------------------------------------------*/
{
  CiPrimValueRec prim;
  
  prim.l = 0;
  
  switch (code)
  {
    case 3 :
      switch (position)
      {
        case 1 :
          {
            CiRoutine g = f;

            g (args[0].p, args[1].l);
          }
          break;
      }
      break;
    case 4 :
      switch (position)
      {
        case 1 :
          {
            CiRoutine g = f;

            g (args[0].p, args[1].p);
          }
          break;
      }
      break;
    case 10 :
      switch (position)
      {
        case 1 :
          {
            CiPRoutine g = (CiPRoutine) f;

            prim.p = (char*) g (args[0].p, args[1].i, args[2].p);
          }
          break;
      }
      break;
    case 13 :
      switch (position)
      {
        case 1 :
          {
            CiIRoutine g = (CiIRoutine) f;

            prim.i = g (args[0].p, args[1].l, args[2].p);
          }
          break;
      }
      break;
    case 14 :
      switch (position)
      {
        case 1 :
          {
            CiRoutine g = f;

            g ();
          }
          break;
      }
      break;
    case 21 :
      switch (position)
      {
        case 1 :
          {
            CiPRoutine g = (CiPRoutine) f;

            prim.p = (char*) g (args[0].p, args[1].p, args[2].p, args[3].i);
          }
          break;
      }
      break;
    case 26 :
      switch (position)
      {
        case 1 :
          {
            CiIRoutine g = (CiIRoutine) f;

            prim.i = g (args[0].p, args[1].p, args[2].p);
          }
          break;
      }
      break;
    case 31 :
      switch (position)
      {
        case 1 :
          {
            CiIRoutine g = (CiIRoutine) f;

            prim.i = g (args[0].p, args[1].p);
          }
          break;
      }
      break;
  }
  
  args = 0;
  position = 0;
  
  if (retValue) *retValue = prim;
}
