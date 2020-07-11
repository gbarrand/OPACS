/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#include <stdlib.h>
#include <string.h>
 
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <CPrinter.h>
#include <CMemory.h>
#include <CString.h>
#include <CFile.h>
#include <CGIF.h>
#include <CTIFF.h>
#include <CFITS.h>
#include <CImageshort.h>
#include <CList.h>
#include <CError.h>

#include <XDisplay.h>


#include <OImageP.h>

typedef unsigned long Pixel;
typedef unsigned char Uchar;
typedef unsigned long Ulong;

#ifdef __cplusplus
extern "C"{
#endif
static void    FreeRecord          (OImage);
static int     GetIdentifier       (OImage,char*);
static XImage* SubstractImageshort (XImage*,int,int,unsigned int,unsigned int);
static Pixel   GetPixel8           (XImage*,int,int);
static int     PutPixel8           (XImage*,int,int,Pixel);
static Pixel   GetPixelshort       (XImage*,int,int);
static int     PutPixelshort       (XImage*,int,int,Pixel);
static void    ReportError         (char*,int);
#ifdef __cplusplus
}
#endif

#define DBL_INT  (int)(float)
#define NotFound (-1)

/*
static unsigned long low_bits_table[] = {
    0x00000000, 0x00000001, 0x00000003, 0x00000007,
    0x0000000f, 0x0000001f, 0x0000003f, 0x0000007f,
    0x000000ff, 0x000001ff, 0x000003ff, 0x000007ff,
    0x00000fff, 0x00001fff, 0x00003fff, 0x00007fff,
    0x0000ffff, 0x0001ffff, 0x0003ffff, 0x0007ffff,
    0x000fffff, 0x001fffff, 0x003fffff, 0x007fffff,
    0x00ffffff, 0x01ffffff, 0x03ffffff, 0x07ffffff,
    0x0fffffff, 0x1fffffff, 0x3fffffff, 0x7fffffff,
    0xffffffff
};
*/

