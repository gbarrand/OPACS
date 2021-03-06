/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef gl_h
#define gl_h

typedef unsigned int   GLenum;
typedef double         GLdouble;
typedef float          GLfloat;
typedef int            GLint;
typedef short          GLshort;
typedef int            GLsizei;
typedef unsigned int   GLbitfield;
typedef float          GLclampf;
typedef double         GLclampd;
typedef unsigned int   GLuint;
typedef unsigned char  GLubyte;
typedef unsigned char  GLboolean;
typedef unsigned short GLushort;
typedef void           GLvoid;

/* ErrorCode */
#define GL_INVALID_ENUM                   0x0500
#define GL_INVALID_VALUE                  0x0501
#define GL_INVALID_OPERATION              0x0502
#define GL_STACK_OVERFLOW                 0x0503
#define GL_STACK_UNDERFLOW                0x0504
#define GL_OUT_OF_MEMORY		  0x0505

/* BeginMode */
#define GL_POINTS                         0x0000
#define GL_LINES                          0x0001
#define GL_LINE_LOOP                      0x0002
#define GL_LINE_STRIP                     0x0003
#define GL_POLYGON                        0x0009
#define GL_QUADS			  0x0007
#define GL_QUAD_STRIP			  0x0008
#define GL_TRIANGLE_FAN			  0x0006

/* AttribMask */
#define GL_COLOR_BUFFER_BIT               0x00004000
#define GL_DEPTH_BUFFER_BIT               0x00000100

/* Boolean */
#define GL_TRUE                           1
#define GL_FALSE                          0


/* MatrixMode */
#define GL_MODELVIEW                      0x1700
#define GL_PROJECTION                     0x1701

/* RenderingMode */
#define GL_RENDER                         0x1C00
#define GL_FEEDBACK                       0x1C01
#define GL_SELECT                         0x1C02

/* GetTarget */
#define GL_VIEWPORT                       0x0BA2
#define GL_RENDER_MODE                    0x0C40
#define GL_SCISSOR_TEST                   0x0C11
#define GL_MODELVIEW_MATRIX               0x0BA6
#define GL_PROJECTION_MATRIX              0x0BA7
#define GL_CURRENT_COLOR		  0x0B00

/* CullFaceMode */
#define GL_FRONT                          0x0404
#define GL_BACK                           0x0405
#define GL_CULL_FACE			  0x0B44

/* PolygonMode */
#define GL_POINT                          0x1B00
#define GL_LINE                           0x1B01
#define GL_FILL                           0x1B02

/* Lighting */
#define GL_LIGHTING			  0x0B50
#define GL_LIGHT0			  0x4000
#define GL_FLAT				  0x1D00
#define GL_FRONT_AND_BACK                 0x0408
#define GL_NORMALIZE			  0x0BA1
#define GL_AMBIENT			  0x1200
#define GL_POSITION			  0x1203
#define GL_SMOOTH			  0x1D01
#define GL_DIFFUSE			  0x1201
#define GL_AMBIENT_AND_DIFFUSE		  0x1602

/* Enable */
#define GL_DEPTH_TEST                     0x0B71

/* Display Lists */
#define GL_COMPILE			  0x1300
#define GL_COMPILE_AND_EXECUTE		  0x1301
#define GL_LIST_BASE			  0x0B32
#define GL_LIST_INDEX			  0x0B33
#define GL_LIST_MODE			  0x0B30

/* Depth buffer */
#define GL_LEQUAL			  0x0203

/* Evaluators */
#define GL_AUTO_NORMAL			  0x0D80
#define	GL_MAP2_VERTEX_4		  0x0DB8
 
/* Pixel Mode / Transfer */
#define GL_UNPACK_ALIGNMENT		  0x0CF5
#define GL_UNPACK_LSB_FIRST		  0x0CF1
#define GL_UNPACK_ROW_LENGTH		  0x0CF2
#define GL_UNPACK_SKIP_PIXELS		  0x0CF4
#define GL_UNPACK_SKIP_ROWS		  0x0CF3
#define GL_UNPACK_SWAP_BYTES		  0x0CF0

/* PixelFormat */
#define GL_RGB                            0x1907

/* ListNameType */
#define GL_FLOAT                          0x1406

/* GL_NO_ERROR must be zero */
#define GL_NO_ERROR GL_FALSE

