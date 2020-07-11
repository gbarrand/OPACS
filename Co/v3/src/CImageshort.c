/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#include <math.h>
#include <stdio.h>
#include <stdlib.h> /*qsort*/

#include <CMemory.h>
#include <CPrinter.h>
#include <CFile.h>
#include <CError.h>

#include <CImageshort.h>

typedef int (*CompareFunction)(const void*, const void*);

typedef unsigned int Uint;

#ifdef __cplusplus
extern "C"{
#endif
static int    Compare                (short*,short*);
static void   Transform              (double,double,double*,double*);
static double GetWeight              (int,int,double,double);
static double GetBSpline             (int,int,double,int);
static void   GetGauss               (double*,double*,int);
static void   SetMatrix              (double*,int,int);
static double GetSpline              (double*,double,int,int);
static void   InitializeBSplineTable (int);
static double GetBSplineByTable      (int,int,double);
static void   InvertMatrix           (double*,int,int);
static double GetSplineCentered      (short*,Uint,Uint,double*,int,int,double,double,double*);
static double InterpolateFirstOrder  (short*,Uint,Uint,double,double);
static double InterpolateSpline      (short*,Uint,Uint,double,double);
static double InterpolateWithWeight  (short*,Uint,Uint,double,double);
static void   ReportError            (char*,int);
#ifdef __cplusplus
}
#endif

#define DBL_INT                 (int)(float)
#define MINIMUM(a,b)            ((a)<(b)?a:b)
#define MAXIMUM(a,b)            ((a)>(b)?a:b)
#define MVAL(a_i33,a_row,a_col) (a_i33[a_row * 3 + a_col])

/*For rebinning.*/
static struct
{
  double PixSizX;               /*  Taille Pixel en X (MM)  */
  double PixSizY;               /*  Taille Pixel en Y (MM)  */
  int    Npol;                  /* Degre du  polynome de Transformation           */
  double sigx[2],sigy[2];       /* "Taille" des Images de reference et a comparer   */
  double xmoy[2],ymoy[2];       /* Milieu des images de reference et a comparer   */
  double Tab_Bspl[700]; 
  double Polxy[5][5][2];        /* Matrice des Parametres de la Transformation    */
} Class = {0.01,0.01,0,{2.0,2.0},{2.0,2.0},{2.0,2.0},{2.0,2.0}};
/***************************************************************************/
void CImageshortGetLUT (
 short* This
,Uint  a_width
,Uint  a_height
,int* a_min 
,int* a_max 
,int* a_nbin 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int       row,col;
  double    sx,sx2;
  int       npix;
  int       ideb,ifin,jdeb,jfin;
/*.........................................................................*/
  if(a_min!=NULL)  *a_min  = 0;
  if(a_max!=NULL)  *a_max  = 0;
  if(a_nbin!=NULL) *a_nbin = 0; 
  if(This==NULL)                      return;
  if( (a_width==0) || (a_height==0) ) return;  
  if (a_width > 200)
    {
      ideb = 50;
      ifin = a_width-50;
    }
  else
    {
      ideb = 1;
      ifin = a_width-1;
    }
  if (a_height > 200)
    {
      jdeb = 50;
      jfin = a_height-50;
    }
  else
    {
      jdeb = 1;
      jfin = a_height-1;
    }

  sx    = sx2 = 0.;
  npix  = 0;
  for (row=jdeb;row<jfin;row++)
    { 
       short* itema;
      itema = This + row * a_width + ideb;
      for (col=ideb;col<ifin;col++)
        { double val;
          val  = (double)(*itema);
          sx  += val;
          sx2 += val * val;
          npix++;
          itema++;
        }
    }

  sx  /= npix;
  sx2 /= npix;
  sx2 -= sx * sx;
  sx2  = sqrt((double) sx2);

  if(a_min!=NULL)  *a_min  = (int)(sx-sx2/3);
  if(a_max!=NULL)  *a_max  = (int)(sx+sx2*2);
  if(a_nbin!=NULL) *a_nbin = 14;

}
/***************************************************************************/
void CImageshortAddConstant (
 short* This
,Uint  a_width
,Uint  a_height
,short  a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int   count;
   short* itema;
  int            nitem;
/*.........................................................................*/
  if(This==NULL) return;
  nitem     = a_width * a_height;
  itema     = This;
  for (count=0;count<nitem;count++)
    { 
      *itema = (*itema) + a_value;
      itema++;
    }
}
/***************************************************************************/
void CImageshortSubstractConstant (
 short* This
,Uint  a_width
,Uint  a_height
,short  a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int   count;
   short* itema;
  int            nitem;
/*.........................................................................*/
  if(This==NULL) return;
  nitem     = a_width * a_height;
  itema     = This;
  for (count=0;count<nitem;count++)
    { 
      *itema = (*itema) - a_value;
      itema++;
    }
}
/***************************************************************************/
void CImageshortMultiplyByConstant (
 short* This
,Uint  a_width
,Uint  a_height
,short  a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int   count;
   short* itema;
  int            nitem;
/*.........................................................................*/
  if(This==NULL) return;
  nitem     = a_width * a_height;
  itema     = This;
  for (count=0;count<nitem;count++)
    { 
      *itema = (*itema) * a_value;
      itema++;
    }
}
/***************************************************************************/
void CImageshortDivideByConstant (
 short* This
,Uint  a_width
,Uint  a_height
,short  a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int   count;
   short* itema;
  int            nitem;
/*.........................................................................*/
  if(This==NULL)            return;
  if(a_value==(short)0) {ReportError("CImageshortDivideByConstant",CErrorValue);return;}
  nitem     = a_width * a_height;
  itema     = This;
  for (count=0;count<nitem;count++)
    { 
      *itema = (*itema) / a_value;
      itema++;
    }
}
/***************************************************************************/
int CImageshortConvolute (
 short*  This
,Uint   a_width
,Uint   a_height
,int    a_celln
,short*  a_cells
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int   count;
   short* itema;
  int            nitem;
  int            ok;
/*.........................................................................*/
  if(This==NULL) return 0;
  nitem     = a_width * a_height;
  itema     = This;
  ok        = 1;
  for (count=0;count<nitem;count++)
    { 
      if((*itema<0)||(*itema>=a_celln))
	{
	  ok = 0;
	}
      else
	{
	  *itema = a_cells[*itema];
	}
      itema++;
    }
  return ok;
}
/******************************************************************************/
void CImageshortFill (
 short* This
,Uint  a_width
,Uint  a_height
,short  a_value 
)
/******************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int   count;
   short* itema;
  int            nitem;
/*.........................................................................*/
  if(This==NULL) return;
  nitem     = a_width * a_height;
  itema     = This;
  for (count=0;count<nitem;count++)
    { 
      *itema = a_value;
      itema++;
    }
}
/******************************************************************************/
void CImageshortCutLower (
 short* This
,Uint  a_width
,Uint  a_height
,short  a_cut
,short  a_value 
)
/******************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int   count;
   short* itema;
  int            nitem;
/*.........................................................................*/
  if(This==NULL) return;
  nitem     = a_width * a_height;
  itema     = This;
  for (count=0;count<nitem;count++)
    { 
      if((*itema)<a_cut) *itema = a_value;
      itema++;
    }
}
/******************************************************************************/
void CImageshortCutUpper (
 short* This
,Uint  a_width
,Uint  a_height
,short  a_cut
,short  a_value 
)
/******************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int   count;
   short* itema;
  int            nitem;
/*.........................................................................*/
  if(This==NULL) return;
  nitem     = a_width * a_height;
  itema     = This;
  for (count=0;count<nitem;count++)
    { 
      if((*itema)>a_cut) *itema = a_value;
      itema++;
    }
}
/******************************************************************************/
void CImageshortLinear (
 short* This
,Uint  a_width
,Uint  a_height
)
/******************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int   count;
   short* itema;
  int            nitem;
  double         a0, a1;
  short*          intens;
  int            out_array;
/*.........................................................................*/
  if(This==NULL)   return;

  intens      = (short*)CMemoryAllocateBlock(4096 * sizeof(short));
  if(intens==NULL) return;
  a1          = 0.6;
  a0          = 32767.0  / pow( (double)4095.0, a1); 
  for (count=1;count<4096;count++)  intens[count] = (short)(a0 * pow( (double) (4096.0/count-1), a1));
  out_array   = 0;

  nitem     = a_width * a_height;
  itema     = This;
  for (count=0;count<nitem;count++)
    { 
      if((*itema)>=(short)4096)
	{
	  out_array = 1;
	  *itema    = 0;
	} 
      else
	{
	  *itema    = intens[*itema];
	} 
      itema++;
    }
  if(out_array==1) ReportError("CImageshortLinear",CErrorFlow);

  CMemoryFreeBlock (intens);
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
short CImageshortGetPixel (
 short* This
,Uint  a_width
,Uint  a_height
,int  a_x
,int  a_y
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 0;
  a_height = 0; /*To avoid C++ warning.*/
  return *(This + a_y * a_width + a_x);
}
/***************************************************************************/
short* CImageshortGetPart (
 short* This
,Uint  a_width
,Uint  a_height
,int  a_px
,int  a_py
,Uint a_pw
,Uint a_ph
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int row,col;
  short* part;
  short* data;
  int   cbeg,cend;
  int   rbeg,rend;
/*.........................................................................*/
  if(This==NULL)                      return NULL;
  if( (a_width==0) || (a_height==0) ) return NULL;   
  if( (a_pw==0)    || (a_ph==0)     ) return NULL;   
/*check limits*/
  rbeg = a_py;
  rend = a_py + a_ph;
  cbeg = a_px;
  cend = a_px + a_pw;
  data = part = (short*)CMemoryAllocateBlock ( a_pw * a_ph * sizeof(short));
  for(row=rbeg;row<rend;row++)
    { 
       short* itema;
      itema = This + row * a_width + cbeg;
      for(col=cbeg;col<cend;col++)
	{
	  *data = *itema;
	  data++;
	  itema++;
	}
    }
  return part;
}
/******************************************************************************/
void CImageshortPutBitmapInXBM_Stream (
 short* This
,Uint  a_width
,Uint  a_height
,char* a_name
,void* a_file
)
/******************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  unsigned int row,col;
  int           ibyte;
/*............................................................................*/
  if(This==NULL) return;
  if( (a_width==0) || (a_height==0) ) return;  
  if(a_file==NULL) return;
  if( (a_name==NULL) || (*a_name=='\0')) return;

