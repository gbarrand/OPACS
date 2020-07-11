/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef glListP_h
#define glListP_h

#include <glList.h>

typedef struct _glOperationRecord  *glOperation;

typedef struct _glOperationRecord
{
  int                     code;
  glListOperationFunction routine;
  GLenum                  mode;
  GLint                   int1,int2;
  GLdouble                double1,double2,double3;
  GLfloat                 float1,float2,float3,float4;
  void*                   pointer1;
} glOperationRecord;

typedef struct _glListRecord
{
  GLuint       name;
  glOperation* operations;
} glListRecord;

#endif /*glListP_h*/
