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
#include <string.h>
#include <stdarg.h>

#include <CTime.h>
#include <CFile.h>
#include <CString.h>
#include <CMemory.h>
#include <CColor.h>
#include <CPrinter.h>
#include <CError.h>

#include <CPS.h>

typedef unsigned char Uchar;
typedef unsigned long Ulong;
typedef unsigned int  Uint;

#ifdef __cplusplus
extern "C"{
#endif
static void  PrintFLN    (char*,...);
static void  PrintV      (char*,va_list);
static void  WriteByte   (Uchar);
static void  ReportError (char*,int);
#ifdef __cplusplus
}
#endif

#define PS_ROTATE(a_x)         CPS_PutInStreamF("%.2f  rotate ",a_x)

#define PS_NEWPATH             CPS_PutInStreamF("n ")
#define PS_STROKE              CPS_PutInStreamF("s ")
#define PS_FILL                CPS_PutInStreamF("f ")
#define PS_CLOSEPATH           CPS_PutInStreamF("cl ")
#define PS_CAP(a_x)            CPS_PutInStreamF ("%1d lc ",a_x)
#define PS_RLINETO(a_x,a_y)    CPS_PutInStreamF ("%.2f %.2f rl ",a_x,a_y)
#define PS_MOVE(a_x,a_y)       CPS_PutInStreamF ("%.2f %.2f m ",a_x,a_y)

#define METAFILE_DEFAULT        "out.ps"
#define METAFILE_SCALE          1.
#define METAFILE_RECORD_LENGTH  80
static struct 
{
  struct {
    int   shade;
    int   portrait;
    int   nbit;
    int   doBack,lineWidth; /*Camera*/
  } param;
  double      deviceWidth;
  double      deviceHeight;
  int         pageNumber;
  long        pagePos;
  double      markSize;
  FILE*       file;
  char*       fname;
  int         number;
  char*       string;
  int         gsave;
  Uchar       buffer[METAFILE_RECORD_LENGTH+1];
} Class = {
{CPS_Color,1,2,1,-1},
(8.5-1.) * 72. * METAFILE_SCALE,     /* 540. * METAFILE_SCALE */
11.      * 72. * METAFILE_SCALE,     /* 792. * METAFILE_SCALE */
0,0,  /*pageNumber,pagePos*/
2.,
NULL,NULL,  /*file,fname*/
0,NULL,  /*number,string*/
0     /*gsave/grestore balance*/
};
/***************************************************************************/
void CPS_ClearClass (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.file!=NULL) CPS_CloseStream ();
  CMemoryFreeBlock      (Class.string);
  Class.string = NULL;
  if(Class.gsave!=0) CWarnF ("CPS : bad gsave/grestore balance : %d.\n",Class.gsave);
  Class.gsave = 0;
}
/***************************************************************************/
/***************************************************************************/
/******** Parameters  ******************************************************/
/***************************************************************************/
/***************************************************************************/
void CPS_SetColorScheme (
 int a_shade
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  Class.param.shade = a_shade;
}
/***************************************************************************/
void CPS_SetOrientation (
 int a_portrait
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  Class.param.portrait = a_portrait;
}
/***************************************************************************/
void CPS_SetBackgroundDrawn (
 int a_doback
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  Class.param.doBack = a_doback;
}
/***************************************************************************/
void CPS_SetBitsPerPixel (
 int a_nbit
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if( (a_nbit==2) || (a_nbit==4) || (a_nbit==8) )
    Class.param.nbit = a_nbit;
  else 
    Class.param.nbit = 2;
}
/***************************************************************************/
void CPS_SetLineWidth (
 int a_width
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  Class.param.lineWidth = a_width;
}
/***************************************************************************/
/***************************************************************************/
/******** File, Buffer *****************************************************/
/***************************************************************************/
/***************************************************************************/
char* CPS_GetFileName (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return Class.fname;
}
/***************************************************************************/
void* CPS_GetStream (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return Class.file;
}
/***************************************************************************/
void CPS_OpenFileForWriting (
 char* a_string
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.file!=NULL) CPS_CloseStream ();
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

  Class.number                          = 0;
  Class.buffer[METAFILE_RECORD_LENGTH]  = '\0';
  Class.pageNumber                      = 0;
