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

#include <CPrinter.h>
#include <CError.h>

static struct 
{
  CErrorHandlerFunction proc;
} Class = {NULL};
/***************************************************************************/
CErrorHandlerFunction CErrorSetHandler (
 CErrorHandlerFunction a_proc
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  CErrorHandlerFunction  old;
/*.........................................................................*/
  old        = Class.proc;
  Class.proc = a_proc;
  return     old;
}
/***************************************************************************/
void CErrorHandle (
 char* a_lib
,char* a_class
,char* a_method
,int   a_code
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.proc!=NULL) 
    {
      Class.proc (a_lib,a_class,a_method,a_code);
    }
  else
    {
           if(a_code==CErrorValue)   CPrinterPutF ("Error : %s : %s : %s : bad value.\n",a_lib,a_class,a_method);  
      else if(a_code==CErrorFailure) CPrinterPutF ("Error : %s : %s : %s : can't do it.\n",a_lib,a_class,a_method);  
      else if(a_code==CErrorHas)     CPrinterPutF ("Error : %s : %s : %s : has not some product.\n",a_lib,a_class,a_method);  
      else if(a_code==CErrorFlow)    CPrinterPutF ("Error : %s : %s : %s : under or over flow.\n",a_lib,a_class,a_method);  
      else if(a_code==CErrorIO)      CPrinterPutF ("Error : %s : %s : %s : input or output problem.\n",a_lib,a_class,a_method);  
      else if(a_code==CErrorCase)    CPrinterPutF ("Error : %s : %s : %s : bad case.\n",a_lib,a_class,a_method);  
      else if(a_code==CErrorSystem)  CPrinterPutF ("Error : %s : %s : %s : can't do it on this system.\n",a_lib,a_class,a_method);  
      else if(a_code==CErrorHandler) CPrinterPutF ("Error : %s : %s : %s : bad handler.\n",a_lib,a_class,a_method);  
      else if(a_code==CErrorFormat)  CPrinterPutF ("Error : %s : %s : %s : bad format.\n",a_lib,a_class,a_method);  
      else if(a_code==CErrorName)    CPrinterPutF ("Error : %s : %s : %s : bad name.\n",a_lib,a_class,a_method);  
      else if(a_code==CErrorExisting)   CPrinterPutF ("Error : %s : %s : %s : some entity does not exist.\n",a_lib,a_class,a_method);  
      else if(a_code==CErrorConversion) CPrinterPutF ("Error : %s : %s : %s : can't convert.\n",a_lib,a_class,a_method);  
      else                            CPrinterPutF ("Error : %s : %s : %s : %d.\n",a_lib,a_class,a_method,a_code);  
    }
}


