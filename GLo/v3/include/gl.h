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

#include <gldef.h>

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