/* Header */
  PrintFLN   ("%%!PS-Adobe-2.0");
 {char* senv;
  senv  = getenv   ("COVERSION");
  if(senv!=NULL) 
    {
      PrintFLN   ("%%%%Creator: Co %s, LAL Orsay.",senv);
    }
  else
    {
      PrintFLN   ("%%%%Creator: Co, LAL Orsay.");
    }}
  PrintFLN   ("%%%%CreationDate: %s",CTimeGetDate());
  PrintFLN   ("%%%%Title: %s",Class.fname);
  PrintFLN   ("%%%%Pages: (atend)");
  PrintFLN   ("%%%%BoundingBox: 0 0 %d %d",(int)Class.deviceWidth,(int)Class.deviceHeight);
  PrintFLN   ("%%%%DocumentFonts: Courier-Bold");
  PrintFLN   ("%%%%DocumentPaperSizes: a4");
  PrintFLN   ("%%%%EndComments");
/* postscript */
  CPS_PutSaveStateInStream      ();
/* General */
  CPS_PutInStreamF     ("/n {newpath} def ");
  CPS_PutInStreamF     ("/cl {closepath} def ");
  CPS_PutInStreamF     ("/s {stroke} def ");
  CPS_PutInStreamF     ("/f {fill} def ");
/* Move */
  CPS_PutInStreamF     ("/m  {moveto} def ");
  CPS_PutInStreamF     ("/rm {rmoveto} def ");
  CPS_PutInStreamF     ("/rl {rlineto} def ");
/* Line */
  CPS_PutInStreamF     ("/lc {setlinecap} def ");
  CPS_PutInStreamF     ("/lw {setlinewidth} def ");
  CPS_PutInStreamF     ("/rgb {setrgbcolor} def ");
  CPS_PutInStreamF     ("/ss {[] 0 setdash} def ") ;            /* style solid       */
  CPS_PutInStreamF     ("/sd {[12 6] 0 setdash} def ");         /* style dashed      */
  CPS_PutInStreamF     ("/so {[6 12] 0 setdash} def ");         /* style dotted      */
  CPS_PutInStreamF     ("/sdo {[18 12 6 12] 0 setdash} def ");  /* style dash dotted */
/* Mark */
  Class.markSize = 2.;
        /*012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789
          0         1         2         3         4         5         6         7         8         */
  CPS_PutInStreamF     ("/ms 2. def /msi .5 def ");        /* mark size */
  CPS_PutInStreamF     ("/cross {ms ms scale -1. -1. rm  ");
  CPS_PutInStreamF     ("2. 2. rl 0. -2. rm -2. 2. rl msi msi scale} def ");
  CPS_PutInStreamF     ("/plus  {ms ms scale -1. 0. rm 2. 0. rl ");
  CPS_PutInStreamF     ("-1. 1. rm 0. -2. rl msi msi scale} def ");
  CPS_PutInStreamF     ("/asterisk {ms ms scale -1. 0. rm 2. 0. rl -1. 1. rm ");
  CPS_PutInStreamF     ("0. -2. rl 0. 1. rm -0.707 -0.707 rm 1.414 1.414 rl ");
  CPS_PutInStreamF     ("0. -1.414 rm -1.414 1.414 rl msi msi scale} def ");
  CPS_PutInStreamF     ("/star {ms ms scale 0. 1. rm -0.6 -1.5 rl ");
  CPS_PutInStreamF     ("1.2 0. rl -0.6 1.5 rl msi msi scale} def ");
