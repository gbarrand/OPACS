/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#ifndef M_PI
#define M_PI       3.1415926535897931160E0
#define M_PI_2     1.5707963267948965580E0  
#endif

#include <CError.h>
#include <CPrinter.h>
#include <CFile.h>
#include <CString.h>
#include <CVector.h>
#include <CMemory.h>

#include <OCamPOV.h>

#ifdef __cplusplus
extern "C"{
#endif
static void  WritePrimitive (OCamera,OPrimitive,int,int,OContext,OMatrix,double);
static void  ReportError    (char*,int);
#ifdef __cplusplus
}
#endif

typedef unsigned char Uchar;
#define METAFILE_DEFAULT        "out.pov"
#define CHECK_DIV(a_var,a_pro)  {if((a_var)==0.) {ReportError(a_pro,CErrorValue);(a_var)=1.;}}

static struct 
{
  FILE* file;
  char* fname;
} Class = {NULL,NULL};
/***************************************************************************/
void OCameraOpenPOV_FileForWriting (
 char* a_string
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.file!=NULL) OCameraClosePOV_Stream ();
  if( (a_string==NULL) || (*a_string=='\0') ) 
    {
      Class.file         = CFileOpenForWriting (METAFILE_DEFAULT);
      CStringDelete      (Class.fname);
      Class.fname        = CStringDuplicate(METAFILE_DEFAULT);
    }
  else
    {
      Class.file         = CFileOpenForWriting (a_string);
      CStringDelete      (Class.fname);
      Class.fname        = CStringDuplicate(a_string);
    }
  if(Class.file==NULL) return;

