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
#include <string.h>

#include <CPrinter.h>
#include <CMemory.h>
#include <CString.h>
#include <CList.h>
#include <OShell.h>
#include <OType.h>

#include <OHistogram.h>
#include <OTuple.h>
#include <OCut.h>
#include <OFit.h>
#include <HoTypes.h>

#include <Ho.h>
#include <HoCommands.h>

#ifdef __cplusplus
extern "C"{
#endif
static OCut* CreateCutList (int,char**,int*,int**);
#ifdef __cplusplus
}
#endif

#define NotFound (-1)
/***************************************************************************/
void HoAddCommands (
 OShell a_osh
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(OCommandGetIdentifier("dumpTuple")!=NULL) return; /*done*/

  HoSetTypes  ();

  OShellAddNewCommand (a_osh,"Ho/dumpTuple"              ,HoExecute_dumpTuple);
  OShellAddNewCommand (a_osh,"Ho/putInHistogram"         ,HoExecute_putInHistogram);
  OShellAddNewCommand (a_osh,"Ho/fillHistogramFromTuple" ,HoExecute_fillHistogramFromTuple);
  OShellAddNewCommand (a_osh,"Ho/operateHistograms"      ,HoExecute_operateHistograms);
  OShellAddNewCommand (a_osh,"Ho/fitHistogram"           ,HoExecute_fitHistogram);
}
/***************************************************************************/
int HoExecute_dumpTuple (
 int    a_argn
,char** a_args
,OProcess a_process
)
/***************************************************************************/
/*
  osh> dumpTuple <string:tuple name>
  osh> dumpTuple <string:tuple name> <integer:first row> <integer:number of rows> <integer:first column> <integer:number of columns>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OTuple otuple;
/*.........................................................................*/
  if( (a_argn!=2) && (a_argn!=6) ) 
    {
      CWarnF("One or five arguments expected in :\n%s\n",OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  otuple = OTupleGetIdentifier(a_args[1]);
  if(otuple==NULL) 
    {
      CWarnF ("Unknown tuple %s in :\n%s\n",a_args[1],OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  if(a_argn==2)
    {
      OTupleDump  (otuple,-1,-1,-1,-1);
    }
  else
    {
      int    status;
      int    row,rows;
      int    col,cols;
      row  = (int)CStringConvertToLong(a_args[2],&status);
      if(status==0)
	{
	  CWarnF ("Argument %s not a number in :\n%s\n",a_args[2],OProcessGetCommand(a_process));
	  return EXIT_FAILURE;
	}
      rows = (int)CStringConvertToLong(a_args[3],&status);
      if(status==0)
	{
	  CWarnF ("Argument %s not a number in :\n%s\n",a_args[3],OProcessGetCommand(a_process));
	  return EXIT_FAILURE;
	}
      
      col  = (int)CStringConvertToLong(a_args[4],&status);
      if(status==0) 
	{
	  col  = OTupleGetColumnIndex (otuple,a_args[4]);
	  if(col==NotFound) 
	    {
	      CWarnF ("%s not a column of %s in :\n%s\n",a_args[4],OTupleGetName(otuple),OProcessGetCommand(a_process));
	      return EXIT_FAILURE;
	    }
	}
      cols = (int)CStringConvertToLong(a_args[5],&status);
      if(status==0)
	{
	  CWarnF ("Argument %s not a number in :\n%s\n",a_args[5],OProcessGetCommand(a_process));
	  return EXIT_FAILURE;
	}
      
      OTupleDump  (otuple,row,rows,col,cols);
  }

  return EXIT_SUCCESS;
}
/***************************************************************************/
int HoExecute_putInHistogram (
 int    a_argn
,char** a_args
,OProcess a_process
)
/***************************************************************************/
/*
  osh> putInHistogram -            <string:property> <string:OHistogram> 
  osh> putInHistogram <string:oid> <string:property> <string:OHistogram> 
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OHistogram ohist;
  OHandle*  hands;
  OHandle*  handa;
/*.........................................................................*/
  if(a_argn!=4)
    {
      CWarnF("Three arguments expected in :\n%s\n",OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return EXIT_FAILURE;
    }

  if(OProcessPreparePipe(a_process,a_args[1])==0) return EXIT_FAILURE;

  ohist = OHistogramGetIdentifier (a_args[3]);
  if(ohist==NULL)  
    {
      CWarnF        ("Unknown OHistogram %s in :\n%s\n",a_args[3],OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return        EXIT_FAILURE;
    }
  if(OHistogramGetDimension(ohist)!=1)
    {
      CWarnF        ("OHistogram %s is not one dimensional in :\n%s\n",a_args[3],OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return        EXIT_FAILURE;
    }
  

  hands  = OProcessGetHandles    (a_process);
  for(handa=hands;(handa!=NULL)&&(*handa!=NULL);handa++) 
    { 
      OType     otype;
      OProperty oprop;
      otype = (OType)OHandleGetType(*handa);
      oprop = OTypeGetPropertyIdentifier (otype,a_args[2]);
      if(oprop==NULL) 
	{
	  CWarnF("Property %s unknown for %s in :\n%s\n",a_args[2],OTypeGetName(otype),OProcessGetCommand(a_process));
	}
      else
        {
	  void*    variable;
	  int      number = 0;
	  int      status;
	  variable = OPropertyGetVariable   (oprop);
	  status   = OTypeGetObjectProperty(otype,(OIdentifier)OHandleGetIdentifier(*handa),oprop,variable,&number);
	  if(status!=0)
	    {
	      int   ptype;
              ptype = OPropertyGetType (oprop);
	      if( (ptype==OPropertyInteger)      ||
		  (ptype==OPropertyUnsignedLong) ||
		  (ptype==OPropertyFloat)        ||
		  (ptype==OPropertyDouble)
		  )
		{
		  OHistogramFillOneDimensional (ohist,*((double*)variable),1.);
		}
	    }
	  CMemoryFreeVariable    (variable,number,status);
	}
    }
  OProcessPutHandles (a_process,NULL);

  return EXIT_SUCCESS;
}
/***************************************************************************/
int HoExecute_fillHistogramFromTuple (
 int    a_argn
,char** a_args
,OProcess a_process
)
/***************************************************************************/
/*
  osh> fillHistogramFromTuple <string:OHistogram name> <string:OTuple name> [columns] [<string:OCut name>]
  osh> fillHistogramFromTuple <string:OHistogram name> <string:OTuple name> <>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OHistogram ohist;
  OTuple     otuple;
  int        dimension;
  int        cutn,count;
  OCut*      cuts;
  int*       cuta;
/*.........................................................................*/
  if(a_argn<3)
    {
      CWarnF("At least two arguments expected in :\n%s\n",OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  ohist = OHistogramGetIdentifier(a_args[1]);
  if(ohist==NULL) 
    {
      CWarnF ("Unknown OHistogram %s in :\n%s\n",a_args[1],OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  otuple = OTupleGetIdentifier(a_args[2]);
  if(otuple==NULL) 
    {
      CWarnF ("Unknown OTuple %s in :\n%s\n",a_args[2],OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  dimension = OHistogramGetDimension(ohist);
  if(dimension<=0)
    {
      CWarnF ("Zero dimensionnal OHistogram %s in :\n%s\n",a_args[1],OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  if(a_argn<3+dimension)
    {
      CWarnF ("At least %d arguments expected in :\n%s\n",3+dimension-1,OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }
  
  cuts                     = CreateCutList (a_argn - (3+dimension),a_args+3+dimension,&cutn,&cuta);
  HoFillHistogramFromTuple (ohist,otuple,dimension,a_args+3,cutn,cuts,0);

  for(count=0;count<cutn;count++) 
    {
      if(cuta[count]==1) OCutDelete(cuts[count]);
    }
  CMemoryFreeBlock         (cuts);
  CMemoryFreeBlock         (cuta);

  return EXIT_SUCCESS;
}
/***************************************************************************/
static OCut* CreateCutList (
 int    a_linen
,char** a_lines
,int*   a_cutn
,int**  a_cuta
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  int   cutn;
  OCut* cuts;
  int*  cuta;
/*.........................................................................*/
  if(a_cutn) *a_cutn = 0;
  if(a_cuta) *a_cuta = NULL;
  if( (a_linen==0) || (a_lines==NULL) ) return NULL;

  cuts  = (OCut*)CMemoryAllocateBlock( a_linen * sizeof(OCut));
  cuta  = (int*) CMemoryAllocateBlock( a_linen * sizeof(int));
  if( (cuts==NULL) || (cuta==NULL) ) return NULL;

  cutn = 0;
  for(count=0;count<a_linen;count++)
    { 
      OCut cut;
      cut  = OCutGetIdentifier(a_lines[count]);
      if(cut==NULL)
	{
          cut           = OCutMake ();
          OCutSetScript (cut,a_lines[count]);          
	  cuts[cutn]    = cut;
	  cuta[cutn]    = 1;
	  cutn++;
	}
      else
	{
	  cuts[cutn] = cut;
	  cuta[cutn] = 0;
	  cutn++;
	}
    }

  if(cutn==0)
    {
      CMemoryFreeBlock (cuts);
      CMemoryFreeBlock (cuta);
      cuts             = NULL;
      cuta             = NULL;
    }

  if(a_cutn) *a_cutn = cutn;
  if(a_cuta) *a_cuta = cuta;
  return cuts;
}
/***************************************************************************/
int HoExecute_operateHistograms (
 int    a_argn
,char** a_args
,OProcess a_process
)
/***************************************************************************/
/*
  osh> operateHistograms <string:result OHistogram name> <string:OHistogram name> <string:OHistogram name> [add,sub,mul,div] 
  osh> operateHistograms <string:result OHistogram name> <string:OHistogram name> <string:OHistogram name> [add,sub,mul,div] [<string:double> <string:double>]  
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OHistogram ohist,ohist1,ohist2;
  int        operation;
  double     c1,c2;
/*.........................................................................*/
  if( (a_argn!=5) && (a_argn!=7) )
    {
      CWarnF("Four or six arguments expected in :\n%s\n",OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  ohist = OHistogramGetIdentifier(a_args[1]);
  if(ohist==NULL) 
    {
      CWarnF ("Unknown OHistogram %s in :\n%s\n",a_args[1],OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  ohist1 = OHistogramGetIdentifier(a_args[2]);
  if(ohist==NULL) 
    {
      CWarnF ("Unknown OHistogram %s in :\n%s\n",a_args[1],OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  ohist2 = OHistogramGetIdentifier(a_args[3]);
  if(ohist==NULL) 
    {
      CWarnF ("Unknown OHistogram %s in :\n%s\n",a_args[1],OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

       if(strncmp(a_args[4],"add",3)==0)
    operation = OHistogramAdd;
  else if(strncmp(a_args[4],"sub",3)==0)
    operation = OHistogramSubstract;
  else if(strncmp(a_args[4],"mul",3)==0)
    operation = OHistogramMultiply;
  else if(strncmp(a_args[4],"div",3)==0)
    operation = OHistogramDivide;
  else
    {
      CWarnF ("Unknown operation %s in :\n%s\n",a_args[4],OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  c1 = c2 = 1.;
  if(a_argn==7)
    {
      int status;
      c1  = CStringConvertToDouble(a_args[5],&status);
      if(status==0) 
	{
	  CWarnF ("Argument %s not a number in :\n%s\n",a_args[5],OProcessGetCommand(a_process));
	  return EXIT_FAILURE;
	}
      c2  = CStringConvertToDouble(a_args[6],&status);
      if(status==0) 
	{
	  CWarnF ("Argument %s not a number in :\n%s\n",a_args[6],OProcessGetCommand(a_process));
	  return EXIT_FAILURE;
	}
    }

  if(OHistogramOperateBinary(ohist,ohist1,ohist2,operation,c1,c2)==0) return EXIT_FAILURE;

  return EXIT_SUCCESS;
}
/***************************************************************************/
int HoExecute_fitHistogram (
 int    a_argn
,char** a_args
,OProcess a_process
)
/***************************************************************************/
/*
  osh> fitHistogram <string:OHistogram name>
 for a Gaussian fit

  osh> fitHistogram <string:OHistogram name> [gaussian, breit_wigner, exponential, polynomial] [<integer:paramn>]
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OHistogram ohist;
  OFitType   type = OFitTypeGaussian;
  OFit       ofit;
  int        status;
/*.........................................................................*/
  if( (a_argn!=2) && (a_argn!=3) && (a_argn!=4) )
    {
      CWarnF("One, two or three arguments expected in :\n%s\n",OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  ohist = OHistogramGetIdentifier(a_args[1]);
  if(ohist==NULL) 
    {
      CWarnF ("Unknown OHistogram %s in :\n%s\n",a_args[1],OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  if(a_argn>=3)
    {
      type = OFitTypeGetIdentifier (a_args[2]);
    }

  ofit        = OFitMake();
  OFitSetType (ofit,type);
  if(type==OFitTypePolynomial) 
    {
      int paramn = 2;
      if(a_argn==4)
	{
	  int    status;
	  paramn = (int)CStringConvertToLong(a_args[3],&status);
	  if(status==0)
	    {
	      CWarnF ("Argument %s not a number in :\n%s\n",a_args[3],OProcessGetCommand(a_process));
	      return EXIT_FAILURE;
	    }
	}
      OFitSetNumberOfParameters (ofit,paramn);
    }

  status      = HoFitHistogram(ohist,ofit,0,NULL,NULL);

  OFitDelete  (ofit);

  return      status==0 ? EXIT_FAILURE : EXIT_SUCCESS;
}
