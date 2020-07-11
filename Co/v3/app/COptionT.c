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
#include <stdio.h>

#include <CPrinter.h>
#include <CMemory.h>

#include <COption.h>

#ifdef __cplusplus
extern "C"{
#endif
static void ProcessOptions  (int);
#ifdef __cplusplus
}
#endif

/***************************************************************************/
int main (
 int   a_argn
,char* a_args[]
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_argn<=1)
    {
      CInfo("\
Test this program with:\n\
   UNIX> COptionT.exe\n\
   UNIX> COptionT.exe -vers\n\
   UNIX> COptionT.exe -latex aaa\n\
   UNIX> COptionT.exe -latex aaa -topic bbb\n\
   UNIX> COptionT.exe -topic bbb -author -latex aaa -vers\n\
\n\
   UNIX> COptionT.exe -topic bbb -autor -latex aaa -vers\n");
      return EXIT_SUCCESS;
    }

#define OPT_VERS     1
#define OPT_LATEX    2
#define OPT_TOPIC    3
#define OPT_RELEASE  4
#define OPT_AUTHOR   5
#define OPT_AVS      6

  COptionInitialize (ProcessOptions);
  COptionAddEntry  ("-vers"    ,OPT_VERS);
  COptionAddEntry  ("-latex"   ,OPT_LATEX);
  COptionAddEntry  ("-topic"   ,OPT_TOPIC);
  COptionAddEntry  ("-rel"     ,OPT_RELEASE);
  COptionAddEntry  ("-author"  ,OPT_AUTHOR);
  COptionAddEntry  ("-avs"     ,OPT_AVS);

  COptionVisit (a_argn,a_args);

  COptionClearClass ();

  CInfoF    ("%s\n",CMemoryGetStatus());

  return  EXIT_SUCCESS;
}
/***************************************************************************/
static void ProcessOptions (
 int   a_opt
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  switch(a_opt)
    {
      case OPT_VERS:
       {char* senv;
	senv           = getenv   ("COVERSION");
	if(senv!=NULL) CInfoF  ("%s\n",senv);
	else           CInfo   ("Environment variable COVERSION not defined.\n");}
        break;
      case OPT_LATEX:
        CInfoF   ("latex %s.\n",COptionGoForth());
        break;
      case OPT_TOPIC:
        CInfoF   ("topic %s.\n",COptionGoForth());
        break;
      case OPT_RELEASE:
        CInfo   ("release.\n");
        break;
      case OPT_AUTHOR:
        CInfo   ("author.\n");
        break;
      case OPT_AVS:
        CInfo   ("avs.\n");
        break;
      default:
        break;
    }

}
