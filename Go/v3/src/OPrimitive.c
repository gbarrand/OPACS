/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#include <stdarg.h>
#include <string.h>

#include <CMemory.h>
#include <CPrinter.h>
#include <CError.h>
 
typedef unsigned long Ulong;

#include <OContextP.h>

#include <OPrimitiveP.h>

#ifdef __cplusplus
extern "C"{
#endif
static int   AllocatePoints       (OPrimitive,int);
static int   AllocateMorePoints   (OPrimitive,int);
static void  SetPoints            (OPrimitive,int,double*,double*,double*);
static int   GetTypeIdentifier    (char*);
static char* ConvertTypeToString  (int);
static void  ReportError          (char*,int);
#ifdef __cplusplus
}
#endif

#define MINIMUM(a,b)             ((a)<(b)?a:b)
#define MAXIMUM(a,b)             ((a)>(b)?a:b)
#define ONumber(arr)  (sizeof(arr) / sizeof(arr[0]))

/***************************************************************************/
void OPrimitiveClearClass (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
}
/***************************************************************************/
OPrimitive OPrimitiveMake (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OPrimitive            This;
/*.........................................................................*/
  This               = (OPrimitive) CMemoryAllocateBlock  (sizeof(OPrimitiveRecord));
  if(This==NULL)     return NULL;

  This->type         = OPrimitiveLines;
  OContextInitialize (&(This->contextRecord));
  This->pointn       = 0;
  This->xs           = NULL;
  This->ys           = NULL;
  This->zs           = NULL;
  This->label        = 0;

  return             This;
}
/***************************************************************************/
OPrimitive OPrimitiveCreate (
 int           a_type 
,OContext      a_context
,int           a_pointn
,double*       a_xs
,double*       a_ys
,double*       a_zs
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OPrimitive            This;
/*.........................................................................*/
  This             = OPrimitiveMake();
  if(This==NULL)   return NULL;
  This->type       = a_type;
  OContextCopy     (&(This->contextRecord),a_context!=NULL ? a_context : OContextGetStaticInstance());
  SetPoints        (This,a_pointn,a_xs,a_ys,a_zs);
  return           This;
}
/***************************************************************************/
void OPrimitiveDelete (
 OPrimitive This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  CMemoryFreeBlock (This->xs);
  CMemoryFreeBlock (This->ys);
  CMemoryFreeBlock (This->zs);
  This->type       = OPrimitiveLines;
  This->pointn     = 0;
  This->xs         = NULL;
  This->ys         = NULL;
  This->zs         = NULL;
  CMemoryFreeBlock (This);
}
/***************************************************************************/
OPrimitive OPrimitiveDuplicate (
 OPrimitive a_from
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_from==NULL) return NULL;
  return OPrimitiveCreate(a_from->type,&(a_from->contextRecord),a_from->pointn,a_from->xs,a_from->ys,a_from->zs);
}
/***************************************************************************/
void OPrimitiveAddPoints (
 OPrimitive This
,int        a_pointn
,double*    a_xs
,double*    a_ys
,double*    a_zs
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int              pos,count;
/*.........................................................................*/
  if(This==NULL)   return;
  if(a_pointn<=0)  return;
  if(a_xs==NULL)   return;
  if(a_ys==NULL)   return;
  if(a_zs==NULL)   return;
  pos              = This->pointn;
  if(AllocateMorePoints(This,a_pointn)==0) return;
  for(count=0;count<a_pointn;count++)
    {
      This->xs[pos] = a_xs[count];
      This->ys[pos] = a_ys[count];
      This->zs[pos] = a_zs[count];
      pos++;
    }
}
/***************************************************************************/
int OPrimitiveGetType (
 OPrimitive This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return OPrimitiveLines;
  return This->type;
}
/***************************************************************************/
void OPrimitiveSetContext (
 OPrimitive This
,OContext   a_context
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)   return;
  OContextCopy     (&(This->contextRecord),a_context!=NULL ? a_context : OContextGetStaticInstance());
}
/***************************************************************************/
int OPrimitiveGetColorIndex (
 OPrimitive This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 0;
  return  OContextGetColorIndex(&(This->contextRecord));
}
/***************************************************************************/
void OPrimitiveSetColorIndex (
 OPrimitive This
,int a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  OContextSetColorIndex(&(This->contextRecord),a_value);
}
/***************************************************************************/
int OPrimitiveGetLabel (
 OPrimitive This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 0;
  return  This->label;
}
/***************************************************************************/
void OPrimitiveSetLabel (
 OPrimitive This
,int   a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  This->label = a_value;
}
/***************************************************************************/
int OPrimitiveGetLineWidth (
 OPrimitive This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 0;
  return  OContextGetLineWidth(&(This->contextRecord));
}
/***************************************************************************/
void OPrimitiveSetLineWidth (
 OPrimitive This
,int a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  OContextSetLineWidth(&(This->contextRecord),a_value);
}
/***************************************************************************/
int OPrimitiveGetMarkerSize (
 OPrimitive This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 0;
  return  OContextGetMarkerSize(&(This->contextRecord));
}
/***************************************************************************/
void OPrimitiveSetMarkerSize (
 OPrimitive This
,int a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  OContextSetMarkerSize(&(This->contextRecord),a_value);
}
/***************************************************************************/
OLineStyle OPrimitiveGetLineStyle (
 OPrimitive This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return OLineStyleSolid;
  return    OContextGetLineStyle(&(This->contextRecord));
}
/***************************************************************************/
void OPrimitiveSetLineStyle (
 OPrimitive This
,OLineStyle a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  OContextSetLineStyle(&(This->contextRecord),a_value);
}
/***************************************************************************/
OMarkerStyle OPrimitiveGetMarkerStyle (
 OPrimitive This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return OMarkerStyleDot;
  return    OContextGetMarkerStyle(&(This->contextRecord));
}
/***************************************************************************/
void OPrimitiveSetMarkerStyle (
 OPrimitive This
,OMarkerStyle a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  OContextSetMarkerStyle(&(This->contextRecord),a_value);
}
/***************************************************************************/
OAreaStyle OPrimitiveGetAreaStyle (
 OPrimitive This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return OAreaStyleSolid;
  return    OContextGetAreaStyle(&(This->contextRecord));
}
/***************************************************************************/
void OPrimitiveSetAreaStyle (
 OPrimitive This
,OAreaStyle a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  OContextSetAreaStyle(&(This->contextRecord),a_value);
}
/***************************************************************************/
int OPrimitiveGetPoints (
 OPrimitive This
,double** a_xs
,double** a_ys
,double** a_zs
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_xs!=NULL) *a_xs = NULL;
  if(a_ys!=NULL) *a_ys = NULL;
  if(a_zs!=NULL) *a_zs = NULL;
  if(This==NULL) return 0;
  if(a_xs!=NULL) *a_xs = This->xs;
  if(a_ys!=NULL) *a_ys = This->ys;
  if(a_zs!=NULL) *a_zs = This->zs;
  return   This->pointn;
}
/***************************************************************************/
int OPrimitiveGetSize (
 OPrimitive This
,double* a_xmin   
,double* a_xmax   
,double* a_ymin   
,double* a_ymax   
,double* a_zmin   
,double* a_zmax   
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int      first;
   int count;
/*.........................................................................*/
  if(This==NULL)   return 0;
  if(a_xmin==NULL) return 0;
  if(a_xmax==NULL) return 0;
  if(a_ymin==NULL) return 0;
  if(a_ymax==NULL) return 0;
  if(a_zmin==NULL) return 0;
  if(a_zmax==NULL) return 0;
  if( (This->xs==NULL) || (This->ys==NULL) || (This->zs==NULL) ) return 0;
  first = 0;
  for(count=0,*a_xmax = This->xs[count],
              *a_ymax = This->ys[count],
	      *a_zmax = This->zs[count],         
              *a_xmin = This->xs[count],
              *a_ymin = This->ys[count],     
	      *a_zmin = This->zs[count];count<This->pointn;count++)
    {
      *a_xmax = MAXIMUM(*a_xmax,This->xs[count]);         
      *a_ymax = MAXIMUM(*a_ymax,This->ys[count]);         
      *a_zmax = MAXIMUM(*a_zmax,This->zs[count]);         
      
      *a_xmin = MINIMUM(*a_xmin,This->xs[count]);         
      *a_ymin = MINIMUM(*a_ymin,This->ys[count]);         
      *a_zmin = MINIMUM(*a_zmin,This->zs[count]);         
      first   = 1;
    }
  return first; /* no point */
}
/***************************************************************************/
int OPrimitiveGetSumOfPoints (
 OPrimitive This
,double* a_x
,double* a_y
,double* a_z
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  int pointn;
/*.........................................................................*/
  if(This==NULL) return 0;
/*Do not initialize *a_x, *a_y, *a_z.*/
  if( (This->xs==NULL) || (This->ys==NULL) || (This->zs==NULL) ) return 0;
  if(This->pointn<=0) return 0;
  if(This->pointn==1) 
    {
      pointn = 1;
    }
  else 
    {
       pointn = ( 
	    (This->xs[0] == This->xs[This->pointn-1]) &&
	    (This->xs[0] == This->xs[This->pointn-1]) &&
	    (This->xs[0] == This->xs[This->pointn-1]) 
	    ) ? This->pointn-1 : This->pointn;
    }
  for(count=0;count<pointn;count++)
    {
      if(a_x!=NULL) (*a_x) += This->xs[count];
      if(a_y!=NULL) (*a_y) += This->ys[count];
      if(a_z!=NULL) (*a_z) += This->zs[count];
    }
  return pointn;
}
/***************************************************************************/
int OPrimitiveGetIthPoint (
 OPrimitive This
,int a_index
,double* a_x
,double* a_y
,double* a_z
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_x!=NULL) *a_x = 0.;
  if(a_y!=NULL) *a_y = 0.;
  if(a_z!=NULL) *a_z = 0.;
  if(This==NULL) return 0;
  if( (a_index<0) || (a_index>=This->pointn) ) return 0;
  if(a_x!=NULL) *a_x = This->xs[a_index];
  if(a_y!=NULL) *a_y = This->ys[a_index];
  if(a_z!=NULL) *a_z = This->zs[a_index];
  return 1;
}
/***************************************************************************/
int OPrimitiveSetAttribute (
 OPrimitive  This
,char*  a_name
,void*  a_user
,void*  a_addr 
,int    a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)    return 0;
  if(a_name==NULL)  return 0;
  if(a_addr==NULL)  return 0; 

       if(strcmp(a_name,"identifier")==0)  {}
  else if(strcmp(a_name,"label")==0) 	   This->label                    = *((int*)a_addr);
  else if(strcmp(a_name,"type")==0) 	   This->type                     = GetTypeIdentifier(*((char**)a_addr));
  else if(strcmp(a_name,"colorIndex")==0)  This->contextRecord.color      = *((int*)a_addr);
  else if(strcmp(a_name,"lineWidth")==0)   This->contextRecord.lineWidth  = *((int*)a_addr);
  else if(strcmp(a_name,"markerSize")==0)  This->contextRecord.markerSize = *((int*)a_addr);
  else if(strcmp(a_name,"lineStyle")==0)   This->contextRecord.lineStyle  = OLineStyleGetIdentifier(*((char**)a_addr));
  else if(strcmp(a_name,"markerStyle")==0) This->contextRecord.markStyle  = OMarkerStyleGetIdentifier(*((char**)a_addr));
  else if(strcmp(a_name,"areaStyle")==0)   This->contextRecord.areaStyle  = OAreaStyleGetIdentifier(*((char**)a_addr));
  else if(strcmp(a_name,"numberOfPoints")==0) 	{}
  else if(strcmp(a_name,"points")==0)
    {
      int    pointn;
      pointn = a_number/3;
      if(AllocatePoints(This,pointn)==1)
	{
	   int count;
	  double*        list;
	  list           = *((double**)a_addr);
	  for(count=0;count<pointn;count++)
	    {
	      This->xs[count] = *list;list++;
	      This->ys[count] = *list;list++;
	      This->zs[count] = *list;list++;
	    }
	}
    }
  else 
    {
      CInfoF ("OPrimitiveSetAttribute: unknown property %s.\n",a_name);
      return 0;
    }

  a_user = NULL;
  return 1;
}
/***************************************************************************/
int OPrimitiveGetAttribute (
 OPrimitive  This
,char*  a_name
,void*  a_user
,void*  a_addr 
,int*   a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_number!=NULL) *a_number = 0;
  if(This==NULL)    return 0;
  if(a_name==NULL)  return 0;
  if(a_addr==NULL)  return 0; 

       if(strcmp(a_name,"identifier")==0) *((Ulong*)a_addr)  = (Ulong)This;
  else if(strcmp(a_name,"label")==0) 	  *((int*)a_addr)    = This->label;
  else if(strcmp(a_name,"type")==0) 	  *((char**)a_addr)  = ConvertTypeToString(OPrimitiveGetType(This));
  else if(strcmp(a_name,"colorIndex")==0) *((int*)a_addr)    = OPrimitiveGetColorIndex     (This);
  else if(strcmp(a_name,"lineWidth")==0)  *((int*)a_addr)    = OPrimitiveGetLineWidth     (This);
  else if(strcmp(a_name,"markerSize")==0) *((int*)a_addr)    = OPrimitiveGetMarkerSize    (This);
  else if(strcmp(a_name,"lineStyle")==0)  *((char**)a_addr)  = OLineStyleGetName(OPrimitiveGetLineStyle(This));
  else if(strcmp(a_name,"markerStyle")==0)*((char**)a_addr)  = OMarkerStyleGetName(OPrimitiveGetMarkerStyle(This));
  else if(strcmp(a_name,"areaStyle")==0)  *((char**)a_addr)  = OAreaStyleGetName(OPrimitiveGetAreaStyle(This));
  else if(strcmp(a_name,"numberOfPoints")==0) *((int*)a_addr)    = This->pointn;
  else if(strcmp(a_name,"points")==0)
    {
      int     number = 0;
      double* doubles;
      doubles = NULL;
      if(This->pointn!=0)
        {
	  doubles = (double*)CMemoryAllocateBlock( (3 * This->pointn) * sizeof(double));
	  if(doubles!=NULL)
	    {
	       int count;
	      number = 0;
	      for(count=0;count<This->pointn;count++) 
		{
		  doubles[number] = This->xs[count];number++;
		  doubles[number] = This->ys[count];number++;
		  doubles[number] = This->zs[count];number++;
		}
	    }
	}
      if(a_number!=NULL)  *a_number = number;
      *((double**)a_addr) = doubles;
      return              FREE_BLOCK;
    }
  else 
    {
      CInfoF("OPrimitiveGetAttribute: unknown property %s.\n",a_name);
      return 0;
    }

  a_user = NULL;
  return 1;
}
static struct {
  char* string;
  int   value;
} OPrimitiveTypeList [] = {
  {"lines"   , OPrimitiveLines},
  {"segments", OPrimitiveSegments},
  {"markers" , OPrimitiveMarkers},
  {"polygon" , OPrimitivePolygon}
};  
/***************************************************************************/
static int GetTypeIdentifier (
 char* a_string 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  unsigned int count;
/*.........................................................................*/
  if( (a_string==NULL) || (*a_string=='\0')) return OPrimitiveLines; 
  for(count=0;count<ONumber(OPrimitiveTypeList);count++)
    {
      if(strcmp(a_string,OPrimitiveTypeList[count].string)==0) return OPrimitiveTypeList[count].value;
    }
  ReportError   ("GetTypeIdentifier",CErrorName);
  return  OPrimitiveLines;
}
/***************************************************************************/
static char* ConvertTypeToString (
 int a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return OPrimitiveTypeList[a_value].string;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static void SetPoints (
 OPrimitive   This
,int     a_pointn
,double* a_xs
,double* a_ys
,double* a_zs
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  int number;
/*.........................................................................*/
  if(a_pointn<=0)  return;
  if(a_xs==NULL)   return;
  if(a_ys==NULL)   return;
  if(a_zs==NULL)   return;
  if( (This->type==OPrimitivePolygon) && (a_pointn>=2) &&
       ((  (a_xs[a_pointn-1])==a_xs[0]) &&
        (  (a_ys[a_pointn-1])==a_ys[0]) &&
        (  (a_zs[a_pointn-1])==a_zs[0]))   )
     {
       /*GL do not want closed list of vertices for polygons.*/
       number  = a_pointn-1;
     }
  else
    {
       number  = a_pointn;
     }
  if(AllocatePoints(This,number)==0) return;
  for(count=0;count<This->pointn;count++)
    {
      This->xs[count] = a_xs[count];
      This->ys[count] = a_ys[count];
      This->zs[count] = a_zs[count];
    }
}
/***************************************************************************/
static int AllocatePoints (
 OPrimitive This
,int     a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CMemoryFreeBlock  (This->xs);
  CMemoryFreeBlock  (This->ys);
  CMemoryFreeBlock  (This->zs);
  This->xs          = NULL;
  This->ys          = NULL;
  This->zs          = NULL;
  This->pointn      = 0;
  if(a_number<=0)   return 1;
  This->xs          = (double*)CMemoryAllocateBlock(a_number*sizeof(double));
  This->ys          = (double*)CMemoryAllocateBlock(a_number*sizeof(double));
  This->zs          = (double*)CMemoryAllocateBlock(a_number*sizeof(double));
  This->pointn      = a_number;
  if( (This->xs==NULL) || (This->ys==NULL) || (This->zs==NULL) ) 
    {
      CMemoryFreeBlock (This->xs);
      CMemoryFreeBlock (This->ys);
      CMemoryFreeBlock (This->zs);
      This->xs         = NULL;
      This->ys         = NULL;
      This->zs         = NULL;
      This->pointn     = 0;
      return           0;
    }
  return 1;
}
/***************************************************************************/
static int AllocateMorePoints (
 OPrimitive This
,int        a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int number;
/*.........................................................................*/
  if(a_number<=0) return 1;
  if(This->pointn==0)
    {
      return AllocatePoints (This,a_number);
    }
  number       = This->pointn + a_number;
  This->xs     = (double*)CMemoryChangeBlockSize (This->xs,number*sizeof(double));
  This->ys     = (double*)CMemoryChangeBlockSize (This->ys,number*sizeof(double));
  This->zs     = (double*)CMemoryChangeBlockSize (This->zs,number*sizeof(double));
  This->pointn = number;
  if( (This->xs==NULL) || (This->ys==NULL) || (This->zs==NULL) ) 
    {
      CMemoryFreeBlock (This->xs);
      CMemoryFreeBlock (This->ys);
      CMemoryFreeBlock (This->zs);
      This->xs         = NULL;
      This->ys         = NULL;
      This->zs         = NULL;
      This->pointn     = 0;
      return           0;
    }
  return 1;
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
  CErrorHandle ("Go","OPrimitive",a_name,a_code);
}