/*      Ouverture du fichier :            */
  fprintf ((FILE*)a_file,"\
#define %s_width %u\n\
#define %s_height %u\n\
static char %s_bits[] = {\n\
",a_name,a_width,
  a_name,a_height,
  a_name);
  
  ibyte        = 0;
  for(row=0;row<a_height;row++)
    { 
       short* itema;
       int   count;
      unsigned char  byte;
      itema = This + row * a_width;
      count = 0;      
      byte  = 0;
      for(col=0;col<a_width;col++)
        {
          byte = (*itema)==0 ? byte : byte + (1<<count);
          if(count==7) 
            {
              fprintf ((FILE*)a_file," 0x%2.2x,",(int)byte);
              count = 0;
              byte  = 0;
              if(ibyte==11) {fprintf((FILE*)a_file,"\n");ibyte=0;}
              else          {ibyte++;}
            } 
          else 
            {count++;}

          itema++;
        }
      if(count!=0) /* end of row: a not written byte remain; write it */
        {
          fprintf ((FILE*)a_file," 0x%2.2x,",(int)byte);
          count = 0;
          byte  = 0;
          if(ibyte==11) {fprintf((FILE*)a_file,"\n");ibyte=0;}
          else          {ibyte++;}
        } 
    }

/* last char is ',' go back to overwrite it*/
  fseek   ((FILE*)a_file,(long int)-1,SEEK_CUR);
  fprintf ((FILE*)a_file,"};\n");
}
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
void CImageshortFilterMedian (
 short* This
,Uint  a_width
,Uint  a_height
,short* a_in_items
,Uint  a_in_width
,Uint  a_in_height
,double a_r 
)
/******************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{ 
  int   x,y,cols,rows;
  int   r,r2,dimTab;
  short* Tab;
/*............................................................................*/
  if(This==NULL)                return;
  if( (a_width==0) || (a_height==0) ) return;  
  if(a_in_items==NULL)                   return;
  if( (a_in_width==0) || (a_in_height==0) ) return;  
  if(This==a_in_items) return;  /*Can't be same image.*/

  cols   = MINIMUM(a_in_width ,a_width);
  rows   = MINIMUM(a_in_height,a_height);

  r2     = DBL_INT (a_r * a_r);
  r      = DBL_INT a_r;

  dimTab   = 2 * r + 1 ; 
  dimTab   = dimTab * dimTab;
  Tab      = (short*)CMemoryAllocateBlock((size_t)(dimTab*sizeof(short)));
  if(Tab==NULL) return;

 {short*    sipt_in;
  short*    sipt_out;
  for (y=0; y < rows; y++ )
    {
      sipt_in   = a_in_items   + y * a_in_width;
      sipt_out  = This         + y * a_width;
      for (x=0 ; x < cols; x++)
      {
         int  i,j,k;
         k = 0;
         for (i= -r; i<=r; i++)
          for (j= -r; j<=r; j++)
            {
	      if (   ((x+i)>=0) && ((x+i)<cols) &&
		     ((y+j)>=0) && ((y+j)<rows) &&
		     ((i*i+j*j)<r2)     )
                 {
 		   Tab[k] = *(sipt_in + j * ((int)a_in_width) + i);
		   k++;
                 }
            }
         qsort(Tab, (size_t)k, (size_t)sizeof(short),(CompareFunction)Compare);
         *sipt_out = Tab[(k-1)/2];
         sipt_in++;
         sipt_out++;
      }
    }}

  CMemoryFreeBlock (Tab);
}
/******************************************************************************/
static int Compare (
 short* a_int1
,short* a_int2
)
/******************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{ 
/*............................................................................*/
  if( (a_int1==NULL) || (a_int2==NULL) ) return 1;
  return (int)((*a_int1) - (*a_int2)); 
}
/******************************************************************************/
void CImageshortFilterMinimum (
 short* This
,Uint  a_width
,Uint  a_height
,short* a_in_items
,Uint  a_in_width
,Uint  a_in_height
,double a_r 
)
/******************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{ 
  int  x,y,cols,rows;
  int  r,r2;
/*............................................................................*/
  if(This==NULL)                return;
  if( (a_width==0) || (a_height==0) ) return;  
  if(a_in_items==NULL)                   return;
  if( (a_in_width==0) || (a_in_height==0) ) return;  
  if(This==a_in_items) return;  /*Can't be same image.*/

  cols   = MINIMUM(a_in_width ,a_width);
  rows   = MINIMUM(a_in_height,a_height);

  r2     = DBL_INT (a_r * a_r);
  r      = DBL_INT a_r;

  for (y=0; y < rows; y++ )
    {
      short*    sipt_in;
      short*    sipt_out;
      sipt_in   = a_in_items   + y * a_in_width;
      sipt_out  = This         + y * a_width;
      for (x=0 ; x < cols; x++)
       {
	 short zMin;
         int  i,j;
         zMin = short_max;
         for (i= -r; i<=r; i++)
          for (j= -r; j<=r; j++)
            {
	      if (   ((x+i)>=0) && ((x+i)<cols) &&
		     ((y+j)>=0) && ((y+j)<rows) &&
		     ((i*i+j*j)<r2) )
                {
		  short z;
                  z    = *(sipt_in + j * ((int)a_in_width) + i);
                  zMin = MINIMUM(z,zMin);
                }
            }
         *sipt_out = zMin;
         sipt_in++;
         sipt_out++;
       }
   }

}
/******************************************************************************/
void CImageshortFilterMaximum (
 short* This
,Uint  a_width
,Uint  a_height
,short* a_in_items
,Uint  a_in_width
,Uint  a_in_height
,double a_r 
)
/******************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{ 
  int  x,y,cols,rows;
  int  r,r2;
/*............................................................................*/
  if(This==NULL)                         return;
  if( (a_width==0) || (a_height==0) ) return;  
  if(a_in_items==NULL)                   return;
  if( (a_in_width==0) || (a_in_height==0) ) return;  
  if(This==a_in_items)              return;  /*Can't be same image.*/

  cols = MINIMUM(a_in_width ,a_width);
  rows = MINIMUM(a_in_height,a_height);

  r2   = DBL_INT (a_r * a_r);
  r    = DBL_INT a_r;
  for (y=0; y < rows; y++ )
    {
      short*    sipt_in;
      short*    sipt_out;
      sipt_in   = a_in_items   + y * a_in_width;
      sipt_out  = This         + y * a_width;
      for (x=0 ; x < cols; x++)
      {
	 short zMax;
         int  i,j;
         zMax = short_min;
         for (i= -r; i<=r; i++)
          for (j= -r; j<=r; j++)
           {
             if (   ((x+i)>=0) && ((x+i)<cols) &&
                    ((y+j)>=0) && ((y+j)<rows) &&
                    ((i*i+j*j)<r2)     )
               {
		  short  z; 
                  z    = *(sipt_in + j * ((int)a_in_width) + i);
                  zMax = MAXIMUM (z,zMax);
               }
           }
         *sipt_out = zMax;
         sipt_in++;
         sipt_out++;
      }
    }

}
/******************************************************************************/
void CImageshortFilterBackground (
 short* This
,Uint  a_width
,Uint  a_height
,short* a_in_items
,Uint  a_in_width
,Uint  a_in_height
,double a_r 
,double a_nSig 
,int a_nRep 
)
/******************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{ 
  int      k,x,y,n,cols,rows;
  int      r,r2;
/*............................................................................*/
  if(This==NULL)                return;
  if( (a_width==0) || (a_height==0) ) return;  
  if(a_in_items==NULL)                   return;
  if( (a_in_width==0) || (a_in_height==0) ) return;  
  if(This==a_in_items) return;  /*Can't be same image.*/

  cols   = MINIMUM(a_in_width ,a_width);
  rows   = MINIMUM(a_in_height,a_height);

  r2     = DBL_INT (a_r * a_r);
  r      = DBL_INT a_r;

  for (y=0; y < rows; y++ )
    {
      short*    sipt_in;
      short*    sipt_out;
      sipt_in   = a_in_items   + y * a_in_width;
      sipt_out  = This         + y * a_width;
      for (x=0 ; x < cols; x++)
      {
       double   sum = 0., sum2, tronc;
       tronc = 100000.0;
       for (k=0; k<a_nRep; k++)
       {
         int  i,j;
         sum  = 0;
         sum2 = 0;
         n    = 0;
         for (i= -r; i<=r; i++)
          for (j= -r; j<=r; j++)
            {
             if (   ((x+i)>=0) && ((x+i)<cols) &&
                    ((y+j)>=0) && ((y+j)<rows) &&
                    ((i*i+j*j)<r2)     )
                {
		  short z;
                  z    = *(sipt_in + j * ((int)a_in_width) + i);
                  if (((double)z)<tronc) 
                    {
                      sum  += z;
                      sum2 += z * z;
                      n++;
                    }
                }
            }
         sum /= n;
         sum2/= n;
         /*printf("debug: x=%d y=%d k=%d n=%d moy=%g sig=%g \n",x,y,k,n,sum,sqrt(sum2 - sum*sum));*/
         tronc = sum + a_nSig * sqrt(sum2 - sum * sum);
       } 
       *sipt_out = (short)(float)sum;
       sipt_in++;
       sipt_out++;
      }
   }
}
/******************************************************************************/
void CImageshortFilterLinear (
 short* This
,Uint  a_width
,Uint  a_height
,short* a_in_items
,Uint  a_in_width
,Uint  a_in_height
)
/******************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int    i,j,imx,jmx;
  double a0, a1;
  short*  intens;
  int    out_array;
/*............................................................................*/
  if(This==NULL)                         return;
  if( (a_width==0) || (a_height==0) ) return;  
  if(a_in_items==NULL)                   return;
  if( (a_in_width==0) || (a_in_height==0) ) return;  

  imx = MINIMUM(a_width ,a_in_width);
  jmx = MINIMUM(a_height,a_in_height);

  intens = (short*)CMemoryAllocateBlock(4096 * sizeof(short));
  if(intens==NULL) return;
  a1 = 0.6;
  a0 = 32767.0  / pow( (double)4095.0, a1); 
  for (i=1; i<4096;i++)  intens[i] = (short)(a0 * pow( (double) (4096.0/i-1), a1));

  out_array = 0;
  for(j=0;j<jmx;j++)
    {
      short*    sipt_in;
      short*    sipt_out;
      sipt_in   = a_in_items  + j * a_in_width;
      sipt_out  = This        + j * a_width;
      for(i=0;i<imx;i++)
        {
          if(*sipt_in>=4096)
            {
              out_array = 1;
              *sipt_out = 0;
            } 
          else
            {
              *sipt_out = intens[*sipt_in];
            } 
          sipt_in++;
          sipt_out++;
       }
    }
  if(out_array==1) ReportError ("CImageshortFilterLinear",CErrorFlow);

  CMemoryFreeBlock (intens);
}
/******************************************************************************/
void CImageshortApplyLowThresold (
 short* This
,Uint  a_width
,Uint  a_height
,short* a_in_items
,Uint  a_in_width
,Uint  a_in_height
,short  a_seuil 
,short  a_val 
)
/******************************************************************************/
/* Could be same image.                                                       */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int i,j,imx,jmx;
/*............................................................................*/
  if(This==NULL)                         return;
  if( (a_width==0) || (a_height==0) ) return;  
  if(a_in_items==NULL)                   return;
  if( (a_in_width==0) || (a_in_height==0) ) return;  

  imx = MINIMUM(a_width ,a_in_width);
  jmx = MINIMUM(a_height,a_in_height);

  for(j=0;j<jmx;j++)
    {
      short*    sipt_in;
      short*    sipt_out;
      sipt_in   = a_in_items   + j * a_in_width;
      sipt_out  = This         + j * a_width;
      for(i=0;i<imx;i++)
        {
          *sipt_out =  (*sipt_in)<a_seuil ? a_val : *sipt_in;
          sipt_in++;
          sipt_out++;
       }
    }
}
/******************************************************************************/
void CImageshortApplyHighThresold (
 short* This
,Uint  a_width
,Uint  a_height
,short* a_in_items
,Uint  a_in_width
,Uint  a_in_height
,short  a_seuil 
,short  a_val 
)
/******************************************************************************/
/* Could be same image.                                                       */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int i,j,imx,jmx;
/*............................................................................*/
  if(This==NULL)                         return;
  if( (a_width==0) || (a_height==0) ) return;  
  if(a_in_items==NULL)                   return;
  if( (a_in_width==0) || (a_in_height==0) ) return;  

  imx = MINIMUM(a_width ,a_in_width);
  jmx = MINIMUM(a_height,a_in_height);

  for(j=0;j<jmx;j++)
    {
      short*    sipt_in;
      short*    sipt_out;
      sipt_in   = a_in_items   + j * a_in_width;
      sipt_out  = This         + j * a_width;
      for(i=0;i<imx;i++)
        {
          *sipt_out =  (*sipt_in)>a_seuil ? a_val : *sipt_in;
          sipt_in++;
          sipt_out++;
       }
    }

}
/******************************************************************************/
void CImageshortOperateUnary (
 short* This
,Uint  a_width
,Uint  a_height
,short* a_in_items
,Uint  a_in_width
,Uint  a_in_height
,int   a_oper 
,short  a_val 
)
/******************************************************************************/
/* Could be same image.                                                       */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int i,j,imx,jmx;
/*............................................................................*/
  if(This==NULL)                return;
  if( (a_width==0) || (a_height==0) ) return;  
  if(a_in_items==NULL)                   return;
  if( (a_in_width==0) || (a_in_height==0) ) return;  

  imx = MINIMUM(a_width ,a_in_width);
  jmx = MINIMUM(a_height,a_in_height);
 
  switch(a_oper)
    {
      case CImageSub:a_val = -a_val; 
      case CImageAdd:
                    for(j=0;j<jmx;j++)
                      {
			short*    sipt_in;
			short*    sipt_out;
                        sipt_in   = a_in_items   + j * a_in_width;
                        sipt_out  = This         + j * a_width;
                        for(i=0;i<imx;i++)
                          {
                            *(sipt_out) = (*sipt_in) + a_val;
                            sipt_in++;
                            sipt_out++;
                          }
                      }
                    break;
      case CImageMul:
                    for(j=0;j<jmx;j++)
                      {
			short*    sipt_in;
			short*    sipt_out;
                        sipt_in   = a_in_items   + j * a_in_width;
                        sipt_out  = This         + j * a_width;
                        for(i=0;i<imx;i++)
                          {
                            *(sipt_out) = (*sipt_in) * a_val;
                            sipt_in++;
                            sipt_out++;
                          }
                      }
                    break;
      case CImageDiv:if(a_val==0) {ReportError("CImageshortOperateUnary",CErrorValue);return;}
                    for(j=0;j<jmx;j++)
                      {
			short*    sipt_in;
			short*    sipt_out;
                        sipt_in   = a_in_items   + j * a_in_width;
                        sipt_out  = This         + j * a_width;
                        for(i=0;i<imx;i++)
                          {
                            *(sipt_out) = (*sipt_in) / a_val;
                            sipt_in++;
                            sipt_out++;
                          }
                      }
                    break;
    }

}
/******************************************************************************/
void CImageshortOperateBinary (
 short* This
,Uint  a_width
,Uint  a_height
,short* a_in1_items
,Uint  a_in1_width
,Uint  a_in1_height
,short* a_in2_items
,Uint  a_in2_width
,Uint  a_in2_height
,int   a_oper 
,short  a_val1 
,short  a_val2 
)
/******************************************************************************/
/* Could be same image.                                                       */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  unsigned int i,j,imx,jmx;
  int div_zero;