static struct 
{
  OImage* extent;
} Class = {NULL};
/***************************************************************************/
void OImageClearClass (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CListDestroy  ((CList)Class.extent,(CListVisitEntryFunction)FreeRecord);
  Class.extent = NULL;
}
/***************************************************************************/
OImage* OImageGetIdentifiers (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return Class.extent;
}
/***************************************************************************/
OImage OImageGetIdentifier  (
  char* a_name 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* pos;
/*.........................................................................*/
  if( (a_name==NULL) || (*a_name=='\0') ) return NULL;
  pos = strchr (a_name,'/');
  if( (pos!=NULL) && (strncmp(a_name,"OImage",6)==0) )
    {
      int        status;
      OImage     identifier;
      identifier = (OImage)CStringConvertToLong(pos+1,&status);
      if(status==0) /*Not a number, then assume object given by a string.*/
	{
	  return (OImage)CListFindEntry ((CList)Class.extent,(CListFindEntryFunction)GetIdentifier,pos+1);
	}
      else
	{
	  if(OImageIsValid(identifier)==0)
	    {
	      return NULL;
	    }
	  else
	    {
	      return identifier;
	    }
	}
    }
  else
    {
      return (OImage)CListFindEntry ((CList)Class.extent,(CListFindEntryFunction)GetIdentifier,a_name);
    }
}
/***************************************************************************/
static int GetIdentifier (
 OImage This
,char* a_name  
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return CStringCompare(OImageGetName(This),a_name);
}
/***************************************************************************/
OImage OImageMake (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OImage This;
/*.........................................................................*/
  This            = (OImage) CMemoryAllocateBlock (sizeof(OImageRecord));
  if(This==NULL)  return NULL;

  CListAddEntry   ((CList*)&(Class.extent),This);

  This->name     = NULL;
  This->fileName = NULL;
  This->fileType = NULL;

  This->colorn   = 0;
  This->reds     = NULL;
  This->greens   = NULL;
  This->blues    = NULL;

  This->display  = NULL;
  This->ximage   = NULL;

  return         This;
}
/***************************************************************************/
OImage OImageCreate (
 Display* a_display
,char* a_name 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OImage This;
/*.........................................................................*/
  This           = OImageMake();
  if(This==NULL) return NULL;
  This->name     = CStringDuplicate(a_name);
  This->display  = a_display;
  return         This;
}
/***************************************************************************/
OImage OImageCreateFromRootWindow (
 Display* a_display
,int a_beg_x
,int a_beg_y
,int a_end_x
,int a_end_y
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OImage       This = NULL;
  Pixmap       pixmap;
/*.........................................................................*/
  if(a_display==NULL) return NULL;
  This                = OImageCreate(a_display,"from_root");
  if(This==NULL)      return NULL;

  pixmap           = XDisplayGetWindowPixmap (a_display,XDefaultRootWindow(a_display),
					      a_beg_x,a_beg_y,a_end_x,a_end_y);
  This->ximage     = XDisplayGetPixmapImage  (a_display,pixmap);
  if(pixmap!=0L)   XFreePixmap (a_display,pixmap);

  XDisplayGetImageColormap    (a_display,This->ximage,
			       &(This->colorn),&(This->reds),&(This->greens),&(This->blues));

  return           This;
}
/***************************************************************************/
OImage OImageCreateFromXBM_File (
 Display* a_display
,char* a_fname  
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*     name;
  OImage    This = NULL;
  Pixmap    pixmap;
/*.........................................................................*/
  if(a_display==NULL)    return NULL;
  name                   = CFileGetName(a_fname);
  This                   = OImageCreate (a_display,name);
  CStringDelete          (name);
  if(This==NULL)         return NULL;
  This->fileName         = CStringDuplicate (a_fname);
  This->fileType         = CStringDuplicate ("xbm");

  pixmap                 = XDisplayLoadPixmapFile  (a_display,a_fname);
  This->ximage           = XDisplayGetPixmapImage (a_display,pixmap);
  if(pixmap!=0L)         XFreePixmap        (a_display,pixmap);

  This->colorn     = 2;
  This->reds        = (double*)CMemoryAllocateBlock(2 * sizeof(double));
  This->greens      = (double*)CMemoryAllocateBlock(2 * sizeof(double));
  This->blues       = (double*)CMemoryAllocateBlock(2 * sizeof(double));
  This->reds[0]     = 1.;
  This->greens[0]   = 1.;
  This->blues[0]    = 1.;
  This->reds[1]     = 0.;
  This->greens[1]   = 0.;
  This->blues[1]    = 0.;

  return           This;
}
/***************************************************************************/
OImage OImageCreateFromGIF_File (
 Display* a_display
,char* a_fname  
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*         name;
  OImage        This;
  FILE*         file;
  unsigned int  width,height;
  Uchar*        data;
  int           colorn;
  double*       reds;
  double*       greens;
  double*       blues;
/*.........................................................................*/
  if(a_display==NULL)    return NULL;

  file                   = CFileOpenForReading (a_fname,NULL);
  if(file==NULL)         return NULL;
  data                   = CGIF_GetImageFromStream (file,&width,&height,&colorn,&reds,&greens,&blues);
  fclose                 (file);
  if(data==NULL)         return NULL;

  name                   = CFileGetName(a_fname);
  This                   = OImageCreate (a_display,name);
  CStringDelete          (name);
  if(This==NULL)         
    {
      CMemoryFreeBlock  (data);
      CMemoryFreeBlock  (reds);
      CMemoryFreeBlock  (greens);
      CMemoryFreeBlock  (blues);
      return            NULL;
    }

  This->fileName         = CStringDuplicate (a_fname);
  This->fileType         = CStringDuplicate ("gif");
  This->colorn           = colorn;
  This->reds             = reds;
  This->greens           = greens;
  This->blues            = blues;

  This->ximage           = XCreateImage  (
			     a_display,
			     XDefaultVisualOfScreen (XDefaultScreenOfDisplay(a_display)),
			     XDisplayGetDepth(a_display),
			     ZPixmap,0,
			     NULL,
			     width,height,8,width * sizeof(char));
  if(This->ximage!=NULL)
    {
      This->ximage->data            = (char*)data;
      This->ximage->f.destroy_image = XImageDelete;
      This->ximage->obdata          = CStringDuplicate(This->name);
      This->ximage->bits_per_pixel  = 8; 
      This->ximage->f.get_pixel     = GetPixel8;
      This->ximage->f.put_pixel     = PutPixel8;
   }

  return                 This;
}
/***************************************************************************/
OImage OImageCreateFromTIFF_File (
 Display* a_display
,char* a_fname  
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*         name;
  OImage        This;
  FILE*         file;
  unsigned int  width,height;
  Uchar*        data;
  int           colorn;
  double*       reds;
  double*       greens;
  double*       blues;
/*.........................................................................*/
  if(a_display==NULL)    return NULL;

  file                   = CFileOpenForReading (a_fname,NULL);
  if(file==NULL)         return NULL;
  data                   = CTIFF_GetImage (file,&width,&height,&colorn,&reds,&greens,&blues);
  fclose                 (file);
  if(data==NULL)         return NULL;

  name                   = CFileGetName(a_fname);
  This                   = OImageCreate (a_display,name);
  CStringDelete          (name);
  if(This==NULL)         
    {
      CMemoryFreeBlock  (data);
      CMemoryFreeBlock  (reds);
      CMemoryFreeBlock  (greens);
      CMemoryFreeBlock  (blues);
      return            NULL;
    }

  This->fileName         = CStringDuplicate    (a_fname);
  This->fileType         = CStringDuplicate    ("tiff");
  This->colorn           = colorn;
  This->reds             = reds;
  This->greens           = greens;
  This->blues            = blues;

  This->ximage           = XCreateImage  (
                             a_display,
			     XDefaultVisualOfScreen (XDefaultScreenOfDisplay(a_display)),
			     XDisplayGetDepth(a_display),
			     ZPixmap,0,
			     NULL,
			     width,height,8,width * sizeof(char));
  if(This->ximage!=NULL)
    {
      This->ximage->data             = (char*)data;
      This->ximage->f.destroy_image  = XImageDelete;
      This->ximage->obdata           = CStringDuplicate(This->name);
      This->ximage->bits_per_pixel   = 8; 
      This->ximage->f.get_pixel      = GetPixel8;
      This->ximage->f.put_pixel      = PutPixel8;
    }

  return                 This;
}
/***************************************************************************/
OImage OImageCreateFromFITS_File (
 Display* a_display
,char* a_fname  
,int  a_x_org   
,int  a_y_org          
,unsigned int  a_width  
,unsigned int  a_height 
,int  a_swap   
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*         name;
  OImage        This;
  FILE*         file;
  short*        data;
  unsigned int  width,height;
/*.........................................................................*/
  if(a_display==NULL)    return NULL;
  file                   = CFileOpenForReading (a_fname,NULL);
  if(file==NULL)         return NULL;

  if( (a_width==0) || (a_height==0) )
    {
      CFITS_GetHeaderFromStream (file,&width,&height);
      rewind                    (file);
    }
  else
    {
      width  = a_width;
      height = a_height;
    }
  data                   = CFITS_GetImageFromStream (file,a_x_org,a_y_org,width,height,a_swap);
  fclose                 (file);
  if(data==NULL)         return NULL;

  name                   = CFileGetName (a_fname);
  This                   = OImageCreate (a_display,name);
  CStringDelete          (name);
  if(This==NULL)         
    {
      CMemoryFreeBlock (data);
      return           NULL;
    }

  This->fileName         = CStringDuplicate (a_fname);
  This->fileType         = CStringDuplicate ("fits");

  This->ximage           = XCreateImage  (
                             a_display,
			     XDefaultVisualOfScreen (XDefaultScreenOfDisplay(a_display)),
			     XDisplayGetDepth(a_display),
			     ZPixmap,0,
			     NULL,
			     width,height,8,width * sizeof(short));

/* XCreateImage will install get_pixel, put_pixel routines  
   for pixel of size screen_bits_per_pixel (could be 8).*/
  if(This->ximage!=NULL)
    {
      This->ximage->data             = (char*)data;
      This->ximage->f.destroy_image  = XImageDelete;
      This->ximage->obdata           = CStringDuplicate(This->name);
      This->ximage->bits_per_pixel   = 16;
      This->ximage->f.get_pixel      = GetPixelshort;
      This->ximage->f.put_pixel      = PutPixelshort;
      This->ximage->f.sub_image      = SubstractImageshort;
    }

/*printf("debug:%d (stars.fits:14:16:20482)\n",data[16 * width + 14]);*/

  return This;
}
/***************************************************************************/
OImage OImageCreateFromBytes (
 Display* a_display
,int a_width
,int a_height
,char* a_data
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OImage        This;
/*.........................................................................*/
  if(a_display==NULL)    return NULL;
  if(a_data==NULL)       return NULL;
  This                   = OImageCreate(a_display,"from_data");
  if(This==NULL)         return NULL;

  This->ximage           = XCreateImage  (
                             a_display,
			     XDefaultVisualOfScreen (XDefaultScreenOfDisplay(a_display)),
			     XDisplayGetDepth(a_display),
			     ZPixmap,0,
			     NULL,
			     a_width,a_height,8,a_width * sizeof(char));
  if(This->ximage!=NULL)
    {
      This->ximage->data            = a_data;
      This->ximage->f.destroy_image = XImageDelete;
      This->ximage->obdata          = CStringDuplicate("user");
      This->ximage->bits_per_pixel  = 8; 
      This->ximage->f.get_pixel     = GetPixel8;
      This->ximage->f.put_pixel     = PutPixel8;
    }

  return                 This;
}
/***************************************************************************/
OImage OImageCreateRotated (
 Display* a_display
,char* a_name
,OImage a_image
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OImage        This;
  unsigned int  width;
  unsigned int  height;
  char*         idata;
  char*         data;
  char*         adata;
  double*       reds;
  double*       greens;
  double*       blues;
  unsigned int  col;
  int           row,count;
/*.........................................................................*/
  if(a_display==NULL) return NULL;

  idata = (char*)OImageGetSizeAndBytes (a_image,&width,&height);
  if((idata==NULL) || (width<=0)|| (height<=0) || (a_image->colorn<=0)) return NULL;
  data           =   (char*)CMemoryAllocateBlock((width * height) * sizeof(char));
  reds           = (double*)CMemoryAllocateBlock(a_image->colorn  * sizeof(double));
  greens         = (double*)CMemoryAllocateBlock(a_image->colorn  * sizeof(double));
  blues          = (double*)CMemoryAllocateBlock(a_image->colorn  * sizeof(double));
  if( (data==NULL) || (reds==NULL) || (greens==NULL) || (blues==NULL) ) 
    {
      CMemoryFreeBlock  (data);
      CMemoryFreeBlock  (reds);
      CMemoryFreeBlock  (greens);
      CMemoryFreeBlock  (blues);
      return            NULL;
    }

  for(count=0;count<a_image->colorn;count++)
    {
      reds   [count] = a_image->reds   [count];
      greens [count] = a_image->greens [count];
      blues  [count] = a_image->blues  [count];
    }

  adata = data;
  for(col=0;col<width;col++)
    for(row=(int)(height-1);row>=0;row--)
      {
        long          vlong;
        unsigned long vulong;
	double        red,green,blue;
	OImageGetPixel   (a_image,col,row,&vlong,&vulong,&red,&green,&blue);
        *adata        = (char)vulong;	
	adata++;
      }

  This         = OImageCreate(a_display,a_name);
  if(This==NULL)    return NULL;

  This->colorn = a_image->colorn;
  This->reds   = reds;
  This->greens = greens;
  This->blues  = blues;

  This->ximage = XCreateImage  (a_display,
		 	        XDefaultVisualOfScreen (XDefaultScreenOfDisplay(a_display)),
				XDisplayGetDepth(a_display),
				ZPixmap,0,
				NULL,
				height,width,8,height * sizeof(char));
  if(This->ximage!=NULL)
    {
      This->ximage->data            = data;
      This->ximage->f.destroy_image = XImageDelete;
      This->ximage->obdata          = CStringDuplicate("user");
      This->ximage->bits_per_pixel  = 8; 
      This->ximage->f.get_pixel     = GetPixel8;
      This->ximage->f.put_pixel     = PutPixel8;
    }

  return                 This;
}
/***************************************************************************/
static Pixel GetPixel8 (
 XImage* This
,int a_x
,int a_y
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)       return 0L;
  if(This->data==NULL) return 0L;
  if ((This->format == ZPixmap) && (This->bits_per_pixel == 8)) 
    {
      unsigned long pixel;
      pixel  = ((char*)This->data)[a_y * This->width + a_x];
      return pixel;
    } 
  else 
    {
      return 0;
    }
}
/***************************************************************************/
static int PutPixel8 (
 XImage* This
,int a_x
,int a_y
,Pixel a_pixel
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)       return 0;
  if(This->data==NULL) return 0;
  if ((This->format == ZPixmap) && (This->bits_per_pixel == 8)) 
    {
      ((char*)This->data)[a_y * This->width + a_x] = (char)a_pixel;
      return 1;
    } 
  else 
    return 0;
}
/***************************************************************************/
static Pixel GetPixelshort (
 XImage* This
,int a_x
,int a_y
)
/***************************************************************************/
/* From routine _XGetPixel16 in X11R5/mit-1/lib/X/XImUtil.c.               */
/* Pb on HP-UX.                                                            */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)       return 0L;
  if(This->data==NULL) return 0L;
  if ((This->format == ZPixmap) && (This->bits_per_pixel == 16)) 
    {
      unsigned long pixel;
/* 
      unsigned char* addr;
      addr = &((unsigned char *)This->data) [a_y * This->bytes_per_line + (a_x << 1)];
      if (This->byte_order == MSBFirst)
	pixel = addr[0] << 8 | addr[1];
      else
	pixel = addr[1] << 8 | addr[0];
      if (This->depth != 16) pixel &= low_bits_table[This->depth];
*/
      pixel  = ((short*)This->data)[a_y * This->width + a_x];
      return pixel;
    } 
  else 
    {
      return 0L;
    }
}
/***************************************************************************/
static int PutPixelshort (
 XImage* This
,int a_x
,int a_y
,Pixel a_pixel
)
/***************************************************************************/
/* From routine _XPutPixel16 in X11R5/mit-1/lib/X/XImUtil.c.               */
/* Pb on HP-UX.                                                            */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)       return 0;
  if(This->data==NULL) return 0;
  if ((This->format == ZPixmap) && (This->bits_per_pixel == 16)) 
    {
   /*
      unsigned char *addr;
      addr = &((unsigned char *)This->data)[a_y * This->bytes_per_line + (a_x << 1)];
      if (This->byte_order == MSBFirst) 
	{
	  addr[0] = (Uchar)(a_pixel >> 8);
	  addr[1] = (Uchar)a_pixel;
	} 
      else 
	{
	  addr[1] = (Uchar)(a_pixel >> 8);
	  addr[0] = (Uchar)a_pixel;
	}
*/
      ((short*)This->data)[a_y * This->width + a_x] = (short)a_pixel;
      return 1;
    } 
  else 
    return 0;
}
/***************************************************************************/
static XImage* SubstractImageshort (
 XImage* a_from
,int a_x
,int a_y
,unsigned int a_width
,unsigned int a_height
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  XImage*  This = NULL;
/*.........................................................................*/
  if(a_from==NULL)         return NULL;
  if(a_from->data==NULL)   return NULL;
  This      = (XImage*) malloc (sizeof(XImage));
  if(This==NULL) return NULL;
  This->width           = a_width;
  This->height          = a_height;
  This->xoffset         = 0;
  This->format          = ZPixmap;
  This->data            = (char*) CImageshortGetPart  (
						    (short*)(a_from->data),
						    (unsigned int)(a_from->width),
						    (unsigned int)(a_from->height),
						    a_x,a_y,a_width,a_height);
  This->byte_order      = a_from->byte_order;
  This->bitmap_unit     = a_from->bitmap_unit;
  This->bitmap_pad      = a_from->bitmap_pad;
  This->depth           = a_from->depth;
  This->bytes_per_line  = a_width * sizeof(short);
  This->bits_per_pixel  = 16;
  This->red_mask        = a_from->red_mask;
  This->green_mask      = a_from->green_mask;
  This->blue_mask       = a_from->blue_mask;
  This->obdata          = CStringDuplicate("subimage");
  
  This->f.create_image  = a_from->f.create_image;
  This->f.destroy_image = a_from->f.destroy_image;
  This->f.get_pixel     = a_from->f.get_pixel;
  This->f.put_pixel     = a_from->f.put_pixel;
  This->f.sub_image     = a_from->f.sub_image;
  This->f.add_pixel     = a_from->f.add_pixel;

  return This;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
int OImageIsValid (
 OImage This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return (CListGetEntryIndex((CList)Class.extent,This,NULL) == NotFound ? 0 : 1);
}
/***************************************************************************/
void OImageSetName (
 OImage This 
,char* a_name 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)     return;
  CStringDelete (This->name);
  This->name    = CStringDuplicate(a_name);
}
/***************************************************************************/
void OImageDelete (
  OImage This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)     return;
  CListRemoveEntry  ((CList)Class.extent,This);
  FreeRecord       (This);
}
/***************************************************************************/
char* OImageGetName (
 OImage This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->name;
}
/***************************************************************************/
static void FreeRecord (
 OImage This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)   return;
  CStringDelete    (This->name);
  CStringDelete    (This->fileName);
  CStringDelete    (This->fileType);

  This->colorn     = 0;
  CMemoryFreeBlock (This->reds);
  CMemoryFreeBlock (This->greens);
  CMemoryFreeBlock (This->blues);

  if(This->ximage!=NULL) XDestroyImage (This->ximage);

  This->name      = NULL;
  This->fileName  = NULL;
  This->fileType  = NULL;
  This->reds      = NULL;
  This->greens    = NULL;
  This->blues     = NULL;
  This->ximage    = NULL;
  CMemoryFreeBlock        (This);
}
/***************************************************************************/
char* OImageGetFileName (
 OImage This   
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->fileName;
}
/***************************************************************************/
void OImageSetFileType (
 OImage This   
,char* a_ftype     
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  CStringDelete (This->fileType);
  This->fileType   = CStringDuplicate (a_ftype);
}
/***************************************************************************/
char* OImageGetFileType (
 OImage This   
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->fileType;
}
/***************************************************************************/
void* OImageGetX11_Image (
 OImage This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->ximage;
}
/***************************************************************************/
Display* OImageGetDisplay (
 OImage This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->display;
}
/***************************************************************************/
void OImageSetX11_Image (
 OImage This
,void* a_image
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  if(This->ximage!=NULL) XDestroyImage (This->ximage);
  This->ximage  = (XImage*)a_image;
}
/***************************************************************************/
void OImageGetRGBs (
 OImage This 
,int* a_ncolor
,double** a_r
,double** a_g
,double** a_b
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_ncolor!=NULL) *a_ncolor = 0;
  if(a_r!=NULL)      *a_r      = NULL;
  if(a_g!=NULL)      *a_g      = NULL;
  if(a_b!=NULL)      *a_b      = NULL;
  if(This==NULL) return;
  if(a_ncolor!=NULL) *a_ncolor = This->colorn;
  if(a_r!=NULL)      *a_r      = This->reds;
  if(a_g!=NULL)      *a_g      = This->greens;
  if(a_b!=NULL)      *a_b      = This->blues;
}
/***************************************************************************/
void* OImageGetSizeAndBytes (
 OImage This 
,unsigned int* a_width
,unsigned int* a_height
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_width!=NULL)  *a_width  = 0;
  if(a_height!=NULL) *a_height = 0;
  if(This==NULL)         return NULL;
  if(This->ximage==NULL) return NULL;
  if(a_width!=NULL)  *a_width  = This->ximage->width;
  if(a_height!=NULL) *a_height = This->ximage->height;
  return (void*)(This->ximage->data);
}
/***************************************************************************/
int OImageGetPixel (
 OImage This 
,int a_col
,int a_row
,long*  a_long
,Ulong* a_ulong
,double* a_red
,double* a_green
,double* a_blue
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  XImage* image;
/*.........................................................................*/
  if(a_long!=NULL)        *a_long  = 0L;
  if(a_ulong!=NULL)       *a_ulong = 0L;
  if(a_red!=NULL)         *a_red   = 0.0;
  if(a_green!=NULL)       *a_green = 0.0;
  if(a_blue!=NULL)        *a_blue  = 0.0;
  if(This==NULL)         return 0;
  if(This->ximage==NULL) return 0;
  image             = This->ximage;
  if((a_col<0)||(a_col>=image->width))  return 0;
  if((a_row<0)||(a_row>=image->height)) return 0;

  if ((image->format == ZPixmap) && (image->bits_per_pixel == 16)) 
    { long       pixel;
      pixel      = (long)CImageshortGetPixel ((short*)(image->data),
					   (unsigned int)(image->width),
					   (unsigned int)(image->height),
					   a_col,a_row);
      if(a_long!=NULL) *a_long   = pixel;
      if( (This->reds!=NULL)   && 
	  (This->greens!=NULL) && 
          (This->blues!=NULL)  && 
	  (pixel>=0)           && 
	  (pixel<This->colorn)
	 )
        {
	  if(a_red!=NULL)         *a_red   = This->reds  [pixel];
	  if(a_green!=NULL)       *a_green = This->greens[pixel];
	  if(a_blue!=NULL)        *a_blue  = This->blues [pixel];
	}
      return               1;
    } 
  else
    { Ulong        pixel;
      pixel        = XGetPixel(image,a_col,a_row);
      if(a_ulong!=NULL)  *a_ulong = pixel;
      if( (This->reds!=NULL)   && 
	  (This->greens!=NULL) && 
          (This->blues!=NULL)  && 
	  ((int)pixel<This->colorn)
	 )
        {
	  if(a_red!=NULL)         *a_red   = This->reds  [pixel];
	  if(a_green!=NULL)       *a_green = This->greens[pixel];
	  if(a_blue!=NULL)        *a_blue  = This->blues [pixel];
	}
      return               0;
    }
}
/***************************************************************************/
int OImageGetAttribute (
 OImage  This
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

       if(strcmp(a_name,"name")==0)     *((char**)a_addr)  = This->name;
  else if(strcmp(a_name,"fileName")==0) *((char**)a_addr)  = This->fileName;
  else if(strcmp(a_name,"fileType")==0) *((char**)a_addr)  = This->fileType;
  else 
    {
      CInfoF ("OImageGetAttribute: unknown property %s.\n",a_name);
      return 0;
    }

  a_user = NULL;
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
  CErrorHandle ("Xo","OImage",a_name,a_code);
}


