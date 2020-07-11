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

/***************************************************************************/
int main (
 int   argc
,char* argv[]
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  FILE*           fp;
  unsigned int    width,height;
  unsigned char*  data;
  int             ncolor;
  double*         reds;
  double*         greens;
  double*         blues;
  char*           string;
/*.........................................................................*/
  if(argc<=1)
    {
      CInfo ("Usage:\n\
CGIFT.exe <gif file>\n\
");
      return EXIT_FAILURE;
    }

  fp              = CFileOpenForReading (argv[1],NULL);
  if(fp==NULL)    return EXIT_FAILURE;

  data            = CGIF_GetImageFromStream (fp,&width,&height,&ncolor,&reds,&greens,&blues);
  fclose          (fp);

  if(data==NULL)
    {
      CInfoF  ("Can't read GIF file %s.\n",argv[1]);
      return  EXIT_FAILURE;
    }

  fp              = CFileOpenForWriting("out.gif");
  CGIF_PutBytesInStream  (fp,data,width,height,ncolor,reds,greens,blues);
  fclose          (fp);

  CMemoryFreeBlock        (data);
  CMemoryFreeBlock        (reds);
  CMemoryFreeBlock        (greens);
  CMemoryFreeBlock        (blues);

  CFileClearClass   ();
  CStringClearClass ();

  CInfo            ("out.gif written.\n");

  string            = CMemoryGetStatus();
  if( (string!=NULL) && (*string!='\0')) CInfoF ("%s\n",string);

  return            EXIT_SUCCESS;
}