/*............................................................................*/
  if(This==NULL)                return;
  if( (a_width==0) || (a_height==0) ) return;  
  if(a_in1_items==NULL)                   return;
  if( (a_in1_width==0) || (a_in1_height==0) ) return;  
  if(a_in2_items==NULL)                   return;
  if( (a_in2_width==0) || (a_in2_height==0) ) return;  

  imx = MINIMUM(a_in1_width ,a_in2_width);
  jmx = MINIMUM(a_in1_height,a_in2_height);
  imx = MINIMUM(a_width ,imx);
  jmx = MINIMUM(a_height,jmx);

  switch(a_oper)
    {
      case CImageSub:
                    for(j=0;j<jmx;j++)
                      {
			short*    sipt_in1;
			short*    sipt_in2;
			short*    sipt_out;
			sipt_in1 = a_in1_items + j * a_in1_width;
			sipt_in2 = a_in2_items + j * a_in2_width;
			sipt_out = This        + j * a_width;
			for(i=0;i<imx;i++)
			  {
			    *sipt_out =  ((*sipt_in1) * a_val1) - ( (*sipt_in2) * a_val2) ;
			    sipt_in1++;
			    sipt_in2++;
			    sipt_out++;
			  }
                      }
                    break;
      case CImageAdd:
                    for(j=0;j<jmx;j++)
                      {
			short*    sipt_in1;
			short*    sipt_in2;
			short*    sipt_out;
			sipt_in1 = a_in1_items + j * a_in1_width;
			sipt_in2 = a_in2_items + j * a_in2_width;
			sipt_out = This        + j * a_width;
			for(i=0;i<imx;i++)
			  {
			    *sipt_out =  ((*sipt_in1) * a_val1) + ( (*sipt_in2) * a_val2) ;
			    sipt_in1++;
			    sipt_in2++;
			    sipt_out++;
			  }
                      }
                    break;
      case CImageMul:
                    for(j=0;j<jmx;j++)
                      {
			short*    sipt_in1;
			short*    sipt_in2;
			short*    sipt_out;
			sipt_in1 = a_in1_items + j * a_in1_width;
			sipt_in2 = a_in2_items + j * a_in2_width;
			sipt_out = This        + j * a_width;
			for(i=0;i<imx;i++)
			  {
			    *sipt_out =  ((*sipt_in1) * a_val1) * ( (*sipt_in2) * a_val2) ;
			    sipt_in1++;
			    sipt_in2++;
			    sipt_out++;
			  }
                      }
                    break;
      case CImageDiv:
                    div_zero = 0;
                    for(j=0;j<jmx;j++)
                      {
			short*    sipt_in1;
			short*    sipt_in2;
			short*    sipt_out;
			sipt_in1 = a_in1_items + j * a_in1_width;
			sipt_in2 = a_in2_items + j * a_in2_width;
			sipt_out = This        + j * a_width;
			for(i=0;i<imx;i++)
			  { short val;
			    val  =  (*sipt_in2) * a_val2 ;
			    if(val==0)
			      {
				div_zero  = 1;
				*sipt_out = 0;
			      }
			    else
			      {
				*sipt_out =  ((*sipt_in1) * a_val1)  / val;
			      }
			    sipt_in1++;
			    sipt_in2++;
			    sipt_out++;
			  }
                      }
                    if(div_zero==1) ReportError("CImageshortOperateBinary",CErrorValue);
                    break;
    }

}
/******************************************************************************/
void CImageshortConvoluteByMatrix (
 short* This
,Uint  a_width
,Uint  a_height
,short* a_in_items
,Uint  a_in_width
,Uint  a_in_height
,int*  a_i33
,int   a_invsc
,short  a_vbord     
)
/******************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int     i,j,imx,jmx;
  short*   sipt_in;
  short*   sipt_out;
/*............................................................................*/
  if(This==NULL)                return;
  if( (a_width==0) || (a_height==0) ) return;  
  if(a_in_items==NULL)                   return;
  if( (a_in_width==0) || (a_in_height==0) ) return;  
  if(This==a_in_items) return;  /*Can't be same image.*/
  if(a_i33==NULL) return;
  if(a_invsc==0) {ReportError("CImageshortConvoluteByMatrix",CErrorValue);return;}

  imx   = MINIMUM(a_in_width ,a_width);
  jmx   = MINIMUM(a_in_height,a_height);

  for(j=1;j<jmx-1;j++)
    {
      for(i=1;i<imx-1;i++)
        { int     conv;
          int     jl;
          sipt_in = a_in_items + j * ((int)a_in_width) + i;
          conv    = 0;
          for(jl=0;jl<3;jl++)
            {
	       short* sipt;
               int   ic;
               sipt  = sipt_in + (jl-1) * ((int)a_in_width) - 1;
               for(ic=0;ic<3;ic++)
                 {
                    conv += (*sipt) * MVAL(a_i33,jl,ic);
                    sipt++;
                 }
            }
          sipt_out    = This + j * ((int)a_width) + i;
          *(sipt_out) = (short)(conv/a_invsc);
        }
    }

