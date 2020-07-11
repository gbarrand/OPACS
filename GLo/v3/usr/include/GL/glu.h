/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef glu_h
#define glu_h

#include <GL/gl.h>

/* Normal vectors */
#define GLU_SMOOTH	  100000
#define GLU_FLAT	  100001
#define GLU_NONE	  100002

/* Quadric draw styles */
#define	GLU_POINT	  100010
#define GLU_LINE	  100011
#define GLU_FILL	  100012
#define GLU_SILHOUETTE	  100013

/* Quadric orientation */
#define GLU_OUTSIDE	  100020
#define GLU_INSIDE	  100021

/* NURBS */
#define GLU_AUTO_LOAD_MATRIX	 100200
#define	GLU_CULLING		 100201
#define	GLU_PARAMETRIC_TOLERANCE 100202
#define	GLU_SAMPLING_TOLERANCE	 100203
#define	GLU_DISPLAY_MODE	 100204
#define	GLU_SAMPLING_METHOD	 100205
#define	GLU_U_STEP		 100206
#define	GLU_V_STEP		 100207
#define	GLU_OUTLINE_POLYGON	 100240
#define GLU_OUTLINE_PATCH	 100241

/* Tesselator */
#define GLU_ERROR                100103

/* Tesselation errors */
#define GLU_TESS_ERROR1	100151  /* missing gluEndPolygon */
#define GLU_TESS_ERROR2 100152  /* missing gluBeginPolygon */
#define GLU_TESS_ERROR3 100153  /* misoriented contour */
#define GLU_TESS_ERROR4 100154  /* vertex/edge intersection */
#define GLU_TESS_ERROR5 100155  /* misoriented or self-intersecting loops */
#define GLU_TESS_ERROR6 100156  /* coincident vertices */
#define GLU_TESS_ERROR7 100157  /* all vertices collinear */
#define GLU_TESS_ERROR8 100158  /* intersecting edges */
#define GLU_TESS_ERROR9 100159  /* not coplanar contours */

#define GLU_NURBS_ERROR1  100251   /* spline order un-supported */
#define GLU_NURBS_ERROR2  100252   /* too few knots */
#define GLU_NURBS_ERROR3  100253   /* valid knot range is empty */
#define GLU_NURBS_ERROR4  100254   /* decreasing knot sequence */
#define GLU_NURBS_ERROR5  100255   /* knot multiplicity > spline order */
#define GLU_NURBS_ERROR6  100256   /* endcurve() must follow bgncurve() */
#define GLU_NURBS_ERROR7  100257   /* bgncurve() must precede endcurve() */
#define GLU_NURBS_ERROR8  100258   /* ctrlarray or knot vector is NULL */
#define GLU_NURBS_ERROR9  100259   /* can't draw pwlcurves */
#define GLU_NURBS_ERROR10 100260   /* missing gluNurbsCurve() */
#define GLU_NURBS_ERROR11 100261   /* missing gluNurbsSurface() */
#define GLU_NURBS_ERROR12 100262   /* endtrim() must precede endsurface() */
#define GLU_NURBS_ERROR13 100263   /* bgnsurface() must precede endsurface() */
#define GLU_NURBS_ERROR14 100264   /* curve of improper type passed as trim curve */
#define GLU_NURBS_ERROR15 100265   /* bgnsurface() must precede bgntrim() */
#define GLU_NURBS_ERROR16 100266   /* endtrim() must follow bgntrim() */
#define GLU_NURBS_ERROR17 100267   /* bgntrim() must precede endtrim()*/
#define GLU_NURBS_ERROR18 100268   /* invalid or missing trim curve*/
#define GLU_NURBS_ERROR19 100269   /* bgntrim() must precede pwlcurve() */
#define GLU_NURBS_ERROR20 100270   /* pwlcurve referenced twice*/
#define GLU_NURBS_ERROR21 100271   /* pwlcurve and nurbscurve mixed */
#define GLU_NURBS_ERROR22 100272   /* improper usage of trim data type */
#define GLU_NURBS_ERROR23 100273   /* nurbscurve referenced twice */
#define GLU_NURBS_ERROR24 100274   /* nurbscurve and pwlcurve mixed */
#define GLU_NURBS_ERROR25 100275   /* nurbssurface referenced twice */
#define GLU_NURBS_ERROR26 100276   /* invalid property */
#define GLU_NURBS_ERROR27 100277   /* endsurface() must follow bgnsurface() */
#define GLU_NURBS_ERROR28 100278   /* intersecting or misoriented trim curves */
#define GLU_NURBS_ERROR29 100279   /* intersecting trim curves */
#define GLU_NURBS_ERROR30 100280   /* UNUSED */
#define GLU_NURBS_ERROR31 100281   /* unconnected trim curves */
#define GLU_NURBS_ERROR32 100282   /* unknown knot error */
#define GLU_NURBS_ERROR33 100283   /* negative vertex count encountered */
#define GLU_NURBS_ERROR34 100284   /* negative byte-stride */
#define GLU_NURBS_ERROR35 100285   /* unknown type descriptor */
#define GLU_NURBS_ERROR36 100286   /* null control point reference */
#define GLU_NURBS_ERROR37 100287   /* duplicate point on pwlcurve */

