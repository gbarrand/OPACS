/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#include <CMemory.h>
#include <CError.h>

#include <CTIFF.h>

#ifdef __cplusplus
extern "C"{
#endif
static int    SwapIntegerBytes (int);
static short  SwapShortBytes   (short);
static void   ReportError      (char*,int);
#ifdef __cplusplus
}
#endif

typedef unsigned char Uchar;

#define SubFileType			255
#define ImageWidth			256
#define ImageLength			257
#define BitsPerSample			258
#define Compression			259

#define StripOffsets			273
#define Orientation			274
#define SamplesPerPixel			277
#define RowsPerStrip			278
#define StripByteCounts			279

#define PhotometricInterpretation	262
#define Thresholding			263
#define CellWidth			264
#define CellLength			265
#define FillOrder			266

#define MinSampleValue			280
#define MaxSampleValue			281
#define XResolution			282
#define YResolution			283
#define PlanarConfiguration		284

#define ColorResponseUnit		290
#define GrayResponseUnit		290
#define GrayResponseCurve		291
#define ColorResponseCurve		291
#define Group3Options			292
#define ResolutionUnit			296

#define SHORT	3
#define LONG	4

/***************************************************************************/
Uchar* CTIFF_GetImage (
 FILE* a_file
,unsigned int* a_width
,unsigned int* a_height
,int* a_ncolor
,double** a_reds
,double** a_greens
,double** a_blues
)
/***************************************************************************/
/* We assume that file comes from Mac then we byte swap short,long         */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  struct
    {
      char  byte_order[2];
      char  tiff_version[2];
      long  first_IFD;
    } tiff_header;
  struct
    {
      short	tag;
      short	type;
      long	count;
      union
	{
	  short	short_value;
	  long	long_value;
	} tag_value;
    } tiff_directory;
  short	 number_of_directories;

  int    count,icount;
  long   header_size;
  int    io_errors;
  short	 max_value, min_value = 0, photo_type,nBits = 0,response_curve, response_unit;
  long	 value = 0L, pixels_per_line = 0L, nLines = 0L, picture_size,next_IFD_offset, offset_to_the_image= 0L;

  Uchar   *pimage,*fimage,*pixmap = (Uchar)NULL;
  Uchar   lut[256];
  int     ncolor;
  double* reds;
  double* greens;
  double* blues;
  unsigned int row,col,width,height;
/*.........................................................................*/
  if(a_width!=NULL)  *a_width     = 0;
  if(a_height!=NULL) *a_height    = 0;
  if(a_ncolor!=NULL) *a_ncolor    = 0;
  if(a_reds!=NULL)   *a_reds       = NULL;
  if(a_greens!=NULL) *a_greens     = NULL;
  if(a_blues!=NULL)  *a_blues      = NULL;
  if(a_file==NULL)  return NULL;

/* read the TIFF header */
  header_size      = (long)sizeof(tiff_header);
  io_errors        = fread ((char *)&tiff_header ,(size_t)header_size, 1, a_file);
  if(io_errors==0) {ReportError("CTIFF_GetImage",CErrorFormat);return NULL;}
/* read in the first IFD */
  tiff_header.first_IFD = SwapIntegerBytes ((int)(tiff_header.first_IFD));
  io_errors             = fseek (a_file,tiff_header.first_IFD,SEEK_SET);
  if(io_errors!=0)      {ReportError("CTIFF_GetImage",CErrorFormat);return NULL;}

/* read how many image directories there are in the TIFF file */
  header_size      = (long)sizeof(number_of_directories);
  io_errors        = fread ((char *)&number_of_directories ,(size_t)header_size, 1, a_file);
  if(io_errors==0) {ReportError("CTIFF_GetImage",CErrorFormat);return NULL;}
  number_of_directories = SwapShortBytes (number_of_directories);

/* the default minimum value */
/* the default maximum value */
/* the default photo metric interpretation */
  if(tiff_header.first_IFD > 0) min_value = 0;					
  max_value  = 256;		
  photo_type = 0;					

  for(count=0;count<number_of_directories;count++)
    {
      header_size      = (long)sizeof(tiff_directory);	
      io_errors        = fread ((char *)&tiff_directory,(size_t)header_size,1,a_file);	
      if(io_errors==0) {ReportError("CTIFF_GetImage",CErrorFormat);return NULL;}
      tiff_directory.type = SwapShortBytes (tiff_directory.type);
      tiff_directory.tag  = SwapShortBytes (tiff_directory.tag);

           if(tiff_directory.type == SHORT)  
	{
          tiff_directory.tag_value.short_value = SwapShortBytes (tiff_directory.tag_value.short_value);
          value          = (long)tiff_directory.tag_value.short_value;
        }
      else if(tiff_directory.type == LONG)   
	{
          tiff_directory.tag_value.long_value = SwapIntegerBytes  ((int)tiff_directory.tag_value.long_value);
          value         = tiff_directory.tag_value.long_value;
        }

/*printf("debug:tiff:tag %d value %d\n",tiff_directory.tag,value);*/

      switch(tiff_directory.tag) /* decipher the TAG */
	{
	  case SubFileType:               break;
  	  case ImageWidth:                pixels_per_line     = value;break;
  	  case ImageLength:               nLines              = value;break;
  	  case BitsPerSample:             nBits               = (short)value;
                                          if(nBits==1)
                                            {
                                               min_value           = 0;
    	   		                       max_value           = 1;
                                            }
                                          else if(nBits==8)
                                            {
                                               min_value           = 0;
    	   		                       max_value           = 256;
                                            }
			                  break;
  	  case MaxSampleValue:            max_value           = (short)value;break;
          case MinSampleValue:            min_value           = (short)value;break;
          case PhotometricInterpretation: photo_type          = (short)value;break;
          case GrayResponseUnit:          response_unit       = (short)value;break;
          case GrayResponseCurve:         response_curve      = (short)value;break;
          case StripOffsets:              offset_to_the_image = value;
	}
    }

  header_size      = (long)sizeof(next_IFD_offset);
  io_errors        = fread ((char *)&next_IFD_offset,(size_t)header_size,1,a_file);
  if(io_errors==0) {ReportError("CTIFF_GetImage",CErrorFormat);return NULL;}
  next_IFD_offset  = SwapIntegerBytes  ((int)next_IFD_offset);

