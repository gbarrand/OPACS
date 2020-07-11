/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#include <stddef.h>

#include <CString.h>
#include <CPrinter.h>
#include <CError.h>

#include <OCamera.h>
#include <Go.h>

#include <GoF77.h>

#ifdef __cplusplus
extern "C"{
#endif
static void StorePoints (int,float*,float*,float*);
static void ReportError (char*,int);
#ifdef __cplusplus
}
#endif

static struct 
{
  ONode       onode;
  OPointList  points;
  OMatrix     matrix;
} Class = {NULL,NULL,NULL};
/***************************************************************************/
void GoF77_ClearClass (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  OPointListDelete (Class.points);
  OMatrixDelete    (Class.matrix);
  Class.points     = NULL;
  Class.matrix     = NULL;
  Class.onode    = NULL;
}
/***************************************************************************/
void GoF77_SetNode (
 ONode This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  Class.onode = This;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
void goaraz (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  OContextSetF(OContextGetStaticInstance(),"r");
}
/***************************************************************************/
void goaset (
 int* color
,int* sline
,int* smark
,int* sarea
,int* width
,int* font
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  OContextSetF (OContextGetStaticInstance(),"clmaw",*color,*sline,*smark,*sarea,*width);
  font     = 0;
}
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
void gocset (
 FString a_name
,int* a_lname
)
/******************************************************************************/
/* set current camera given it's name                                         */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*             cname;
/*.........................................................................*/
  cname             = FStringConvertToC    (a_name,*a_lname);
  OCameraSetCurrent (OCameraGetIdentifier(cname));
  CStringDelete     (cname);
}
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
void goocre (
 FString a_name
,int* a_lname
)
/******************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*         cname;
/*.........................................................................*/
  cname         = FStringConvertToC      (a_name,*a_lname);
  Class.onode = ONodeCreate (cname);
  CStringDelete (cname);
}
/******************************************************************************/
void goview (
)
/******************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  OCameraViewNode (OCameraGetCurrent(),Class.onode);
  Class.onode   = NULL;
}
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
void gopgon (
 int*   a_npt
,float* a_xs
,float* a_ys
,float* a_zs
)
/******************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  StorePoints  (*a_npt,a_xs,a_ys,a_zs);
  GoAddPolygonToNode   (Class.onode,Class.points);
}
/******************************************************************************/
void goplin (
 int*   a_npt
,float* a_xs
,float* a_ys
,float* a_zs
)
/******************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  StorePoints      (*a_npt,a_xs,a_ys,a_zs);
  GoAddLinesToNode (Class.onode,Class.points);
}
/******************************************************************************/
void gopmar (
 int*   a_npt
,float* a_xs
,float* a_ys
,float* a_zs
)
/******************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  StorePoints        (*a_npt,a_xs,a_ys,a_zs);
  GoAddMarkersToNode (Class.onode,Class.points);
}
/******************************************************************************/
void gopseg (
 int*   a_npt
,float* a_xs
,float* a_ys
,float* a_zs
)
/******************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  StorePoints    (*a_npt,a_xs,a_ys,a_zs);
  GoAddSegmentsToNode (Class.onode,Class.points);
}
/******************************************************************************/
void goptxt (
 FString a_string
,int* a_lstring
,float* a_x
,float* a_y
,float* a_z
,float* a_width
,float* a_height
)
/******************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*         cstring;
/*.........................................................................*/
  cstring              = FStringConvertToC   (a_string,*a_lstring);
  GoAddTextFieldToNode (Class.onode,cstring,(double)(*a_x),(double)(*a_y),(double)(*a_z),(double)(*a_width),(double)(*a_height));
  CStringDelete        (cstring);
}
/******************************************************************************/
static void StorePoints (
 int    a_npt
,float* a_xs
,float* a_ys
,float* a_zs
)
/******************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int   count;
/*.........................................................................*/
  if( (Class.points==NULL) || (a_npt>OPointListGetSize(Class.points)))
    {
      OPointListDelete (Class.points);
      Class.points = OPointListCreate(a_npt);
    }
  OPointListLimit (Class.points,a_npt);
  for(count=0;count<a_npt;count++) 
    {
      OPointListSetIthEntry(Class.points,count,(double)(a_xs[count]),(double)(a_ys[count]),(double)(a_zs[count]));
    }
}
/***************************************************************************/
/***************************************************************************/
/*********** for GEANT/OCamera interface (see also ogt.f) ******************/
/***************************************************************************/
/***************************************************************************/
void gogshp (
 int* a_shape
,float* a_pars
,float* a_rot
,float* a_trans
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{ 
/*.........................................................................*/
  if(Class.matrix==NULL) Class.matrix = OMatrixCreate(OMatrixIdentity);

  if(a_rot[9]!=0.0) 
    {
      OMatrixInitialize (Class.matrix,OMatrixFollowing,
        (double)a_rot[0],(double)a_rot[3],(double)a_rot[6],(double)a_trans[0],
        (double)a_rot[1],(double)a_rot[4],(double)a_rot[7],(double)a_trans[1],
        (double)a_rot[2],(double)a_rot[5],(double)a_rot[8],(double)a_trans[2],
                      0.,              0.,              0.,                1.);
    }
  else
    {
      OMatrixInitialize (Class.matrix,OMatrixTranslation,
        (double)a_trans[0],(double)a_trans[1],(double)a_trans[2]);
    }
  switch(*a_shape)
    {
      case 1:
             {double        dx,dy,dz;
              dx            = a_pars[0];
              dy            = a_pars[1];
              dz            = a_pars[2];
              GoAddGEANT_TRD2_ToNode (Class.onode,Class.matrix,dx,dy,dx,dy,dz);
              }break;
      case 2:
             {double        dxd,dxu,dy,dz;
              dxd           = a_pars[0];
              dxu           = a_pars[1];
              dy            = a_pars[2];
              dz            = a_pars[3];
	      GoAddGEANT_TRD2_ToNode (Class.onode,Class.matrix,
                             dxd,dy,dxu,dy,dz);
              }break;
      case 3:
             {double        dxd,dxu,dyd,dyu,dz;
              dxd           = a_pars[0];
              dxu           = a_pars[1];
              dyd           = a_pars[2];
              dyu           = a_pars[3];
              dz            = a_pars[4];
	      GoAddGEANT_TRD2_ToNode (Class.onode,Class.matrix,
                             dxd,dyd,dxu,dyu,dz);
	     }break; 
      case 4:
             {
	       double DZ,TX,TY,H1,BL1,TL1,TTH1,H2,BL2,TL2,TTH2;
	       DZ     = a_pars[0];
	       TX     = a_pars[1];
	       TY     = a_pars[2];
	       H1     = a_pars[3];
	       BL1    = a_pars[4];
	       TL1    = a_pars[5];
	       TTH1   = a_pars[6];
	       H2     = a_pars[7];
	       BL2    = a_pars[8];
	       TL2    = a_pars[9];
	       TTH2   = a_pars[10];
	       GoAddGEANT_TRAP_ToNode (Class.onode,Class.matrix,
                             DZ,TX,TY,H1,BL1,TL1,TTH1,H2,BL2,TL2,TTH2);
              }break;
                         
      case 5:
             {double        rmin,rmax,dz;
              rmin          = a_pars[0];
              rmax          = a_pars[1];
              dz            = a_pars[2];
              GoAddGEANT_TUBE_ToNode (Class.onode,Class.matrix,rmin,rmax,dz);
              }break;
      case 6:
             {double        rmin,rmax,dz,phmin,phmax;
              rmin          = a_pars[0];
              rmax          = a_pars[1];
              dz            = a_pars[2];
              phmin         = a_pars[3];
              phmax         = a_pars[4];
              GoAddGEANT_TUBS_ToNode (Class.onode,Class.matrix,
                             rmin,rmax,dz,phmin,phmax);
              }break;
      case 7:
             {double        dz,rmin1,rmax1,rmin2,rmax2;
              dz            = a_pars[0];
              rmin1         = a_pars[1];
              rmax1         = a_pars[2];
              rmin2         = a_pars[3];
              rmax2         = a_pars[4];
              GoAddGEANT_CONE_ToNode (Class.onode,Class.matrix,
                             dz,rmin1,rmax1,rmin2,rmax2);
              }break;
      case 8:
             {double        dz,rmin1,rmax1,rmin2,rmax2,phmin,phmax;
              dz            = a_pars[0];
              rmin1         = a_pars[1];
              rmax1         = a_pars[2];
              rmin2         = a_pars[3];
              rmax2         = a_pars[4];
              phmin         = a_pars[5];
              phmax         = a_pars[6];
              GoAddGEANT_CONS_ToNode (Class.onode,Class.matrix,dz,rmin1,rmax1,rmin2,rmax2,phmin,phmax);
              }break;

/* SPHE
      case 9:
              break;
*/
      case 10:
/*
* 'PARA' is a parallelpiped.   It has 6 parameters,  the half length   *
*        in x, the half length in y, the half length in z, the angle   *
*        w.r.t. the y axis from the centre  of the low y edge to the   *
*        centre of the  high y edge, and the theta  phi polar angles   *
*        from the centre of the low z face to the centre of the high   *
*        z face.                                                       *
*
*  10       PARA     DX,DY,DZ,TXY,TXZ,TYZ                              *
*/
	      {
               double DX,DY,TXY,TXZ,TYZ;
	       double DZ,TX,TY,H1,BL1,TL1,TTH1,H2,BL2,TL2,TTH2;
	       DX   = a_pars[0];
	       DY   = a_pars[1];
	       DZ   = a_pars[2];
	       TXY  = a_pars[3];
	       TXZ  = a_pars[4];
	       TYZ  = a_pars[5];
	       TX   = TXZ;
	       TY   = TYZ;
	       H1   = DY;
	       BL1  = DX;
	       TL1  = DX;
	       TTH1 = TXY;
	       H2   = DY;
	       BL2  = DX;
	       TL2  = DX;
	       TTH2 = TXY;
	       GoAddGEANT_TRAP_ToNode (Class.onode,Class.matrix,DZ,TX,TY,H1,BL1,TL1,TTH1,H2,BL2,TL2,TTH2);
              }break;

      case 11:
              GoAddGEANT_PGON_ToNode (Class.onode,Class.matrix, a_pars);
              break;

      case 12:
              GoAddGEANT_PCON_ToNode (Class.onode,Class.matrix, a_pars);
              break;

/* ELTU
      case 13:
              break;
*/

      case 14:
	     {double RMIN,RMAX,DZ,PHI;
              RMIN = a_pars[0]; 
              RMAX = a_pars[1]; 
              DZ   = a_pars[2]; 
              PHI  = a_pars[3]; 
              GoAddGEANT_HYPE_ToNode (Class.onode,Class.matrix,RMIN,RMAX,DZ,PHI);
              }break;

      default:CPrinterPutF ("oxgshp:%d is a bad shape type.\n",*a_shape);
              break;
    }
}
/******************************************************************************/
void goglin (
 int*   a_npt
,float* a_coords
)
/******************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int    count;
  float* coords;
/*.........................................................................*/
  if( (Class.points==NULL) || (*a_npt>OPointListGetSize(Class.points)))
    {
      OPointListDelete (Class.points);
      Class.points = OPointListCreate(*a_npt);
    }
  OPointListLimit (Class.points,*a_npt);
  coords          = a_coords;
  for(count=0;count<*a_npt;count++) 
    {
      float x,y,z;
      x    = *coords;coords++;
      y    = *coords;coords++;
      z    = *coords;coords++;
      OPointListSetIthEntry(Class.points,count,(double)x,(double)y,(double)z);
    }
  GoAddLinesToNode    (Class.onode,Class.points);
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
  CErrorHandle ("Go","GoF77",a_name,a_code);
}