#ifdef __cplusplus
extern "C"{
#endif
void      glBegin         (GLenum);
void      glEnd           ();
void      glVertex3d      (GLdouble,GLdouble,GLdouble);
void      glVertex3f      (GLfloat,GLfloat,GLfloat);
void      glVertex2i      (GLint,GLint);
void      glVertex2d      (GLdouble,GLdouble);
void      glVertex2fv     (const GLfloat*);
void      glVertex2s      (GLshort,GLshort);
void      glVertex2sv     (const GLshort*);
void      glVertex2f      (GLfloat,GLfloat);
void      glLineWidth     (GLfloat);
void      glLineStipple   (GLint,GLushort);
void      glPointSize     (GLfloat);
void      glColor3d       (GLdouble,GLdouble,GLdouble);
void      glColor3f       (GLfloat,GLfloat,GLfloat);
void      glColor3fv      (const GLfloat*);
void      glColor3ub      (GLubyte,GLubyte,GLubyte);
void      glViewport      (GLint,GLint,GLsizei,GLsizei);
void      glFlush         ();
void      glFinish        ();
void      glScissor       (GLint,GLint,GLsizei,GLsizei);
void      glShadeModel    (GLenum);
void      glClear         (GLbitfield);
void      glClearColor    (GLclampf,GLclampf,GLclampf,GLclampf);
void      glClearDepth    (GLclampd);
void      glMatrixMode    (GLenum);
void      glLoadIdentity  (void);
void      glLoadMatrixd   (GLdouble*);
void      glMultMatrixd   (const GLdouble*);
void      glMultMatrixf   (const GLfloat*);
void      glPopMatrix     (void);
void      glPushMatrix    (void);
void      glRotatef       (GLfloat,GLfloat,GLfloat,GLfloat);
void      glRotated       (GLdouble,GLdouble,GLdouble,GLdouble);
void      glScalef        (GLfloat,GLfloat,GLfloat);
void      glScaled        (GLdouble,GLdouble,GLdouble);
void      glTranslated    (GLdouble,GLdouble,GLdouble);
void      glTranslatef    (GLfloat,GLfloat,GLfloat);
void      glFrustum       (GLdouble,GLdouble,GLdouble,GLdouble,GLdouble,GLdouble);
void      glOrtho         (GLdouble,GLdouble,GLdouble,GLdouble,GLdouble,GLdouble);
void      glSelectBuffer  (GLsizei,GLuint*);
GLint     glRenderMode    (GLenum);
void      glInitNames     (void);
void      glLoadName      (GLuint);
void      glPushName      (GLuint);
void      glPopName       ();
void      glGetIntegerv   (GLenum,GLint*);
void      glGetDoublev    (GLenum,GLdouble*);
void      glPolygonMode   (GLenum,GLenum);
void      glEnable        (GLenum);
void      glDisable       (GLenum);
GLboolean glIsEnabled     (GLenum);
GLuint    glGenLists      (GLsizei);
void      glNewList       (GLuint,GLenum);
void      glEndList       ();
void      glCallList      (GLuint);
void      glDeleteLists   (GLuint,GLsizei);
void      glRectd         (GLdouble,GLdouble,GLdouble,GLdouble);
void      glRecti         (GLint,GLint,GLint,GLint);
void      glRects         (GLshort,GLshort,GLshort,GLshort);

/*Not implemented.*/
void      glClearIndex    (GLfloat);
void      glIndexf        (GLfloat);
void      glIndexi        (GLint);
void      glCullFace      (GLenum);
void      glNormal3d      (GLdouble,GLdouble,GLdouble);
void      glNormal3f      (GLfloat,GLfloat,GLfloat);
void      glEdgeFlag      (GLboolean);
void      glDepthFunc     (GLenum);
void      glDrawBuffer    (GLenum);
void      glLightfv       (GLenum,GLenum,const GLfloat*);
void      glMaterialfv    (GLenum,GLenum,const GLfloat*);
void      glRasterPos3d   (GLdouble,GLdouble,GLdouble);
void      glBitmap        (GLsizei,GLsizei,GLfloat,GLfloat,GLfloat,GLfloat,const GLubyte*);
void      glPixelStorei   (GLenum,GLint);
void      glReadPixels    (GLint,GLint,GLsizei,GLsizei,GLenum,GLenum,GLvoid*);
#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindgl(0); /**/
#endif

#endif  /*gl_h*/
