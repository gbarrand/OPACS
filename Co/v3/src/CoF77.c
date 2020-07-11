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

#include <CString.h>
#include <CFile.h>
#include <CPrinter.h>
#include <CMath.h>
#include <OProcess.h>

#include <CoF77.h>

/***************************************************************************/
void cwrite (
 FString  a_fstr
,int*  a_lstr
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*          string;
/*.........................................................................*/
  string           = FStringConvertToC   (a_fstr,*a_lstr);
  if(string!=NULL) CPrinterPutF ("%s\n",string);
  CStringDelete    (string);
}
/***************************************************************************/
void cbindu (
 FString  a_fstr
,int*  a_lstr
,int*  a_unit
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* string;
  int   unit;
/*.........................................................................*/
  string               = FStringConvertToC   (a_fstr,*a_lstr);
  unit                 = *a_unit;
  CFileBindToF77_Unit  (string,unit);
  CStringDelete        (string);
}
/***************************************************************************/
int cdargn (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int                  argn;
/*.........................................................................*/
  OProcessGetArguments (&argn);
  return               argn;
}
/***************************************************************************/
int cdargi (
 int* a_index
,FString  a_fstr
,int*  a_lstr
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int     argn;
  char**  args;
  int     argi;
/*.........................................................................*/
  args    = OProcessGetArguments (&argn);
  if( (args==NULL) || (argn<=0) ) return 0; 
  argi    = (*a_index) - 1;
  if( (argi<0) || (argi>=argn) )  return 0;
  return FStringInitializeFromC (a_fstr,*a_lstr,args[argi]);
}
/***************************************************************************/
float crando (
)
/***************************************************************************/
/*
  Produce random number in [0.,1.].
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return (float)CMathGetRandomFlat();
}
