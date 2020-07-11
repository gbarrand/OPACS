/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
/*
#define DEBUG
*/

#include <stddef.h>
#include <stdio.h>

#include <CMemory.h>
#include <CString.h>
#include <CCharacter.h>
#include <CVector.h>

#include <OContextP.h>
#include <OPointListP.h>
#include <CLkListP.h>
#include <ONodeP.h>
#include <OShapeP.h>

typedef struct _OShapeBoxRecord*      OShapeBox;
typedef struct _OShapeStringRecord*   OShapeString;

#ifdef __cplusplus
extern "C"{
#endif
static OPrimitive* RepresentBox              (ONode);
static void        DeleteString              (ONode);
static OPrimitive* RepresentString           (ONode);
static OPrimitive* RepresentStringWithMatrix (char*,double,double,double,OMatrix,OContext);
static void        AddCharacter              (OPrimitive**,OMatrix,char,double*,OContext);
#ifdef __cplusplus
}
#endif
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
ONode OShapeBoxMake (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OShapeBox          This;
/*.........................................................................*/
#ifdef DEBUG
  printf             ("debug : OShapeBoxMake.\n");
#endif
  This               = (OShapeBox) CMemoryAllocateBlock  (sizeof(OShapeBoxRecord));
  if(This==NULL)     return NULL;
  ONodeInitialize    ((ONode)This);
  This->node.isShape                = 1;
  This->node.representShapeFunction = RepresentBox;
  /*Shape.*/
  This->box.x        = 0.;
  This->box.y        = 0.;
  This->box.z        = 0.;
  This->box.sx       = 1.;
  This->box.sy       = 1.;
  This->box.sz       = 1.;
  OContextInitialize (&(This->box.context));
  return             (ONode)This;
}
/***************************************************************************/
ONode OShapeBoxCreate (
 double a_x
,double a_y
,double a_z
,double a_sx
,double a_sy
,double a_sz
,OContext a_context
,char* a_name
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OShapeBox       This;
/*.........................................................................*/
  This            = (OShapeBox)OShapeBoxMake ();
  if(This==NULL)  return NULL;
  This->node.name = CStringDuplicate(a_name);
  This->box.x     = a_x;
  This->box.y     = a_y;
  This->box.z     = a_z;
  This->box.sx    = a_sx;
  This->box.sy    = a_sy;
  This->box.sz    = a_sz;
  OContextCopy    (&(This->box.context),a_context);
  return          (ONode)This;
}
/***************************************************************************/
ONode OShapeBoxCreateF (
 double a_x
,double a_y
,double a_z
,double a_sx
,double a_sy
,double a_sz
,OContext a_context
,int   a_number
,char* a_format
,...
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  va_list       args;
  char*         string;
  OShapeBox     This;
/*.........................................................................*/
  va_start       (args,a_format);
  string         = CStringCreateV(a_number,a_format,args);
  va_end         (args);
  This           = (OShapeBox)OShapeBoxCreate (a_x,a_y,a_z,a_sx,a_sy,a_sz,a_context,string);
  CStringDelete  (string);
  return         (ONode)This;
}
/***************************************************************************/
static OPrimitive* RepresentBox (
 ONode This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OShapeBox   ThisBox;
  OPrimitive* prims = NULL;
  double      x,y,z;
  double      x_maximum,y_maximum,z_maximum;
  double      cx[8];
  double      cy[8];
  double      cz[8];
  OPointList  points;
/*.........................................................................*/
  ThisBox        = (OShapeBox)This;

  points         = OPointListCreate(5);
  prims          = (OPrimitive*)CListCreate (6);

  x              = ThisBox->box.x;
  y              = ThisBox->box.y;
  z              = ThisBox->box.z;
  x_maximum      = x + ThisBox->box.sx;
  y_maximum      = y + ThisBox->box.sy;
  z_maximum      = z + ThisBox->box.sz;

  cx[0] = x;
  cy[0] = y;
  cz[0] = z;
  cx[1] = x_maximum;
  cy[1] = y;
  cz[1] = z;
  cx[2] = x;
  cy[2] = y_maximum;
  cz[2] = z;
  cx[3] = x_maximum;
  cy[3] = y_maximum;
  cz[3] = z;
  cx[4] = x;
  cy[4] = y;
  cz[4] = z_maximum;
  cx[5] = x_maximum;
  cy[5] = y;
  cz[5] = z_maximum;
  cx[6] = x;
  cy[6] = y_maximum;
  cz[6] = z_maximum;
  cx[7] = x_maximum;
  cy[7] = y_maximum;
  cz[7] = z_maximum;

/*z back*/
  OPointListSetIthEntry   (points,0,cx[0],cy[0],cz[0]);
  OPointListSetIthEntry   (points,1,cx[2],cy[2],cz[2]);
  OPointListSetIthEntry   (points,2,cx[3],cy[3],cz[3]);
  OPointListSetIthEntry   (points,3,cx[1],cy[1],cz[1]);
  OPointListSetIthEntry   (points,4,cx[0],cy[0],cz[0]);
  if (OContextGetModeling(&(ThisBox->box.context))==OModelingSolid) 
    prims[0] = OPrimitiveCreate (OPrimitivePolygon,
				 &(ThisBox->box.context),
				 points->number,
				 points->xs,points->ys,points->zs);
  else 
    prims[0] = OPrimitiveCreate (OPrimitiveLines,
				 &(ThisBox->box.context),
				 points->number,
				 points->xs,points->ys,points->zs);

/*y front*/
  OPointListSetIthEntry   (points,0,cx[2],cy[2],cz[2]);
  OPointListSetIthEntry   (points,1,cx[6],cy[6],cz[6]);
  OPointListSetIthEntry   (points,2,cx[7],cy[7],cz[7]);
  OPointListSetIthEntry   (points,3,cx[3],cy[3],cz[3]);
  OPointListSetIthEntry   (points,4,cx[2],cy[2],cz[2]);
  if (OContextGetModeling(&(ThisBox->box.context))==OModelingSolid) 
    prims[1] = OPrimitiveCreate (OPrimitivePolygon,
				 &(ThisBox->box.context),
				 points->number,
				 points->xs,points->ys,points->zs);
  else 
    prims[1] = OPrimitiveCreate (OPrimitiveLines,
				 &(ThisBox->box.context),
				 points->number,
				 points->xs,points->ys,points->zs);

/*x front*/
  OPointListSetIthEntry   (points,0,cx[1],cy[1],cz[1]);
  OPointListSetIthEntry   (points,1,cx[3],cy[3],cz[3]);
  OPointListSetIthEntry   (points,2,cx[7],cy[7],cz[7]);
  OPointListSetIthEntry   (points,3,cx[5],cy[5],cz[5]);
  OPointListSetIthEntry   (points,4,cx[1],cy[1],cz[1]);
  if (OContextGetModeling(&(ThisBox->box.context))==OModelingSolid) 
    prims[2] = OPrimitiveCreate (OPrimitivePolygon,
				 &(ThisBox->box.context),
				 points->number,
				 points->xs,points->ys,points->zs);
  else 
    prims[2] = OPrimitiveCreate (OPrimitiveLines,
				 &(ThisBox->box.context),
				 points->number,
				 points->xs,points->ys,points->zs);

/*y back*/
  OPointListSetIthEntry   (points,0,cx[0],cy[0],cz[0]);
  OPointListSetIthEntry   (points,1,cx[1],cy[1],cz[1]);
  OPointListSetIthEntry   (points,2,cx[5],cy[5],cz[5]);
  OPointListSetIthEntry   (points,3,cx[4],cy[4],cz[4]);
  OPointListSetIthEntry   (points,4,cx[0],cy[0],cz[0]);
  if (OContextGetModeling(&(ThisBox->box.context))==OModelingSolid) 
    prims[3] = OPrimitiveCreate (OPrimitivePolygon,
				 &(ThisBox->box.context),
				 points->number,
				 points->xs,points->ys,points->zs);
  else 
    prims[3] = OPrimitiveCreate (OPrimitiveLines,
				 &(ThisBox->box.context),
				 points->number,
				 points->xs,points->ys,points->zs);

/*x back*/
  OPointListSetIthEntry   (points,0,cx[0],cy[0],cz[0]);
  OPointListSetIthEntry   (points,1,cx[4],cy[4],cz[4]);
  OPointListSetIthEntry   (points,2,cx[6],cy[6],cz[6]);
  OPointListSetIthEntry   (points,3,cx[2],cy[2],cz[2]);
  OPointListSetIthEntry   (points,4,cx[0],cy[0],cz[0]);
  if (OContextGetModeling(&(ThisBox->box.context))==OModelingSolid) 
    prims[4] = OPrimitiveCreate (OPrimitivePolygon,
				 &(ThisBox->box.context),
				 points->number,
				 points->xs,points->ys,points->zs);
  else 
    prims[4] = OPrimitiveCreate (OPrimitiveLines,
				 &(ThisBox->box.context),
				 points->number,
				 points->xs,points->ys,points->zs);

/*z front*/
  OPointListSetIthEntry   (points,0,cx[4],cy[4],cz[4]);
  OPointListSetIthEntry   (points,1,cx[5],cy[5],cz[5]);
  OPointListSetIthEntry   (points,2,cx[7],cy[7],cz[7]);
  OPointListSetIthEntry   (points,3,cx[6],cy[6],cz[6]);
  OPointListSetIthEntry   (points,4,cx[4],cy[4],cz[4]);
  if (OContextGetModeling(&(ThisBox->box.context))==OModelingSolid) 
    prims[5] = OPrimitiveCreate (OPrimitivePolygon,
				 &(ThisBox->box.context),
				 points->number,
				 points->xs,points->ys,points->zs);
  else 
    prims[5] = OPrimitiveCreate (OPrimitiveLines,
				 &(ThisBox->box.context),
				 points->number,
				 points->xs,points->ys,points->zs);

  OPointListDelete   (points);
  return             prims;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
ONode OShapeStringMake (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OShapeString        This;
/*.........................................................................*/
#ifdef DEBUG
  printf             ("debug : OShapeStringMake.\n");
#endif
  This                = (OShapeString)CMemoryAllocateBlock(sizeof(OShapeStringRecord));
  if(This==NULL)      return NULL;
  ONodeInitialize     ((ONode)This);
  This->node.isShape                = 1;
  This->node.representShapeFunction = RepresentString;
  This->node.deleteShapeFunction    = DeleteString;
  /*Shape.*/
  This->string.string = NULL;
  This->string.x      = 0.;
  This->string.y      = 0.;
  This->string.z      = 0.;
  This->string.Xx     = 1.;
  This->string.Xy     = 0.;
  This->string.Xz     = 0.;
  This->string.Yx     = 0.;
  This->string.Yy     = 1.;
  This->string.Yz     = 0.;
  This->string.size   = 1.;
  This->string.xalign = 'C';
  This->string.yalign = 'B';
  OContextInitialize  (&(This->string.context));
  return              (ONode)This;
}
/***************************************************************************/
ONode OShapeStringCreate (
 char*    a_string  
,double   a_x
,double   a_y
,double   a_z
,double   a_Xx
,double   a_Xy
,double   a_Xz
,double   a_Yx
,double   a_Yy
,double   a_Yz
,double   a_size    
,char     a_xalign  
,char     a_yalign  
,OContext a_context
,char*    a_name
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OShapeString  This;
/*.........................................................................*/
  This                = (OShapeString)OShapeStringMake ();
  if(This==NULL)      return NULL;
  This->node.name     = CStringDuplicate(a_name);
  This->string.string = CStringDuplicate(a_string);
  This->string.x      = a_x;
  This->string.y      = a_y;
  This->string.z      = a_z;
  This->string.Xx     = a_Xx;
  This->string.Xy     = a_Xy;
  This->string.Xz     = a_Xz;
  This->string.Yx     = a_Yx;
  This->string.Yy     = a_Yy;
  This->string.Yz     = a_Yz;
  This->string.size   = a_size;
  This->string.xalign = a_xalign;
  This->string.yalign = a_yalign;
  OContextCopy        (&(This->string.context),a_context);
  return              (ONode)This;
}
/***************************************************************************/
ONode OShapeStringCreateF (
 char*    a_string  
,double   a_x
,double   a_y
,double   a_z
,double   a_Xx
,double   a_Xy
,double   a_Xz
,double   a_Yx
,double   a_Yy
,double   a_Yz
,double   a_size    
,char     a_xalign  
,char     a_yalign  
,OContext a_context
,int      a_number
,char*    a_format
,...
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  va_list        args;
  char*          string;
  OShapeString   This;
/*.........................................................................*/
  va_start       (args,a_format);
  string         = CStringCreateV(a_number,a_format,args);
  va_end         (args);
  This           = (OShapeString)OShapeStringCreate (
                    a_string,
		    a_x,a_y,a_z,
		    a_Xx,a_Xy,a_Xz,
		    a_Yx,a_Yy,a_Yz,
		    a_size,
		    a_xalign,a_yalign,
		    a_context,
		    string);
  CStringDelete  (string);
  return         (ONode)This;
}
/***************************************************************************/
ONode OShapeAlignedStringCreateF (
 char*    a_string  
,double   a_x
,double   a_y
,double   a_z
,double   a_Xx
,double   a_Xy
,double   a_Xz
,double   a_Yx
,double   a_Yy
,double   a_Yz
,double   a_size    
,char     a_xalign  
,char     a_yalign  
,OContext a_context
,int      a_number
,char*    a_format
,...
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  va_list        args;
  char*          string;
  OShapeString   This;
/*.........................................................................*/
  va_start       (args,a_format);
  string         = CStringCreateV(a_number,a_format,args);
  va_end         (args);
  This           = (OShapeString)OShapeStringCreate (
                    a_string,
		    a_x,a_y,a_z,
		    a_Xx,a_Xy,a_Xz,
		    a_Yx,a_Yy,a_Yz,
		    a_size,
		    a_xalign,a_yalign,
		    a_context,
		    string);
  CStringDelete         (string);
  ONodeSetCameraAligned ((ONode)This,1);
  ONodeSetHookPoint     ((ONode)This,a_x,a_y,a_z);
  return                (ONode)This;
}
/***************************************************************************/
static void DeleteString (
 ONode This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OShapeString              ThisString;
/*.........................................................................*/
  ThisString                = (OShapeString)This;
  CStringDelete             (ThisString->string.string);
  ThisString->string.string = NULL;
}
/***************************************************************************/
static OPrimitive* RepresentString (
 ONode This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OShapeString ThisString;
  OPrimitive*  prims = NULL;
  OMatrix      matrix,trans,scale,P;
  double       Xx,Xy,Xz;
  double       Yx,Yy,Yz;
  double       Zx,Zy,Zz;
/*.........................................................................*/
  ThisString                = (OShapeString)This;

  if(ThisString->string.string==NULL)    return NULL;
  if(*(ThisString->string.string)=='\0') return NULL;

  Xx                        = ThisString->string.Xx;
  Xy                        = ThisString->string.Xy;
  Xz                        = ThisString->string.Xz;
  Yx                        = ThisString->string.Yx;
  Yy                        = ThisString->string.Yy;
  Yz                        = ThisString->string.Yz;
  CVectorNormalize          (&Xx,&Xy,&Xz);
  CVectorNormalize          (&Yx,&Yy,&Yz);
  CVectorMultiply           (Xx,Xy,Xz,Yx,Yy,Yz,&Zx,&Zy,&Zz);
  CVectorMultiply           (Zx,Zy,Zz,Xx,Xy,Xz,&Yx,&Yy,&Yz);
/* now X,Y,Z normalized frame */
  P                         = OMatrixCreate(OMatrixFollowing,
					    Xx,Yx,Zx,0.,
					    Xy,Yy,Zy,0.,
					    Xz,Yz,Zz,0.,
					    0.,0.,0.,1.);

  trans                     = OMatrixCreate (OMatrixStringAlign,
					     ThisString->string.string,
					     (int)(ThisString->string.xalign),
					     (int)(ThisString->string.yalign));
  scale                     = OMatrixCreate (OMatrixScale,
					     .8 * ThisString->string.size,
					     ThisString->string.size,
					     1.);
  matrix                    = OMatrixMultiplyMatrices  (3,P,scale,trans);
  OMatrixDelete             (trans);
  OMatrixDelete             (scale);
  OMatrixDelete             (P);

  prims                     = RepresentStringWithMatrix (
			        ThisString->string.string,
				ThisString->string.x,
				ThisString->string.y,
				ThisString->string.z,
				matrix,
				&(ThisString->string.context));
  OMatrixDelete             (matrix);

  return                    prims;
}
/***************************************************************************/
static OPrimitive* RepresentStringWithMatrix (
 char* a_string 
,double a_x
,double a_y
,double a_z
,OMatrix a_matrix 
,OContext a_context
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OPrimitive*  prims = NULL;
  OMatrix      matrix,translate;
  char*        itema;
  double       length,width = 0.;
/*.........................................................................*/
  if( (a_string==NULL) || (*a_string=='\0') ) return NULL;
  translate     = OMatrixCreate(OMatrixTranslation,a_x,a_y,a_z);
  matrix        = OMatrixCreate(OMatrixIdentity);
  length        = 0.;
  for(itema=a_string;*itema!='\0';itema++)
    { 
      OMatrix                          shift;
      shift                            = OMatrixCreate(OMatrixTranslation,length,0.,0.);
      OMatrixMultiplyMatricesAndAssign (matrix,3,translate,a_matrix,shift);
      AddCharacter                     (&prims,matrix,*itema,&width,a_context);
      OMatrixDelete                    (shift); 
      length                         += width + 0.2;
    }
  OMatrixDelete (matrix);
  OMatrixDelete (translate);
  return        prims;
}
/***************************************************************************/
static void AddCharacter (
 OPrimitive** a_prims
,OMatrix a_matrix 
,char a_char 
,double* a_width
,OContext a_context
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
#define HERSHEY__MAX_POLY   4
#define HERSHEY__MAX_POINT  160 /* 40 * 4 */
  int    max_point[HERSHEY__MAX_POLY];
  double xp[HERSHEY__MAX_POINT],yp[HERSHEY__MAX_POINT];
  int    ipoly;
  int    number,ipoint;
/*.........................................................................*/
  if (OContextGetFont(a_context)==OFontGreek)
    CCharacterGetGreekFontPoints  (a_char,&number,max_point,xp,yp,a_width);
  else
    CCharacterGetASCII_FontPoints (a_char,&number,max_point,xp,yp,a_width);
  ipoint = 0;
  for (ipoly=0;ipoly<number;ipoly++)
    { 
      int    pointn;
      pointn = max_point[ipoly];
      if(pointn>0) 
	{
	  int        count;
	  OPointList points;
	  OPrimitive prim;
	  points     = OPointListCreate (pointn);
	  for(count=0;count<pointn;count++)
	    {
	      double                x,y,z;
	      OMatrixMultiplyVector (a_matrix,xp[ipoint],yp[ipoint],0.,&x,&y,&z);
	      ipoint                ++;
	      OPointListSetIthEntry (points,count,x,y,z);
	    }
	  prim              = OPrimitiveCreate (OPrimitiveLines,
						a_context,
						points->number,
						points->xs,points->ys,points->zs);
	  CListAddEntry     ((CList*)a_prims,prim);
	  OPointListDelete  (points);
	}
    }
}
