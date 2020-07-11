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
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include <CError.h>
#include <CPrinter.h>
#include <CFile.h>
#include <CString.h>
#include <CVector.h>
#include <CMemory.h>

#include <OCamVRML.h>

#ifdef __cplusplus
extern "C"{
#endif
static void  WritePrimitive (OCamera,OPrimitive,int,int,OContext);
static int   GetKind        ();
static void  SetLineStyle   (OLineStyle);
static void  ReportError    (char*,int);
#ifdef __cplusplus
}
#endif

typedef unsigned char Uchar;
#define METAFILE_DEFAULT        "out.iv"
#define CHECK_DIV(a_var,a_pro)  {if((a_var)==0.) {ReportError(a_pro,CErrorValue);(a_var)=1.;}}

static struct 
{
  FILE* file;
  char* fname;
} Class = {NULL,NULL};
/***************************************************************************/
void OCameraOpenVRML_FileForWriting (
 char* a_string
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.file!=NULL) OCameraCloseVRML_Stream ();
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
  if(GetKind()==0)
    {
      fprintf (Class.file,"#Inventor V2.0 ascii\n");
      fprintf (Class.file,"#VRML V1.0 ascii\n");
    }
  else
    {
      fprintf (Class.file,"#VRML V1.0 ascii\n");
      fprintf (Class.file,"#Inventor V2.0 ascii\n");
    } 
 {char* senv;
  senv  = getenv   ("GOVERSION");
  if(senv!=NULL) 
    {
      fprintf (Class.file,"#Creator: Go %s, LAL Orsay.\n\n",senv);
    }
  else
    {
      fprintf (Class.file,"#Creator: Go, LAL Orsay.\n\n");
    }}
}
/***************************************************************************/
void OCameraCloseVRML_Stream (
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
void OCameraPutSceneInVRML_Stream (
 OCamera This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OColormap   ocmap;
  double      red,green,blue;
  ONode*      items;
  ONode*      itema;
  double      left,right,bottom,top,neard,fard,aspect;
  double      matrix[16];
  OProjection projection;
  int         width,height;
  double      fieldMin,fieldMax;
  double      vrpx,vrpy,vrpz;
  double      upx,upy,upz;
  int         bw;
  double      dx,dy,dz,dangle;
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

  aspect               = ((double)width)/((double)height);

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
/*
   glClearColor (red,green,blue,0.0);
*/

  fprintf (Class.file,"Separator {\n");
  fprintf (Class.file,"LightModel  { model BASE_COLOR}\n");
  fprintf (Class.file,"Material {diffuseColor 1 1 1}\n\n");

  if (projection == OProjectionParallel)
    {
      double        size;
      size          = fieldMax - fieldMin;
      neard         = - 1000. * size;
      fard          =   1000. * size;

      OMatrixGetDirection  (0.,0.,-1.,0.,1.,0.,
                            -vrpx,-vrpy,-vrpz,upx,upy,upz,
		            &dx,&dy,&dz,&dangle);

      fprintf       (Class.file,"OrthographicCamera {\n");
      fprintf       (Class.file," position     %g %g %g\n", vrpx, vrpy, vrpz);
      fprintf       (Class.file," orientation  %g %g %g %g\n",dx,dy,dz,dangle);
      fprintf       (Class.file," nearDistance %g\n",neard);
      fprintf       (Class.file," farDistance  %g\n",fard);
      fprintf       (Class.file," height       %g\n",top-bottom);
      fprintf       (Class.file," aspectRatio  %g\n",aspect);
      fprintf       (Class.file,"}\n");
    }
  else if (projection == OProjectionPerspective)
    {
      double        lookx,looky,lookz;
      double        dopx,dopy,dopz;
      double        eyex,eyey,eyez;
      double        angle;
      double        widthAngle,heightAngle,aspectRatio;

      angle         = OCameraGetProjectionAngle (This);
      neard         = 2. * tan(angle/2.);
      CHECK_DIV     (neard,"Traverse")
      neard         = (fieldMax - fieldMin)/neard;
      fard          = 1000. * neard;

      lookx        = 0.; 
      looky        = 0.;
      lookz        = 0.;

      dopx         = lookx - vrpx;
      dopy         = looky - vrpy;
      dopz         = lookz - vrpz;
      CVectorNormalize  (&(dopx),&(dopy),&(dopz));

      eyex         = vrpx - dopx * neard;
      eyey         = vrpy - dopy * neard;
      eyez         = vrpz - dopz * neard;

      OMatrixGetDirection  (0.,0.,-1.,0.,1.,0.,
                            dopx,dopy,dopz,upx,upy,upz,
		            &dx,&dy,&dz,&dangle);

      widthAngle   = 2. * atan2 ( (top-bottom) /2.,neard);
      heightAngle  = 2. * atan2 ( (right-left) /2.,neard);
      aspectRatio  = widthAngle/heightAngle; 

      fprintf (Class.file,"PerspectiveCamera {\n");
      fprintf (Class.file," position     %g %g %g\n",eyex,eyey,eyez);
      fprintf (Class.file," orientation  %g %g %g %g\n",dx,dy,dz,dangle);
      fprintf (Class.file," nearDistance %g\n",neard);
      fprintf (Class.file," farDistance  %g\n",fard);
      fprintf (Class.file," heightAngle  %g\n",heightAngle);
      fprintf (Class.file," aspectRatio  %g\n",aspectRatio);
      fprintf (Class.file,"}\n");
    }


  OMatrixConvertToGL_Matrix   (OCameraGetMatrix(This),matrix);
  fprintf (Class.file,"MatrixTransform { matrix \n");
  fprintf (Class.file," %g %g %g %g\n",matrix[ 0],matrix[ 1],matrix[ 2],matrix[ 3]);
  fprintf (Class.file," %g %g %g %g\n",matrix[ 4],matrix[ 5],matrix[ 6],matrix[ 7]);
  fprintf (Class.file," %g %g %g %g\n",matrix[ 8],matrix[ 9],matrix[10],matrix[11]);
  fprintf (Class.file," %g %g %g %g\n",matrix[12],matrix[13],matrix[14],matrix[15]);
  fprintf (Class.file,"}\n");
  
  if(items!=NULL)
    {
      int      highLight = 0;
      int      back      = 0;
      OContext context;
      context               = OContextMake();
      OContextSetFromScript (context,OCameraGetHighlightContext(This));
      for(itema=items;*itema!=NULL;itema++)
	{ 
	  char*   name;
	  OMatrix ctm;

	  name    = ONodeGetName(*itema);
	  if(name!=NULL) fprintf (Class.file,"# ONode %s \n",ONodeGetName(*itema));
	  fprintf (Class.file,"Separator {\n");

	  ctm     = ONodeGetMatrix (*itema);
	  if(ctm!=NULL)
	    {
	      OMatrixConvertToGL_Matrix   (ctm,matrix);
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
		  WritePrimitive  (This,*prima,back,highLight,context);
		}
	    }}

	  fprintf (Class.file,"}\n\n");

	} /*end_for*/
      OContextDelete (context);


    }

  fprintf (Class.file,"}\n");

}
/***************************************************************************/
void OCameraProduceVRML_File (
 OCamera This
,char*   a_fname
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  OCameraOpenVRML_FileForWriting (a_fname);
  OCameraPutSceneInVRML_Stream   (This);
  OCameraCloseVRML_Stream        ();
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
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  int           pointn;
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

  pointn = OPrimitiveGetPoints   (a_prim,&xs,&ys,&zs);
  if( (xs==NULL) || (ys==NULL) || (zs==NULL) )  return;
  if(pointn<=0)          return;

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
  OLineStyle    sline;
  OMarkerStyle  smark;
  sline  = a_highlight==1 ? OContextGetLineStyle  (a_context) : OPrimitiveGetLineStyle     (a_prim);
  smark  = a_highlight==1 ? OContextGetMarkerStyle(a_context) : OPrimitiveGetMarkerStyle   (a_prim);
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
  fprintf (Class.file,"BaseColor { rgb [%g %g %g]}\n",red,green,blue);
*/
  fprintf (Class.file,"Material {diffuseColor %g %g %g}\n",red,green,blue);

  switch (OPrimitiveGetType(a_prim))
    {
          case OPrimitivePolygon:
	    fprintf       (Class.file,"DrawStyle {\n");
	    fprintf       (Class.file," style       FILLED\n");
	    fprintf       (Class.file,"}\n");
            break;
          case OPrimitiveLines      :
	    fprintf       (Class.file,"DrawStyle {\n");
	    fprintf       (Class.file," style       LINES\n");
	    fprintf       (Class.file," lineWidth   %d\n",width);
/*	    fprintf       (Class.file," linePattern 255\n");*/
	    fprintf       (Class.file,"}\n");
            break;
          case OPrimitiveSegments      :
	    fprintf       (Class.file,"DrawStyle {\n");
	    fprintf       (Class.file," style       LINES\n");
	    fprintf       (Class.file," lineWidth   %d\n",width);
/*	    fprintf       (Class.file," linePattern 255\n");*/
	    fprintf       (Class.file,"}\n");
            break;
          case OPrimitiveMarkers      :
	    fprintf       (Class.file,"DrawStyle {\n");
	    fprintf       (Class.file," style       POINTS\n");
	    fprintf       (Class.file," lineWidth   %d\n",width);
	  /*fprintf       (Class.file," linePattern %d\n",a_width);*/
	    fprintf       (Class.file,"}\n");
            break;
    }

  fprintf       (Class.file,"Coordinate3 { point [\n");
  for (count=0;count<pointn;count++) 
    {
      fprintf (Class.file," %g %g %g",xs[count],ys[count],zs[count]);
      if(count!=pointn-1)
	{
	  fprintf (Class.file,",\n");
	}
    }
  fprintf (Class.file,"]}\n");

  switch (OPrimitiveGetType(a_prim))
    {
          case OPrimitivePolygon:
	    fprintf       (Class.file,"IndexedFaceSet { coordIndex [\n");
            for (count=0;count<pointn;count++) 
              {
		fprintf    (Class.file,"%d,\n",count);
	      }
	    fprintf       (Class.file,"-1]}\n");
            break;
          case OPrimitiveLines      :
	    fprintf       (Class.file,"IndexedLineSet { coordIndex [\n");
            for (count=0;count<pointn;count++) 
              {
		fprintf    (Class.file,"%d,\n",count);
	      }
	    fprintf       (Class.file,"-1]}\n");
            break;
          case OPrimitiveSegments      :
	   {int      segmentn;
            segmentn = pointn/2;
            for (count=0;count<segmentn;count++) 
              {
		fprintf (Class.file,"IndexedLineSet { coordIndex [%d,%d,-1]}\n",2*count,2*count+1);
	      }
            }break;
          case OPrimitiveMarkers      :
	    fprintf       (Class.file,"IndexedLineSet { coordIndex [\n");
            for (count=0;count<pointn;count++) 
              {
		fprintf    (Class.file,"%d,\n",count);
	      }
	    fprintf       (Class.file,"-1]}\n");
            break;
    }

}
/***************************************************************************/
static void SetLineStyle (
 OLineStyle a_style
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  fprintf       (Class.file,"DrawStyle {\n");
  fprintf       (Class.file," style       LINES\n");
/*
  fprintf       (Class.file," lineWidth   %d\n",a_width);
  fprintf       (Class.file," linePattern %d\n",a_width);
*/
  fprintf       (Class.file,"}\n");
/*
  switch(a_style)
    {
      case OLineStyleSolid:
        glLineStipple (1,0x0000);
        break;
      case OLineStyleDashed:
        glLineStipple (1,0x00FF);
        break;
      case OLineStyleDotted:
        glLineStipple (1,0x0101);
        break;
      case OLineStyleDashDotted:
        glLineStipple (1,0x1C47);
        break;
    }
*/
  a_style = OLineStyleSolid;
}
/***************************************************************************/
static int GetKind (
)
/***************************************************************************/
/*
  return 0 : Inventor.
  return 1 : VRML.
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* string;
  int   type;
/*.........................................................................*/
  if(Class.fname==NULL) return 0;
  string           = CFileGetType (Class.fname);
  if(string==NULL) return 0;
  type             = strcmp(string,"wrl")==0 ? 1 : 0; 
  CStringDelete    (string);
  return           type;
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
  CErrorHandle ("Go","OCamVRML",a_name,a_code);
}
