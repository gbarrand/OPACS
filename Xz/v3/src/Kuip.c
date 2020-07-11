/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#include <stddef.h>

#include <CText.h>

#include <Kuip.h>

static struct {
  int          init;
  int          size;
} Class = {0,0};
/***************************************************************************/
void KuipInitialize (
 int a_size
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.init==1) return;
  Class.size   = a_size;
  okinit       (&Class.size);
  Class.init   = 1;
}
/***************************************************************************/
void KuipExecuteScript (
 char* a_string
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int    linen = 0;
  char** lines = NULL;
/*.........................................................................*/
  KuipInitialize (0);
  lines  = CTextCreateFromString (a_string,&linen);
  if( (linen!=0) && (lines!=NULL) )
    {
      int count;
      for(count=0;count<linen;count++)
	{
	  ku_exec (lines[count]);
	}
    }
  CTextDelete (linen,lines);
}

