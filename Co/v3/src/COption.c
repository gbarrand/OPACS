/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#include <string.h>

#include <CPrinter.h>
#include <CString.h>

#include <COption.h>

#ifdef __cplusplus
extern "C"{
#endif
static int    GetOption       ();
#ifdef __cplusplus
}
#endif

#define NotFound (-1)

#define MAX_OPT 50
static struct 
{
  int               silent;
  COptionDoFunction optProc;
  int               opt_count;
  int               number;
  int               argn;
  char**            args;
  char*             names[MAX_OPT];
  int               opts[MAX_OPT];
} Class = {0,NULL,0,0,0,NULL};
/***************************************************************************/
void COptionInitialize (
 COptionDoFunction a_proc
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  Class.optProc   = a_proc;
  Class.number    = 0;
  Class.opt_count = 0;
  Class.argn      = 0;
  Class.args      = NULL;
}
/***************************************************************************/
void COptionClearClass (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
/*.........................................................................*/
  for(count=0;count<Class.number;count++)
    {
      CStringDelete(Class.names[count]);
      Class.names[count] = NULL;
    }
  Class.optProc   = NULL;
  Class.number    = 0;
  Class.opt_count = 0;
  Class.argn      = 0;
  Class.args      = NULL;
  Class.silent    = 0;
}
/***************************************************************************/
void COptionSetSilent (
  int a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  Class.silent = a_value;
}
/***************************************************************************/
void COptionAddEntry (
 char* a_name
,int a_opt
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  Class.names[Class.number] = CStringDuplicate(a_name);
  Class.opts [Class.number] = a_opt;
  Class.number++;
}
/***************************************************************************/
void COptionVisit (
 int a_argn
,char** a_args
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if( (a_argn==0) || (a_args==NULL) ) return;
  if(Class.optProc==NULL)     return;
  if(a_argn<=1)
    {
      Class.optProc(0);
    }
  else
    {
      Class.argn = a_argn;
      Class.args = a_args;
      Class.opt_count = 1;
      while(1)
	{
	  int      opt;
	  opt      = GetOption ();
	  if(opt==NotFound) return;
	  if(opt==0)
	    {
	      if(Class.silent==0)
		{
		  CWarnF ("Option %s unknown.\n",Class.args[Class.opt_count]);
		}
	      return;
	    }
	  else
	    {
	      Class.optProc(opt);
	      Class.opt_count++;
	    }
	  
	} 
    }
}
/***************************************************************************/
char* COptionGoForth (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if( (Class.argn==0) || (Class.args==NULL) ) return NULL;
  Class.opt_count++;
  if(Class.opt_count>=Class.argn) return NULL;
  return Class.args[Class.opt_count];
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static int GetOption (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
/*.........................................................................*/
  if(Class.opt_count>=Class.argn) return NotFound;
  for(count=0;count<Class.number;count++)
    {
      char* arg;
      char* string;
      arg    = Class.args[Class.opt_count];
      string = Class.names[count];
      if( (arg!=NULL)    && 
	  (string!=NULL) && 
	  (strncmp(arg,string,strlen(string))==0)
	   ) 
	return Class.opts[count];
    }
  return 0;
}