/*    Bordure :  */
/* 1ere ligne de l'image  */
  sipt_out = This;
  for(i=0;i<imx;i++)
    {
      *(sipt_out) = a_vbord;
      sipt_out++;
    }
/* derniere ligne de l'image  */
  sipt_out = This + (jmx-1) * ((int)a_width);
  for(i=0;i<imx;i++)
    {
      *(sipt_out) = a_vbord;
      sipt_out++;
    }
/* 1ere colonne de l'image  */
  sipt_out  = This;
  for(j=0;j<jmx;j++)
    {
      *(sipt_out) = a_vbord;
      sipt_out   += a_width;
    }
/* derniere colonne de l'image  */
  sipt_out  = This + ((int)a_width) - 1;
  for(j=0;j<jmx;j++)
    {
      *(sipt_out) = a_vbord;
      sipt_out   += a_width;
    }

}
/***************************************************************************/
/***************************************************************************/
/******* rebinning *********************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static void Transform (
 double xDest
,double yDest
,double* XSrc
,double* YSrc
)
/***************************************************************************/
/*     application de la transformation                                    */
/*    Cette routine prend des coordonnees en mm et applique une            */
/*  transformation polynomiale et fournit les coordonnees                  */
/*   XSrc,YSrc                                                             */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  double xIn,yIn;
  double xo = 0.0, yo = 0.0;
