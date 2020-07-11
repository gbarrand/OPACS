/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include <CPrinter.h>
#include <CMemory.h>

#ifdef HAS_GL

#include <gl.h>
#include <glu.h>

#include <glPS.h>

#ifdef __cplusplus
extern "C"{
#endif
static void DrawCube ();
#ifdef __cplusplus
}
#endif

/***************************************************************************/
int main (
 int    a_argn
,char*  a_args[]
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  glPS_OpenFileForWriting        ("out.ps");

/*Page 1*/
  glPS_BeginPage     (400,200);

  glEnable        (GL_DEPTH_TEST);
  glViewport      (0,0,400,200);
  glClearColor    (0.8,0.8,0.8,0.);
  glClear         (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glViewport      (100,100,200,100);
  glClearColor    (0.,0.,0.,0.);

  glMatrixMode    (GL_PROJECTION); 
  glLoadIdentity  ();
  glFrustum       (-2.,2.,-1.,1.,1.,2.);
/*gluPerspective  (90.,2.,1./tan(M_PI_2/2.),2.);*/

  glMatrixMode    (GL_MODELVIEW);
  glLoadIdentity  ();
  gluLookAt       (0.,0.,3.,  /*eye*/
                   0.,0.,0.,  /*center*/
                   0.,1.,0.); /*up*/
  glClear         (GL_COLOR_BUFFER_BIT);
  glRotated       (45.,1.,1.,0.);
  DrawCube        ();
  glFinish        ();

  glPS_EndPage     ();

/*Page 2*/
  glPS_BeginPage     (200,400);

  glEnable        (GL_DEPTH_TEST);
  glViewport      (0,0,200,400);
  glClearColor    (0.4,0.4,0.4,0.);
  glClear         (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glViewport      (50,200,100,200);
  glClearColor    (0.,0.,0.,0.);

  glMatrixMode    (GL_PROJECTION); 
  glLoadIdentity  ();
  glFrustum       (-1.,1.,-2.,2.,1.,2.);
/*gluPerspective  (90.,0.5,1./tan(M_PI_2/2.),2.);*/

  glMatrixMode    (GL_MODELVIEW);
  glLoadIdentity  ();
  gluLookAt       (3.,0.,0.,  /*eye*/
                   0.,0.,0.,  /*center*/
                   0.,1.,0.); /*up*/
  glClear         (GL_COLOR_BUFFER_BIT);
  glRotated       (45.,1.,1.,0.);
  DrawCube        ();
  glFinish        ();

  glPS_EndPage     ();

  glPS_CloseStream       ();

  CInfoF          ("%s\n",CMemoryGetStatus());

  a_argn = 0;
  a_args = NULL;
  return EXIT_SUCCESS;
}
/***************************************************************************/
static void DrawCube (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  glLineWidth    (4.0);

  glColor3d      (1.0,0.0,0.);   
  glBegin        (GL_POLYGON);
  glVertex3d     ( 0.5,-0.5,0.5);
  glVertex3d     ( 0.5, 0.5,0.5);
  glVertex3d     (-0.5, 0.5,0.5);
  glVertex3d     (-0.5,-0.5,0.5);
  glEnd          ();

  glColor3d      (0.0,1.0,0.);   
  glBegin        (GL_POLYGON);
  glVertex3d     ( 0.5,-0.5,-0.5);
  glVertex3d     ( 0.5, 0.5,-0.5);
  glVertex3d     (-0.5, 0.5,-0.5);
  glVertex3d     (-0.5,-0.5,-0.5);
  glEnd          ();

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

}

#else
int main() {printf ("C macro HAS_GL not defined.\n");return 0;}
#endif
