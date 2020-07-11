/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#ifndef M_PI
#define M_PI       3.1415926535897931160E0
#define M_PI_2     1.5707963267948965580E0  
#endif

#include <CMemory.h>
#include <CVector.h>
#include <OMatrix.h>

#include <gl.h>

#include <glu.h>

typedef void (*ErrorFunction)(GLenum);

struct GLUquadricObj {
	GLenum	DrawStyle;		/* GLU_FILL, LINE, SILHOUETTE, or POINT */
	GLenum Orientation;		/* GLU_INSIDE or GLU_OUTSIDE */
	GLboolean TextureFlag;		/* Generate texture coords? */
	GLenum Normals;		        /* GLU_NONE, GLU_FLAT, or GLU_SMOOTH */
	ErrorFunction ErrorFunc;	/* Error handler callback function */
};


#ifdef __cplusplus
extern "C"{
#endif
static void TransformPoint (GLdouble out[4],const GLdouble m[16],const GLdouble in[4]);
static void quadric_error  (GLUquadricObj*,GLenum,const char*);
#ifdef __cplusplus
}
#endif

#define GLU_NO_ERROR	GL_NO_ERROR
#define DEG_TO_RAD(A)   ((A)*(M_PI/180.0))
#define SIND(A)         sin( (A)*(M_PI/180.0) )
#define COSD(A)         cos( (A)*(M_PI/180.0) )
/***************************************************************************/
const GLubyte* gluErrorString ( 
 GLenum errorCode 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   static char *tess_error[] = {
      "missing gluEndPolygon",
      "missing gluBeginPolygon",
      "misoriented contour",
      "vertex/edge intersection",
      "misoriented or self-intersecting loops",
      "coincident vertices",
      "colinear vertices",
      "intersecting edges",
      "not coplanar contours"
   };
   static char *nurbs_error[] = {
      "spline order un-supported",
      "too few knots",
      "valid knot range is empty",
      "decreasing knot sequence knot",
      "knot multiplicity greater than order of spline",
      "endcurve() must follow bgncurve()",
      "bgncurve() must precede endcurve()",
      "missing or extra geometric data",
      "can't draw pwlcurves",
      "missing bgncurve()",
      "missing bgnsurface()",
      "endtrim() must precede endsurface()",
      "bgnsurface() must precede endsurface()",
      "curve of improper type passed as trim curve",
      "bgnsurface() must precede bgntrim()",
      "endtrim() must follow bgntrim()",
      "bgntrim() must precede endtrim()",
      "invalid or missing trim curve",
      "bgntrim() must precede pwlcurve()",
      "pwlcurve referenced twice",
      "pwlcurve and nurbscurve mixed",
      "improper usage of trim data type",
      "nurbscurve referenced twice",
      "nurbscurve and pwlcurve mixed",
      "nurbssurface referenced twice",
      "invalid property",
      "endsurface() must follow bgnsurface()",
      "misoriented trim curves",
      "intersecting trim curves",
      "UNUSED",
      "unconnected trim curves",
      "unknown knot error",
      "negative vertex count encountered",
      "negative byte-stride encounteed",
      "unknown type descriptor",
      "null control array or knot vector",
      "duplicate point on pwlcurve"
   };

   /* GL Errors */
   if (errorCode==GL_NO_ERROR) {
      return (GLubyte *) "no error";
   }
   else if (errorCode==GL_INVALID_VALUE) {
      return (GLubyte *) "invalid value";
   }
   else if (errorCode==GL_INVALID_ENUM) {
      return (GLubyte *) "invalid enum";
   }
   else if (errorCode==GL_INVALID_OPERATION) {
      return (GLubyte *) "invalid operation";
   }
   else if (errorCode==GL_STACK_OVERFLOW) {
      return (GLubyte *) "stack overflow";
   }
   else if (errorCode==GL_STACK_UNDERFLOW) {
      return (GLubyte *) "stack underflow";
   }
   else if (errorCode==GL_OUT_OF_MEMORY) {
      return (GLubyte *) "out of memory";
   }
   /* GLU Errors */
   else if (errorCode==GLU_NO_ERROR) {
      return (GLubyte *) "no error";
   }
   else if (errorCode==GLU_INVALID_ENUM) {
      return (GLubyte *) "invalid enum";
   }
   else if (errorCode==GLU_INVALID_VALUE) {
      return (GLubyte *) "invalid value";
   }
   else if (errorCode==GLU_OUT_OF_MEMORY) {
      return (GLubyte *) "out of memory";
   }
   else if (errorCode==GLU_INCOMPATIBLE_GL_VERSION) {
      return (GLubyte *) "incompatible GL version";
   }
   else if (errorCode>=GLU_TESS_ERROR1 && errorCode<=GLU_TESS_ERROR9) {
      return (GLubyte *) tess_error[errorCode-GLU_TESS_ERROR1];
   }
   else if (errorCode>=GLU_NURBS_ERROR1 && errorCode<=GLU_NURBS_ERROR37) {
      return (GLubyte *) nurbs_error[errorCode-GLU_NURBS_ERROR1];
   }
   else {
      return NULL;
   }
}
/***************************************************************************/
void gluPerspective (
 GLdouble a_fovy
,GLdouble a_aspect
,GLdouble a_near
,GLdouble a_far
)
/***************************************************************************/
/*
  a_aspect = width/height;
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  double    angle,left,right,top,bottom;
/*.........................................................................*/
  angle     = fabs((a_fovy * M_PI / 180.)/2.);

  top       = a_near * tan (angle);
  bottom    = -top;
  right     = top * a_aspect;
  left      = -right;

  glFrustum (left,right,bottom,top,a_near,a_far);
}
/***************************************************************************/
void gluLookAt (
 GLdouble a_eye_x
,GLdouble a_eye_y
,GLdouble a_eye_z
,GLdouble a_center_x
,GLdouble a_center_y
,GLdouble a_center_z
,GLdouble a_up_x
,GLdouble a_up_y
,GLdouble a_up_z
)
/***************************************************************************/
/* 
  Defaults are :
   eye    = (0.,0., 0.) 
   vrp    = (0.,0.,-1.) window place.
   center = (0.,0.,-2.) looked point.
   dop    = (0.,0.,-1.) direction of projection.
   up     = (0.,1., 0.);
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  double  dop_x,dop_y,dop_z,angle,x,y,z;
/*.........................................................................*/
  dop_x         = a_center_x - a_eye_x;
  dop_y         = a_center_y - a_eye_y;
  dop_z         = a_center_z - a_eye_z;
  if(CVectorGetNorme(dop_x,dop_y,dop_z)==0.) return;
  
  OMatrixGetDirection  (dop_x,dop_y,dop_z,  a_up_x,a_up_y,a_up_z,
		        0.,0.,-1.,          0.,1.,0.,
		        &x,&y,&z,&angle);
  angle         *= 180./M_PI;

  glRotated     (angle,x,y,z);

  glTranslated  (-a_eye_x,-a_eye_y,-a_eye_z);
}
/***************************************************************************/
int gluProject (
 GLdouble objx
,GLdouble objy
,GLdouble objz
,const GLdouble model[16]
,const GLdouble proj[16]
,const GLint viewport[4]
,GLdouble* winx
,GLdouble* winy
,GLdouble* winz
)
/***************************************************************************/
/* Projection du point (objx,objy,obz) sur l'ecran (winx,winy,winz).       */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  GLdouble in[4],out[4];  /* matrice de transformation */
/*.........................................................................*/
  /* initilise la matrice et le vecteur a transformer */
  in[0]=objx; in[1]=objy; in[2]=objz; in[3]=1.0;
  TransformPoint(out,model,in);
  TransformPoint(in,proj,out);

  /* d'ou le resultat normalise entre -1 et 1*/
  in[0]/=in[3];in[1]/=in[3];in[2]/=in[3];
  
  /* en coordonnees ecran */
  *winx = viewport[0]+(1+in[0])*viewport[2]/2;
  *winy = viewport[1]+(1+in[1])*viewport[3]/2;
  /* entre 0 et 1 suivant z */
  *winz = (1+in[2])/2;

  return GL_TRUE;
}
/***************************************************************************/
/******* From Mesa-1.2.5/src-glu/quadric.c *********************************/
/***************************************************************************/
/***************************************************************************/
GLUquadricObj* gluNewQuadric (
 void 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   GLUquadricObj *q;

   q = (GLUquadricObj *) CMemoryAllocateBlock ( sizeof(struct GLUquadricObj) );
   if (q) {
      q->DrawStyle = GLU_FILL;
      q->Orientation = GLU_OUTSIDE;
      q->TextureFlag = GL_FALSE;
      q->Normals = GLU_SMOOTH;
      q->ErrorFunc = NULL;
   }
   return q;
}
/***************************************************************************/
void gluDeleteQuadric ( 
 GLUquadricObj* state 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   if (state) {
      CMemoryFreeBlock( (void *) state );
   }
}
/***************************************************************************/
void gluQuadricDrawStyle (
 GLUquadricObj* quadObject
,GLenum drawStyle 
)
/***************************************************************************/
/*
 * Set the drawing style to be GLU_FILL, GLU_LINE, GLU_SILHOUETTE, or GLU_POINT.
 */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   if (quadObject && (drawStyle==GLU_FILL || drawStyle==GLU_LINE
		   || drawStyle==GLU_SILHOUETTE || drawStyle==GLU_POINT)) {
      quadObject->DrawStyle = drawStyle;
   }
   else {
      quadric_error( quadObject, GLU_INVALID_ENUM, "qluQuadricDrawStyle" );
   }
}
/***************************************************************************/
void gluQuadricOrientation (
 GLUquadricObj* quadObject
,GLenum orientation 
)
/***************************************************************************/
/*
 * Set the orientation to GLU_INSIDE or GLU_OUTSIDE.
 */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   if (quadObject && (orientation==GLU_INSIDE || orientation==GLU_OUTSIDE)) {
      quadObject->Orientation = orientation;
   }
   else {
      quadric_error( quadObject, GLU_INVALID_ENUM, "qluQuadricOrientation" );
   }
}
/***************************************************************************/
void gluQuadricCallback ( 
 GLUquadricObj* qobj
,GLenum which
,void (*fn)() 
)
/***************************************************************************/
/*
 * Set the error handler callback function.
 */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   if (qobj && which==GLU_ERROR) {
      qobj->ErrorFunc = (ErrorFunction)fn;
   }
}
/***************************************************************************/
void gluQuadricNormals (
 GLUquadricObj* quadObject
,GLenum normals 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   if (quadObject
         && (normals==GLU_NONE || normals==GLU_FLAT || normals==GLU_SMOOTH)) {
      quadObject->Normals = normals;
   }
}
/***************************************************************************/
void gluQuadricTexture (
 GLUquadricObj* quadObject
,GLboolean textureCoords 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   if (quadObject) {
      quadObject->TextureFlag = textureCoords;
   }
}
/***************************************************************************/
void gluCylinder ( 
 GLUquadricObj *qobj
,GLdouble baseRadius
,GLdouble topRadius
,GLdouble height
,GLint slices
,GLint stacks 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   GLdouble da, r, dr, dz;
   GLfloat x, y, z, nz, nsign;
   GLboolean normal_state;
   GLint i, j;

   normal_state = glIsEnabled( GL_NORMALIZE );
   glEnable( GL_NORMALIZE );

   if (qobj->Orientation==GLU_INSIDE) {
      nsign = -1.0;
   }
   else {
      nsign = 1.0;
   }

   da = 2.0*M_PI / slices;
   dr = (topRadius-baseRadius) / stacks;
   dz = height / stacks;
   nz = (baseRadius-topRadius) / height;  /* Z component of normal vectors */

   if (qobj->DrawStyle==GLU_POINT) {
      glBegin( GL_POINTS );
      for (i=0;i<slices;i++) {
	 x = cos(i*da);
	 y = sin(i*da);
	 glNormal3f( x*nsign, y*nsign, nz*nsign );

	 z = 0.0;
	 r = baseRadius;
	 for (j=0;j<=stacks;j++) {
	    glVertex3f( x*r, y*r, z );
	    z += dz;
	    r += dr;
	 }
      }
      glEnd();
   }
   else if (qobj->DrawStyle==GLU_LINE || qobj->DrawStyle==GLU_SILHOUETTE) {
      /* Draw rings */
      if (qobj->DrawStyle==GLU_LINE) {
	 z = 0.0;
	 r = baseRadius;
	 for (j=0;j<=stacks;j++) {
	    glBegin( GL_LINE_LOOP );
	    for (i=0;i<slices;i++) {
	       x = cos(i*da);
	       y = sin(i*da);
	       glNormal3f( x*nsign, y*nsign, nz*nsign );
	       glVertex3f( x*r, y*r, z );
	    }
	    glEnd();
	    z += dz;
	    r += dr;
	 }
      }
      else {
	 /* draw one ring at each end */
	 if (baseRadius!=0.0) {
	    glBegin( GL_LINE_LOOP );
	    for (i=0;i<slices;i++) {
	       x = cos(i*da);
	       y = sin(i*da);
	       glNormal3f( x*nsign, y*nsign, nz*nsign );
	       glVertex3f( x*baseRadius, y*baseRadius, 0.0 );
	    }
	    glEnd();
	    glBegin( GL_LINE_LOOP );
	    for (i=0;i<slices;i++) {
	       x = cos(i*da);
	       y = sin(i*da);
	       glNormal3f( x*nsign, y*nsign, nz*nsign );
	       glVertex3f( x*topRadius, y*topRadius, height );
	    }
	    glEnd();
	 }
      }
      /* draw length lines */
      glBegin( GL_LINES );
      for (i=0;i<slices;i++) {
	 x = cos(i*da);
	 y = sin(i*da);
	 glNormal3f( x*nsign, y*nsign, nz*nsign );
	 glVertex3f( x*baseRadius, y*baseRadius, 0.0 );
	 glVertex3f( x*topRadius, y*topRadius, height );
      }
      glEnd();
   }
   else if (qobj->DrawStyle==GLU_FILL) {
      for (i=0;i<slices;i++) {
	 GLfloat x1 = cos(i*da);
	 GLfloat y1 = sin(i*da);
	 GLfloat x2 = cos((i+1)*da);
	 GLfloat y2 = sin((i+1)*da);
	 z = 0.0;
	 r = baseRadius;
	 glBegin( GL_QUAD_STRIP );
	 for (j=0;j<=stacks;j++) {
	    if (nsign==1.0) {
	       glNormal3f( x1*nsign, y1*nsign, nz*nsign );
	       glVertex3f( x1*r, y1*r, z );
	       glNormal3f( x2*nsign, y2*nsign, nz*nsign );
	       glVertex3f( x2*r, y2*r, z );
	    }
	    else {
	       glNormal3f( x2*nsign, y2*nsign, nz*nsign );
	       glVertex3f( x2*r, y2*r, z );
	       glNormal3f( x1*nsign, y1*nsign, nz*nsign );
	       glVertex3f( x1*r, y1*r, z );
	    }
	    z += dz;
	    r += dr;
	 }
	 glEnd();
      }
   }

   if (!normal_state) {
      glDisable( GL_NORMALIZE );
   }
}
/***************************************************************************/
void gluSphere ( 
 GLUquadricObj* qobj
,GLdouble radius
,GLint slices
,GLint stacks 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   GLfloat rho, drho, theta, dtheta;
   GLfloat x, y, z;
   GLint i, j;
   GLboolean normals;
   GLfloat nsign;

   if (qobj->Normals==GLU_NONE) {
      normals = GL_FALSE;
   }
   else {
      normals = GL_TRUE;
   }
   if (qobj->Orientation==GLU_INSIDE) {
      nsign = -1.0;
   }
   else {
      nsign = 1.0;
   }

   drho = M_PI / (GLfloat) stacks;
   dtheta = 2.0 * M_PI / (GLfloat) slices;

   if (qobj->DrawStyle==GLU_FILL) {
      /* draw +Z end as a triangle fan */
      glBegin( GL_TRIANGLE_FAN );
      glNormal3f( 0.0, 0.0, 1.0 );
      glVertex3f( 0.0, 0.0, nsign * radius );
      for (j=0;j<=slices;j++) {
	 theta = (j==slices) ? 0.0 : j * dtheta;
	 x = cos(theta) * sin(drho);
	 y = sin(theta) * sin(drho);
	 z = nsign * cos(drho);
	 if (normals)  glNormal3f( x*nsign, y*nsign, z*nsign );
	 glVertex3f( x*radius, y*radius, z*radius );
      }
      glEnd();

      /* draw intermediate stacks as quad strips */
      for (i=1;i<stacks-1;i++) {
	 rho = i * drho;
	 glBegin( GL_QUAD_STRIP );
	 for (j=0;j<=slices;j++) {
	    theta = (j==slices) ? 0.0 : j * dtheta;
	    x = cos(theta) * sin(rho);
	    y = sin(theta) * sin(rho);
	    z = nsign * cos(rho);
	    if (normals)  glNormal3f( x*nsign, y*nsign, z*nsign );
	    glVertex3f( x*radius, y*radius, z*radius );
	    x = cos(theta) * sin(rho+drho);
	    y = sin(theta) * sin(rho+drho);
	    z = nsign * cos(rho+drho);
	    if (normals)  glNormal3f( x*nsign, y*nsign, z*nsign );
	    glVertex3f( x*radius, y*radius, z*radius );
	 }
	 glEnd();
      }

      /* draw -Z end as a triangle fan */
      glBegin( GL_TRIANGLE_FAN );
      glNormal3f( 0.0, 0.0, -1.0 );
      glVertex3f( 0.0, 0.0, -radius*nsign );
      rho = M_PI - drho;
      for (j=slices;j>=0;j--) {
	 theta = (j==slices) ? 0.0 : j * dtheta;
	 x = cos(theta) * sin(rho);
	 y = sin(theta) * sin(rho);
	 z = nsign * cos(rho);
	 if (normals)  glNormal3f( x*nsign, y*nsign, z*nsign );
	 glVertex3f( x*radius, y*radius, z*radius );
      }
      glEnd();
   }
   else if (qobj->DrawStyle==GLU_LINE || qobj->DrawStyle==GLU_SILHOUETTE) {
      /* draw stack lines */
      for (i=1;i<stacks-1;i++) {
	 rho = i * drho;
	 glBegin( GL_LINE_LOOP );
	 for (j=0;j<slices;j++) {
	    theta = j * dtheta;
	    x = cos(theta) * sin(rho);
	    y = sin(theta) * sin(rho);
	    z = cos(rho);
	    if (normals)  glNormal3f( x*nsign, y*nsign, z*nsign );
	    glVertex3f( x*radius, y*radius, z*radius );
	 }
	 glEnd();
      }
      /* draw slice lines */
      for (j=0;j<slices;j++) {
	 theta = j * dtheta;
	 glBegin( GL_LINE_STRIP );
	 for (i=0;i<=stacks;i++) {
	    rho = i * drho;
	    x = cos(theta) * sin(rho);
	    y = sin(theta) * sin(rho);
	    z = cos(rho);
	    if (normals)  glNormal3f( x*nsign, y*nsign, z*nsign );
	    glVertex3f( x*radius, y*radius, z*radius );
	 }
	 glEnd();
      }
   }
   else if (qobj->DrawStyle==GLU_POINT) {
      /* top and bottom-most points */
      glBegin( GL_POINTS );
      if (normals)  glNormal3f( 0.0, 0.0, nsign );
      glVertex3d( 0.0, 0.0, radius );
      if (normals)  glNormal3f( 0.0, 0.0, -nsign );
      glVertex3d( 0.0, 0.0, -radius );

      /* loop over stacks */
      for (i=1;i<stacks-1;i++) {
	 rho = i * drho;
	 for (j=0;j<slices;j++) {
	    theta = j * dtheta;
	    x = cos(theta) * sin(rho);
	    y = sin(theta) * sin(rho);
	    z = cos(rho);
	    if (normals)  glNormal3f( x*nsign, y*nsign, z*nsign );
	    glVertex3f( x*radius, y*radius, z*radius );
	 }
      }
      glEnd();
   }

}
/***************************************************************************/
void gluDisk (
 GLUquadricObj* qobj
,GLdouble innerRadius
,GLdouble outerRadius
,GLint slices
,GLint loops 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   GLdouble a, da;
   GLfloat r, dr;
   GLfloat x, y;
   GLfloat r1, r2;
   GLint s, l;

   /* Normal vectors */
   if (qobj->Normals!=GLU_NONE) {
      if (qobj->Orientation==GLU_OUTSIDE) {
	 glNormal3f( 0.0, 0.0, +1.0 );
      }
      else {
	 glNormal3f( 0.0, 0.0, -1.0 );
      }
   }

   da = 2.0*M_PI / slices;
   dr = (outerRadius-innerRadius) / (GLfloat) loops;

   switch (qobj->DrawStyle) {
      case GLU_FILL:
         r1 = innerRadius;
         for (l=0;l<loops;l++) {
	    r2 = r1 + dr;
	    if (qobj->Orientation==GLU_OUTSIDE) {
	       glBegin( GL_QUAD_STRIP );
	       for (s=0;s<=slices;s++) {
		  if (s==slices) a = 0.0;
		  else  a = s * da;
		  glVertex2f( r2*sin(a), r2*cos(a) );
		  glVertex2f( r1*sin(a), r1*cos(a) );
	       }
	       glEnd();
	    }
	    else {
	       glBegin( GL_QUAD_STRIP );
	       for (s=slices;s>=0;s--) {
		  if (s==slices) a = 0.0;
		  else  a = s * da;
		  glVertex2f( r2*sin(a), r2*cos(a) );
		  glVertex2f( r1*sin(a), r1*cos(a) );
	       }
	       glEnd();
	    }
	    r1 = r2;
	 }
         break;
      case GLU_LINE:
	 /* draw rings */
	 for (r=innerRadius; r<=outerRadius; r+=dr) {
	    glBegin( GL_LINE_LOOP );
	    for (a=0.0; a<2.0*M_PI; a+=da) {
	       glVertex2f( r*sin(a), r*cos(a) );
	    }
	    glEnd();
	 }
	 /* draw spokes */
	 for (a=0.0; a<2.0*M_PI; a+=da) {
	    x = sin(a);
	    y = cos(a);
	    glBegin( GL_LINE_STRIP );
	    for (r=innerRadius; r<=outerRadius; r+=dr) {
	       glVertex2f( r*x, r*y );
	    }
	    glEnd();
	 }
	 break;
      case GLU_POINT:
	 glBegin( GL_POINTS );
	 for (a=0.0; a<2.0*M_PI; a+=da) {
	    x = sin(a);
	    y = cos(a);
	    for (r=innerRadius; r<=outerRadius; r+=dr) {
	       glVertex2f( r*x, r*y );
	    }
	 }
	 glEnd();
	 break;
      case GLU_SILHOUETTE:
	 if (innerRadius!=0.0) {
	    glBegin( GL_LINE_LOOP );
	    for (a=0.0; a<2.0*M_PI; a+=da) {
	       x = innerRadius * sin(a);
	       y = innerRadius * cos(a);
	       glVertex2f( x, y );
	    }
	    glEnd();
	 }
	 glBegin( GL_LINE_LOOP );
	 for (a=0; a<2.0*M_PI; a+=da) {
	    x = outerRadius * sin(a);
	    y = outerRadius * cos(a);
	    glVertex2f( x, y );
	 }
	 glEnd();
	 break;
   }
}
/***************************************************************************/
void gluPartialDisk ( 
 GLUquadricObj* qobj
,GLdouble innerRadius
,GLdouble outerRadius
,GLint slices
,GLint loops
,GLdouble startAngle
,GLdouble sweepAngle 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   if (qobj->Normals!=GLU_NONE) {
      if (qobj->Orientation==GLU_OUTSIDE) {
	 glNormal3f( 0.0, 0.0, +1.0 );
      }
      else {
	 glNormal3f( 0.0, 0.0, -1.0 );
      }
   }

   if (qobj->DrawStyle==GLU_POINT) {
      GLint loop, slice;
      GLdouble radius, delta_radius;
      GLdouble angle, delta_angle;
      delta_radius = (outerRadius - innerRadius) / (loops-1);
      delta_angle = DEG_TO_RAD((sweepAngle) / (slices-1));
      glBegin( GL_POINTS );
      radius = innerRadius;
      for (loop=0; loop<loops; loop++) {
	 angle = DEG_TO_RAD(startAngle);
	 for (slice=0; slice<slices; slice++) {
	    glVertex2d( radius * sin(angle), radius * cos(angle) );
	    angle += delta_angle;
	 }
	 radius += delta_radius;
      }
      glEnd();
   }
   else if (qobj->DrawStyle==GLU_LINE) {
      GLint loop, slice;
      GLdouble radius, delta_radius;
      GLdouble angle, delta_angle;
      delta_radius = (outerRadius - innerRadius) / (loops-1);
      delta_angle = DEG_TO_RAD((sweepAngle) / (slices-1));
      /* draw rings */
      radius = innerRadius;
      for (loop=0; loop<loops; loop++) {
	 angle = DEG_TO_RAD(startAngle);
	 glBegin( GL_LINE_STRIP );
	 for (slice=0; slice<slices; slice++) {
	    glVertex2d( radius * sin(angle), radius * cos(angle) );
	    angle += delta_angle;
	 }
	 glEnd();
	 radius += delta_radius;
      }
      /* draw spokes */
      angle = DEG_TO_RAD(startAngle);
      for (slice=0; slice<slices; slice++) {
	 radius = innerRadius;
	 glBegin( GL_LINE_STRIP );
	 for (loop=0; loop<loops; loop++) {
	    glVertex2d( radius * sin(angle), radius * cos(angle) );
	    radius += delta_radius;
	 }
	 glEnd();
	 angle += delta_angle;
      }
   }
   else if (qobj->DrawStyle==GLU_SILHOUETTE) {
      GLint slice;
      GLdouble angle, delta_angle;
      delta_angle = DEG_TO_RAD((sweepAngle) / (slices-1));
      /* draw outer ring */
      glBegin( GL_LINE_STRIP );
      angle = DEG_TO_RAD(startAngle);
      for (slice=0; slice<slices; slice++) {
	 glVertex2d( outerRadius * sin(angle), outerRadius * cos(angle) );
	 angle += delta_angle;
      }
      glEnd();
      /* draw inner ring */
      if (innerRadius>0.0) {
	 glBegin( GL_LINE_STRIP );
	 angle = DEG_TO_RAD(startAngle);
	 for (slice=0; slice<slices; slice++) {
	    glVertex2d( innerRadius * sin(angle), innerRadius * cos(angle) );
	    angle += delta_angle;
	 }
	 glEnd();
      }
      /* draw spokes */
      if (sweepAngle<360.0) {
	 GLdouble stopAngle = startAngle + sweepAngle;
	 glBegin( GL_LINES );
	 glVertex2d( innerRadius*SIND(startAngle), innerRadius*COSD(startAngle) );
	 glVertex2d( outerRadius*SIND(startAngle), outerRadius*COSD(startAngle) );
	 glVertex2d( innerRadius*SIND(stopAngle), innerRadius*COSD(stopAngle) );
	 glVertex2d( outerRadius*SIND(stopAngle), outerRadius*COSD(stopAngle) );
	 glEnd();
      }
   }
   else if (qobj->DrawStyle==GLU_FILL) {
      GLint loop, slice;
      GLdouble radius, delta_radius;
      GLdouble angle, delta_angle;
      delta_radius = (outerRadius - innerRadius) / (loops-1);
      delta_angle = DEG_TO_RAD((sweepAngle) / (slices-1));
      radius = innerRadius;
      for (loop=0; loop<loops-1; loop++) {
	 glBegin( GL_QUAD_STRIP );
	 angle = DEG_TO_RAD(startAngle);
	 for (slice=0; slice<slices; slice++) {
	    if (qobj->Orientation==GLU_OUTSIDE) {
	       glVertex2d( (radius+delta_radius)*sin(angle),
			   (radius+delta_radius)*cos(angle) );
	       glVertex2d( radius * sin(angle), radius * cos(angle) );
	    }
	    else {
	       glVertex2d( radius * sin(angle), radius * cos(angle) );
	       glVertex2d( (radius+delta_radius)*sin(angle),
			   (radius+delta_radius)*cos(angle) );
	    }
	    angle += delta_angle;
	 }
	 glEnd();
	 radius += delta_radius;
      }
   }
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
GLUnurbsObj* gluNewNurbsRenderer (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return NULL;
}
/***************************************************************************/
void gluNurbsProperty (
 GLUnurbsObj* a_nurbs
,GLenum a_property
,GLfloat a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  a_nurbs    = NULL;
  a_property = 0;
  a_value    = 0.;
}
/***************************************************************************/
void gluNurbsSurface  (
 GLUnurbsObj* a_nurbs
,GLint a_uknot_count
,GLfloat* a_uknot
,GLint a_vknot_count
,GLfloat* a_vknot
,GLint a_u_stride
,GLint a_v_stride
,GLfloat* a_ctlarray
,GLint a_uorder
,GLint a_vorder
,GLenum a_type
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
 a_nurbs       = NULL;
 a_uknot_count = 0;
 a_uknot       = NULL;
 a_vknot_count = 0;
 a_vknot       = NULL;
 a_u_stride    = 0;
 a_v_stride    = 0;
 a_ctlarray    = NULL;
 a_uorder      = 0;
 a_vorder      = 0;
 a_type        = 0;
}
/***************************************************************************/
void gluBeginSurface  (
 GLUnurbsObj* a_nurbs
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  a_nurbs = NULL;
}
/***************************************************************************/
void gluEndSurface    (
 GLUnurbsObj* a_nurbs
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  a_nurbs = NULL;
}
/***************************************************************************/
void gluDeleteNurbsRenderer (
 GLUnurbsObj* a_nurbs
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  a_nurbs = NULL;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static void TransformPoint (
 GLdouble out[4]
,const GLdouble m[16]
,const GLdouble in[4] 
)
/***************************************************************************/
/*
 * Transform a point (column vector) by a 4x4 matrix.  I.e.  out = m * in
 * Input:  m - the 4x4 matrix
 *         in - the 4x1 vector
 * Output:  out - the resulting 4x1 vector.
 */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
#define M(row,col)  m[col*4+row]
   out[0] = M(0,0) * in[0] + M(0,1) * in[1] + M(0,2) * in[2] + M(0,3) * in[3];
   out[1] = M(1,0) * in[0] + M(1,1) * in[1] + M(1,2) * in[2] + M(1,3) * in[3];
   out[2] = M(2,0) * in[0] + M(2,1) * in[1] + M(2,2) * in[2] + M(2,3) * in[3];
   out[3] = M(3,0) * in[0] + M(3,1) * in[1] + M(3,2) * in[2] + M(3,3) * in[3];
#undef M
}
/***************************************************************************/
static void quadric_error ( 
 GLUquadricObj* qobj
,GLenum error
,const char *msg 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   /* Call the error call back function if any */
   if (qobj->ErrorFunc) {
      (*qobj->ErrorFunc)( error );
   }
   /* Print a message to stdout if MESA_DEBUG variable is defined */
   if (getenv("MESA_DEBUG")) {
      fprintf(stderr,"GLUError: %s: %s\n", gluErrorString(error), msg );
   }
}
