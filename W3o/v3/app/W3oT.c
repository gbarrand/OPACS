#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <HTAccess.h>

#include <moHTextP.h>
extern HText* HTMainText;

static char defaultURL[] = "http://www.lal.in2p3.fr/OPACS/v3/welcome.html";

/***************************************************************************/
int main (
 int   a_argn
,char* a_args[]
)
/***************************************************************************/
/*
  From Xo/v3/src/moww.c/doit.
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* URL; 
  int status;
/*.........................................................................*/
  URL = a_argn==2 ? a_args[1] : defaultURL;

  status = HTLoadAbsolute (URL);
  if (status==1)
    {
      if(HTMainText!=NULL)
	{
	  printf ("%s\n",HTMainText->htmlSrc!=NULL ? HTMainText->htmlSrc:"(nil)");
	}
    }
  else if (status==-1)
    {
      printf ("Can't retreive :\n%s\n",URL);
      return EXIT_FAILURE;
    }
  
  if(HTMainText!=NULL)
    {
      if(HTMainText->htmlSrc!=NULL) free (HTMainText->htmlSrc);
      HTMainText->htmlSrc     = NULL;
      free                    (HTMainText);
      HTMainText               = NULL;
    }

  return EXIT_SUCCESS;
}













