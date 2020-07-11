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
#include <math.h>
#ifndef M_PI
#define M_PI       3.1415926535897931160E0
#define M_PI_2     1.5707963267948965580E0  
#endif

#include <CPrinter.h>
#include <CError.h>
#include <CMemory.h>
#include <CVector.h>

#include <OMatrixP.h>

#ifdef __cplusplus
extern "C"{
#endif
static int   TestSign                   (double,double,double,double,double,double);
static void  InitializeV                (OMatrix,int,va_list);
static void  MultiplyMatricesAndAssignV (OMatrix,int,va_list);
static void  ReportError                (char*,int);
#ifdef __cplusplus
}
#endif

typedef unsigned long Ulong;

#define MAXIMUM(a,b)           ((a)>(b)?a:b)
#define olutl_d33(a_11,a_12,a_13,a_21,a_22,a_23,a_31,a_32,a_33)\
 (a_11 * (a_22 * a_33 - a_32 * a_23)- a_21 * (a_12 * a_33 - a_32 * a_13) + a_31 * (a_12 * a_23 - a_22 * a_13))

static struct {
  double doubles[16];
} Class = {OMatrix_identity};
/***************************************************************************/
/***************************************************************************/
void OMatrixGetDirection (
 double a_ux
,double a_uy
,double a_uz
,double a_vx
,double a_vy
,double a_vz
,double a_ax
,double a_ay
,double a_az
,double a_bx
,double a_by
,double a_bz
,double* a_dx
,double* a_dy
,double* a_dz
,double* a_angle
)
/***************************************************************************/
/*
  Bring (u,v,w) over (a,b,c).

  A rotation is :
   M = exp(angle N) = I + (1-cos(angle)) N*N + sin(angle) N
 where:
       |  0  z -y |
   N = | -z  0  x |     x*x + y*y + z*z = 1;  N is antisymetric.
       |  y -x  0 |

         | -(z*z+y*y)         xy          xz|   
   N*N = |        xy  -(z*z+x*x)          yz|  is symetric.
         |        xz          yz  -(x*x+y*y)| 

   We have :
    trace(N*N)   = -2  
    sin(angle) N = (M - t(M))/2
 where t(M) is the transpose of M, and : 
    cos(angle)   = (trace(M) - trace(I) + 2 )/2 = (trace(M)-1)/2
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  double  ux,uy,uz;
  double  vx,vy,vz;
  double  wx,wy,wz;
  double  ax,ay,az;
  double  bx,by,bz;
  double  cx,cy,cz;
  OMatrix r1,r2,matrix;
  double  trace,cangle;
/*.........................................................................*/
  ux = a_ux;
  uy = a_uy;
  uz = a_uz;
  vx = a_vx;
  vy = a_vy;
  vz = a_vz;
  ax = a_ax;
  ay = a_ay;
  az = a_az;
  bx = a_bx;
  by = a_by;
  bz = a_bz;
  CVectorNormalize (&ux,&uy,&uz);
  CVectorNormalize (&vx,&vy,&vz);
  CVectorMultiply  (ux,uy,uz,vx,vy,vz,&wx,&wy,&wz);
  CVectorNormalize (&ax,&ay,&az);
  CVectorNormalize (&bx,&by,&bz);
  CVectorMultiply  (ax,ay,az,bx,by,bz,&cx,&cy,&cz);
#ifdef DEBUG
  printf ("debug : u : %g %g %g\n",ux,uy,uz);
  printf ("debug : v : %g %g %g\n",vx,vy,vz);
  printf ("debug : w : %g %g %g\n",wx,wy,wz);
  printf ("debug : \n");
  printf ("debug : a : %g %g %g\n",ax,ay,az);
  printf ("debug : b : %g %g %g\n",bx,by,bz);
  printf ("debug : c : %g %g %g\n",cx,cy,cz);
#endif  
/*Bring (u,v,w) over (a,b,c).*/
/*r1 : bring (u,v,w) over (x,y,z).*/
/*r2 : bring (x,y,z) over (a,b,c).*/
  r1 = OMatrixCreate(OMatrixFollowing,
				   ux,uy,uz,0.,
				   vx,vy,vz,0.,
				   wx,wy,wz,0.,
				   0.,0.,0.,1.); 
  r2 = OMatrixCreate(OMatrixFollowing,
				   ax,bx,cx,0.,
				   ay,by,cy,0.,
				   az,bz,cz,0.,
				   0.,0.,0.,1.);
  matrix      = OMatrixMultiply (r2,r1);
  trace       = OMatrixGetTrace (matrix) - 1.;
  cangle      = (trace - 1.) * 0.5;
#ifdef DEBUG
  printf      ("debug : cangle : %g \n",cangle);