/* Header */
  fprintf (Class.file,"//\n");

 {char* senv;
  senv  = getenv   ("GOVERSION");
  if(senv!=NULL) 
    {
      fprintf (Class.file,"// Creator: Go %s, LAL Orsay.\n",senv);
    }
  else
    {
      fprintf (Class.file,"// Creator: Go, LAL Orsay.\n");
    }}
 fprintf (Class.file,"// Visualize with : xfpovray\n");
 fprintf (Class.file,"#version 3.0\n");
}
/***************************************************************************/
void OCameraClosePOV_Stream (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.file==NULL) return;
  fclose         (Class.file);
  Class.file     = NULL;
  CStringDelete  (Class.fname);
  Class.fname    = NULL;
}
/***************************************************************************/
void OCameraPutSceneInPOV_Stream (
 OCamera This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OColormap   ocmap;
  double      red,green,blue;
  ONode*      items;
  ONode*      itema;
  double      left,right,bottom,top,neard;
  OProjection projection;
  int         width,height;
  double      fieldMin,fieldMax;
  double      vrpx,vrpy,vrpz;
  double      upx,upy,upz;
  int         bw;
  double      cx,cy,cz;
  double      dopx,dopy,dopz;
  int         cut = 1;
/*.........................................................................*/
  if(This==NULL) return;

  ocmap = OCameraGetColormap(This);
  if(OColormapIsValid(ocmap)==0) 
    {
      CWarnF  ("Camera %s has no OColormap.\n",OCameraGetName(This));
      return;
    }

  items = OCameraGetNodes (This);

  OCameraGetViewLimits (This,&left,&right,&bottom,&top);
  OCameraGetSize       (This,&width,&height);
  OCameraGetField      (This,&fieldMin,&fieldMax);
  OCameraGetVRP        (This,&vrpx,&vrpy,&vrpz);
  OCameraGetUpVector   (This,&upx,&upy,&upz);

  projection           = OCameraGetProjection(This);
  
  if (projection == OProjectionParallel)
    {
    }
  else if (projection == OProjectionPerspective)
    {
    }
  
  bw    = OCameraGetBlackAndWhite(This);
  if(bw==1) 
    { 
      red = green = blue = 0.;}
  else if(bw==2) 
    { 
      red = green = blue = 1.;}
  else              
    {
      OColormapGetIthRGB    (ocmap,OCameraGetBackgroundColorIndex(This),&red,&green,&blue);
    }

  OCameraGetCenter (This,&cx,&cy,&cz);

/* 
 In POV :
/CameraPosition distance theta phi
theta = angle in degree from z axis
phi   = angle in degree from x axis

 POV do not have parallel projection built in,
we have to emulate it with a perspective projection
with a little angle (one degree).
*/

  dopx          = cx - vrpx;
  dopy          = cy - vrpy;
  dopz          = cz - vrpz;
  CVectorNormalize  (&(dopx),&(dopy),&(dopz));
  if (projection == OProjectionParallel)
    {
      fprintf       (Class.file,"camera { //%s\n",OCameraGetName(This));
      fprintf       (Class.file,"  orthographic\n");
      fprintf       (Class.file,"  location  <%g,%g,%g>\n", vrpx,vrpy,vrpz);
      fprintf       (Class.file,"  sky       <%g,%g,%g>\n", upx ,upy ,upz);
      fprintf       (Class.file,"  right     <-%g,0,0>\n" , right-left);
      fprintf       (Class.file,"  up        <0,%g,0>\n"  , top-bottom);
      fprintf       (Class.file,"  look_at   <%g,%g,%g>\n", cx  ,cy  ,cz);
      fprintf       (Class.file,"}\n\n");
      fprintf       (Class.file,"background   { color rgb<%g,%g,%g>}\n",red,green,blue);
      fprintf       (Class.file,"light_source { <%g,%g,%g> color rgb<1,1,1>}\n",vrpx,vrpy,vrpz);
    }
  else if (projection == OProjectionPerspective)
    {
      double        angle = M_PI_2;
      double        eyex,eyey,eyez;
      double        focal;
      angle         = OCameraGetProjectionAngle (This);
      if(angle<=0.) angle = M_PI_2;
      focal         = 1./tan(angle/2.);
      neard         = focal * (fieldMax - fieldMin)/2.;
      eyex          = vrpx - dopx * neard;
      eyey          = vrpy - dopy * neard;
      eyez          = vrpz - dopz * neard;
      fprintf       (Class.file,"camera { //%s\n",OCameraGetName(This));
      fprintf       (Class.file,"  perspective\n");
      fprintf       (Class.file,"  location  <%g,%g,%g>\n", eyex,eyey,eyez);
      fprintf       (Class.file,"  direction <%g,%g,%g>\n", neard*dopx,neard*dopy,neard*dopz);
      fprintf       (Class.file,"  sky       <%g,%g,%g>\n", upx ,upy ,upz);
      fprintf       (Class.file,"  right     <-%g,0,0>\n" , right-left);
      fprintf       (Class.file,"  up        <0,%g,0>\n"  , top-bottom);
      fprintf       (Class.file,"  look_at   <%g,%g,%g>\n", cx,cy,cz);
      fprintf       (Class.file,"}\n\n");
      fprintf       (Class.file,"background   { color rgb<%g,%g,%g>}\n",red,green,blue);
      fprintf       (Class.file,"light_source { <%g,%g,%g> color rgb<1,1,1>}\n",eyex,eyey,eyez);
    }

  fprintf       (Class.file,"// ********  O B J E C T S *******\n");
  if(cut==1)
    {
      fprintf       (Class.file,"difference {\n");
      fprintf       (Class.file,"union {\n");
    }
  if(items!=NULL)
    {
      int                   highLight = 0;
      int                   back      = 0;
      OContext              context;
      double                radius;
      radius                = (fieldMax - fieldMin) * 0.001;
      context               = OContextMake();
      OContextSetFromScript (context,OCameraGetHighlightContext(This));
      for(itema=items;*itema!=NULL;itema++)
	{ 
	  char*   name;
	  OMatrix matrix,ctm;

	  name    = ONodeGetName(*itema);
	  if(name!=NULL) fprintf (Class.file,"// ONode %s \n",ONodeGetName(*itema));

	  ctm     = ONodeGetMatrix (*itema);
	  matrix  = NULL;
	  if(ctm!=NULL)
	    {
	      matrix  = OMatrixMultiply (OCameraGetMatrix(This),ctm);
	    }
	  else
	    {
	      matrix  = OMatrixDuplicate (OCameraGetMatrix(This));
	    }
	  
	  highLight    = ONodeGetHighlight (*itema); 
	  back         = ONodeGetBlink     (*itema);
	  
         /*Loop over primitives.*/
	 {OPrimitive* prima;
	  OPrimitive* prims;
	  prims  = ONodeGetPrimitives(*itema);
	  if(prims!=NULL)
	    {
	      for(prima=prims;*prima!=NULL;prima++)
		{
		  WritePrimitive  (This,*prima,back,highLight,context,matrix,radius);
		}
	    }}

	 OMatrixDelete(matrix);

	} /*end_for*/
      OContextDelete (context);
    }
  if(cut==1)
    {
      double  ax,ay,az;
      double  bx,by,bz;
      double  radius = 200.;
      OMatrixMultiplyVector (OCameraGetMatrix(This),radius,radius,-1000. ,&ax,&ay,&az);
      OMatrixMultiplyVector (OCameraGetMatrix(This),radius,radius, 1000. ,&bx,&by,&bz);
      fprintf       (Class.file,"}\n");
      fprintf       (Class.file,"cylinder {<%g,%g,%g>,<%g,%g,%g>,%g}\n",ax,ay,az,bx,by,bz,radius);
      fprintf       (Class.file,"}\n");
    }

}
/***************************************************************************/
void OCameraProducePOV_File (
 OCamera This
,char*   a_fname
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  OCameraOpenPOV_FileForWriting (a_fname);
  OCameraPutSceneInPOV_Stream   (This);
  OCameraClosePOV_Stream        ();
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static void WritePrimitive (
 OCamera     This 
,OPrimitive  a_prim 
,int         a_back
,int         a_highlight
,OContext    a_context
,OMatrix     a_matrix
,double      a_radius
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int           count;
  int           pointn;
  double*       pxs;
  double*       pys;
  double*       pzs;
  double*       xs;
  double*       ys;
  double*       zs;
  double        red,green,blue;
  int           index;
  int           width;
  int           bw;
  OColormap     ocmap;
/*.........................................................................*/
  if(This==NULL) return;

  pointn = OPrimitiveGetPoints   (a_prim,&pxs,&pys,&pzs);
  if( (pxs==NULL) || (pys==NULL) || (pzs==NULL) )  return;
  if(pointn<=0)          return;

 {double*       axs;
  double*       ays;
  double*       azs;
  axs = xs = (double*)CMemoryAllocateBlock(pointn * sizeof(double));
  ays = ys = (double*)CMemoryAllocateBlock(pointn * sizeof(double));
  azs = zs = (double*)CMemoryAllocateBlock(pointn * sizeof(double));
  if( (xs==NULL) || (ys==NULL) || (zs==NULL) )
    {
      CMemoryFreeBlock (xs);
      CMemoryFreeBlock (ys);
      CMemoryFreeBlock (zs);
      return;
    }
  for(count=0;count<pointn;count++,pxs++,pys++,pzs++,axs++,ays++,azs++)
    {
      OMatrixMultiplyVector (a_matrix,*pxs,*pys,*pzs,axs,ays,azs);
    }}
  
  if(a_back!=0)
    {
      index = OCameraGetBackgroundColorIndex(This);
    }
  else 
    {
      index = a_highlight==1 ? OContextGetColorIndex(a_context) : OPrimitiveGetColorIndex(a_prim);
    }
  width  = a_highlight==1 ? OContextGetLineWidth  (a_context) : OPrimitiveGetLineWidth     (a_prim);
/*
  OAreaStyle sarea;
  sarea      = OPrimitiveGetAreaStyle (a_prim);
*/

  ocmap = OCameraGetColormap(This);
  bw    = OCameraGetBlackAndWhite(This);

       if(bw==1) 
    { 
      red = green = blue = 1.;
    }
  else if(bw==2) 
    { 
      red = green = blue = 0.;
    }
  else 
    {
      OColormapGetIthRGB    (ocmap,index,&red,&green,&blue);
    }

       /*
       red = green = blue = 0.8;
       */

  switch (OPrimitiveGetType(a_prim))
    {
          case OPrimitivePolygon:
	    for (count=2;count<pointn;count++) 
	      {
		fprintf       (Class.file,"triangle {");	
		fprintf       (Class.file,"<%g,%g,%g>," ,xs[0]      ,ys[0]      ,zs[0]);
		fprintf       (Class.file,"<%g,%g,%g>," ,xs[count-1],ys[count-1],zs[count-1]);
		fprintf       (Class.file,"<%g,%g,%g>\n",xs[count]  ,ys[count]  ,zs[count]);
		fprintf       (Class.file," pigment {color rgb<%g,%g,%g>}\n",red,green,blue);
		fprintf       (Class.file," finish  {ambient 0.8}\n");
		fprintf       (Class.file,"}\n");
	      }
	    if(OCameraAreEdgesVisible(This)==0)
	      {
		if(bw==1) 
		  { 
		    red = green = blue = 0.;
		  }
		else if(bw==2) 
		  { 
		    red = green = blue = 1.;
		  }
		else 
		  {
		    OColormapGetIthRGB    (ocmap,OCameraGetEdgeColorIndex(This),&red,&green,&blue);
		  }
		for (count=0;count<pointn;count++) 
		  {
		    fprintf       (Class.file,"cylinder {");	
		    fprintf       (Class.file,"<%g,%g,%g>,",xs[count-1],ys[count-1],zs[count-1]);
		    fprintf       (Class.file,"<%g,%g,%g>,",xs[count]  ,ys[count]  ,zs[count]);
		    fprintf       (Class.file,"%g\n"       ,a_radius);
		    fprintf       (Class.file," pigment {color rgb<%g,%g,%g>}\n",red,green,blue);
		    fprintf       (Class.file,"}\n");
		  }
	      }
            break;
	    /*
          case OPrimitiveLines      :
	    for (count=1;count<pointn;count++) 
	      {
		fprintf       (Class.file,"cylinder {");	
		fprintf       (Class.file,"<%g,%g,%g>,",xs[count-1],ys[count-1],zs[count-1]);
		fprintf       (Class.file,"<%g,%g,%g>,",xs[count]  ,ys[count]  ,zs[count]);
		fprintf       (Class.file,"%g\n"       ,a_radius);
		fprintf       (Class.file," pigment {color rgb<%g,%g,%g>}\n",red,green,blue);
		fprintf       (Class.file,"}\n");
	      }
            break;
          case OPrimitiveSegments      :
	   {int      segmentn,ipoint;
	    segmentn = pointn/2; 
	    ipoint   = 0;
	    for (count=0;count<segmentn;count++) 
	      {
		fprintf (Class.file,"cylinder {");
		fprintf (Class.file,"<%g,%g,%g>,",xs[ipoint],ys[ipoint],zs[ipoint]);
                ipoint++;
		fprintf (Class.file,"<%g,%g,%g>,",xs[ipoint],ys[ipoint],zs[ipoint]);
                ipoint++;
		fprintf (Class.file,"%g\n"       ,a_radius);
		fprintf (Class.file," pigment {color rgb<%g,%g,%g>}\n",red,green,blue);
		fprintf (Class.file,"}\n");
	      }
            }
	    break;
          case OPrimitiveMarkers      :
	    for (count=0;count<pointn;count++) 
	      {
		fprintf (Class.file,"sphere {<%g,%g,%g>,%g\n",xs[count],ys[count],zs[count],a_radius);
		fprintf (Class.file," pigment {color rgb<%g,%g,%g>}\n",red,green,blue);
		fprintf (Class.file,"}\n");
	      }
	    break;
	    */
    }

  CMemoryFreeBlock (xs);
  CMemoryFreeBlock (ys);
  CMemoryFreeBlock (zs);
}
/***************************************************************************/
static void ReportError (
 char* a_name
,int a_code
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CErrorHandle ("Go","OCamPOV",a_name,a_code);
}
