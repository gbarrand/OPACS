/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#include <CPrinter.h>
#include <CMemory.h>

#include <CPS.h>

#include <gl.h>
#include <ZBuffer.h>
#include <glDriver.h>
#include <glDriverZ.h>
#include <glDriverPS.h>

#include <glPS.h>

typedef unsigned long Pixel;
typedef unsigned int  Uint;

#ifdef __cplusplus
extern "C"{
#endif
static void  PutZBufferInStream ();
static int   GetRGB             (unsigned int,unsigned int,double*,double*,double*);
#ifdef __cplusplus
}
#endif

#define MINIMUM(a,b) ((a)<(b)?a:b)
#define MAXIMUM(a,b) ((a)>(b)?a:b)

static struct {
  int     pageWidth,pageHeight; /*In pixels.*/
  int     colorn;
  double* reds;
  double* greens;
  double* blues;
} Class = {0,0,0,NULL,NULL,NULL};
/***************************************************************************/
void glPS_OpenFileForWriting (
 char* a_fname
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CPS_OpenFileForWriting      (a_fname);
}
/***************************************************************************/
void glPS_CloseStream (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CPS_CloseStream  ();
  glClearClass     ();
  CPS_ClearClass   ();
}
/***************************************************************************/
void glPS_BeginPage (
 int a_width
,int a_height
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_width <=0) a_width  = 100;
  if(a_height<=0) a_height = 100;
  CPS_PutBeginPageInStream ();
  CPS_PutPageScaleInStream (a_width,a_height);
  glPS_BeginView           (a_width,a_height);
}
/***************************************************************************/
void glPS_EndPage (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  glPS_EndView             ();
  CPS_PutEndPageInStream   ();
}
/***************************************************************************/
void glPS_BeginView (
 int a_width
,int a_height
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_width <=0) a_width  = 100;
  if(a_height<=0) a_height = 100;
  glSetDriver     (glDriverPS_ClearClass,
		   glDriverPS_ConvertRGB_ToPixel,
		   glDriverPS_ConvertPixelToRGB,
		   glDriverPS_SetViewport,
		   glDriverPS_ClearColorBuffer,
		   glDriverPS_DrawPrimitive,
		   glDriverPS_DrawBuffer);
  Class.pageWidth     = a_width;
  Class.pageHeight    = a_height;
  glDriverPS_GetRGBs  (NULL,NULL,NULL,NULL);
  glDriverZ_SetWindow (Class.pageWidth,Class.pageHeight);
}
/***************************************************************************/
void glPS_EndView (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(glIsEnabled(GL_DEPTH_TEST)==GL_FALSE) 
    {
      glDriverPS_RestoreGraphicState ();
    }
  else
    {
      PutZBufferInStream();
    }
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static void PutZBufferInStream (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  ZPixel*   data;
  int       zbw,zbh;
/*.........................................................................*/
  data      = ZBufferGetColorBuffer(&zbw,&zbh);
  if(data==NULL) return;

/*
  printf ("debug:PutZBufferInStream:window:%d %d zbuf:%d %d\n",Class.pageWidth,Class.pageHeight,zbw,zbh);
*/

  glDriverPS_GetRGBs   (&Class.colorn,&Class.reds,&Class.greens,&Class.blues);

  CPS_PutImageInStream ((Uint)Class.pageWidth,(Uint)Class.pageHeight,GetRGB);

  CMemoryFreeBlock     (Class.reds);
  CMemoryFreeBlock     (Class.greens);
  CMemoryFreeBlock     (Class.blues);
  Class.colorn         = 0;
  Class.reds           = NULL;
  Class.greens         = NULL;
  Class.blues          = NULL;
}
/***************************************************************************/
static int GetRGB (
 unsigned int a_col
,unsigned int a_row
,double* a_red
,double* a_green
,double* a_blue
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  ZPixel index;
/*.........................................................................*/
  index  = ZBufferGetClippedPixel ((int)a_col,(int)a_row);
  if(index>=(unsigned int)Class.colorn) 
    {
      *a_red     = 0.;
      *a_green   = 0.;
      *a_blue    = 0.;
      return     0;
    }
  else
    {
      *a_red     = Class.reds  [index];
      *a_green   = Class.greens[index];
      *a_blue    = Class.blues [index];
      return     1;
    }
}