#endif  
  if(cangle==1.)
    {
      *a_dx    = 0.;
      *a_dy    = 0.;
      *a_dz    = 1.;
      *a_angle = 0.;  /*Then axis has no importance.*/
    }
  else if(cangle==-1.)  /*More tricky.*/
    {
      OMatrix       s,i,m1,nn;
      double        x2,y2,z2,xy,xz,yz,dx,dy,dz;
      s             = OMatrixGetSymetricPart    (matrix);
      i             = OMatrixCreate             (OMatrixIdentity);
      m1            = OMatrixSubstract          (s,i);
      nn            = OMatrixMultiplyByConstant (m1,0.5);
      x2            = OMatrixGetElement00 (nn) + 1.;
      y2            = OMatrixGetElement11 (nn) + 1.;
      z2            = OMatrixGetElement22 (nn) + 1.;
      xy            = OMatrixGetElement01 (nn);
      xz            = OMatrixGetElement02 (nn);
      yz            = OMatrixGetElement12 (nn);
#ifdef DEBUG
      printf        ("debug : x2 %g y2 %g z2 %g\n",x2,y2,z2);
      printf        ("debug : xy %g xz %g yz %g\n",xy,xz,yz);
#endif
      OMatrixDelete (m1);
      OMatrixDelete (nn);
      OMatrixDelete (i);
      OMatrixDelete (s);
      dx = sqrt(x2);  /*Good sign ?*/
      dy = sqrt(y2);  /*Good sign ?*/
      dz = sqrt(z2);  /*Good sign ?*/
      *a_angle = M_PI;
           if(TestSign( dx, dy, dz,xy,xz,yz)==1) { *a_dx = dx;*a_dy = dy;*a_dz = dz; }
      else if(TestSign(-dx, dy, dz,xy,xz,yz)==1) { *a_dx =-dx;*a_dy = dy;*a_dz = dz; }
      else if(TestSign( dx,-dy, dz,xy,xz,yz)==1) { *a_dx = dx;*a_dy =-dy;*a_dz = dz; }
      else if(TestSign( dx, dy,-dz,xy,xz,yz)==1) { *a_dx = dx;*a_dy = dy;*a_dz =-dz; }
      else if(TestSign(-dx,-dy, dz,xy,xz,yz)==1) { *a_dx =-dx;*a_dy =-dy;*a_dz = dz; }
      else if(TestSign(-dx, dy,-dz,xy,xz,yz)==1) { *a_dx =-dx;*a_dy = dy;*a_dz =-dz; }
      else if(TestSign( dx,-dy,-dz,xy,xz,yz)==1) { *a_dx = dx;*a_dy =-dy;*a_dz =-dz; }
      else if(TestSign(-dx,-dy,-dz,xy,xz,yz)==1) { *a_dx =-dx;*a_dy =-dy;*a_dz =-dz; }
      else
	{
#ifdef DEBUG
	  printf ("debug : strange case !\n");
#endif
	  *a_dx    = 0.;
	  *a_dy    = 0.;
	  *a_dz    = 1.;
	  *a_angle = 0.;  /*Then axis has no importance.*/
	}
    }
  else
    {
      double   sangle;
      OMatrix  n;
      sangle   = sqrt (1. - cangle * cangle);
      n        = OMatrixGetAntisymetricPart (matrix);
      *a_dx    =       OMatrixGetElement12 (n) / sangle;
      *a_dy    = -1. * OMatrixGetElement02 (n) / sangle;
      *a_dz    =       OMatrixGetElement01 (n) / sangle;
      *a_angle = atan2(-1. * sangle,cangle);
      OMatrixDelete (n);
    }
  OMatrixDelete (r1);
  OMatrixDelete (r2);
  OMatrixDelete (matrix);
}
/***************************************************************************/
static int TestSign (
 double a_x
,double a_y
,double a_z
,double a_xy
,double a_xz
,double a_yz
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
#define SIGN(a)  (a>=0.? 1:-1)
/*.........................................................................*/
#ifdef DEBUG
  printf ("debug : %g %g %g : %g %g %g\n",a_x,a_y,a_z,a_xy,a_xz,a_yz);
#endif
  if ( SIGN(a_x * a_y) != SIGN(a_xy)) return 0;
  if ( SIGN(a_z * a_x) != SIGN(a_xz)) return 0;
  if ( SIGN(a_y * a_z) != SIGN(a_yz)) return 0;
  return 1;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
OMatrix OMatrixMake (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OMatrix     This;
/*.........................................................................*/
  This        = (OMatrixRecord*)CMemoryAllocateBlock(sizeof(OMatrixRecord));
  if(This==NULL)   return NULL;
  OMatrixInitialize (This,OMatrixIdentity);
  return      This;
}
/***************************************************************************/
OMatrix OMatrixCreate (
 int a_type   
,...
)
/***************************************************************************/
/*
 Description :
   Creates a 4x4 matrix according to a given type and parameters.
 Arguments :
  - a_type : type of matrix to create. 
     OMatrixIdentity : identity matrix,
     OMatrixScale : xyz scale matrix, 3 doubles must follow,
     OMatrixScaleIsometric : uniform scale matrix, 1 double must follow,
     OMatrixScaleTranslation : scale and translation, 6 doubles must follow,
     OMatrixTranslationScale : translation and scale, 6 doubles must follow,
     OMatrixRotationX : x rotation , 1 double (angle in radian) must follow,
     OMatrixRotationY : y rotation , 1 double (angle in radian) must follow,
     OMatrixRotationZ : z rotation , 1 double (angle in radian) must follow,
     OMatrixRotationAxis : rotation around an axis, 4 doubles 
       (radian angle + axis vector) must follow,
     OMatrixTranslation : xyz translation, 3 doubles must follow,
     OMatrixZToVector : bring z axis to a vector , 3 doubles must follow,
     OMatrixVectorToZ : bring a vector to z axis, 3 doubles must follow,
     OMatrixFollowing : create a matrix given its content, 16 doubles must follow :
                        00,01,02,03, 10,11,12,13, 20,21,22,23, 30,31,32,33.
  - ... : according to the type of matrix we want to create a 
          varying list of argument is expected.
 Example : create a translation matrix :
...
#include <OMatrix.h>
...
 {OMatrix      omatrix;
  omatrix      = OMatrixCreate (OMatrixTranslation,0.,0.,100.);
...
  OMatrixDelete (omatrix);
  } 
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OMatrix  This;
  va_list  args;
/*.........................................................................*/
  This               = OMatrixMake();
  if(This==NULL)     return NULL;

  va_start           (args,a_type);
  InitializeV        (This,a_type,args);
  va_end             (args);

  return             This;
}
/***************************************************************************/
void OMatrixDelete (
 OMatrix This
)
/***************************************************************************/
/*
 Description :
   Deletes an OMatrix object. The object is removed from memory. 
  Further references to the object is then forbidden.
 Arguments :
  This : OMatrix concerned.
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  CMemoryFreeBlock  (This);
}
/***************************************************************************/
void OMatrixInitialize (
 OMatrix This 
,int a_type   
,...
)
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  va_list  args;
/*.........................................................................*/
  va_start  (args,a_type);
  InitializeV (This,a_type,args);
  va_end    (args);
}
/***************************************************************************/
static void InitializeV (
 OMatrix This 
,int  a_type   
,va_list  a_args  
)
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  switch(a_type)
    {
      case OMatrixFollowing:
        {
	  This->e00 = va_arg(a_args,double);
	  This->e01 = va_arg(a_args,double);
	  This->e02 = va_arg(a_args,double);
	  This->e03 = va_arg(a_args,double);
	  
	  This->e10 = va_arg(a_args,double);
	  This->e11 = va_arg(a_args,double);
	  This->e12 = va_arg(a_args,double);
	  This->e13 = va_arg(a_args,double);
	  
	  This->e20 = va_arg(a_args,double);
	  This->e21 = va_arg(a_args,double);
	  This->e22 = va_arg(a_args,double);
	  This->e23 = va_arg(a_args,double);
	  
	  This->e30 = va_arg(a_args,double);
	  This->e31 = va_arg(a_args,double);
	  This->e32 = va_arg(a_args,double);
	  This->e33 = va_arg(a_args,double);
	}
	break;
      case OMatrixIdentity:
	{
	  This->e00 = 1.       ;This->e01 =        0.;This->e02=        0.;This->e03 = 0.;
	  This->e10 = 0.       ;This->e11 =        1.;This->e12=        0.;This->e13 = 0.;
	  This->e20 = 0.       ;This->e21 =        0.;This->e22=        1.;This->e23 = 0.;
	  This->e30 = 0.       ;This->e31 =        0.;This->e32=        0.;This->e33 = 1.;
	}
	break;
      case OMatrixScale:
	{
	  double a_x,a_y,a_z;
	  a_x = va_arg(a_args,double);
	  a_y = va_arg(a_args,double);
	  a_z = va_arg(a_args,double);
	  This->e00 = a_x      ;This->e01 =        0.;This->e02=        0.;This->e03 = 0.;
	  This->e10 = 0.       ;This->e11 =       a_y;This->e12=        0.;This->e13 = 0.;
	  This->e20 = 0.       ;This->e21 =        0.;This->e22=       a_z;This->e23 = 0.;
	  This->e30 = 0.       ;This->e31 =        0.;This->e32=        0.;This->e33 = 1.;
	}
	break;
      case OMatrixScaleIsometric:
	{
	  double a_value;
	  a_value = va_arg(a_args,double);
	  OMatrixInitialize (This,OMatrixScale,a_value,a_value,a_value);
	}
	break;
      case OMatrixScaleTranslation:
	{
	  double a_sx,a_sy,a_sz,a_tx,a_ty,a_tz;
	  a_sx = va_arg(a_args,double);
	  a_sy = va_arg(a_args,double);
	  a_sz = va_arg(a_args,double);
	  a_tx = va_arg(a_args,double);
	  a_ty = va_arg(a_args,double);
	  a_tz = va_arg(a_args,double);
	  This->e00 = a_sx     ;This->e01 =        0.;This->e02=        0.;This->e03 = a_tx;
	  This->e10 = 0.       ;This->e11 =      a_sy;This->e12=        0.;This->e13 = a_ty;
	  This->e20 = 0.       ;This->e21 =        0.;This->e22=      a_sz;This->e23 = a_tz;
	  This->e30 = 0.       ;This->e31 =        0.;This->e32=        0.;This->e33 = 1.;
	}
	break;
      case OMatrixTranslationScale:
	{
	  double a_sx,a_sy,a_sz,a_tx,a_ty,a_tz;
	  a_tx = va_arg(a_args,double);
	  a_ty = va_arg(a_args,double);
	  a_tz = va_arg(a_args,double);
	  a_sx = va_arg(a_args,double);
	  a_sy = va_arg(a_args,double);
	  a_sz = va_arg(a_args,double);
	  This->e00 = a_sx     ;This->e01 =        0.;This->e02=        0.;This->e03 = a_sx * a_tx;
	  This->e10 = 0.       ;This->e11 =      a_sy;This->e12=        0.;This->e13 = a_sy * a_ty;
	  This->e20 = 0.       ;This->e21 =        0.;This->e22=      a_sz;This->e23 = a_sz * a_tz;
	  This->e30 = 0.       ;This->e31 =        0.;This->e32=        0.;This->e33 = 1.;
	}
	break;
      case OMatrixRotationAxis:
	{
	  OMatrix      inter,identity,matrix_square;
	  double       alph,beta,a_angle;
	  double       a_x,a_y,a_z;
	  /*.........................................................................*/
	  a_angle      = va_arg(a_args,double);
	  a_x          = va_arg(a_args,double);
	  a_y          = va_arg(a_args,double);
	  a_z          = va_arg(a_args,double);

	  CVectorNormalize    (&a_x,&a_y,&a_z);

	  inter           = OMatrixCreate(OMatrixIdentity);
	  matrix_square   = OMatrixCreate(OMatrixIdentity);
	  identity        = OMatrixCreate(OMatrixIdentity);
	  inter->e00 =     0.;inter->e01 =    -a_z;inter->e02 =   a_y;inter->e03 = 0.;
	  inter->e10 =    a_z;inter->e11 =      0.;inter->e12 =  -a_x;inter->e13 = 0.;
	  inter->e20 =   -a_y;inter->e21 =     a_x;inter->e22 =    0.;inter->e23 = 0.;
	  inter->e30 =     0.;inter->e31 =      0.;inter->e32 =    0.;inter->e33 = 0.;
	  OMatrixMultiplyAndAssign (matrix_square,inter,inter);
	  alph = 1.- cos (a_angle);
	  beta  =     sin (a_angle);

	  This->e00 = identity->e00 + alph * matrix_square->e00 + beta * inter->e00;
	  This->e10 = identity->e10 + alph * matrix_square->e10 + beta * inter->e10;
	  This->e20 = identity->e20 + alph * matrix_square->e20 + beta * inter->e20;
	  This->e30 = identity->e30 + alph * matrix_square->e30 + beta * inter->e30;
	  
	  This->e01 = identity->e01 + alph * matrix_square->e01 + beta * inter->e01;
	  This->e11 = identity->e11 + alph * matrix_square->e11 + beta * inter->e11;
	  This->e21 = identity->e21 + alph * matrix_square->e21 + beta * inter->e21;
	  This->e31 = identity->e31 + alph * matrix_square->e31 + beta * inter->e31;
	  
	  This->e02 = identity->e02 + alph * matrix_square->e02 + beta * inter->e02;
	  This->e12 = identity->e12 + alph * matrix_square->e12 + beta * inter->e12;
	  This->e22 = identity->e22 + alph * matrix_square->e22 + beta * inter->e22;
	  This->e32 = identity->e32 + alph * matrix_square->e32 + beta * inter->e32;
	  
	  This->e03 = identity->e03 + alph * matrix_square->e03 + beta * inter->e03;
	  This->e13 = identity->e13 + alph * matrix_square->e13 + beta * inter->e13;
	  This->e23 = identity->e23 + alph * matrix_square->e23 + beta * inter->e23;
	  This->e33 = identity->e33 + alph * matrix_square->e33 + beta * inter->e33;

	  OMatrixDelete (inter);
	  OMatrixDelete (identity);
	  OMatrixDelete (matrix_square);
	}
	break;
      case OMatrixRotationX:
	{
	  double alph,beta,a_angle;
	  a_angle = va_arg(a_args,double);
	  alph    = cos (a_angle);
	  beta    = sin (a_angle);
	  This->e00 = 1.       ;This->e01 =        0.;This->e02=        0.;This->e03 = 0.;
	  This->e10 = 0.       ;This->e11 =      alph;This->e12=     -beta;This->e13 = 0.;
	  This->e20 = 0.       ;This->e21 =      beta;This->e22=      alph;This->e23 = 0.;
	  This->e30 = 0.       ;This->e31 =        0.;This->e32=        0.;This->e33 = 1.;
	}
	break;
      case OMatrixRotationY:
	{
	  double alph,beta,a_angle;
	  a_angle = va_arg(a_args,double);
	  alph    = cos (a_angle);
	  beta    = sin (a_angle);
	  This->e00 = alph     ;This->e01 =        0.;This->e02=      beta;This->e03 = 0.;
	  This->e10 = 0.       ;This->e11 =        1.;This->e12=        0.;This->e13 = 0.;
	  This->e20 = -beta    ;This->e21 =        0.;This->e22=      alph;This->e23 = 0.;
	  This->e30 = 0.       ;This->e31 =        0.;This->e32=        0.;This->e33 = 1.;
	}
	break;
      case OMatrixRotationZ:
	{
	  double alph,beta,a_angle;
	  a_angle = va_arg(a_args,double);
	  alph    = cos (a_angle);
	  beta    = sin (a_angle);
	  This->e00 = alph    ;This->e01 =     -beta;This->e02=        0.;This->e03 = 0.;
	  This->e10 =  beta    ;This->e11 =     alph;This->e12=        0.;This->e13 = 0.;
	  This->e20 = 0.       ;This->e21 =        0.;This->e22=        1.;This->e23 = 0.;
	  This->e30 = 0.       ;This->e31 =        0.;This->e32=        0.;This->e33 = 1.;
	}
	break;
      case OMatrixTranslationAxis:
	{
	  OMatrix      v_to_z,tz,z_to_v;
	  double       a_value;
	  double       a_x,a_y,a_z;
	  /*.........................................................................*/
	  a_value = va_arg(a_args,double);
	  a_x     = va_arg(a_args,double);
	  a_y     = va_arg(a_args,double);
	  a_z     = va_arg(a_args,double);
	  /*   CVectorNormalize (&a_x,&a_y,&a_z);*/
	  
	  v_to_z                = OMatrixCreate(OMatrixVectorToZ,a_x,a_y,a_z);
	  tz                    = OMatrixCreate(OMatrixTranslation,0.,0.,a_value);
	  z_to_v                = OMatrixCreate(OMatrixZToVector,a_x,a_y,a_z);
	  OMatrixMultiplyTripletAndAssign (This,z_to_v,tz,v_to_z);
	  
	  OMatrixDelete         (v_to_z);
	  OMatrixDelete         (tz);
	  OMatrixDelete         (z_to_v);
	}
	break;
      case OMatrixShear:
	{
	  OMatrixInitialize  (This,OMatrixIdentity);
	  This->e02    = va_arg(a_args,double);
	  This->e12    = va_arg(a_args,double);
	}
	break;
      case OMatrixTranslation:
	{
	  double a_x,a_y,a_z;
	  /*.........................................................................*/
	  a_x = va_arg(a_args,double);
	  a_y = va_arg(a_args,double);
	  a_z = va_arg(a_args,double);
	  This->e00 = 1.       ;This->e01 =        0.;This->e02=        0.;This->e03 = a_x;
	  This->e10 = 0.       ;This->e11 =        1.;This->e12=        0.;This->e13 = a_y;
	  This->e20 = 0.       ;This->e21 =        0.;This->e22=        1.;This->e23 = a_z;
	  This->e30 = 0.       ;This->e31 =        0.;This->e32=        0.;This->e33 = 1.;
	}
	break;
      case OMatrixZToVector:
	{
          double a_x,a_y,a_z,norme;
	  /*.........................................................................*/
	  a_x   = va_arg(a_args,double);
	  a_y   = va_arg(a_args,double);
	  a_z   = va_arg(a_args,double);
	  norme = CVectorGetNorme (a_x,a_y,a_z);
	  if(norme==(double)0.)
	    {
	      ReportError       ("OMatrixZToVector",CErrorValue);
	      OMatrixInitialize (This,OMatrixIdentity);
	    }
	  else
	    {
	      double                 teta,phi;
	      OMatrix                rot_teta,rot_phi;
	      teta                   = acos (a_z/norme);
     	      phi                    = ((a_x==(double)0.)&&(a_y==(double)0.)) ? 0. : atan2 (a_y,a_x);
	      rot_teta               = OMatrixCreate (OMatrixRotationY,teta);
	      rot_phi                = OMatrixCreate (OMatrixRotationZ,phi);
	      OMatrixMultiplyAndAssign     (This,rot_phi,rot_teta);
	      OMatrixDelete          (rot_phi);
	      OMatrixDelete          (rot_teta);
	    }
	}
	break;
      case OMatrixVectorToZ:
	{
	  double a_x,a_y,a_z,norme;
	  /*.........................................................................*/
	  a_x = va_arg(a_args,double);
	  a_y = va_arg(a_args,double);
	  a_z = va_arg(a_args,double);
	  norme = CVectorGetNorme (a_x,a_y,a_z);
	  if(norme==(double)0.)
	    {
	      ReportError     ("OMatrixVectorToZ",CErrorValue);
	      OMatrixInitialize (This,OMatrixIdentity);
	    }
	  else        
	    { 
	      double  teta,phi;
	      OMatrix rot_teta,rot_phi;
	      teta    = - acos (a_z/norme);
	      phi     = ((a_x==(double)0.)&&(a_y==(double)0.)) ? 0. : - atan2 (a_y,a_x);
	      rot_teta               = OMatrixCreate   (OMatrixRotationY,teta);
	      rot_phi                = OMatrixCreate   (OMatrixRotationZ,phi);
	      OMatrixMultiplyAndAssign     (This,rot_teta ,rot_phi);
	      OMatrixDelete          (rot_phi);
	      OMatrixDelete          (rot_teta);
	    }
	}
	break;
      case OMatrixStringAlign:
	{
	  int     nchar;
	  double  tx,ty;
	  char*   a_string;
	  char    a_xalign;
	  char    a_yalign;
	  a_string = va_arg(a_args,char*);
	  a_xalign = (char)va_arg(a_args,int);
	  a_yalign = (char)va_arg(a_args,int);
	  if(a_string==NULL) return;
	  nchar     = strlen(a_string);
	  if(nchar<=0) return;
	  tx = ty = 0.;
	  switch (a_yalign)
	    {
  	      case 'C':
	      case 'c':ty = -1./3.;break;
	      case 'T':
	      case 't':ty = -2./3.;break;
	      case 'B':
	      case 'b':
	      default:break;
	    }
	  switch (a_xalign)
	    {
	      case 'C':
	      case 'c':tx = -nchar/2.;break;
	      case 'R':
	      case 'r':tx =   -nchar;break;
	      case 'L':
	      case 'l':
	      default:break;
	    }
	  This->e00 = 1.       ;This->e01 =        0.;This->e02=        0.;This->e03 = tx;
	  This->e10 = 0.       ;This->e11 =        1.;This->e12=        0.;This->e13 = ty;
	  This->e20 = 0.       ;This->e21 =        0.;This->e22=        1.;This->e23 = 0.;
	  This->e30 = 0.       ;This->e31 =        0.;This->e32=        0.;This->e33 = 1.;
	}
	break;
      case OMatrixGL:
	{
	  double* gl;
	  gl      = va_arg(a_args,double*);
	  if(gl!=NULL)
	    {
	      This->e00 = gl[ 0];
	      This->e10 = gl[ 1];
	      This->e20 = gl[ 2];
	      This->e30 = gl[ 3];
	  
	      This->e01 = gl[ 4];
	      This->e11 = gl[ 5];
	      This->e21 = gl[ 6];
	      This->e31 = gl[ 7];
	  
	      This->e02 = gl[ 8];
	      This->e12 = gl[ 9];
	      This->e22 = gl[10];
	      This->e32 = gl[11];
	      
	      This->e03 = gl[12];
	      This->e13 = gl[13];
	      This->e23 = gl[14];
	      This->e33 = gl[15];
	    }
	}
        break;
      default:
	ReportError ("OMatrixInitialize",CErrorValue);
        break;
    }
}
/***************************************************************************/
void OMatrixCopy (
 OMatrix This 
,OMatrix a_from   
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)        return;
  if(a_from==NULL)      return;
  if(a_from==This) return;
  *This = *a_from;
