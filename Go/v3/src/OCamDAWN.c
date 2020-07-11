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

#include <OCamDAWN.h>

#ifdef __cplusplus
extern "C"{
#endif
static void  WritePrimitive (OCamera,OPrimitive,int,int,OContext,OMatrix,double);
static void  ReportError    (char*,int);
#ifdef __cplusplus
}
#endif

typedef unsigned char Uchar;
#define METAFILE_DEFAULT        "out.prim"
#define CHECK_DIV(a_var,a_pro)  {if((a_var)==0.) {ReportError(a_pro,CErrorValue);(a_var)=1.;}}

static struct 
{
  FILE* file;
  char* fname;
} Class = {NULL,NULL};
/***************************************************************************/
void OCameraOpenDAWN_FileForWriting (
 char* a_string
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.file!=NULL) OCameraCloseDAWN_Stream ();
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
  fprintf (Class.file,"##\n");

 {char* senv;
  senv  = getenv   ("GOVERSION");
  if(senv!=NULL) 
    {
      fprintf (Class.file,"# Creator: Go %s, LAL Orsay.\n",senv);
    }
  else
    {
      fprintf (Class.file,"# Creator: Go, LAL Orsay.\n");
    }}
 fprintf (Class.file,"# Visualize with : dawn -v %s.\n",Class.fname);
}
/***************************************************************************/
void OCameraCloseDAWN_Stream (
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
void OCameraPutSceneInDAWN_Stream (
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
  double      angle = M_PI_2;
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

  fprintf (Class.file,"\
/BoundingBox %g %g %g %g %g %g\n\
",fieldMin,fieldMax,fieldMin,fieldMax,fieldMin,fieldMax);

  OCameraGetCenter (This,&cx,&cy,&cz);

/* 
 In DAWN :
/CameraPosition distance theta phi
theta = angle in degree from z axis
phi   = angle in degree from x axis

 DAWN do not have parallel projection built in,
we have to emulate it with a perspective projection
with a little angle (one degree).
*/

  if (projection == OProjectionParallel)
    {
      angle = 1. * M_PI/180.; 
    }
  else if (projection == OProjectionPerspective)
    {
      angle         = OCameraGetProjectionAngle (This);
      if(angle<=0.) angle = M_PI_2;
    }

     {double        dopx,dopy,dopz;
      double        eyex,eyey,eyez;
      double        focal;

      focal         = 1./tan(angle/2.);
      neard         = focal * (fieldMax - fieldMin)/2.;

      dopx         = cx - vrpx;
      dopy         = cy - vrpy;
      dopz         = cz - vrpz;
      CVectorNormalize  (&(dopx),&(dopy),&(dopz));

      eyex         = vrpx - dopx * neard;
      eyey         = vrpy - dopy * neard;
      eyez         = vrpz - dopz * neard;

     {double        vx,vy,vz,distance,theta,phi;
      vx            = eyex - cx;
      vy            = eyey - cy;
      vz            = eyez - cz;
      distance      = sqrt (vx * vx + vy * vy + vz * vz);
      theta         = CVectorGetPolarAngle (vx,vy,vz) * 180. / M_PI;
      phi           = CVectorGetAzimuth    (vx,vy,vz) * 180. / M_PI;
      fprintf       (Class.file,"/CameraPosition %g %g %g\n", distance, theta, phi);}
      fprintf       (Class.file,"/TargetPoint    %g %g %g\n",cx,cy,cz);

      fprintf       (Class.file,"/FocalDistance  %g\n",focal);}


  fprintf               (Class.file,"\
/Surface\n\
!SetCamera\n\
!OpenDevice\n\
");

  if(items!=NULL)
    {
      int      highLight = 0;
      int      back      = 0;
      OContext context;
      double   radius;
      radius                = (fieldMax - fieldMin) * 0.001;
      context               = OContextMake();
      OContextSetFromScript (context,OCameraGetHighlightContext(This));
      fprintf               (Class.file,"!BeginModeling\n");
      for(itema=items;*itema!=NULL;itema++)
	{ 
	  char*   name;
	  OMatrix matrix,ctm;

	  name    = ONodeGetName(*itema);
	  if(name!=NULL) fprintf (Class.file,"# ONode %s \n",ONodeGetName(*itema));

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
      fprintf        (Class.file,"!EndModeling\n");
      OContextDelete (context);


    }

  fprintf (Class.file,"\
!DrawAll\n\
!CloseDevice\n\
");

}
/***************************************************************************/
void OCameraProduceDAWN_File (
 OCamera This
,char*   a_fname
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  OCameraOpenDAWN_FileForWriting (a_fname);
  OCameraPutSceneInDAWN_Stream   (This);
  OCameraCloseDAWN_Stream        ();
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

  fprintf (Class.file,"/ColorRGB %g %g %g\n",red,green,blue);
  switch (OPrimitiveGetType(a_prim))
    {
          case OPrimitivePolygon:
	    fprintf       (Class.file,"/Brep\n");
	    fprintf       (Class.file,"BeginVertex\n");
	    for (count=0;count<pointn;count++) 
	      {
		fprintf (Class.file,"%d %g %g %g\n",count+1,xs[count],ys[count],zs[count]);
	      }
	    fprintf       (Class.file,"EndVertex\n");
	    fprintf       (Class.file,"BeginFacet\n");
	    for (count=1;count<=pointn;count++)
	      {
		fprintf (Class.file,"%d ",count);
	      }
	    fprintf (Class.file,";\n");
	    fprintf       (Class.file,"EndFacet\n");
	    fprintf       (Class.file,"/EndBrep\n");
            break;
          case OPrimitiveLines      :
	    fprintf       (Class.file,"/Polyline\n");
	    for (count=0;count<pointn;count++) 
	      {
		fprintf (Class.file,"/PLVertex %g %g %g\n",xs[count],ys[count],zs[count]);
	      }
	    fprintf       (Class.file,"/EndPolyline\n");
            break;
          case OPrimitiveSegments      :
	   {int      segmentn,ipoint;
	    segmentn = pointn/2; 
	    ipoint   = 0;
	    for (count=0;count<segmentn;count++) 
	      {
		fprintf (Class.file,"/Polyline\n");
		fprintf (Class.file,"/PLVertex %g %g %g\n",xs[ipoint],ys[ipoint],zs[ipoint]);
                ipoint++;
		fprintf (Class.file,"/PLVertex %g %g %g\n",xs[ipoint],ys[ipoint],zs[ipoint]);
                ipoint++;
		fprintf (Class.file,"/EndPolyline\n");
	      }
            }break;
          case OPrimitiveMarkers      :
	    for (count=0;count<pointn;count++) 
	      {
		fprintf (Class.file,"/Polyline\n");
		fprintf (Class.file,"/PLVertex %g %g %g\n",xs[count],ys[count],zs[count]);
		fprintf (Class.file,"/PLVertex %g %g %g\n",xs[count]+a_radius,ys[count]+a_radius,zs[count]+a_radius);
		fprintf (Class.file,"/EndPolyline\n");
	      }
	    /*
	    for (count=0;count<pointn;count++) 
	      {
		fprintf (Class.file,"/MarkCircle2D %g %g %g %g\n",xs[count],ys[count],zs[count],a_radius);
	      }
	    */
	    break;
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
  CErrorHandle ("Go","OCamDAWN",a_name,a_code);
}