/*.........................................................................*/
  xIn = (xDest - Class.xmoy[0] )/Class.sigx[0];
  yIn = (yDest - Class.ymoy[0] )/Class.sigy[0];
  if (Class.Npol==1)
    {
      xo = (double)(Class.Polxy[0][0][0] + Class.Polxy[1][0][0] * xIn +
		    Class.Polxy[0][1][0] * yIn);
      yo = (double)(Class.Polxy[0][0][1] + Class.Polxy[1][0][1] * xIn +
		    Class.Polxy[0][1][1] * yIn);
    }
  else if(Class.Npol<=4)
    {
      int    k,l;
      double x1,y1;
      xo = (double)0.0;
      yo = (double)0.0;
      x1 = (double)1.0 ;
      for (k=0;k<=Class.Npol;k++)
	{
	  y1 = (double)1.0;
	  for (l=0;l<=Class.Npol-k;l++)
	    {
	      xo += (double)Class.Polxy[k][l][0] * x1 * y1;
	      yo += (double)Class.Polxy[k][l][1] * x1 * y1;
	      y1 *= (double)yIn;
	    }
	  x1 *= (double)xIn;
	}
    }
  else
    {
      xo = xIn;
      yo = yIn;
    }
  *XSrc  = ( Class.sigx[1] * xo + Class.xmoy[1] ) ;
  *YSrc  = ( Class.sigy[1] * yo + Class.ymoy[1] ) ;
}
/***************************************************************************/
static double GetWeight (
 int xInt
,int yInt
,double XSrc
,double YSrc
)
/***************************************************************************/
/* Routine utilisee par la methode d'interpolation lineaire ponderee       */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  XSrc = fabs (XSrc -xInt);
  YSrc = fabs (YSrc -yInt);
  if (XSrc <= 1. && YSrc <= 1.) return ( (1. - XSrc) * (1. - YSrc) );
  else                          return (0.0);
}
/***************************************************************************/
static double GetBSpline ( 
 int degre 
,int i
,double s
,int degini
)
/***************************************************************************/
/* Routine calculant la valeur d'une fonction BSpline de degre quelconque  */  
/* centree sur un intervalle i                                             */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  double temp = 0.0;
/*.........................................................................*/
  if ( (degre ==  degini) )  s = s - 0.5*(degini-1);   
  switch  (degre)
    {
    case 0 :
      if ( s >= (double) (i-1) && s < (double) i ) temp = 1.0;
      break;
    default:
      temp = ( ( ( s- (double) (i-degre-1) )  * GetBSpline(degre-1,i-1,s,degini)                 
		+ (    (double)i - s        ) * GetBSpline(degre-1,i  ,s,degini) 
		)) /  ( degre +1)  ;
      break;
    } 
  return temp;
}
/***************************************************************************/
static void GetGauss ( 
 double* matrice 
,double* coef
,int     Nbint
)
/***************************************************************************/
/* Pivotation d'une matrice par methode de Gauss                           */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int i,j,k,max;
  double temp;
