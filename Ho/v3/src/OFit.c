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
#include <stdio.h>

#include <CMemory.h>
#include <CPrinter.h>
#include <CError.h>
#include <CString.h>

#include <OStatement.h>

#include <OFitP.h>

#ifdef __cplusplus
extern "C"{
#endif
static void AllocateArray (OFit,int);
static void FreeArray     (OFit);
static void ReportError   (char*,int);
#ifdef __cplusplus
}
#endif

#define ONumber(arr)  (sizeof(arr) / sizeof(arr[0]))
#define DVOID__POS    -1.

static struct 
{
  OFitFitFunction fitProc;
} Class = {NULL};
/***************************************************************************/
void OFitSetFitFunction (
 OFitFitFunction a_proc
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  Class.fitProc = a_proc;
}
/***************************************************************************/
OFit OFitMake (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OFit This;
   int count;
/*.........................................................................*/
  This              = (OFitRecord*)CMemoryAllocateBlock(sizeof(OFitRecord));
  if(This==NULL)    return NULL;
  This->fitType     = OFitTypeNone;
  This->npar        = 0;
  This->pars        = NULL;
  This->starts      = NULL;
  This->steps       = NULL;
  This->errs        = NULL;
  This->auto_starts = 1;
  This->auto_steps  = 1;

  This->script      = NULL;
  This->ostmt       = NULL;
  for(count=0;count<MAX_PARAM;count++) This->variable[count] = NULL;

  return          This;
}
/***************************************************************************/
void OFitDelete (
 OFit This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
/*.........................................................................*/
  if(This==NULL) return;
  OStatementDelete  (This->ostmt);
  CStringDelete     (This->script);
  This->script      = NULL;
  for(count=0;count<MAX_PARAM;count++) 
    {
      CStringDelete         (This->variable[count]);
      This->variable[count] = NULL;
    }

  FreeArray        (This);
  CMemoryFreeBlock         (This);
}
/***************************************************************************/
void OFitSetType (
 OFit This 
,OFitType a_type 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  This->fitType = a_type;
  FreeArray     (This);
  switch (This->fitType)
    {
      case OFitTypeNone:        break;
      case OFitTypeGaussian:    AllocateArray(This,3);break;
      case OFitTypeBreitWigner: AllocateArray(This,3);break;
      case OFitTypeExponential: AllocateArray(This,2);break;
      case OFitTypePolynomial:  break; /* done with OFitSetNumberOfParameters */
      case OFitTypeUser:        break; /* done with OFitSetScript */
    }
}
/***************************************************************************/
OFitType OFitGetType (
 OFit This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return OFitTypeNone;
  return This->fitType;
}
/***************************************************************************/
void OFitSetNumberOfParameters (
 OFit This 
,int a_number 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  if(This->fitType!=OFitTypePolynomial) return;
  if((a_number<0)||(a_number>=MAX_PARAM)) {ReportError ("OFitSetNumberOfParameters",CErrorFlow);return;}
  FreeArray  (This);
  AllocateArray (This,a_number);
}
/***************************************************************************/
int OFitGetNumberOfParameters (
 OFit This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 0;
  return This->npar;
}
/***************************************************************************/
void OFitSetStartValues (
 OFit This 
,int a_number
,double* a_list
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
/*.........................................................................*/
  if(This==NULL)   return;
  if(a_list==NULL) return;
  if((a_number<0)||(a_number>=MAX_PARAM)) {ReportError ("OFitSetStartValues",CErrorFlow);return;}
  if(This->npar==0)
    {
      FreeArray  (This);
      AllocateArray (This,a_number);
    }
  else if(a_number!=This->npar)
    {
      ReportError ("OFitSetStartValues",CErrorValue);
      return;
    }
  for(count=0;count<This->npar;count++)
    {
      This->starts[count] = a_list[count];
    }
}
/***************************************************************************/
double OFitGetIthStartValue (
 OFit This 
,int a_count
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)                         return 0.;
  if((a_count<0)||(a_count>=This->npar)) return 0.;
  return This->starts[a_count];
}
/***************************************************************************/
void OFitSetStartValuesAutomated (
 OFit This 
,int a_auto 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  This->auto_starts = a_auto;
}
/***************************************************************************/
int OFitAreStartValuesAutomated (
 OFit This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 1;
  return This->auto_starts;
}
/***************************************************************************/
void OFitSetScript (
 OFit This 
,char* a_script
,int a_number
,char** a_list
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
/*.........................................................................*/
  if(This==NULL)     return;
  if(a_script==NULL) return;
  if(a_list==NULL)   return;
  OFitSetType (This,OFitTypeUser);
  if((a_number<0)||(a_number>=MAX_PARAM)) {ReportError ("OFitSetNames",CErrorFlow);return;}
  FreeArray  (This);
  AllocateArray (This,a_number);
  for(count=0;count<This->npar;count++)
    {
      CStringDelete         (This->variable[count]);
      This->variable[count] = CStringDuplicate(a_list[count]);
    }
  CStringDelete     (This->script);
  This->script      = CStringDuplicate (a_script);
  if(This->script==NULL) return;

  This->variable[This->npar] = CStringDuplicate("x");

  OStatementDelete  (This->ostmt);
  This->ostmt  = OStatementCreate (This->script,This->npar+1,This->variable);

}
/***************************************************************************/
char** OFitGetVariableNames (
 OFit This 
,int* a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_number!=NULL) *a_number = 0;
  if(This==NULL)     return NULL;
  if(a_number!=NULL) *a_number = This->npar;
  return This->variable;
}
/***************************************************************************/
char* OFitGetScript (
 OFit This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->script;
}
/***************************************************************************/
double OFitEvaluateScript (
 OFit    This   
,double  a_xx
,int     a_valn
,double* a_vals
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)   return DVOID__POS;
  if(a_valn<=0)    return DVOID__POS;
  if(a_vals==NULL) return DVOID__POS;
  if(This->fitType!=OFitTypeUser) return DVOID__POS;
  if(This->ostmt!=NULL) 
    { 
       int count;
      double       vals[MAX_PARAM+1];
      for(count=0;count<a_valn;count++) vals[count] = a_vals[count];
      vals[a_valn] = a_xx;
      return       OStatementEvaluate(This->ostmt,a_valn+1,vals);
    }
  else
    return DVOID__POS;
}
/***************************************************************************/
void OFitSetSteps (
 OFit This 
,int a_number
,double* a_list
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
/*.........................................................................*/
  if(This==NULL)   return;
  if(a_list==NULL) return;
  if((a_number<0)||(a_number>=MAX_PARAM)) {ReportError ("OFitSetSteps",CErrorFlow);return;}
  if(This->npar==0)
    {
      FreeArray  (This);
      AllocateArray (This,a_number);
    }
  else if(a_number!=This->npar)
    {
      ReportError ("OFitSetSteps",CErrorValue);
      return;
    }
  for(count=0;count<This->npar;count++)
    {
      This->steps[count] = a_list[count];
    }
}
/***************************************************************************/
double OFitGetIthStep (
 OFit This 
,int a_count
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)                              return 0.;
  if((a_count<0)||(a_count>=This->npar)) return 0.;
  return This->steps[a_count];
}
/***************************************************************************/
int OFitExecute (
 OFit This 
,int a_npt
,double* a_xxs
,double* a_vals
,double* a_errs
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
/*.........................................................................*/
  if(This==NULL)          return 0;
  if(Class.fitProc==NULL) {CWarn ("No fit proc declared.\n");return 0;}
  for(count=0;count<This->npar;count++)
    {
      This->pars[count] = This->starts [count];
      This->errs[count] = This->steps  [count];
    }
  Class.fitProc ((void*)This,(int)This->fitType,a_npt,a_xxs,a_vals,a_errs,This->npar,This->pars,This->errs);
  return        1;
}
/***************************************************************************/
void OFitSetStepsAutomated (
 OFit This 
,int a_auto 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  This->auto_steps = a_auto;
}
/***************************************************************************/
int OFitAreStepsAutomated (
 OFit This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 1;
  return This->auto_steps;
}
/***************************************************************************/
char* OFitGetInformations (
 OFit This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  char* string;
  int   pos;
/*.........................................................................*/
  string  = (char*)CMemoryAllocateBlock(1024 * sizeof(char));
  if(string==NULL) return NULL;
  string[0] = '\0';
  if(This==NULL) return string;
  switch(This->fitType)
    {
      case OFitTypeNone:
           return  string;
      case OFitTypeGaussian:
           if(This->npar!=0) sprintf(string,"Gaussian fit\n[h*exp(-.5((x-m)/s)**2)]:\n\
h:%10g %10g\n\
m:%10g %10g\n\
s:%10g %10g",This->pars[0],This->errs[0],
             This->pars[1],This->errs[1],
             This->pars[2],This->errs[2]);
           return string;
      case OFitTypeBreitWigner:
           if(This->npar!=0) sprintf(string,"Breit Wigner fit\n[h/(1.+(2.*(x-c)/w)**2))]:\n\
h:%10g %10g\n\
c:%10g %10g\n\
w:%10g %10g",This->pars[0],This->errs[0],
             This->pars[1],This->errs[1],
             This->pars[2],This->errs[2]);
           return string;
      case OFitTypeExponential:
           if(This->npar!=0) sprintf(string,"Exponential fit\n[exp(a+b*x)]:\n\
a:%10g %10g\n\
b:%10g %10g",This->pars[0],This->errs[0],
             This->pars[1],This->errs[1]);
           return string;
      case OFitTypePolynomial:
           pos = CStringPrintF (string,"Polynomial fit\n[p0+p1*x+...]:\nnparam:%d\n",This->npar);
           for(count=0;count<This->npar;count++)
             { 
               pos += CStringPrintF(string+pos,"p%d:%10g %10g\n",count,This->pars[count],This->errs[count]);
             }
           string[pos-1] = 0; /*remove last \n */
           return string;
      case OFitTypeUser:
           pos = CStringPrintF
                 (string,"User fit\n%s:\nnparam:%d\n",This->script!=NULL ? This->script : "(nil)" ,This->npar);
           for(count=0;count<This->npar;count++)
             { 
               pos += CStringPrintF
                      (string+pos,"%s:%10g %10g\n",This->variable[count],This->pars[count],This->errs[count]);
             }
           string[pos-1] = 0; /*remove last \n */
           return string;
    }
  return string;
}
/***************************************************************************/
char* OFitBuildScript (
 OFit This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  char* string;
  int pos;
/*.........................................................................*/
  if(This==NULL) return NULL;
  switch (This->fitType)
    {
      case OFitTypeNone: return NULL;
      case OFitTypeGaussian:
           if(This->npar==0) return NULL;
           return CStringCreateF (256,"CMathGetGauss(x,%g,%g,%g)",This->pars[0],This->pars[1],This->pars[2]);
      case OFitTypeBreitWigner:
           if(This->npar==0) return NULL;
           return CStringCreateF (256,"CMathGetBreitWigner(x,%g,%g,%g)",This->pars[0],This->pars[1],This->pars[2]);
      case OFitTypeExponential:
           if(This->npar==0) return NULL;
           return CStringCreateF(256,"exp((%g)+x*(%g))",This->pars[0],This->pars[1]);
      case OFitTypePolynomial:
           if(This->npar==0) return NULL;
           string = (char*)CMemoryAllocateBlock((100 + This->npar * 50)*sizeof(char));
           if(string==NULL) return NULL;
           pos    = CStringPrintF (string,"CMathGetPolynomial(x,%d",This->npar);
           for(count=0;count<This->npar;count++)
             { 
               pos += CStringPrintF(string+pos,",%g",This->pars[count]);
             }
           pos += CStringPrintF(string+pos,")");
           return       string;
      case OFitTypeUser:
           if(This->script==NULL) return NULL;
           if(This->npar==0)   return 0;
           string = CStringDuplicate(This->script);
           for(count=0;count<This->npar;count++)
             { char    sval[64];
               sprintf (sval,"%g",This->pars[count]);
               CStringReplaceWord (&string,This->variable[count],sval);
             }
           return string;
    }
  return NULL;
}
/***************************************************************************/
static void AllocateArray (
 OFit This
,int a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
/*.........................................................................*/
  if(This==NULL)       return;
  if(a_number<=0) return;
  This->npar   = a_number;
  This->pars   = (double*)CMemoryAllocateBlock(This->npar  * sizeof(double));
  This->starts = (double*)CMemoryAllocateBlock(This->npar  * sizeof(double));
  This->steps  = (double*)CMemoryAllocateBlock(This->npar  * sizeof(double));
  This->errs   = (double*)CMemoryAllocateBlock(This->npar  * sizeof(double));
  if( (This->pars==NULL) || (This->starts==NULL) || (This->steps==NULL) || (This->errs==NULL) )
    {
      This->npar   = 0;
      This->pars   = This->starts = This->steps  = This->errs = NULL;
    }
  for(count=0;count<This->npar;count++)
    {
      This->pars   [count] = 0.;
      This->starts [count] = 0.;
      This->steps  [count] = 0.;
      This->errs   [count] = 0.;
    }
}
/***************************************************************************/
static void FreeArray (
 OFit This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  This->npar       = 0;
  CMemoryFreeBlock         (This->pars);
  CMemoryFreeBlock         (This->starts);
  CMemoryFreeBlock         (This->steps);
  CMemoryFreeBlock         (This->errs);
  This->pars       = NULL;
  This->starts     = NULL;
  This->steps      = NULL;
  This->errs       = NULL;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static struct {
  char*    string;
  OFitType value;
} OFitTypeList [] = {
  {"none",          OFitTypeNone},
  {"gaussian",      OFitTypeGaussian},
  {"breit_wigner",  OFitTypeBreitWigner},
  {"exponential",   OFitTypeExponential},
  {"polynomial",    OFitTypePolynomial},
  {"user",          OFitTypeUser}
};  
/***************************************************************************/
char** OFitTypeGetNames (
 int* a_number
)
/***************************************************************************/
/* CMemoryFreeBlock returned list when used */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  static char* fitType[6] = {"none","gaussian","breit_wigner","exponential","polynomial","user"};
/*.........................................................................*/
  if(a_number!=NULL) *a_number = 6;
  return fitType;
}
/***************************************************************************/
OFitType OFitTypeGetIdentifier (
 char* a_string 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  unsigned int count;
/*.........................................................................*/
  for(count=0;count<ONumber(OFitTypeList);count++)
    {
      if( (a_string!=NULL) && (strcmp(a_string,OFitTypeList[count].string)==0) ) 
	return OFitTypeList[count].value;
    }
  CWarnF ("%s not an OFitType.\n",a_string);
  return OFitTypeNone;
}
/***************************************************************************/
char* OFitTypeGetName (
 OFitType a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  unsigned int count;
/*.........................................................................*/
  for(count=0;count<ONumber(OFitTypeList);count++)
    if(a_value==OFitTypeList[count].value) return OFitTypeList[count].string;
  CWarnF ("%d not an OFitType.\n",a_value);
  return NULL;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static void ReportError (
 char* a_name
,int a_code
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CErrorHandle ("Ho","OFit",a_name,a_code);
}