/* now read in the TIFF picture */
  io_errors        = fseek (a_file,offset_to_the_image,SEEK_SET);
  if(io_errors!=0) {ReportError("CTIFF_GetImage",CErrorFormat);return NULL;}
  picture_size     = pixels_per_line * nLines * nBits/8;

  fimage = (Uchar*)CMemoryAllocateBlock((size_t)(picture_size*sizeof(Uchar)));
  if(fimage==NULL) return NULL;

  io_errors      = fread ((char *)fimage,(size_t)picture_size,1,a_file);
  if(io_errors==0) {ReportError("CTIFF_GetImage",CErrorFormat);CMemoryFreeBlock(fimage);return NULL;}
  fclose         (a_file);

  width = height = 0;
  if(nBits==1)
    {
      Uchar*        data;
#define IBIT(a_i,a_b)  (((Uchar)((a_i)&(1<<a_b)))>>a_b)
      width         = (int)pixels_per_line;
      height        = (int)nLines;
      pixmap        = (Uchar*)CMemoryAllocateBlock((size_t)(width * height *sizeof(Uchar)));
      if(pixmap==NULL)   {CMemoryFreeBlock(fimage);return NULL;}

      data          = fimage;
      pimage        = pixmap;
      count         = 7;
      for (row=0;row<height;row++) 
	for (col=0;col<width;col++) 
	  {
	    *pimage = (Uchar)IBIT(*data,count);pimage++;
	    if(count==0) {count=7;data++;}
	    else count--;
	  }
    }
  else if(nBits==8)
    {
      width         = (int)(pixels_per_line+1);
      height        = (int)nLines;
      pixmap        = (Uchar*)CMemoryAllocateBlock((size_t)(width * height * sizeof(Uchar)));
      if(pixmap==NULL)   {CMemoryFreeBlock(fimage);return NULL;}
      for(count=0;count<picture_size;count++) pixmap[count] = fimage[count];
    }
  CMemoryFreeBlock      (fimage);

/* build colormap and put colormap index value in image sample */
  ncolor    = max_value - min_value + 1;
  for(count=0;count<500;count++)   lut[count] = 0;
  reds    = (double*)CMemoryAllocateBlock(ncolor * sizeof(double));
  greens  = (double*)CMemoryAllocateBlock(ncolor * sizeof(double));
  blues   = (double*)CMemoryAllocateBlock(ncolor * sizeof(double));
  if( (reds==NULL) || (greens==NULL)  || (blues==NULL) ) 
    {  
      CMemoryFreeBlock  (pixmap);
      return    NULL;
    }
  icount    = 0;
  for(count=min_value;count<=max_value;count++)
    { double             grey;
      grey               = 1.-((double)icount)/(ncolor-1);
      reds  [icount]     = grey;
      greens[icount]     = grey;
      blues [icount]     = grey;
      lut[count]         = icount;
      icount++;
    }
  pimage = pixmap;
  for (row=0;row<height;row++) 
    for (col=0;col<width;col++) 
      {
        *pimage = lut[*pimage];
        pimage++;
      }
  if(a_width!=NULL)  *a_width  = width;
  if(a_height!=NULL) *a_height = height;
  if(a_ncolor!=NULL) *a_ncolor = ncolor;
  if(a_reds!=NULL)   *a_reds   = reds;
  if(a_greens!=NULL) *a_greens = greens;
  if(a_blues!=NULL)  *a_blues  = blues;
  return pixmap;
}
/***************************************************************************/
static int SwapIntegerBytes (
 int a_in 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int b1,b2,b3,b4;
/*.........................................................................*/
  b1 = a_in & 0xff;
  b2 = ((a_in & 0xff00)>>8);
  b3 = ((a_in & 0xff0000)>>16);
  b4 = ((a_in & 0xff000000)>>24);
  return b4 + (b3<<8)  + (b2<<16) + (b1<<24);
}
/***************************************************************************/
static short SwapShortBytes (
 short a_in 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  short b1,b2;
/*.........................................................................*/
  b1    = a_in & 0xff;
  b2    = a_in & 0xff00;
  b2    = b2 >>8;
  return b2 + (b1<<8);
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
  CErrorHandle ("Co","CTIFF",a_name,a_code);
}
