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

#ifdef __cplusplus
extern "C"{
#endif
static int  CStringRaise (char*);
static char CCharacterConvertToUpperCase   (char);
#ifdef __cplusplus
}
#endif

#define CInfoF printf
static char what[] = "@(#)oupper v3";
/***************************************************************************/
int main (
 int a_argn
,char** a_args
)
/***************************************************************************/
{
/*.........................................................................*/
  what[0] = '@'; /*c++ no warning.*/
  if(a_argn<=1) 
    {
      CInfoF ("%s","");
      return EXIT_SUCCESS;
    }

  CStringRaise (a_args[1]);
  CInfoF       ("%s\n",a_args[1]);
  return       EXIT_SUCCESS;
}
/***************************************************************************/
static int CStringRaise (
 char* This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* itema;
  int   changed = 0;
/*.........................................................................*/
  if(This==NULL) return 0;
  for(itema=This;*itema!='\0';itema++)
    { char   c;
      c      = *itema;
      *itema = CCharacterConvertToUpperCase(c);
      if(c!=(*itema)) changed = 1;
    }
  return changed;
}
/***************************************************************************/
static char CCharacterConvertToUpperCase (
 char This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return ((This) >= 'a' && (This) <= 'z' ?  This - 'a' + 'A' : This);
}
