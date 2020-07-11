/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
static char what[] = "@(#)Co v3";

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

#include <CPrinter.h>

static struct 
{
  CPrinterPutFunction   putFunction;
  CPrinterPutV_Function putV_Function;
} Class = {NULL,NULL};
/***************************************************************************/
CPrinterPutFunction CPrinterSetPutFunction (
 CPrinterPutFunction a_proc
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  CPrinterPutFunction old;
/*.........................................................................*/
  what[0]          = '@'; /*c++ no warning.*/
  old              = Class.putFunction;
  Class.putFunction = a_proc;
  return           old;
}
/***************************************************************************/
void CPrinterPut (
 char* a_string
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.putFunction!=NULL) Class.putFunction(a_string);
  else                       printf ("%s",a_string);
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
CPrinterPutV_Function CPrinterSetPutV_Function (
 CPrinterPutV_Function a_proc
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  CPrinterPutV_Function old;
/*.........................................................................*/
  old                 = Class.putV_Function;
  Class.putV_Function = a_proc;
  return              old;
}
/***************************************************************************/
void CPrinterPutF (
 char* a_format
,...
)
/***************************************************************************/
/*
 Description :
   Prints a formatted message. The printer used could be changed with
  the CPrinterSetPutV_Function routine. Default printer is the 
  vprintf routine that print on stdio channel.
 Arguments :
  Same as printf routine.
 Example : 
...
#include <CPrinter.h>
...
  CPrinterPutF ("Hello, I dump an integer : %d\n",10);
  CInfoF       ("CInfoF is an alias for CPrinterPutF.\n");
  CWarnF       ("CWarnF is an other alias for CPrinterPutF.\n");
...
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  va_list        args;
/*.........................................................................*/
  if(a_format==NULL)     return;
  va_start               (args,a_format);
  if(Class.putV_Function) Class.putV_Function(a_format,args);
  else                   vprintf             (a_format,args);
  va_end                 (args);
}