/* Text */
  CPS_PutInStreamF     ("/sh {show} def ");
  CPS_PutInStreamF     ("/df {/Courier-Bold findfont} def ");
  CPS_PutInStreamF     ("/mf {makefont setfont} def ");
  PrintFLN   ("%%%%EndProlog");
}
/***************************************************************************/
void CPS_CloseStream (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.file==NULL) return;
  CPS_PutRestoreStateInStream    ();
  PrintFLN    ("%%%%Trailer");
  PrintFLN    ("%%%%Pages: %d",Class.pageNumber);
  PrintFLN    ("%%%%EOF");
  fclose         (Class.file);
  Class.file     = NULL;
  CStringDelete  (Class.fname);
  Class.fname    = NULL;
}
/***************************************************************************/
void CPS_PutInStreamF (
 char* a_format 
,...
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  va_list  args;
  Uchar    *pointer;
  int      length,nlength;
/*.........................................................................*/
  if(Class.file==NULL) return;
  va_start (args,a_format);
  PrintV  (a_format,args);
  va_end   (args);
  length   = strlen(Class.string);
  if(length>METAFILE_RECORD_LENGTH) 
    {
      ReportError ("CPS_PutInStreamF",CErrorFlow);
      return;
    }

  nlength  = Class.number + length;
  if(nlength>METAFILE_RECORD_LENGTH)
    {
      Class.buffer[Class.number] = '\0';
      if(fprintf(Class.file,"%s\n",(char*)Class.buffer)<0) ReportError("CPS_PutInStreamF",CErrorIO);
      Class.number = 0;
      nlength              = length;
    }
  pointer = Class.buffer + Class.number;
  strcpy  ((char*)pointer,Class.string);
  Class.number = nlength;
}
/***************************************************************************/
static void PrintFLN (
 char* a_format 
,...
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  va_list  args;
/*.........................................................................*/
  if(Class.file==NULL) return;
  va_start (args,a_format);
  PrintV  (a_format,args);
  va_end   (args);
/* put buffer in file */
  if(Class.number>0)
    {
      Class.buffer[Class.number] = '\0';
      if(fprintf (Class.file,"%s\n",(char*)Class.buffer)<0) ReportError("PrintFLN",CErrorIO);
      Class.number = 0;
    }
/* put comment in file */
  if(fprintf (Class.file,"%s\n",Class.string)<0) ReportError("PrintFLN",CErrorIO);
}
/***************************************************************************/
static void PrintV (
 char* This 
,va_list a_args  
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
#define MAX_STR    2048
/*.........................................................................*/
  if(Class.string==NULL) 
    {
      Class.string = (char*)CMemoryAllocateBlock(MAX_STR * sizeof(char));
      if(Class.string==NULL) return;
    }
  Class.string[MAX_STR-1]  = '\0';
  vsprintf           (Class.string,This,a_args);
  if(Class.string[MAX_STR-1]!='\0') 
    {
      ReportError ("PrintV",CErrorFlow);
      Class.string[0] = '\0';
    }
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
void CPS_PutPageScaleInStream (
 double a_width 
,double a_height 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  double scale;
/*.........................................................................*/
  if(a_width <=0.) a_width  = 100.;
  if(a_height<=0.) a_height = 100.;

  CPS_PutScaleInStream (1./METAFILE_SCALE,1./METAFILE_SCALE);
  CPS_PutTranslationInStream ((double)(Class.deviceWidth/20.),(double)(Class.deviceHeight/30.));

  if(Class.deviceWidth<=Class.deviceHeight)
    scale = (a_height<=a_width ? Class.deviceWidth /a_width  : Class.deviceWidth /a_height );
  else 
    scale = (a_height<=a_width ? Class.deviceHeight /a_width : Class.deviceHeight /a_height );

 {double xtra,ytra;
  if(Class.param.portrait==1)
    {
      xtra           = (Class.deviceWidth  - scale * a_width)/2.;
      ytra           = (Class.deviceHeight - scale * a_height)/2.;
    }
  else
    {
      CPS_PutTranslationInStream       (Class.deviceWidth,0.);
      PS_ROTATE      (90.);
      xtra           = (Class.deviceHeight  - scale * a_width)/2.;
      ytra           = (Class.deviceWidth   - scale * a_height)/2.;
    }
  CPS_PutTranslationInStream (xtra,ytra);}

  CPS_PutScaleInStream (scale,scale);
}
/***************************************************************************/
/***************************************************************************/
/********* Image ***********************************************************/
/***************************************************************************/
/***************************************************************************/
void CPS_PutImageInStream (
 Uint a_width
,Uint a_height
,CPS_GetRGB_Function a_proc
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int      nbhex;
  Uint     row,col,col_max;
  int      status;

  double   dr,dg,db;
  Uchar    red,green,blue,b;
/*.........................................................................*/
  if((a_width<=0)||(a_height<=0)) return;
  if(a_proc==NULL)                return;

  CPS_PutSaveStateInStream      ();
  CPS_PutInStreamF     ("%d %d scale ", a_width, a_height ); 
  status = 1;
  if(Class.param.shade!=0) /*grey*/
    {
      CPS_PutInStreamF   ("/picstr %d string def ",a_width); 
      CPS_PutInStreamF   ("%d %d %d ",a_width,a_height,8); 
      CPS_PutInStreamF   ("[ %d 0 0 -%d 0 %d ] ",a_width,a_height,a_height); 
      CPS_PutInStreamF   ("{ currentfile picstr readhexstring pop } " );
      PrintFLN ("image " );
      for ( row = 0; row < a_height; row++ )
        {
          for ( col = 0; col < a_width; col++)    
            { double    fgrey;
              Uchar     grey;
	      status    = a_proc(col,row,&dr,&dg,&db)==0 ? 0 : status;
              fgrey     = CColorConvertRGB_ToGrey(dr,dg,db);
	      grey      = (Uchar) ( 255. * fgrey);
	      WriteByte (grey);
            }
        }
      nbhex     = a_width * a_height * 2; 
      PrintFLN ("%%%% nbhex digit          :%d ",nbhex); 
      PrintFLN ("%%%% nbhex/record_length  :%d ",nbhex/METAFILE_RECORD_LENGTH); 
      PrintFLN ("%%%% nbhex%%record_length :%d ",nbhex%METAFILE_RECORD_LENGTH); 
    }
  else if(Class.param.nbit==2)
    { int       nbyte2;
      nbyte2    = (a_width   *  3)/4;
      nbyte2   /=3;
      nbyte2   *=3;
      col_max   = (nbyte2  *  4)/3;
/* 2 bit for r and g and b   */
/* rgbs following each other */
      CPS_PutInStreamF   ("/rgbstr %d string def ",nbyte2); 
      CPS_PutInStreamF   ("%d %d %d ",col_max,a_height,2); 
      CPS_PutInStreamF   ("[ %d 0 0 -%d 0 %d ] ",col_max,a_height,a_height); 
      CPS_PutInStreamF   ("{ currentfile rgbstr readhexstring pop } " );
      CPS_PutInStreamF   ("false 3 " );
      PrintFLN ("colorimage " );
      for ( row = 0; row < a_height; row++ )
        {
          for ( col = 0; col < col_max; col+=4)    
            {
	          status  = a_proc(col,row,&dr,&dg,&db)==0 ? 0 : status;
                  red     = (Uchar) ( 3. * dr);
                  green   = (Uchar) ( 3. * dg);
                  blue    = (Uchar) ( 3. * db);
                  b       = red;
                  b       = (b<<2)+green;
                  b       = (b<<2)+blue;
	          status  = a_proc(col+1,row,&dr,&dg,&db)==0 ? 0 : status;
                  red     = (Uchar) ( 3. * dr);
                  green   = (Uchar) ( 3. * dg);
                  blue    = (Uchar) ( 3. * db);
                  b     = (b<<2)+red;
                  WriteByte (b);

                  b       = green;
                  b       = (b<<2)+blue;
	          status  = a_proc(col+2,row,&dr,&dg,&db)==0 ? 0 : status;
                  red     = (Uchar) ( 3. * dr);
                  green   = (Uchar) ( 3. * dg);
                  blue    = (Uchar) ( 3. * db);
                  b     = (b<<2)+red;
                  b     = (b<<2)+green;
                  WriteByte (b);

                  b       = blue;
	          status  = a_proc(col+3,row,&dr,&dg,&db)==0 ? 0 : status;
                  red     = (Uchar) ( 3. * dr);
                  green   = (Uchar) ( 3. * dg);
                  blue    = (Uchar) ( 3. * db);
                  b     = (b<<2)+red;
                  b     = (b<<2)+green;
                  b     = (b<<2)+blue;
                  WriteByte (b);
            }
        }
    }
  else if(Class.param.nbit==4) 
    { int       nbyte4;
      nbyte4    = (a_width  * 3)/2;
      nbyte4   /=3;
      nbyte4   *=3;
      col_max   = (nbyte4 * 2)/3;
/* 4 bit for r and g and b   */
/* rgbs following each other */
      CPS_PutInStreamF   ("/rgbstr %d string def ",nbyte4); 
      CPS_PutInStreamF   ("%d %d %d ",col_max,a_height,4); 
      CPS_PutInStreamF   ("[ %d 0 0 -%d 0 %d ] ",col_max,a_height,a_height); 
      CPS_PutInStreamF   ("{ currentfile rgbstr readhexstring pop } " );
      CPS_PutInStreamF   ("false 3 " );
      PrintFLN ("colorimage " );
      for ( row = 0; row < a_height; row++ )
        {
          for ( col = 0; col < col_max; col+=2)    
            {
	          status  = a_proc(col,row,&dr,&dg,&db)==0 ? 0 : status;
                  red     = (Uchar) ( 15. * dr);
                  green   = (Uchar) ( 15. * dg);
                  CPS_PutInStreamF ("%x%x",red,green);
                  blue    = (Uchar) ( 15. * db);

	          status  = a_proc(col+1,row,&dr,&dg,&db)==0 ? 0 : status;
                  red     = (Uchar) ( 15. * dr);
                  CPS_PutInStreamF ("%x%x",blue,red);
                  green   = (Uchar) ( 15. * dg);
                  blue    = (Uchar) ( 15. * db);
                  CPS_PutInStreamF ("%x%x",green,blue);
            }
        }
    }
  else 
    { int       nbyte8;
      nbyte8    = a_width   * 3;
/* 8 bit for r and g and b   */
      CPS_PutInStreamF   ("/rgbstr %d string def ",nbyte8); 
      CPS_PutInStreamF   ("%d %d %d ",a_width,a_height,8); 
      CPS_PutInStreamF   ("[ %d 0 0 -%d 0 %d ] ",a_width,a_height,a_height); 
      CPS_PutInStreamF   ("{ currentfile rgbstr readhexstring pop } " );
      CPS_PutInStreamF   ("false 3 " );
      PrintFLN   ("colorimage " );
      for ( row = 0; row < a_height; row++ )
        {
          for ( col = 0; col < a_width; col++)    
            {
	      status     = a_proc(col,row,&dr,&dg,&db)==0 ? 0 : status;
              red        = (Uchar) ( 255. * dr);
              WriteByte (red);
              green      = (Uchar) ( 255. * dg);
              WriteByte (green);
              blue       = (Uchar) ( 255. * db);
              WriteByte (blue);
            }
        }
    }
  if(status==0) CWarnF ("CPS_PutImageInStream: problem to retreive some pixel rgb.\n");
  CPS_PutRestoreStateInStream    ();
}
/***************************************************************************/
static void WriteByte (
 Uchar a_byte
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Uchar l,h;
/*.........................................................................*/
  h = a_byte / 16;
  l = a_byte % 16;
  CPS_PutInStreamF ("%x%x",h,l);
}
/***************************************************************************/
/***************************************************************************/
/*********** Drawing *******************************************************/
/***************************************************************************/
/***************************************************************************/
void CPS_PutSaveStateInStream (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CPS_PutInStreamF   ("gsave ");
  Class.gsave++;
}
/***************************************************************************/
void CPS_PutRestoreStateInStream (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CPS_PutInStreamF   ("grestore ");
  Class.gsave--;
}
/***************************************************************************/
void CPS_PutTranslationInStream (
 double a_x
,double a_y
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CPS_PutInStreamF("%.2f %.2f translate ",a_x,a_y);
}
/***************************************************************************/
void CPS_PutScaleInStream (
 double a_x
,double a_y
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CPS_PutInStreamF("%.2f %.2f scale ",a_x,a_y);
}
/***************************************************************************/
void CPS_PutBeginPageInStream (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  Class.pageNumber++;
  PrintFLN   ("%%%%Page: %d %d",Class.pageNumber,Class.pageNumber);
  CPS_PutSaveStateInStream      ();
}
/***************************************************************************/
void CPS_PutEndPageInStream (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CPS_PutInStreamF   ("showpage ");
  CPS_PutRestoreStateInStream ();
}
/***************************************************************************/
void CPS_PutRGB_InStream (
 double a_r 
,double a_g
,double a_b 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
       if(Class.param.shade==CPS_Color)       CPS_PutInStreamF    ("%.2f %.2f %.2f rgb ",a_r,a_g,a_b);
  else if(Class.param.shade==CPS_Grey)        CPS_PutInStreamF    ("%.2f setgray ",CColorConvertRGB_ToGrey(a_r,a_g,a_b));
  else if(Class.param.shade==CPS_BlackWhite)  CPS_PutInStreamF    ("0. setgray ",CColorConvertRGB_ToGrey(a_r,a_g,a_b));
}
/***************************************************************************/
void CPS_PutLineWidthInStream (
 int a_width
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.param.lineWidth<0) CPS_PutInStreamF ("%.1f lw ",(float)(a_width));
  else                        CPS_PutInStreamF ("%.1f lw ",(float)Class.param.lineWidth); 
}
/***************************************************************************/
void CPS_PutMarkerSizeInStream (
 double a_size
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_size==Class.markSize) return;
  Class.markSize = a_size;
  CPS_PutInStreamF("/ms %g def /msi %g def ",a_size,1./a_size);
}
/***************************************************************************/
void CPS_PutBackgroundInStream (
 double a_r
,double a_g
,double a_b
,double a_width 
,double a_height 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  PS_NEWPATH; 
  PS_MOVE       (0.,0.);
  PS_RLINETO    (a_width,0.);
  PS_RLINETO    (0.,a_height);
  PS_RLINETO    (-a_width,0.);
  PS_RLINETO    (0.,-a_height);
  PS_CLOSEPATH;
  if(Class.param.doBack==1) 
    {
/* Back */
      CPS_PutSaveStateInStream    ();
      CPS_PutRGB_InStream         (a_r,a_g,a_b);
      PS_FILL;       
      CPS_PutRestoreStateInStream ();
    }
/* Clip */
  CPS_PutInStreamF            ("clip ");
}
/***************************************************************************/
void CPS_PutFrameInStream (
 double a_r
,double a_g
,double a_b
,double a_width 
,double a_height 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  PS_NEWPATH;
  PS_MOVE         (0.,0.);
  PS_RLINETO      (a_width,0.);
  PS_RLINETO      (0.,a_height);
  PS_RLINETO      (-a_width,0.);
  PS_RLINETO      (0.,-a_height);
  PS_CLOSEPATH;
  CPS_PutRGB_InStream      (a_r,a_g,a_b);
  CPS_PutLineWidthInStream (1);
  PS_CAP                   (1);
  CPS_PutInStreamF         ("ss ");
  PS_STROKE;
}
/***************************************************************************/
int CPS_SetAttribute (
 void*  This
,char*  a_name
,void*  a_user
,void*  a_addr 
,int    a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_name==NULL)  return 0;
  if(a_addr==NULL)  return 0; 

       if(strcmp(a_name,"fileName")==0)       
    {
      char*  fname;
      char*  string;
      fname  = CPS_GetFileName();
      string = *((char**)a_addr);
      if( (string!=NULL) && 
	 ( (fname==NULL) || (strcmp(fname,string)!=0) )) CPS_OpenFileForWriting  (string);
    }
  else if(strcmp(a_name,"colorScheme")==0)       
    {
      char*  string;
      string = *((char**)a_addr);
      if(string==NULL) return 0;
      if(strcmp(string,"color")==0)  
	{
	  CPS_SetColorScheme (CPS_Color);
	}
      else if( (strcmp(string,"grey")==0) ||
	      (strcmp(string,"gray")==0) )   
	{
	  CPS_SetColorScheme       (CPS_Grey);
	  CPS_SetBackgroundDrawn   (0);
	}
      else if(strcmp(string,"bw")==0)     
	{
	  CPS_SetColorScheme       (CPS_BlackWhite);
	  CPS_SetBackgroundDrawn   (0);
	}
      else 
	{
	  CWarnF ("CPS_SetAttribute : bad option value %s.\n",string);
	  return 0;
	}
    }
  else if(strcmp(a_name,"orientation")==0)       
    {
      char*  string;
      string = *((char**)a_addr);
      if(string==NULL) return 0;
      if(strcmp(string,"portrait")==0)  
	{
	  CPS_SetOrientation (1);
	}
      else if(strcmp(string,"landscape")==0)  
	{
	  CPS_SetOrientation (0);
	}
      else 
	{
	  CWarnF ("CPS_SetAttribute : bad option value %s.\n",string);
	  return 0;
	}
    } 
  else if(strcmp(a_name,"backgroundDrawn")==0)       
    {
      Class.param.doBack    = *((int*)a_addr);
    }  
  else if(strcmp(a_name,"bitsPerPixel")==0)       
    {
      Class.param.nbit      = *((int*)a_addr);
    }  
  else if(strcmp(a_name,"lineWidth")==0)       
    {
      Class.param.lineWidth = *((int*)a_addr);
    }  
  else 
    {
      CInfoF ("CPS_SetAttribute : unknown property %s.\n",a_name); 
      return 0;
   }

  This     = NULL;
  a_user   = NULL;
  a_number = 0;
  return   1;
}
/***************************************************************************/
int CPS_GetAttribute (
 void* This
,char* a_name
,void* a_user
,void* a_addr 
,int*  a_number
)
/***************************************************************************/
/* 
   This==NULL means we want infos on the class.
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_number!=NULL) *a_number = 0;
  if(a_name==NULL)  return 0;
  if(a_addr==NULL)  return 0; 

       if(strcmp(a_name,"fileName")==0)       
    {
      *((char**)a_addr) = Class.fname;
    }
  else if(strcmp(a_name,"colorScheme")==0)       
    {
      static char* shades[3]       = {"color","grey","bw"};
      if(Class.param.shade==CPS_Color)
	{
          *((char**)a_addr) = shades[0];
	}
      else if(Class.param.shade==CPS_Grey)
	{
          *((char**)a_addr) = shades[1];
	}
      else if(Class.param.shade==CPS_BlackWhite)
	{
          *((char**)a_addr) = shades[2];
	}
    }
  else if(strcmp(a_name,"orientation")==0)       
    {
      static char* orientations[2] = {"portrait","landscape"};
      if(Class.param.portrait==1)
	{
          *((char**)a_addr) = orientations[0];
	}
      else 
	{
          *((char**)a_addr) = orientations[1];
	}
    } 
  else if(strcmp(a_name,"backgroundDrawn")==0)       
    {
      *((int*)a_addr) = Class.param.doBack;
    }  
  else if(strcmp(a_name,"bitsPerPixel")==0)       
    {
      *((int*)a_addr) = Class.param.nbit;
    }  
  else if(strcmp(a_name,"lineWidth")==0)       
    {
      *((int*)a_addr) = Class.param.lineWidth;
    }  
  else 
    {
      CInfoF ("CPS_GetAttribute : unknown property %s.\n",a_name);
      return 0;
    }

  This   = NULL;
  a_user = NULL;
  return 1;
}
/***************************************************************************/
int CPS_DoMethod (
 void*  This
,char*  a_name
,void*  a_user
,int    a_argn
,char** a_args
,void* a_addr 
,int*  a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_number!=NULL) *a_number = 0;
  if(a_name==NULL)  return 0;

  if(This==NULL)
    {
       if(strcmp(a_name,"close")==0)  
	 {
           CPS_CloseStream();
	 }
       else 
	 {
	   CInfoF("CPS_DoMethod : %s is not a class routine property.\n",a_name);
	   return 0;
	 }
       return 1;
    }

  a_user = NULL;
  a_argn = 0;
  a_args = NULL;
  a_addr = NULL;
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
  CErrorHandle ("Co","CPS",a_name,a_code);
}