/*.........................................................................*/
  for (j=0;j<Nbint;j++)
    {
      max = j;
      for (i=j+1;i<=Nbint-1;i++)
	if ( fabs(matrice[j+i*(Nbint+1)]) > 
	    fabs(matrice[j+max*(Nbint+1)]) ) max = i;
      for (k=0;k<=Nbint;k++)
	{
          temp = matrice[k+j*(Nbint+1)];
          matrice[k+j*(Nbint+1)] = matrice[k+max*(Nbint+1)];
	  matrice[k+max*(Nbint+1)] = temp ;
	}
      for (i=j+1;i<Nbint;i++)
	for (k=Nbint;k>=j;k--)
	  matrice[k+i*(Nbint+1)] = matrice[k+i*(Nbint+1)]
	    - matrice[k+j*(Nbint+1)]
	      * matrice[j+i*(Nbint+1)]
		/ matrice[j+j*(Nbint+1)];
    }
  for (j=Nbint-1;j>=0;j--)
    {
      temp = 0.0;
      for (k=j+1;k<Nbint;k++)
	{
	  temp = temp + coef[k] * matrice[k+j*(Nbint+1)];
	}
      coef[j] = ( matrice[Nbint + j*(Nbint+1)] -temp )	/ matrice[j + j*(Nbint+1)];
    }
}
/***************************************************************************/
static void SetMatrix (
 double* matrice
,int degre
,int Nbint
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int i,j;
/*.........................................................................*/
  for (j=0;j<Nbint;j++)
    {
      for (i=0;i<Nbint;i++)
        { 
	  matrice [i + j * (Nbint+1)] = GetBSplineByTable(degre,i+1,(double)j);
        }
     }
}    
/***************************************************************************/
static double GetSpline (
 double* coef
,double s
,int degre
,int Nbint
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int i;
   double temp =0.0;
  int v;
/*.........................................................................*/
  v = 1+(int)(s) - (int)(Nbint/2);
  for (i=0;i<Nbint;i++) 
    {
      temp += coef[i] * GetBSplineByTable(degre,i+v,s);
    }
  return temp;
}
/***************************************************************************/
static void InitializeBSplineTable (
 int degre
)
/***************************************************************************/
/* Tabulation des valeurs prises par une fonction BSpline en fonction de son */
/* degre                                                                     */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int i;
/*.........................................................................*/
  for (i=0;i<700;i++)
    {
      Class.Tab_Bspl[i] = GetBSpline(degre,7,((double)i / 100) + 0.5 * (degre-1),degre);
    }
}
/***************************************************************************/
static double GetBSplineByTable (
 int degre
,int i
,double s
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  s       = s - 0.5 * (degre-1);  
  return  (((s > i-7) && (s < i )) ? Class.Tab_Bspl[(int)( 100* ( s - i + 7 ) )] : 0);
}
/***************************************************************************/
static void  InvertMatrix (
 double* matrice
,int degre
,int Nbint
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int j,jj;
  double m[200];
/*.........................................................................*/
  InitializeBSplineTable (degre);
  for(j=0;j<Nbint;j++)
    {
      SetMatrix (m,degre,Nbint);
      for (jj=0;jj<Nbint;jj++) 	m[Nbint +jj*(Nbint+1)] = (double)(0.0);
      m[Nbint + j * (Nbint+1)] = (double)(1.0);
      GetGauss (m ,  (matrice+j*(Nbint+1)) ,Nbint);
    }
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static double GetSplineCentered (
 short* This
,Uint  a_width
,Uint  a_height
,double* matrice
,int Nbint
,int degre
,double X
,double Y
,double* coef
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int i;
   double* PtrMat;
   short* p1  ;
   double* pCoef;
   int j;
   short* p0 ;
   int dec;
  double   Pixval[20] ;
  int      x,y;
  int      nbint;
/*.........................................................................*/
  if(This==NULL)                  return 0.;
  if( (a_width==0) || (a_height==0) ) return 0.;  

  x = (int)X;
  y = (int)Y;

/* **************** INITIALISATION  ******************** */

  nbint = (int)(Nbint/2);
  p0    = This + x - nbint  + (y - nbint) * a_width;
  p1    = p0;

/**********************************************************************/
  for (dec=0;dec<Nbint;dec++)
    {
      PtrMat  = matrice;
      pCoef   = coef;
      for (j=0;j<Nbint;j++)
	{      
	  *pCoef   = 0;
	  p1       = p0;
	  for (i=0;i<Nbint;i++) 
	    {
	      *pCoef  =  (*pCoef) + (*PtrMat) * (double)(*p1);
	      p1     += a_width;
	      PtrMat++;
	    } 
	  PtrMat++;
	  pCoef++;
	}
    Pixval[dec] = GetSpline(coef,Y,degre,Nbint);
    p0++;
  }

  PtrMat  = matrice;
  pCoef   = coef;
  for (j=0;j<Nbint;j++)
    {
      *pCoef = 0;
      for (i=0;i<Nbint;i++) 
        {
	  *pCoef = (*pCoef) + (*PtrMat) * Pixval[i] ;
	  PtrMat++;
	}
      PtrMat++; 
      pCoef++;
    }
  return  (GetSpline(coef,X,degre,Nbint));
}
/***************************************************************************/
static double InterpolateFirstOrder (
 short* This
,Uint  a_width
,Uint  a_height
,double XSrc
,double YSrc
)
/***************************************************************************/
/* Methode d'interpolation lineaire                                        */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{ 
  int x,y;
  int f00,f01,f10,f11;
/*.........................................................................*/
  if(This==NULL)                  return 0.;
  if( (a_width==0) || (a_height==0) ) return 0.;  
  x = (int)XSrc;
  y = (int)YSrc;
  XSrc = XSrc - x;
  YSrc = YSrc - y;
  f00 = This [ x +         y * a_width];
  f01 = This [ x +     (y+1) * a_width];
  f10 = This [ x + 1 +     y * a_width];
  f11 = This [ x + 1 + (y+1) * a_width];
  return  ( XSrc    *YSrc    *f11
	  + XSrc    *(1-YSrc)*f10
	  + (1-XSrc)*YSrc    *f01
	  + (1-XSrc)*(1-YSrc)*f00 );
}
/***************************************************************************/
static double InterpolateSpline (
 short* This
,Uint  a_width
,Uint  a_height
,double XSrc
,double YSrc
)
/***************************************************************************/
/* Methode la plus simple (simpliste) d'interpolation                      */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{   
/*.........................................................................*/
  if(This==NULL)                  return 0.;
  if( (a_width==0) || (a_height==0) ) return 0.;  
  return This [(int) XSrc + ((int) YSrc) * a_width];
}
/***************************************************************************/
static double InterpolateWithWeight (
 short* This
,Uint  a_width
,Uint  a_height
,double XSrc
,double YSrc
)
/***************************************************************************/
/* Autre Methode d'interpolation lineaire   ( Ponderation )                */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{   
  int xInt , yInt ,i , j;
  double Temp = 0.0;
/*.........................................................................*/
  if(This==NULL)                  return 0.;
  if( (a_width==0) || (a_height==0) ) return 0.;  

  xInt = (int)XSrc;       
  yInt = (int)YSrc;
  for (i=xInt-1;i<=xInt+2;i++)
     for (j=yInt-1;j<=yInt+2;j++)
        {
	  if (GetWeight(i,j,XSrc,YSrc)>0.001)
	    {
	      Temp = Temp + This [i + j * a_width] * GetWeight(i,j,XSrc,YSrc);
	    }
        }
  XSrc = Temp ;
  return XSrc;
}
/***************************************************************************/
void CImageshortChangeBinning (
 short* This
,Uint  a_width
,Uint  a_height
,short* a_in_items
,Uint  a_in_width
,Uint  a_in_height
,int a_x
,int a_y
,int a_in_x
,int a_in_y
,int Methode
,int Nbint
,int degre
,double EchX
,double EchY
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{ 
  unsigned int    col,row;
  int    i,imax,imin,jmin,jmax;
  double XSrc,YSrc,XDst,YDst;
  double coef[20];
  double matrice[200];
  short*  pixDst;
/*.........................................................................*/
  if(This==NULL)                         return;
  if( (a_width==0) || (a_height==0) ) return;  
  if(a_in_items==NULL)                   return;
  if( (a_in_width==0) || (a_in_height==0) ) return;  
  if(This==a_in_items) return;  /*Can't be same image ???*/

  if(Class.Npol==0) CImageshortLoadTransformFile ("image.transf");

  for (i=0;i<200;i++) matrice[i] = 0.0;

  jmin = (int)(Nbint/2);
  imin = (int)(Nbint/2);
  jmax = a_in_height - jmin;
  imax = a_in_width  - imin;

/* ########################################################## */
  pixDst = This;
  if ( Methode >= 4 )
    {
      InvertMatrix (matrice,degre,Nbint);
      
      for (row=0; row<a_height; row++)    
        for (col=0; col<a_width ; col++)
          {

/*     J'applique le facteur d'echelle , Offset et NumPix -> MM  */
	    XDst = (double)(col)*EchX+(double)(a_x-1);
	    XDst *= Class.PixSizX;
	    YDst = (double)(row)*EchY+(double)(a_y-1);
	    YDst *= Class.PixSizY;

/*    J'applique transf. geometrique   */
	    Transform (XDst,YDst,&XSrc,&YSrc); 

/*     Je transforme X et Y (en MM sur l'image) en Numero de Pixel */      
	    XSrc  = XSrc/Class.PixSizX - (double)(a_in_x-1);
	    YSrc  = YSrc/Class.PixSizY - (double)(a_in_y-1);

	    if ( (int)XSrc >imin && (int)XSrc <imax && (int)YSrc>jmin && (int)YSrc<jmax )
	      {
		double  tem;
		tem     = GetSplineCentered (a_in_items,a_in_width,a_in_height,matrice,Nbint,degre,XSrc,YSrc,coef); 
		*pixDst = (tem>short_max) ? short_max : (short) tem;      
	      }
            else
              {
		*pixDst = 0; 
	      }

	    pixDst++;
	  }            
    }
  else    
    {  double (*Interpol) (short*,Uint,Uint,double,double);
  
       switch (Methode)
	  { case 1:  Interpol = InterpolateSpline;
		     break;
	    case 2:  Interpol = InterpolateWithWeight;
		     break;
	    case 3:  Interpol = InterpolateFirstOrder;
		     break; 
	    default: ReportError("CImageshortChangeBinning",CErrorCase);
		     return;
           }
    
      for (row=0; row<a_height; row++)
	for (col=0; col<a_width; col++)
	  { 
    /*	   J'applique le facteur d'echelle , Offset et NumPix -> MM  */
	   XDst = (double)(col)*EchX+(double)(a_x-1);
	   XDst *= Class.PixSizX;
	   YDst = (double)(row)*EchX+(double)(a_y-1);
	   YDst *= Class.PixSizY;
    
	   Transform(XDst,YDst,&XSrc,&YSrc);
    
    /*	   Je transforme X et Y (en MM sur l'image) en Numero de Pixel */      
	   XSrc = XSrc/Class.PixSizX - (double)(a_in_x-1);
	   YSrc = YSrc/Class.PixSizY - (double)(a_in_y-1);
    
	   *pixDst = (short)(( (int)XSrc >imin && (int)XSrc <imax && (int)YSrc>jmin && (int)YSrc<jmax ) ?
                       Interpol (a_in_items,a_in_width,a_in_height,XSrc,YSrc) : 0); 
	   pixDst++;
	 }
     }
}
/***************************************************************************/
void CImageshortTransform (
 short* This
,Uint  a_width
,Uint  a_height
,int a_OffX
,int a_OffY
,double a_fac
)
/***************************************************************************/
/*               Reza 13/08/91                                             */
/*  Constitution de l'image des deformations de la                         */
/*  transformation geometrique                                             */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  unsigned int row,col;
  short*  pixDst;
/*.........................................................................*/
  if(This==NULL)                      return;
  if( (a_width==0) || (a_height==0) ) return;  

  if(Class.Npol==0) CImageshortLoadTransformFile ("image.transf");

  pixDst = This;
  for(row=0;row<a_height;row++)
    {
      for(col=0;col<a_width;col++)
	{
	  double XDst,YDst,XSrc,YSrc;
	  short   sk;
	  /*    J'applique le facteur d'echelle , Offset et NumPix -> MM  */
	  XDst   = (double)(col+a_OffX-1) * Class.PixSizX;
	  YDst   = (double)(row+a_OffY-1) * Class.PixSizY;
	  
	  Transform (XDst,YDst,&XSrc,&YSrc);

	  /*     Je transforme X et Y (en MM sur l'image) en Numero de Pixel */      
	  XSrc    = (XSrc/Class.PixSizX)-(double)(a_OffX-1); 
	  YSrc    = (YSrc/Class.PixSizY)-(double)(a_OffY-1);
     
	  sk      = (short) (a_fac * sqrt(XSrc*XSrc+YSrc*YSrc));

	  *pixDst = sk;  
          pixDst++;
	}
    }

}
/***************************************************************************/
void CImageshortLoadTransformFile (
 char* a_fname
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  FILE* file;
/*.........................................................................*/
  file = CFileOpenForReading (a_fname,NULL);
  if (file==NULL)  return;

  CInfoF ("Fichier %s ouvert.\n",a_fname);

  fscanf  (file,"%le%le%le%le",Class.xmoy,Class.sigx,Class.ymoy,Class.sigy);
  CInfoF   ("xmoy : %g xsig:%g ymoy:%g ysig:%g.\n",Class.xmoy[0],Class.sigx[0],Class.ymoy[0],Class.sigy[0]);

  fscanf  (file,"%le%le%le%le",Class.xmoy+1,Class.sigx+1,Class.ymoy+1,Class.sigy+1);
  CInfoF   ("xmoy : %g xsig:%g ymoy:%g ysig:%g.\n",Class.xmoy[1],Class.sigx[1],Class.ymoy[1],Class.sigy[1]);

  fscanf  (file,"%d",&Class.Npol);
  CInfoF  ("pol : %d.\n",Class.Npol);

  if(Class.Npol<=4) 
    {
      int i,j,k;
      for (i=0; i<=Class.Npol;i++)
	for (j=0; j<=Class.Npol-i;j++)
	  for (k=0;k<=1;k++)
	    {
	      fscanf(file,"%le",&(Class.Polxy[i][j][k]));
	      CInfoF("i : %d j : %d k : %d  xy : %g\n",i,j,k,Class.Polxy[i][j][k]);
        }
    }

  fclose(file);
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
  CErrorHandle ("Co","CImageshort",a_name,a_code);
}
