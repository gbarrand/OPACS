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

#include <CMemory.h>
#include <CPrinter.h>
#include <CString.h>
#include <CFile.h>
#include <CGIF.h>
#include <CPS.h>

#ifdef __cplusplus
extern "C"{
#endif
static int GetRGB (unsigned int,unsigned int,double*,double*,double*);
#ifdef __cplusplus
}
#endif

static char what[] = "@(#)GIF_To_PS v3";
static struct 
{
  unsigned int    width,height;
  unsigned char*  data;
  int             colorn;
  double*         reds;
  double*         greens;
  double*         blues;
} Class = {0,0,NULL,0,NULL,NULL,NULL};
/***************************************************************************/
int main (
 int   argc
,char* argv[]
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  FILE*        fp;
  char*        string;
/*.........................................................................*/
  what[0] = '@'; /*c++ no warning.*/
  if(argc<=1)
    {
      CInfo ("Usage:\n\
CGIFT.exe <gif file>\n\
");
      return EXIT_SUCCESS;
    }

  fp                     = CFileOpenForReading (argv[1],NULL);
  if(fp==NULL)           return EXIT_SUCCESS;
  Class.data             = CGIF_GetImageFromStream (fp,
						    &Class.width,
						    &Class.height,
						    &Class.colorn,
						    &Class.reds,&Class.greens,&Class.blues);
  fclose                 (fp);
  if(Class.data==NULL)   return EXIT_SUCCESS;
  if(Class.width<=0)     return EXIT_SUCCESS;
  if(Class.height<=0)    return EXIT_SUCCESS;

  CPS_OpenFileForWriting    ("out.ps");
  CPS_PutBeginPageInStream  ();
  CPS_PutPageScaleInStream  ((double)Class.width,(double)Class.height);
  CPS_PutImageInStream      (Class.width,Class.height,GetRGB);
  CPS_PutEndPageInStream    ();
  CPS_CloseStream           ();

  CMemoryFreeBlock       (Class.data);
  CMemoryFreeBlock       (Class.reds);
  CMemoryFreeBlock       (Class.greens);
  CMemoryFreeBlock       (Class.blues);

  CFileClearClass        ();
  CStringClearClass      ();
  CPS_ClearClass         ();

  CInfo                  ("out.ps written.\n");

  string                 = CMemoryGetStatus();
  if( (string!=NULL) && (*string!='\0')) CInfoF ("%s\n",string);

  return                  EXIT_SUCCESS;
}
/***************************************************************************/
static int GetRGB (
 unsigned int a_col
,unsigned int a_row
,double* a_red
,double* a_green
,double* a_blue
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  unsigned char index;
/*.........................................................................*/
  index = *(Class.data + Class.width * a_row + a_col);
  if(index>=Class.colorn) 
    {
      *a_red     = 0.;
      *a_green   = 0.;
      *a_blue    = 0.;
      return     0;
    }
  else
    {
      *a_red     = Class.reds  [index];
      *a_green   = Class.greens[index];
      *a_blue    = Class.blues [index];
      return     1;
    }
}
