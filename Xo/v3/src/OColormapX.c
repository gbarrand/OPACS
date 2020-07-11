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

#include <CString.h>
#include <CMemory.h>
#include <CColor.h>
#include <CError.h>
#include <CPrinter.h>

#include <OColormap.h>

#include <XDisplay.h>

#include <OColormapX.h>

typedef unsigned long Ulong;

typedef struct _GColorRec *GColor;

typedef struct _GColorRec 
{
  double        red,green,blue;
  unsigned long pixel;
  int           allocated;
} GColorRec;

#ifdef __cplusplus
extern "C"{
#endif
static unsigned long ConvertRGB_ToPixel (double,double,double,int*);
static int           SolvePixels        (void*,int,unsigned long*,int*,double);
static void          ClearColors        ();
static void          ReportError        (char*,int);
#ifdef __cplusplus
}
#endif

#define MAX_COLORS   256
static struct 
{
  Display*  display;
  int       colori;
  GColorRec colors[MAX_COLORS];  
} Class = {NULL,0};
/******************************************************************************/
void OColormapXClearClass (
)
/******************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*............................................................................*/
  ClearColors   ();
  Class.display = NULL;
}
/***************************************************************************/
Ulong OColormapXGetPixel (
 void* This  
,int a_index
,Display* a_display
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  double        red,green,blue;
/*.........................................................................*/
  Class.display      = a_display;
  OColormapGetIthRGB ((OColormap)This,a_index,&red,&green,&blue);
  return             ConvertRGB_ToPixel (red,green,blue,NULL);
}
/***************************************************************************/
Ulong* OColormapXGetPixels (
 void* This  
,Display* a_display
,int    a_solve
,double a_prec
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  int    pixeln;
  Ulong* pixels;
  int*   oks;
/*.........................................................................*/
  if(This==NULL)   return NULL;
  pixeln      = OColormapGetSize((OColormap)This);
  if(pixeln==0) return NULL;
  pixels      = (Ulong*)CMemoryAllocateBlock(pixeln*sizeof(Ulong));
  oks         = (int*)  CMemoryAllocateBlock(pixeln*sizeof(int));
  if( (pixels==NULL) || (oks==NULL) ) 
    {
      CMemoryFreeBlock(pixels);
      CMemoryFreeBlock(oks);
      return NULL;
    }
  Class.display = a_display;
  for(count=0;count<pixeln;count++)
    {
      double        red,green,blue;
      OColormapGetIthRGB   ((OColormap)This,count,&red,&green,&blue);
      pixels[count] = ConvertRGB_ToPixel (red,green,blue,&(oks[count]));
    }
  if(a_solve!=0) 
    {
      int pbs;
      pbs = SolvePixels (This,pixeln,pixels,oks,a_prec);
      if(pbs!=0) 
	{
	  CWarnF("For OColormap %s; problem to resolve %d rgbs to pixels.\n",
	         OColormapGetName((OColormap)This)!=NULL ? 
		 OColormapGetName((OColormap)This) : "(nil)",pbs);
	}
    }
  CMemoryFreeBlock    (oks);
  return      pixels;
}
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
static int SolvePixels (
 void*  This
,int    a_pixeln
,Ulong* a_pixels
,int*   a_oks
,double a_prec 
)
/******************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  int      pbs;
/*............................................................................*/
  if(This==NULL) return 0;
  if( (a_pixeln==0) || (a_pixels==NULL) || (a_oks==NULL) ) return 0;
  pbs = 0;
  for(count=0;count<a_pixeln;count++)
    {
      if(a_oks[count]==0)
        { 
	   int index;
          int         found;
	  double      red,green,blue;
          found       = 0;
	  OColormapGetIthRGB ((OColormap)This,count,&red,&green,&blue);
	  for(index=0;index<a_pixeln;index++)
	    {
	      if(a_oks[index]==1)
                {
		  double      ired,igreen,iblue;
		  OColormapGetIthRGB ((OColormap)This,index,&ired,&igreen,&iblue);
  	          if( 
		     (fabs(red   - ired)   <= a_prec) &&
		     (fabs(green - igreen) <= a_prec) &&
		     (fabs(blue  - iblue)  <= a_prec) 
                    )
		    {
                      found = 1;
                      break;  
		    }
		}
            }
	  if(found==1) /*Found a neighbourg color.*/
            {
	      a_pixels [count] = a_pixels[index];
	    }
          else 
            {
	      CWarnF("Unable to find a pixel for: %d: %g %g %g.\n",count,red,green,blue);
	      pbs++;
	    }
	}
    }
  return pbs;
}
/***************************************************************************/
static Ulong ConvertRGB_ToPixel (
 double a_red
,double a_green
,double a_blue
,int*   a_status
)
/***************************************************************************/
/* From glDriverX.                                                         */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  int     found;
  Ulong   pixel;
  int     status,allocated;
/*.........................................................................*/
  if(a_status!=NULL) *a_status = 0;
  if(Class.display==NULL) return 0L;

  found = 0;
  for(count=0;count<Class.colori;count++)
    {
      if (
           (a_red   == Class.colors[count].red)   &&
           (a_green == Class.colors[count].green) &&
           (a_blue  == Class.colors[count].blue)  
         )
        {
          found = 1;
          break;
	}
    }
  if(found==1) 
    {
      if(a_status!=NULL) *a_status = 1;
      return Class.colors[count].pixel;
    }

  status = XDisplayGetPixelFromRGB (Class.display,
		          a_red,a_green,a_blue,
			  &pixel,&allocated);
  if(status==0) 
    {
/*      ReportError  ("ConvertRGB_ToPixel",CErrorConversion);*/
      return 0L;
    }

  if(Class.colori>=MAX_COLORS)  
    {
      ReportError  ("ConvertRGB_ToPixel",CErrorFlow);
      return pixel;
    }
  Class.colors[Class.colori].red       = a_red;  
  Class.colors[Class.colori].green     = a_green;
  Class.colors[Class.colori].blue      = a_blue;
  Class.colors[Class.colori].pixel     = pixel;
  Class.colors[Class.colori].allocated = allocated;
  Class.colori ++;
  if(a_status!=NULL) *a_status = 1;
  return       pixel;
}
/***************************************************************************/
static void ClearColors (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  Ulong        pixels[MAX_COLORS];
  int          pixeli = 0;
/*.........................................................................*/
  if(Class.display==NULL)  return;
  for(count=0;count<Class.colori;count++)
    {
      if(Class.colors[count].allocated==1)
	{
	  pixels[pixeli] = Class.colors[count].pixel;
	  pixeli ++;
	}
    }
  XFreeColors  (Class.display,DefaultColormapOfScreen(DefaultScreenOfDisplay(Class.display)),pixels,pixeli,0L);
  Class.colori = 0;
}
/******************************************************************************/
static void ReportError (
 char* a_name
,int a_code
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CErrorHandle ("Xo","OColormapX",a_name,a_code);
}
