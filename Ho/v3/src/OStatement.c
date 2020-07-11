/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
/*#define DEBUG*/

#ifdef DEBUG
#include <stdio.h>
#endif

#include <CString.h>
#include <CMemory.h>
#include <CPrinter.h>
#include <CError.h>
#include <CMath.h>

#include <CiFunctions.h>
#include <CiStat.h>

#include <OStatement.h>

#ifdef __cplusplus
extern "C"{
#endif
CiRoutine CiBindCMath(char*);

static int   InitializeClass    (int);
static void  ReportError        (char*,int);
#ifdef __cplusplus
}
#endif

#define MAX_PARAM 51  /* MINUIT limit + 1*/

static struct 
{
  int               init;
  int               warn;
  CiFunctionsRecord ciProcs;
  CiVariable        ciVar[MAX_PARAM];
} Class = {0,0,CiNULL};
/***************************************************************************/
static int InitializeClass (
 int a_warn
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
/*.........................................................................*/
  if(Class.init==1) return 1; /* already done */

#ifdef DEBUG
  printf ("debug : InitializeClass\n");
#endif

  if(CiLoaded()==0) 
    {
      if( (a_warn==1) && (Class.warn==0) )
	{
	  CWarn      ("Ci package not declared to Ho/OStatement class.\n");
	  Class.warn = 1;
	}
      return 0;
    }
    
#ifdef DEBUG
  printf ("debug : InitializeClass : 0\n");
#endif

  CiParserStartup        ();

#ifdef DEBUG
  printf ("debug : InitializeClass : 1\n");
#endif

/*Those bind are also done in Wo/OCi.c*/
  if(CiSymbolGetReference("CiBindmath"   ,NULL)==NULL) CiBindClass ("CiBindmath"  ,(CiRoutine)CiBindmath);
  if(CiSymbolGetReference("CiBindstdio"  ,NULL)==NULL) CiBindClass ("CiBindstdio" ,(CiRoutine)CiBindstdio);
  if(CiSymbolGetReference("CiBindstdlib" ,NULL)==NULL) CiBindClass ("CiBindstdlib",(CiRoutine)CiBindstdlib);
  if(CiSymbolGetReference("CiBindstdarg" ,NULL)==NULL) CiBindClass ("CiBindstdarg",(CiRoutine)CiBindstdarg);
  if(CiSymbolGetReference("CiBindCMath"  ,NULL)==NULL) CiBindClass ("CiBindCMath" ,(CiRoutine)CiBindCMath);

#ifdef DEBUG
  printf ("debug : InitializeClass : 2\n");
#endif

  CiDo                   ("\n\
#define __CI_BIND__\n\
/*For curve tracing.*/\n\
#include <math.h>\n\
/*For fitting (CMathGetGauss,...).*/\n\
#include <stdio.h>\n\
#include <stdlib.h>\n\
#include <stdarg.h>\n\
#include <CMath.h>\n\
");

#ifdef DEBUG
  printf ("debug : InitializeClass : 3\n");
#endif

/* declare variables */
  if(CiVariableGetReference("OStmt_0")==NULL)  /*Not done.*/
    {
      char*   string;
      string  = (char*)CMemoryAllocateBlock(MAX_PARAM * 30  * sizeof(char));
      if(string!=NULL)
	{
	  int       pos;
	  pos       = CStringPrintF(string,"");
	  for(count=0;count<MAX_PARAM;count++) 
	    { 
	      pos += CStringPrintF(string+pos,"double OStmt_%d;",count);
	    }
          CiDo(string);
	}
      CMemoryFreeBlock        (string);
    }

#ifdef DEBUG
  printf ("debug : InitializeClass : 4\n");
#endif

/* get variables reference */
  for(count=0;count<MAX_PARAM;count++) 
    { char*               name;
      name                = CStringCreateF (64,"OStmt_%d",count);
      Class.ciVar[count]  = CiVariableGetReference(name);
      CStringDelete       (name);
    }

#ifdef DEBUG
  printf ("debug : InitializeClass : end\n");
#endif

  Class.init = 1;

  return     1;
}
/***************************************************************************/
void OStatementSetFunctions (
 OStatementCiFunction a_CiParserStartup
,OStatementCiFunction a_CiVariableGetReference
,OStatementCiFunction a_CiTextCompile
,OStatementCiFunction a_CiVariableDelete
,OStatementCiFunction a_CiVariableGetValue
,OStatementCiFunction a_CiValuePSet
,OStatementCiFunction a_CiValueCastToDouble
,OStatementCiFunction a_CiStatementExecute
,OStatementCiFunction a_CiFunctionGetBind
,OStatementCiFunction a_CiSymbolGetReference
,OStatementCiFunction a_CiStatementDelete
,OStatementCiFunction a_CiFunctionSwitchNew
,OStatementCiFunction a_CiDo
,OStatementCiFunction a_CiFunctionBind
,OStatementCiFunction a_CiParserCleanup
,OStatementCiFunction a_CiPathNew
,OStatementCiFunction a_CiParserDeclareErrorHandler
,OStatementCiFunction a_CiBindClass
,OStatementCiFunction a_CiSetPrintFunction
,OStatementCiFunction a_CiBindmath
,OStatementCiFunction a_CiBindstdarg
,OStatementCiFunction a_CiBindstdio
,OStatementCiFunction a_CiBindstdlib
,OStatementCiFunction a_CiBindstring
,OStatementCiFunction a_CiBindtime
,OStatementCiFunction a_CiBindCi
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
   Class.ciProcs.CiParserStartup             = (CiParserStartupFunction)a_CiParserStartup;
   Class.ciProcs.CiVariableGetReference      = (CiVariableGetReferenceFunction)a_CiVariableGetReference;
   Class.ciProcs.CiTextCompile               = (CiTextCompileFunction)a_CiTextCompile;
   Class.ciProcs.CiVariableDelete            = (CiVariableDeleteFunction)a_CiVariableDelete;
   Class.ciProcs.CiVariableGetValue          = (CiVariableGetValueFunction)a_CiVariableGetValue;
   Class.ciProcs.CiValuePSet                 = (CiValuePSetFunction)a_CiValuePSet;
   Class.ciProcs.CiValueCastToDouble         = (CiValueCastToDoubleFunction)a_CiValueCastToDouble;
   Class.ciProcs.CiStatementExecute          = (CiStatementExecuteFunction)a_CiStatementExecute;
   Class.ciProcs.CiFunctionGetBind           = (CiFunctionGetBindFunction)a_CiFunctionGetBind;
   Class.ciProcs.CiSymbolGetReference        = (CiSymbolGetReferenceFunction)a_CiSymbolGetReference;
   Class.ciProcs.CiStatementDelete           = (CiStatementDeleteFunction)a_CiStatementDelete;
   Class.ciProcs.CiFunctionSwitchNew         = (CiFunctionSwitchNewFunction)a_CiFunctionSwitchNew;
   Class.ciProcs.CiDo                        = (CiDoFunction)a_CiDo;
   Class.ciProcs.CiFunctionBind              = (CiFunctionBindFunction)a_CiFunctionBind;
   Class.ciProcs.CiParserCleanup             = (CiParserCleanupFunction)a_CiParserCleanup;
   Class.ciProcs.CiPathNew                   = (CiPathNewFunction)a_CiPathNew;
   Class.ciProcs.CiParserDeclareErrorHandler = (CiParserDeclareErrorHandlerFunction)a_CiParserDeclareErrorHandler;
   Class.ciProcs.CiBindClass                 = (CiBindClassFunction)a_CiBindClass;
   Class.ciProcs.CiSetPrintFunction          = (CiSetPrintFunctionFunction)a_CiSetPrintFunction;

   Class.ciProcs.CiBindmath   = (CiBindFunction)a_CiBindmath;
   Class.ciProcs.CiBindstdarg = (CiBindFunction)a_CiBindstdarg;
   Class.ciProcs.CiBindstdio  = (CiBindFunction)a_CiBindstdio;
   Class.ciProcs.CiBindstdlib = (CiBindFunction)a_CiBindstdlib;
   Class.ciProcs.CiBindstring = (CiBindFunction)a_CiBindstring;
   Class.ciProcs.CiBindtime   = (CiBindFunction)a_CiBindtime;
   Class.ciProcs.CiBindCi     = (CiBindFunction)a_CiBindCi;
}
/***************************************************************************/
void OStatementClearClass (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
/*.........................................................................*/
#ifdef DEBUG
  printf ("debug : OStatementClearClass\n");
#endif

  if(Class.init==0) return;

  for(count=0;count<MAX_PARAM;count++) 
    { 
      CiVariableDelete     (Class.ciVar[count]);
      Class.ciVar[count]   = NULL;
    }

  CiParserCleanup ();

  Class.ciProcs.CiParserStartup             = NULL;
  Class.ciProcs.CiVariableGetReference      = NULL;
  Class.ciProcs.CiTextCompile               = NULL;
  Class.ciProcs.CiVariableDelete            = NULL;
  Class.ciProcs.CiVariableGetValue          = NULL;
  Class.ciProcs.CiValuePSet                 = NULL;
  Class.ciProcs.CiValueCastToDouble         = NULL;
  Class.ciProcs.CiStatementExecute          = NULL;
  Class.ciProcs.CiFunctionGetBind           = NULL;
  Class.ciProcs.CiSymbolGetReference        = NULL;
  Class.ciProcs.CiStatementDelete           = NULL;
  Class.ciProcs.CiFunctionSwitchNew         = NULL;
  Class.ciProcs.CiDo                        = NULL;
  Class.ciProcs.CiFunctionBind              = NULL;
  Class.ciProcs.CiParserCleanup             = NULL;
  Class.ciProcs.CiPathNew                   = NULL;
  Class.ciProcs.CiParserDeclareErrorHandler = NULL;
  Class.ciProcs.CiBindClass                 = NULL;
  Class.ciProcs.CiSetPrintFunction          = NULL;

  Class.ciProcs.CiBindmath                  = NULL;
  Class.ciProcs.CiBindstdarg                = NULL;
  Class.ciProcs.CiBindstdio                 = NULL;
  Class.ciProcs.CiBindstdlib                = NULL;
  Class.ciProcs.CiBindstring                = NULL;
  Class.ciProcs.CiBindtime                  = NULL;
  Class.ciProcs.CiBindCi                    = NULL;

  Class.warn         = 0;

  Class.init = 0;
}
/***************************************************************************/
OStatement OStatementCreate (
 char*  a_script
,int    a_varn
,char** a_vars
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OStatement     This;
   int count;
  char*     string;
/*.........................................................................*/
#ifdef DEBUG
  printf ("debug : OStatementCreate\n");
#endif

  if(InitializeClass(1)==0) return NULL;

  if(a_varn>MAX_PARAM) {ReportError("OStatementCreate",CErrorFlow);return NULL;}

/* script to Ci */
  string      = CStringDuplicate  (a_script);
  CStringConcatenate  (&string,";");
  for(count=0;count<a_varn;count++) 
    { char*             name;
      name              = CStringCreateF (64,"OStmt_%d",count);
      CStringReplaceWord (&string,a_vars[count],name);
      CStringDelete     (name);
    }

  This             = CiTextCompile (string);

  CStringDelete    (string);

  return           (OStatement)This;

}
/***************************************************************************/
double OStatementEvaluate (
 OStatement   This
,int     a_valn
,double* a_vals
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
/*.........................................................................*/
#ifdef DEBUG
  printf ("debug : OStatementEvaluate\n");
#endif

  if(This==NULL)        return 0.;
  if(a_valn>MAX_PARAM) {ReportError("OStatementEvaluate",CErrorFlow);return 0.;}

/* set values of arguments */
  for(count=0;count<a_valn;count++) 
    { double         varg;
      varg           = a_vals[count];
      CiValuePSet    (CiVariableGetValue(Class.ciVar[count]),(CiPointer)&varg);
    }

  return CiValueCastToDouble(CiStatementExecute((CiStatement)This));
}
/***************************************************************************/
OStatementInterpretedFunction OStatementGetFunctionIdentifier (
 char* a_string
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
#ifdef DEBUG
  printf ("debug : OStatementGetIdentifier\n");
#endif

  if(InitializeClass(0)==0) return NULL;
  return    (OStatementInterpretedFunction)CiFunctionGetBind (a_string);
}
/***************************************************************************/
void* OStatementGetSymbolIdentifier (
 char* a_string
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
#ifdef DEBUG
  printf ("debug : OStatementGetSymbolIdentifier\n");
#endif
  if(a_string==NULL) return NULL;

  if(InitializeClass(0)==0) return NULL;

#ifdef DEBUG
  printf ("debug : OStatementGetSymbolIdentifier : CiSymbolGetReference\n");
#endif

 {void* p;
  p     = (void*)CiSymbolGetReference(a_string,NULL);
#ifdef DEBUG
  printf ("debug : OStatementGetSymbolIdentifier : %lu\n",p);
#endif
  return p;}

  /*  return (void*)CiSymbolGetReference(a_string,NULL);*/
}
/***************************************************************************/
void OStatementDelete (
 OStatement This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
#ifdef DEBUG
  printf ("debug : OStatementDelete\n");
#endif

  if(This==NULL)            return;
  if(InitializeClass(0)==0) return;
#ifdef DEBUG
  printf ("debug : OStatementDelete : CiStatementDelete \n");
#endif
  CiStatementDelete         ((CiStatement)This);
}
/***************************************************************************/
static void ReportError (
 char* a_name
,int a_code
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CErrorHandle ("Ho","OStatement",a_name,a_code);
}
#include <CiStat.ic>