/* Errors */
#define GLU_INVALID_ENUM	        100900
#define GLU_INVALID_VALUE		100901
#define GLU_OUT_OF_MEMORY		100902
#define GLU_INCOMPATIBLE_GL_VERSION	100903

typedef struct GLUquadricObj GLUquadricObj;
typedef struct GLUnurbs      GLUnurbsObj;

#ifdef __cplusplus
extern "C"{
#endif

const GLubyte* gluErrorString        (GLenum);

GLUquadricObj* gluNewQuadric         (void);
void           gluDeleteQuadric      (GLUquadricObj*);
void           gluQuadricDrawStyle   (GLUquadricObj*,GLenum);
void           gluQuadricOrientation (GLUquadricObj*,GLenum);
void           gluQuadricNormals     (GLUquadricObj*,GLenum);
void           gluQuadricTexture     (GLUquadricObj*,GLboolean);
void           gluQuadricCallback    (GLUquadricObj*,GLenum,void(*)());
void           gluCylinder           (GLUquadricObj*,GLdouble,GLdouble,GLdouble,GLint,GLint);
void           gluSphere             (GLUquadricObj*,GLdouble,GLint,GLint);
void           gluDisk               (GLUquadricObj*,GLdouble,GLdouble,GLint,GLint);
void           gluPartialDisk        (GLUquadricObj*,GLdouble,GLdouble,GLint,GLint,GLdouble,GLdouble);

void gluPerspective   (GLdouble,GLdouble,GLdouble,GLdouble);
void gluLookAt        (GLdouble,GLdouble,GLdouble,GLdouble,GLdouble,GLdouble,GLdouble,GLdouble,GLdouble);
int  gluProject       (GLdouble,GLdouble,GLdouble,const GLdouble model[16],const GLdouble proj[16],const GLint viewport[4],GLdouble*,GLdouble*,GLdouble*);

/*Not implemented.*/
GLUnurbsObj* gluNewNurbsRenderer    (void);
void         gluNurbsProperty       (GLUnurbsObj*,GLenum,GLfloat);
void         gluNurbsSurface        (GLUnurbsObj*,GLint,GLfloat*,GLint,GLfloat*,GLint,GLint,GLfloat*,GLint,GLint,GLenum);
void         gluBeginSurface        (GLUnurbsObj*);
void         gluEndSurface          (GLUnurbsObj*);
void         gluDeleteNurbsRenderer (GLUnurbsObj*);
#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindglu(0); /**/
#endif

#endif  /*glu_h*/
