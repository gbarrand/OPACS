/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef glList_h
#define glList_h
 
typedef struct _glListRecord  *glList;

typedef void(*glListOperationFunction)();

#define glOperation_glBegin     1
#define glOperation_glEnd       2
#define glOperation_glVertex3d  3
#define glOperation_glColor3d   4
#define glOperation_glLineWidth 5
#define glOperation_glBitmap    6

#include <gl.h>
#ifdef __cplusplus
extern "C"{
#endif
void                 glListClearClass                    ();
glList*              glListGetIdentifiers                ();
glList               glListGetIdentifier                 (GLuint);
glList               glListMake                          ();
glList               glListCreate                        (GLuint);
int                  glListIsValid                       (glList);
void                 glListDelete                        (glList);
void                 glListSetName                       (glList,GLuint);
GLuint               glListGetName                       (glList);
void                 glListAddOperation                  (glList,int,glListOperationFunction,...);
void                 glListExecute                       (glList);
#ifdef __cplusplus
}
#endif

#endif  /*glList_h*/