/*
  This->e00 = a_from->e00;
  This->e01 = a_from->e01;
  This->e02 = a_from->e02;
  This->e03 = a_from->e03;
  
  This->e10 = a_from->e10;
  This->e11 = a_from->e11;
  This->e12 = a_from->e12;
  This->e13 = a_from->e13;
  
  This->e20 = a_from->e20;
  This->e21 = a_from->e21;
  This->e22 = a_from->e22;
  This->e23 = a_from->e23;
	  
  This->e30 = a_from->e30;
  This->e31 = a_from->e31;
  This->e32 = a_from->e32;
  This->e33 = a_from->e33; */
}
/***************************************************************************/
void OMatrixRemoveTranslations (
 OMatrix This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  This->e03 = 0.;
  This->e13 = 0.;
  This->e23 = 0.;
}
/***************************************************************************/
void OMatrixRemoveIsometricScale (
 OMatrix This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  double  deter;
  OMatrix scale,tmp;
/*.........................................................................*/
  if(This==NULL) return;
  deter = OMatrixGetDeterminant (This);
  if(deter==0.) return;
  scale         = OMatrixCreate(OMatrixScaleIsometric,1./pow(fabs(deter),1./3.));
  tmp           = OMatrixMultiply(This,scale);
  OMatrixCopy   (This,tmp);
  OMatrixDelete (tmp);
  OMatrixDelete (scale);
}
/***************************************************************************/
double OMatrixGetDeterminant (
 OMatrix This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  double e00,e01,e02,e03;
/*.........................................................................*/
  if(This==NULL) return 0.;
  e00 =  olutl_d33      (This->e11 ,This->e12,This->e13,
                         This->e21 ,This->e22,This->e23,
                         This->e31 ,This->e32,This->e33);
  e01 = -olutl_d33      (This->e01 ,This->e02,This->e03,
                         This->e21 ,This->e22,This->e23,
                         This->e31 ,This->e32,This->e33);
  e02 =  olutl_d33      (This->e01 ,This->e02,This->e03,
                         This->e11 ,This->e12,This->e13,
                         This->e31 ,This->e32,This->e33);
  e03 = -olutl_d33      (This->e01 ,This->e02,This->e03,
                         This->e11 ,This->e12,This->e13,
                         This->e21 ,This->e22,This->e23);
                  
  return      (This->e00  * e00
             + This->e10  * e01
             + This->e20  * e02
             + This->e30  * e03) ;
}
/***************************************************************************/
double OMatrixGetTrace (
 OMatrix This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 0.;
  return       This->e00 + This->e11 + This->e22 + This->e33;
}
/***************************************************************************/
double OMatrixInvertAndAssign (
 OMatrix This 
,OMatrix a_to_invert 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  double deter;
/*.........................................................................*/
  if(This==NULL)        return 0.;
  if(a_to_invert==NULL) return 0.;
  This->e00 =  olutl_d33      (a_to_invert->e11 ,a_to_invert->e12,a_to_invert->e13,
			       a_to_invert->e21 ,a_to_invert->e22,a_to_invert->e23,
			       a_to_invert->e31 ,a_to_invert->e32,a_to_invert->e33);
  This->e01 = -olutl_d33      (a_to_invert->e01 ,a_to_invert->e02,a_to_invert->e03,
			       a_to_invert->e21 ,a_to_invert->e22,a_to_invert->e23,
			       a_to_invert->e31 ,a_to_invert->e32,a_to_invert->e33);
  This->e02 =  olutl_d33      (a_to_invert->e01 ,a_to_invert->e02,a_to_invert->e03,
			       a_to_invert->e11 ,a_to_invert->e12,a_to_invert->e13,
			       a_to_invert->e31 ,a_to_invert->e32,a_to_invert->e33);
  This->e03 = -olutl_d33      (a_to_invert->e01 ,a_to_invert->e02,a_to_invert->e03,
			       a_to_invert->e11 ,a_to_invert->e12,a_to_invert->e13,
			       a_to_invert->e21 ,a_to_invert->e22,a_to_invert->e23);
                  
  deter =   a_to_invert->e00  * This->e00
             + a_to_invert->e10  * This->e01
             + a_to_invert->e20  * This->e02
             + a_to_invert->e30  * This->e03 ;

  if(deter==(double)0.) return 0.;
                  

  This->e10 = -olutl_d33      (a_to_invert->e10 ,a_to_invert->e12,a_to_invert->e13,
			       a_to_invert->e20 ,a_to_invert->e22,a_to_invert->e23,
			       a_to_invert->e30 ,a_to_invert->e32,a_to_invert->e33);
  This->e11 =  olutl_d33      (a_to_invert->e00 ,a_to_invert->e02,a_to_invert->e03,
			       a_to_invert->e20 ,a_to_invert->e22,a_to_invert->e23,
			       a_to_invert->e30 ,a_to_invert->e32,a_to_invert->e33);
  This->e12 = -olutl_d33      (a_to_invert->e00 ,a_to_invert->e02,a_to_invert->e03,
			       a_to_invert->e10 ,a_to_invert->e12,a_to_invert->e13,
			       a_to_invert->e30 ,a_to_invert->e32,a_to_invert->e33);
  This->e13 =  olutl_d33      (a_to_invert->e00 ,a_to_invert->e02,a_to_invert->e03,
			       a_to_invert->e10 ,a_to_invert->e12,a_to_invert->e13,
			       a_to_invert->e20 ,a_to_invert->e22,a_to_invert->e23);
                  
  This->e20 =  olutl_d33      (a_to_invert->e10 ,a_to_invert->e11,a_to_invert->e13,
			       a_to_invert->e20 ,a_to_invert->e21,a_to_invert->e23,
			       a_to_invert->e30 ,a_to_invert->e31,a_to_invert->e33);
  This->e21 = -olutl_d33      (a_to_invert->e00 ,a_to_invert->e01,a_to_invert->e03,
			       a_to_invert->e20 ,a_to_invert->e21,a_to_invert->e23,
			       a_to_invert->e30 ,a_to_invert->e31,a_to_invert->e33);
  This->e22 =  olutl_d33      (a_to_invert->e00 ,a_to_invert->e01,a_to_invert->e03,
			       a_to_invert->e10 ,a_to_invert->e11,a_to_invert->e13,
			       a_to_invert->e30 ,a_to_invert->e31,a_to_invert->e33);
  This->e23 = -olutl_d33      (a_to_invert->e00 ,a_to_invert->e01,a_to_invert->e03,
			       a_to_invert->e10 ,a_to_invert->e11,a_to_invert->e13,
			       a_to_invert->e20 ,a_to_invert->e21,a_to_invert->e23);
                  
  This->e30 = -olutl_d33      (a_to_invert->e10 ,a_to_invert->e11,a_to_invert->e12,
			       a_to_invert->e20 ,a_to_invert->e21,a_to_invert->e22,
			       a_to_invert->e30 ,a_to_invert->e31,a_to_invert->e32);
  This->e31 =  olutl_d33      (a_to_invert->e00 ,a_to_invert->e01,a_to_invert->e02,
			       a_to_invert->e20 ,a_to_invert->e21,a_to_invert->e22,
			       a_to_invert->e30 ,a_to_invert->e31,a_to_invert->e32);
  This->e32 = -olutl_d33      (a_to_invert->e00 ,a_to_invert->e01,a_to_invert->e02,
			       a_to_invert->e10 ,a_to_invert->e11,a_to_invert->e12,
			       a_to_invert->e30 ,a_to_invert->e31,a_to_invert->e32);
  This->e33 =  olutl_d33      (a_to_invert->e00 ,a_to_invert->e01,a_to_invert->e02,
			       a_to_invert->e10 ,a_to_invert->e11,a_to_invert->e12,
			       a_to_invert->e20 ,a_to_invert->e21,a_to_invert->e22);

  This->e00 /= deter;  
  This->e01 /= deter;  
  This->e02 /= deter;  
  This->e03 /= deter;  

  This->e10 /= deter;  
  This->e11 /= deter;  
  This->e12 /= deter;  
  This->e13 /= deter;  

  This->e20 /= deter;  
  This->e21 /= deter;  
  This->e22 /= deter;  
  This->e23 /= deter;  

  This->e30 /= deter;  
  This->e31 /= deter;  
  This->e32 /= deter;  
  This->e33 /= deter;  

  return deter;
}
/***************************************************************************/
void OMatrixMultiplyByConstantAndAssign (
 OMatrix This 
,OMatrix a_matrix
,double  a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)     return;
  if(a_matrix==NULL) return;

  This->e00 = a_matrix->e00 * a_value;  
  This->e01 = a_matrix->e01 * a_value;  
  This->e02 = a_matrix->e02 * a_value;  
  This->e03 = a_matrix->e03 * a_value;  

  This->e10 = a_matrix->e10 * a_value;  
  This->e11 = a_matrix->e11 * a_value;  
  This->e12 = a_matrix->e12 * a_value;  
  This->e13 = a_matrix->e13 * a_value;  

  This->e20 = a_matrix->e20 * a_value;  
  This->e21 = a_matrix->e21 * a_value;  
  This->e22 = a_matrix->e22 * a_value;  
  This->e23 = a_matrix->e23 * a_value;  

  This->e30 = a_matrix->e30 * a_value;  
  This->e31 = a_matrix->e31 * a_value;  
  This->e32 = a_matrix->e32 * a_value;  
  This->e33 = a_matrix->e33 * a_value;  
}
/***************************************************************************/
void OMatrixAddAndAssign (
 OMatrix This 
,OMatrix a_1
,OMatrix a_2
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  if(a_1==NULL)  return;
  if(a_2==NULL)  return;

  This->e00 = a_1->e00 + a_2->e00;
  This->e01 = a_1->e01 + a_2->e01;
  This->e02 = a_1->e02 + a_2->e02;
  This->e03 = a_1->e03 + a_2->e03;

  This->e10 = a_1->e10 + a_2->e10;
  This->e11 = a_1->e11 + a_2->e11;
  This->e12 = a_1->e12 + a_2->e12;
  This->e13 = a_1->e13 + a_2->e13;

  This->e20 = a_1->e20 + a_2->e20;
  This->e21 = a_1->e21 + a_2->e21;
  This->e22 = a_1->e22 + a_2->e22;
  This->e23 = a_1->e23 + a_2->e23;

  This->e30 = a_1->e30 + a_2->e30;
  This->e31 = a_1->e31 + a_2->e31;
  This->e32 = a_1->e32 + a_2->e32;
  This->e33 = a_1->e33 + a_2->e33;
}
/***************************************************************************/
void OMatrixSubstractAndAssign (
 OMatrix This 
,OMatrix a_1
,OMatrix a_2
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  if(a_1==NULL)  return;
  if(a_2==NULL)  return;

  This->e00 = a_1->e00 - a_2->e00;
  This->e01 = a_1->e01 - a_2->e01;
  This->e02 = a_1->e02 - a_2->e02;
  This->e03 = a_1->e03 - a_2->e03;

  This->e10 = a_1->e10 - a_2->e10;
  This->e11 = a_1->e11 - a_2->e11;
  This->e12 = a_1->e12 - a_2->e12;
  This->e13 = a_1->e13 - a_2->e13;

  This->e20 = a_1->e20 - a_2->e20;
  This->e21 = a_1->e21 - a_2->e21;
  This->e22 = a_1->e22 - a_2->e22;
  This->e23 = a_1->e23 - a_2->e23;

  This->e30 = a_1->e30 - a_2->e30;
  This->e31 = a_1->e31 - a_2->e31;
  This->e32 = a_1->e32 - a_2->e32;
  This->e33 = a_1->e33 - a_2->e33;
}
/***************************************************************************/
void OMatrixMultiplyAndAssign (
 OMatrix This 
,OMatrix a_1 
,OMatrix a_2 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  if(a_1==NULL)  return;
  if(a_2==NULL)  return;
  if( (This==a_1) || (This==a_2) )
    {
      CWarnF ("MultiplyAndAssign : result matrix can't be a multiplier one.\n");
      return;
    }
  This->e00 = a_1->e00 * a_2->e00 + a_1->e01 * a_2->e10 + a_1->e02 * a_2->e20 + a_1->e03 * a_2->e30;
  This->e10 = a_1->e10 * a_2->e00 + a_1->e11 * a_2->e10 + a_1->e12 * a_2->e20 + a_1->e13 * a_2->e30;
  This->e20 = a_1->e20 * a_2->e00 + a_1->e21 * a_2->e10 + a_1->e22 * a_2->e20 + a_1->e23 * a_2->e30;
  This->e30 = a_1->e30 * a_2->e00 + a_1->e31 * a_2->e10 + a_1->e32 * a_2->e20 + a_1->e33 * a_2->e30;

  This->e01 = a_1->e00 * a_2->e01 + a_1->e01 * a_2->e11 + a_1->e02 * a_2->e21 + a_1->e03 * a_2->e31;
  This->e11 = a_1->e10 * a_2->e01 + a_1->e11 * a_2->e11 + a_1->e12 * a_2->e21 + a_1->e13 * a_2->e31;
  This->e21 = a_1->e20 * a_2->e01 + a_1->e21 * a_2->e11 + a_1->e22 * a_2->e21 + a_1->e23 * a_2->e31;
  This->e31 = a_1->e30 * a_2->e01 + a_1->e31 * a_2->e11 + a_1->e32 * a_2->e21 + a_1->e33 * a_2->e31;

  This->e02 = a_1->e00 * a_2->e02 + a_1->e01 * a_2->e12 + a_1->e02 * a_2->e22 + a_1->e03 * a_2->e32;
  This->e12 = a_1->e10 * a_2->e02 + a_1->e11 * a_2->e12 + a_1->e12 * a_2->e22 + a_1->e13 * a_2->e32;
  This->e22 = a_1->e20 * a_2->e02 + a_1->e21 * a_2->e12 + a_1->e22 * a_2->e22 + a_1->e23 * a_2->e32;
  This->e32 = a_1->e30 * a_2->e02 + a_1->e31 * a_2->e12 + a_1->e32 * a_2->e22 + a_1->e33 * a_2->e32;

  This->e03 = a_1->e00 * a_2->e03 + a_1->e01 * a_2->e13 + a_1->e02 * a_2->e23 + a_1->e03 * a_2->e33;
  This->e13 = a_1->e10 * a_2->e03 + a_1->e11 * a_2->e13 + a_1->e12 * a_2->e23 + a_1->e13 * a_2->e33;
  This->e23 = a_1->e20 * a_2->e03 + a_1->e21 * a_2->e13 + a_1->e22 * a_2->e23 + a_1->e23 * a_2->e33;
  This->e33 = a_1->e30 * a_2->e03 + a_1->e31 * a_2->e13 + a_1->e32 * a_2->e23 + a_1->e33 * a_2->e33;

}
/***************************************************************************/
void OMatrixTransposeAndAssign (
 OMatrix This 
,OMatrix a_matrix
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)     return;
  if(a_matrix==NULL) return;

  This->e00 = a_matrix->e00;
  This->e01 = a_matrix->e10;
  This->e02 = a_matrix->e20;
  This->e03 = a_matrix->e30;

  This->e10 = a_matrix->e01;
  This->e11 = a_matrix->e11;
  This->e12 = a_matrix->e21;
  This->e13 = a_matrix->e31;

  This->e20 = a_matrix->e02;
  This->e21 = a_matrix->e12;
  This->e22 = a_matrix->e22;
  This->e23 = a_matrix->e32;

  This->e30 = a_matrix->e03;
  This->e31 = a_matrix->e13;
  This->e32 = a_matrix->e23;
  This->e33 = a_matrix->e33;

}
/***************************************************************************/
void OMatrixMultiplyTripletAndAssign (
 OMatrix This 
,OMatrix a_1 
,OMatrix a_2 
,OMatrix a_3 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OMatrixRecord matrix;
/*.........................................................................*/
  OMatrixMultiplyAndAssign (&matrix,a_1,a_2);
  OMatrixMultiplyAndAssign (This,&matrix,a_3);
}
/***************************************************************************/
void OMatrixMultiplyMatricesAndAssign (
 OMatrix This 
,int a_number
,...
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  va_list                    args;
/*.........................................................................*/
  va_start                   (args,a_number);
  MultiplyMatricesAndAssignV (This,a_number,args);
  va_end                     (args);
}
/***************************************************************************/
void OMatrixSetElement32 (
 OMatrix This 
,double a_val 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  This->e32 = a_val;
}
/***************************************************************************/
void OMatrixSetElement23 (
 OMatrix This 
,double a_val 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  This->e23 = a_val;
}
/***************************************************************************/
void OMatrixSetElement33 (
 OMatrix This 
,double a_val 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  This->e33 = a_val;
}
/***************************************************************************/
double OMatrixGetElement00 (
 OMatrix This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 0.;
  return This->e00;
}
/***************************************************************************/
double OMatrixGetElement11 (
 OMatrix This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 0.;
  return This->e11;
}
/***************************************************************************/
double OMatrixGetElement22 (
 OMatrix This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 0.;
  return This->e22;
}
/***************************************************************************/
double OMatrixGetElement01 (
 OMatrix This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 0.;
  return This->e01;
}
/***************************************************************************/
double OMatrixGetElement02 (
 OMatrix This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 0.;
  return This->e02;
}
/***************************************************************************/
double OMatrixGetElement12 (
 OMatrix This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 0.;
  return This->e12;
}
/***************************************************************************/
/********* Produce new matrix **********************************************/
/***************************************************************************/
OMatrix OMatrixDuplicate (
 OMatrix This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OMatrix matrix;
/*.........................................................................*/
  if(This==NULL) return NULL;
  matrix   = OMatrixCreate(OMatrixIdentity);
  OMatrixCopy (matrix,This);
  return matrix;
}
/***************************************************************************/
OMatrix OMatrixMultiply (
 OMatrix This 
,OMatrix a_2 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OMatrix matrix;
/*.........................................................................*/
  matrix   = OMatrixCreate(OMatrixIdentity);
  OMatrixMultiplyAndAssign (matrix,This,a_2);
  return matrix;
}
/***************************************************************************/
OMatrix OMatrixMultiplyMatrices (
 int a_number
,...
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OMatrix                    matrix;
  va_list                    args;
/*.........................................................................*/
  matrix                     = OMatrixCreate(OMatrixIdentity);
  va_start                   (args,a_number);
  MultiplyMatricesAndAssignV (matrix,a_number,args);
  va_end                     (args);
  return                     matrix;
}
/***************************************************************************/
OMatrix OMatrixAdd (
 OMatrix This 
,OMatrix a_2 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OMatrix matrix;
/*.........................................................................*/
  matrix              = OMatrixCreate(OMatrixIdentity);
  OMatrixAddAndAssign (matrix,This,a_2);
  return              matrix;
}
/***************************************************************************/
OMatrix OMatrixSubstract (
 OMatrix This 
,OMatrix a_2 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OMatrix matrix;
/*.........................................................................*/
  matrix                    = OMatrixCreate(OMatrixIdentity);
  OMatrixSubstractAndAssign (matrix,This,a_2);
  return                    matrix;
}
/***************************************************************************/
OMatrix OMatrixMultiplyTriplet (
 OMatrix This 
,OMatrix a_2 
,OMatrix a_3 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OMatrix matrix;
/*.........................................................................*/
  matrix   = OMatrixCreate(OMatrixIdentity);
  OMatrixMultiplyTripletAndAssign (matrix,This,a_2,a_3);
  return matrix;
}
/***************************************************************************/
OMatrix OMatrixInvert (
 OMatrix This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OMatrix          matrix;
/*.........................................................................*/
  matrix           = OMatrixCreate(OMatrixIdentity);
  OMatrixInvertAndAssign (matrix,This);
  return           matrix;
}
/***************************************************************************/
OMatrix OMatrixTranspose (
 OMatrix This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OMatrix          matrix;
/*.........................................................................*/
  matrix                    = OMatrixCreate(OMatrixIdentity);
  OMatrixTransposeAndAssign (matrix,This);
  return                    matrix;
}
/***************************************************************************/
OMatrix OMatrixMultiplyByConstant (
 OMatrix This 
,double  a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OMatrix          matrix;
/*.........................................................................*/
  matrix                             = OMatrixCreate(OMatrixIdentity);
  OMatrixMultiplyByConstantAndAssign (matrix,This,a_value);
  return                             matrix;
}
/***************************************************************************/
void OMatrixMultiplyVector (
 OMatrix This 
,double a_x
,double a_y
,double a_z
,double* a_ox
,double* a_oy
,double* a_oz
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   double t;
/*.........................................................................*/
  if(This==NULL) return;
  t     =  This->e30 * a_x + This->e31 * a_y + This->e32 * a_z + This->e33;
  *a_ox = (This->e00 * a_x + This->e01 * a_y + This->e02 * a_z + This->e03)/t;
  *a_oy = (This->e10 * a_x + This->e11 * a_y + This->e12 * a_z + This->e13)/t;
  *a_oz = (This->e20 * a_x + This->e21 * a_y + This->e22 * a_z + This->e23)/t;
}
/***************************************************************************/
void OMatrixConvertToGL_Matrix (
 OMatrix This
,double* a_gl
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  if(a_gl==NULL) return;

  a_gl[ 0] = This->e00;
  a_gl[ 1] = This->e10;
  a_gl[ 2] = This->e20;
  a_gl[ 3] = This->e30;

  a_gl[ 4] = This->e01;
  a_gl[ 5] = This->e11;
  a_gl[ 6] = This->e21;
  a_gl[ 7] = This->e31;

  a_gl[ 8] = This->e02;
  a_gl[ 9] = This->e12;
  a_gl[10] = This->e22;
  a_gl[11] = This->e32;

  a_gl[12] = This->e03;
  a_gl[13] = This->e13;
  a_gl[14] = This->e23;
  a_gl[15] = This->e33;

}
/***************************************************************************/
int OMatrixIsIdentity (
 OMatrix This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 0;

  if(This->e00!=(double)1.) return 0;
  if(This->e01!=(double)0.) return 0;
  if(This->e02!=(double)0.) return 0;
  if(This->e03!=(double)0.) return 0;

  if(This->e10!=(double)0.) return 0;
  if(This->e11!=(double)1.) return 0;
  if(This->e12!=(double)0.) return 0;
  if(This->e13!=(double)0.) return 0;

  if(This->e20!=(double)0.) return 0;
  if(This->e21!=(double)0.) return 0;
  if(This->e22!=(double)1.) return 0;
  if(This->e23!=(double)0.) return 0;

  if(This->e30!=(double)0.) return 0;
  if(This->e31!=(double)0.) return 0;
  if(This->e32!=(double)0.) return 0;
  if(This->e33!=(double)1.) return 0;

  return 1;
}
/***************************************************************************/
OMatrix OMatrixGetSymetricPart (
 OMatrix This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OMatrix m1,m2,m3;
/*.........................................................................*/
  m1                  = OMatrixTranspose          (This);
  m2                  = OMatrixAdd                (This,m1);
  m3                  = OMatrixMultiplyByConstant (m2,0.5);
  OMatrixDelete       (m1);
  OMatrixDelete       (m2);
  return              m3;
}
/***************************************************************************/
OMatrix OMatrixGetAntisymetricPart (
 OMatrix This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OMatrix m1,m2,m3,m4;
/*.........................................................................*/
  m1                  = OMatrixTranspose          (This);
  m2                  = OMatrixMultiplyByConstant (m1,-1.);
  m3                  = OMatrixAdd                (This,m2);
  m4                  = OMatrixMultiplyByConstant (m3,0.5);
  OMatrixDelete       (m1);
  OMatrixDelete       (m2);
  OMatrixDelete       (m3);
  return              m4;
}
/***************************************************************************/
void OMatrixDump (
 OMatrix This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  CPrinterPutF ("%g %g %g %g\n",This->e00,This->e01,This->e02,This->e03);
  CPrinterPutF ("%g %g %g %g\n",This->e10,This->e11,This->e12,This->e13);
  CPrinterPutF ("%g %g %g %g\n",This->e20,This->e21,This->e22,This->e23);
  CPrinterPutF ("%g %g %g %g\n",This->e30,This->e31,This->e32,This->e33);
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
  CErrorHandle ("Co","OMatrix",a_name,a_code);
}
/***************************************************************************/
int OMatrixSetAttribute (
 OMatrix  This
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

       if(strcmp(a_name,"identifier")==0)        {}
  else if(strcmp(a_name,"values")==0)
    {
      if(a_number!=16)
	{
	  CWarnF ("OMatrixSetAttribute : For property %s, 16 values are requested.\n",a_name);
	  return 0;
	}
      else
	{
	  double*         list;
	  list            = *((double**)a_addr);
          OMatrixInitialize     (This,OMatrixFollowing,
				       list[ 0],list[ 1],list[ 2],list[ 3],
				       list[ 4],list[ 5],list[ 6],list[ 7],
				       list[ 8],list[ 9],list[10],list[11],
				       list[12],list[13],list[14],list[15]);
	}
    }
  else 
    {
      CInfoF ("OMatrixSetAttribute : unknown property %s.\n",a_name);
      return 0;
    }

  a_user = NULL;
  return 1;
}
/***************************************************************************/
int OMatrixGetAttribute (
 OMatrix  This
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

       if(strcmp(a_name,"identifier")==0)        *((Ulong*)a_addr)  = (Ulong)This;
  else if(strcmp(a_name,"values")==0)
    {
      Class.doubles[0]  = This->e00;
      Class.doubles[1]  = This->e01;
      Class.doubles[2]  = This->e02;
      Class.doubles[3]  = This->e03;

      Class.doubles[4]  = This->e10;
      Class.doubles[5]  = This->e11;
      Class.doubles[6]  = This->e12;
      Class.doubles[7]  = This->e13;

      Class.doubles[8]  = This->e20;
      Class.doubles[9]  = This->e21;
      Class.doubles[10] = This->e22;
      Class.doubles[11] = This->e23;
      
      Class.doubles[12] = This->e30;
      Class.doubles[13] = This->e31;
      Class.doubles[14] = This->e32;
      Class.doubles[15] = This->e33;

      if(a_number!=NULL) *a_number = 16;
      *((double**)a_addr) = Class.doubles;
    }
  else 
    {
      CInfoF ("OMatrixGetAttribute : unknown property %s.\n",a_name);
      return 0;
    }

  a_user = NULL;
  return 1;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static void MultiplyMatricesAndAssignV (
 OMatrix This 
,int     a_number
,va_list a_args  
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int           count;
/*.........................................................................*/
  OMatrixInitialize (This,OMatrixIdentity);
  for(count=0;count<a_number;count++)
    {
      OMatrixRecord            matrix;
      OMatrix                  arg;
      OMatrixCopy              (&matrix,This);
      arg                      = va_arg(a_args,OMatrix);
      OMatrixMultiplyAndAssign (This,&matrix,arg);
    }
}
