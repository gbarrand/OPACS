/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
static char what[] = "@(#)Ho v3";

#include <string.h>
#include <math.h>

#include <CMemory.h>
#include <CString.h>
#include <CList.h>
#include <CCharacter.h>
#include <CError.h>
#include <CPrinter.h>

#include <OStatement.h>

#include <Ho.h>

#ifdef __cplusplus
extern "C"{
#endif
static void    FillHistogram         (OTuple,OHistogram,int,OCut*,int,int*,int);
static void    FillScatter           (OTuple,OScatter,int,OCut*,int,int*);
static int     IsRowAccepted         (OTuple,int,int,OCut*);
static void    SetCuts               (OTuple,int,OCut*);
static void    CutTupleWithOperation (OTuple,int,OCut*,char*,OTuple,int);
static OTuple  DoOperation           (OTuple,int,char**,int*,int*,OCut**);
static int     MapAxis               (OTuple,int,OAxis);
static int     IsOperation           (char*);
static char*   GetVariables          (char*,int*,char***);
static void    ReportError           (char*,int);
#ifdef __cplusplus
}
#endif

#define NotFound      (-1)

static struct 
{
  CList cvts;
  char* defs[OAxisMaximumNumber];
} Class = {NULL,{"0","1","2","3"}};
/***************************************************************************/
void HoClearClass (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CListDelete (Class.cvts);
  Class.cvts  = NULL;
  what[0]     = '@'; /*c++ no warning.*/
}
/***************************************************************************/
void HoAddConverter (
  HoConvertFunction a_proc
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CListAddEntryFunction ((CList*)&Class.cvts,(CEntryFunction)a_proc);
}
/***************************************************************************/
void* HoConvert (
  char* a_string
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  HoConvertFunction* itema;
  void*              obj;
/*.........................................................................*/
  for(itema=(HoConvertFunction*)Class.cvts;(itema!=NULL) && (*itema!=NULL);itema++)
    {
       obj = (*itema)(a_string);
       if(obj!=NULL) return obj;      
    }
  return NULL;
}
/***************************************************************************/
int HoFitHistogram (
 OHistogram This  
,OFit       a_fit   
,int        a_binn
,int*       a_is
,int*       a_js
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int       count;
  int       binn;
  int       npar;
  char*     name;
  int       ipt;
  int       nbin;
  double*   xxs;
  double*   vals;
  double*   errs;
#define MAX_PARAM 50
  double    list[MAX_PARAM];

  double    zero = 0.;
  int       retval;
/*.........................................................................*/
  if(This==NULL)                       return 0;
  if(a_fit==NULL)                      return 0;
  if(OFitGetType(a_fit)==OFitTypeNone) return 1;

  name = OHistogramGetName (This);
  name = name==NULL ? "(nil)" : name;

  npar = OFitGetNumberOfParameters(a_fit);
  if(npar==0) 
    {
      CWarnF ("Can't fit histogram %s :\n number of fit parameters is null.\n",name);
      return 0;
    }

  if(OHistogramGetDimension(This)!=1) 
    {
      CWarnF ("Can't fit histogram %s :\n its dimension is not one.\n",name);
      return 0;
    }

  if(OHistogramGetNumberOfEntries(This)==0) 
    {
      CWarnF ("Can't fit histogram %s :\n it has no entries.\n",name);
      return 0;
    }

/*
  double    sw;
  sw = OHistogramGetSumOfWeights (This);
  if(sw==zero)
    {
      CWarnF ("Can't fit histogram %s :\n its total sum of weights is zero.\n",name);
      return 0;
    }*/

/* get histos datas {(abs,val,err)} */
  OHistogramGetIthAxisAttributesF (This,OAxisX,"n",&nbin);
  if(nbin<=0) 
    {
      CWarnF ("Can't fit histogram %s :\n its number of bins is zero.\n",name);
      return 0;
    }

  xxs  = (double*)CMemoryAllocateBlock(nbin*sizeof(double));
  vals = (double*)CMemoryAllocateBlock(nbin*sizeof(double));
  errs = (double*)CMemoryAllocateBlock(nbin*sizeof(double));
  if( (xxs==NULL) || (vals==NULL) || (errs==NULL) )    return 0;
  ipt  = 0;  
  binn = ( (a_binn!=0) && (a_is!=NULL) ) ? a_binn : nbin;
  for(count=0;count<binn;count++)
    { double           xx,val,err;
      int              bini,nent;
      bini             = ( (a_binn!=0) && (a_is!=NULL) ) ? a_is[count] : count;
      nent             = OHistogramGetBinNumberOfEntries (This,bini);
      if(nent==0)      continue;  /* do not take bin without entry */
      xx               = OHistogramGetIthAxisBinMiddle    (This,OAxisX,bini);  /* take middle of bin */
      val              = OHistogramGetBinSumOfWeights     (This,bini);
      err              = sqrt (fabs(val)); /* take statistical error */
      if(err==zero) 
        { 
	  CWarnF           ("Can't fit histogram %s :\n can't compute an error for bin %d.\n",name,bini);
          CMemoryFreeBlock (xxs);
          CMemoryFreeBlock (vals);
          CMemoryFreeBlock (errs);
          return      0;
        }
      xxs [ipt] = xx;
      vals[ipt] = val;
      errs[ipt] = err;
      ipt++;
    }
  if(ipt==0)
    {
      CWarnF           ("Can't fit histogram %s :\n unable to find starting points.\n",name);
      CMemoryFreeBlock (xxs);
      CMemoryFreeBlock (vals);
      CMemoryFreeBlock (errs);
      return       0;
    }

  if(OFitAreStartValuesAutomated(a_fit)==1) 
    { /* auto-set starting values */
      switch(OFitGetType(a_fit))
	{
	case OFitTypeNone:    
	  break;
	case OFitTypeGaussian:
	  {double          bmin,bmax;
	   OHistogramGetBinSumOfWeightsRange (This,&bmin,&bmax);
	   list[0] = bmax-bmin;
	   list[1] = OHistogramGetIthAxisMeanValue(This,OAxisX);
	   list[2] = OHistogramGetIthAxisStandardDeviation   (This,OAxisX);
           if(list[2]==zero)
	     {
	       CWarnF           ("Can't fit histogram %s :\n starting standard deviation is zero !!.\n",name);
	       CMemoryFreeBlock (xxs);
	       CMemoryFreeBlock (vals);
	       CMemoryFreeBlock (errs);
	       return       0;
	     }
	   OFitSetStartValues (a_fit,3,list);
	 }break;
	case OFitTypeBreitWigner:
	  {double     bmin,bmax;
	   OHistogramGetBinSumOfWeightsRange(This,&bmin,&bmax);
	   list[0] = bmax-bmin;
	   list[1] = OHistogramGetIthAxisMeanValue         (This,OAxisX);
	   list[2] = OHistogramGetIthAxisStandardDeviation (This,OAxisX);
           if(list[2]==zero)
	     {
	       CWarnF           ("Can't fit histogram %s :\n starting standard deviation is zero !!.\n",name);
	       CMemoryFreeBlock (xxs);
	       CMemoryFreeBlock (vals);
	       CMemoryFreeBlock (errs);
	       return       0;
	     }
	   OFitSetStartValues (a_fit,3,list);
	 }break;
	case OFitTypeExponential:  
	 {
	   int          status1,status2;
	   double       xx1,val1,xx2,val2,a,b;
           status1      = OHistogramGetHalfLowerMeanPoint (This,&xx1,&val1);
           status2      = OHistogramGetHalfUpperMeanPoint (This,&xx2,&val2);
           if( (status1==0) || (status2==0) || ((xx2-xx1)==zero) )
	     {
	       CWarnF           ("Can't fit histogram %s :\n unable to compute starting values for fit parameters.\n",name);
	       CMemoryFreeBlock (xxs);
	       CMemoryFreeBlock (vals);
	       CMemoryFreeBlock (errs);
	       return       0;
	     }
	   /*printf  ("debug: %g %g : %g %g\n",xx1,val1,xx2,val2);*/
           /*Deduce a,b of exp(a+bx) with the two upper points.*/ 
	   b       = log( val2 / val1) / ( xx2 - xx1);
	   a       = log( val1 / exp ( b * xx1 ) );
	   list[0] = a;
	   list[1] = b;
	   OFitSetStartValues (a_fit,2,list);
	 }break;
	case OFitTypePolynomial: 
	  if(npar==2)
	    {
	      int          status1,status2;
	      double       xx1,val1,xx2,val2,a,b;
	      status1      = OHistogramGetHalfLowerMeanPoint (This,&xx1,&val1);
	      status2      = OHistogramGetHalfUpperMeanPoint (This,&xx2,&val2);
	      if( (status1==0) || (status2==0) || ((xx2-xx1)==zero) )
		{
		  CWarnF           ("Can't fit histogram %s :\n unable to compute starting values for fit parameters.\n",name);
		  CMemoryFreeBlock (xxs);
		  CMemoryFreeBlock (vals);
		  CMemoryFreeBlock (errs);
		  return       0;
		}
	      /*
		printf  ("debug: %g %g : %g %g\n",xx1,val1,xx2,val2);
		*/
	      /*Deduce a,b of a+b*x with the two upper points.*/ 
	      b       = (val2 - val1) / (xx2 - xx1);
	      a       = val1 - b * xx1;
	      list[0] = a;
	      list[1] = b;
	      OFitSetStartValues (a_fit,2,list);
	    }
	  break; /* assume done externaly */
	case OFitTypeUser:       break; /* assume done externaly */
	}
    }
 
  if(OFitAreStepsAutomated(a_fit)==1) 
    { /* auto-set steps */
      for(count=0;count<npar;count++)
	{ double val;
	  val    = OFitGetIthStartValue(a_fit,count);
	  if(val==zero)
	    {
/*
	      CWarnF ("Can't fit histogram %s :\n unable to compute step value for fit parameter number %d.\n",name,count);
	      CMemoryFreeBlock (xxs);
	      CMemoryFreeBlock (vals);
	      CMemoryFreeBlock (errs);
	      return           0;
*/
	      val = 0.1;
	    } 
	  else
	    {
	      val = fabs(val)/10.;
	    }
	  list[count] = val;
	}
      OFitSetSteps (a_fit,npar,list);
  }

  retval   = OFitExecute   (a_fit,ipt,xxs,vals,errs);

  CMemoryFreeBlock (xxs);
  CMemoryFreeBlock (vals);
  CMemoryFreeBlock (errs);

  a_js     = NULL;
  return   retval;
}
/***************************************************************************/
/***************************************************************************/
/******* OHistogram ********************************************************/
/***************************************************************************/
/***************************************************************************/
OHistogram HoFillHistogramFromTuple (
 OHistogram a_hist 
,OTuple a_tuple 
,int    a_bindn
,char** a_binds
,int a_cutn
,OCut* a_cuts
,int a_profile
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OHistogram ohist;
  int        count;
  OTuple     otuple;
  int        cutn;
  OCut*      cuts;
  char*      binds[OAxisMaximumNumber];
  int        cols[OAxisMaximumNumber];
  int        changed;
/*.........................................................................*/
  if(a_hist==NULL)
    {
      if(a_bindn<=0) return NULL;
      ohist = OHistogramMake();
      OHistogramSetDimension        (ohist,a_profile==1 ? 1 : a_bindn);
    }
  else
    {
      if(a_bindn<=0) return a_hist;
      ohist = a_hist;
    }

  if(OHistogramGetDimension(ohist)==0) 
    {
      OHistogramSetDimension(ohist,1); /* ohist could come from OHistogramMake() */
    }

  for(count=0;count<a_bindn;count++)
    {
      if(a_binds==NULL)
	{
	  binds[count] = Class.defs[count];
	}
      else
	{
	  binds[count] = a_binds[count];
	  if(binds[count]==NULL) binds[count] = Class.defs[count];
	}
      cols[count]  = 0;
    }

  cutn        = a_cutn;
  cuts        = a_cuts;

  otuple      = DoOperation (a_tuple,a_bindn,binds,cols,&cutn,&cuts);

  changed     = 0;
  for(count=0;count<a_bindn;count++)
    {
      OAxis                           oaxis;
      char*                           string;
      oaxis                           = OHistogramGetIthAxis(ohist,(OAxisIndex)count);
      if(OAxisVerifyNumberOfBins(oaxis,20)==1) changed = 1;
      if(MapAxis(otuple,cols[count],oaxis)==1) changed = 1;
      string                          = OTupleGetIthLabel (otuple,cols[count]);
      OHistogramSetIthAxisAttributesF (ohist,(OAxisIndex)count,"t",string);
    }

  if(changed==1) OHistogramAllocateMemory  (ohist);

  FillHistogram (otuple,ohist,cutn,cuts,a_bindn,cols,a_profile);

  if(otuple!=a_tuple) OTupleDelete(otuple);

  if(a_profile==1) OHistogramDoProfile (ohist); 

  return ohist;
}
/***************************************************************************/
static void FillHistogram (
 OTuple This 
,OHistogram a_hist 
,int a_cutn
,OCut* a_cuts
,int  a_number 
,int* a_cols
,int a_profile
)
/***************************************************************************/
/* a_number * (col,nbin,min,max)                                           */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
   int row;
  int       rows;
  int       dim;
/*.........................................................................*/
  if(This==NULL)   return;
  if((a_number<=0)||(a_number>OAxisMaximumNumber)) return;
  if(a_hist==NULL) return;

  for(count=0;count<a_number;count++)
    {
      if(OTupleVerifyColumnIndex(This,a_cols[count])==0) return;
    }

  SetCuts        (This,a_cutn,a_cuts); /* convert col name in col number in cuts */
  rows                    = OTupleGetNumberOfRows(This);

  dim = OHistogramGetDimension(a_hist);
  if( (a_profile==1) && (dim==1) && (a_number>=2))
    {
      for (row=0;row<rows;row++) 
	{
	  if(IsRowAccepted(This,row,a_cutn,a_cuts)==1) 
	    OHistogramFillOneDimensional 
	      (a_hist,OTupleGetValueCareless(This,row,a_cols[0]),OTupleGetValueCareless(This,row,a_cols[1]));
	}
    }
  else if( (a_number>=1) && (dim==1))
    {
      for (row=0;row<rows;row++) 
	{
	  if(IsRowAccepted(This,row,a_cutn,a_cuts)==1) 
	    OHistogramFillOneDimensional (a_hist,OTupleGetValueCareless(This,row,a_cols[0]),1.);
	}
    }
  else if( (a_number>=2) && (dim==2))
    {
      for (row=0;row<rows;row++) 
	{
	  if(IsRowAccepted(This,row,a_cutn,a_cuts)==1) 
	      OHistogramFillTwoDimensional 
		(a_hist,OTupleGetValueCareless(This,row,a_cols[0]),OTupleGetValueCareless(This,row,a_cols[1]),1.);
	}
    }
}
/***************************************************************************/
/***************************************************************************/
/******** OScatter *********************************************************/
/***************************************************************************/
/***************************************************************************/
OScatter HoFillScatterFromTuple (
 OScatter a_scat 
,OTuple a_tuple 
,int    a_bindn
,char** a_binds
,int a_cutn
,OCut* a_cuts
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OScatter     oscat;
  int       count;
  int       cutn;
  OCut*     cuts;
  OTuple    otuple;
  char*     binds[OAxisMaximumNumber];
  int       cols[OAxisMaximumNumber];
/*.........................................................................*/
  if(a_scat==NULL) 
    {
      if(a_bindn<=0) return NULL;
      oscat             = OScatterMake();
    }
  else
    {
      if(a_bindn<=0) return a_scat;
      oscat = a_scat;
    }
  
 {int ncol;
  ncol              = OTupleGetNumberOfColumns(a_tuple);
  if(a_bindn>ncol)  a_bindn = ncol;}

  OScatterSetDimension (oscat,a_bindn);

  for(count=0;count<a_bindn;count++)
    {
      if(a_binds==NULL)
	{
	  binds[count] = Class.defs[count];
	}
      else
	{
	  binds[count] = a_binds[count];
	  if(binds[count]==NULL) binds[count] = Class.defs[count];
	}
      cols[count]  = 0;
    }

  cutn        = a_cutn;
  cuts        = a_cuts;

  otuple      = DoOperation (a_tuple,a_bindn,binds,cols,&cutn,&cuts);

  for(count=0;count<a_bindn;count++)
    {
      char*                         string;
      MapAxis                       (otuple,cols[count],OScatterGetIthAxis(oscat,(OAxisIndex)count));
      string                        = OTupleGetIthLabel (otuple,cols[count]);
      OScatterSetIthAxisAttributesF (oscat,(OAxisIndex)count,"t",string);
    }

  FillScatter  (otuple,oscat,cutn,cuts,a_bindn,cols);

  if(otuple!=a_tuple) OTupleDelete(otuple);

  return  oscat;
}
/***************************************************************************/
static void FillScatter (
 OTuple This 
,OScatter a_scat
,int a_cutn
,OCut* a_cuts
,int  a_number 
,int* a_cols
)
/***************************************************************************/
/* a_number * (col,nbin,min,max)                                           */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
   int row;
  int       rows;
/*.........................................................................*/
  if(This==NULL)   return;
  if((a_number<=0)||(a_number>OAxisMaximumNumber)) return;
  if(a_scat==NULL) return;

  for(count=0;count<a_number;count++)
    {
      if(OTupleVerifyColumnIndex (This,a_cols[count])==0) return;
    }

  OScatterAllocatePoints        (a_scat,OTupleGetNumberOfRows (This));
  SetCuts        (This,a_cutn,a_cuts); /* convert col name in col number in cuts */
  rows                    = OTupleGetNumberOfRows(This);
  switch(a_number) 
    {
       case 1:
	  for (row=0;row<rows;row++) 
	    {
	      if(IsRowAccepted(This,row,a_cutn,a_cuts)==1) 
  	        OScatterFillOneDimensional (a_scat,OTupleGetValueCareless(This,row,a_cols[0]));
	    }
	  break;
       case 2:
	  for (row=0;row<rows;row++) 
	    {
	      if(IsRowAccepted(This,row,a_cutn,a_cuts)==1) 
  	        OScatterFillTwoDimensional (a_scat,OTupleGetValueCareless(This,row,a_cols[0]),
				         OTupleGetValueCareless(This,row,a_cols[1]));
	    }
	  break;
       case 3:
	  for (row=0;row<rows;row++) 
	    {
	      if(IsRowAccepted(This,row,a_cutn,a_cuts)==1) 
  	        OScatterFillThreeDimensional (a_scat,OTupleGetValueCareless(This,row,a_cols[0]),
				           OTupleGetValueCareless(This,row,a_cols[1]),
				           OTupleGetValueCareless(This,row,a_cols[2]));
	    }
	  break;
    }
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static OTuple DoOperation (
 OTuple   a_tuple 
,int      a_bindn
,char**   a_binds
,int*     a_cols
,int*     a_cutn
,OCut**   a_cuts
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int    count;
  OTuple otuple = NULL;
  int    oper;
/*.........................................................................*/
  if(a_tuple==NULL) return NULL;
  oper = 0;
  for(count=0;count<a_bindn;count++)
    {
      int    col;
      col    = OTupleGetColumnIndexFromString (a_tuple,a_binds[count]);
      if( (col==NotFound) && (IsOperation(a_binds[count])==1) )
	{ 
	  if(oper==0)
	    {
	      otuple           = OTupleCreate 
		                 (NULL,a_bindn,OTupleGetNumberOfRows(a_tuple),"operation",a_bindn,a_binds);
	      OTupleInitialize (otuple);
	      oper       = 1; 
	    }
	}
      else /*Column of the tuple.*/
        {
	  a_cols[count] = col;
	}
    }
  if(oper==1)
    {
      for(count=0;count<a_bindn;count++)
	{
	  CutTupleWithOperation  (a_tuple,*a_cutn,*a_cuts,a_binds[count],otuple,count);
	  a_cols[count]   = count;
	}
      if(a_cutn!=NULL) *a_cutn     = 0;
      if(a_cuts!=NULL) *a_cuts     = NULL;
    }
  else
    {
      otuple = a_tuple;
    }
  return otuple;
}  
/***************************************************************************/
static int MapAxis (
 OTuple This 
,int    a_col   
,OAxis  a_axis   
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int autoscale,changed;
/*.........................................................................*/
  if(This==NULL)  return 0;
  if(a_axis==NULL) return 0;
  changed = 0;
  if(OTupleVerifyColumnIndex(This,a_col)==1) 
    {
      double              min,max;
      OAxisGetAttributesF (a_axis,"amx",&autoscale,&min,&max);
      if(autoscale==1)   
        { 
	  double               tmin,tmax;
          OTupleGetColumnRange (This,a_col,&tmin,&tmax);
	  if((tmin!=min)||(tmax!=max))
	    {
	      OAxisSetAttributesF  (a_axis,"mx",tmin,tmax);
	      changed              = 1;
	    }
        }
    }
  OAxisAdjust (a_axis);
  return      changed;
}
/***************************************************************************/
static void CutTupleWithOperation (
 OTuple This  
,int    a_cutn
,OCut*  a_cuts 
,char*  a_string 
,OTuple a_to     
,int    a_col    
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  int         varn = 0;
  char**      vars = NULL;
  char*       varb = NULL;
  char**      itema;
  int         icol,row,rows;

  int*        cols = NULL;
  int         coli = 0;

  OStatement       ostmt;
/*.........................................................................*/
  if(This==NULL)     return;
  if(a_to==NULL)     return;
  if(a_string==NULL) return;
  if(OTupleVerifyColumnIndex (a_to,a_col)==0) return;

  rows    = OTupleGetNumberOfRows      (This);
  icol    = OTupleGetColumnIndexFromString (This,a_string);
  if(icol!=NotFound) /* a_string is a column of This */ 
    {
      SetCuts (This,a_cutn,a_cuts); /* convert col name in col number in cuts */
/* compute result and put it in a_to OTuple at column a_col */
      row     = 0;
      for(count=0;count<rows;count++)
        { 
          if(IsRowAccepted(This,count,a_cutn,a_cuts)==0) continue; /* row cutted */
          OTupleFill (a_to,row,a_col,OTupleGetValueCareless(This,count,icol));
          row++;
        }
      OTupleLimitRows (a_to,row);
      return ;
    }

/* get variable names */
  varb               = GetVariables (a_string,&varn,&vars);
  CListRemoveDuplicates ((CList)vars,(CListMatchEntryFunction)CStringCompare,0);
  varn               = CListGetSize((CList)vars);
  cols               = (int*)CMemoryAllocateBlock(varn * sizeof(int));
  if( (varn==0) || (vars==NULL) || (cols==NULL) )
    { 
      ReportError    ("CutTupleWithOperation",CErrorFormat);
      CMemoryFreeBlock (cols);
      CMemoryFreeBlock (vars);
      CMemoryFreeBlock (varb);
      return;
    }

/* get columns of variables */
  itema = vars;
  while(*itema!=NULL)
    { 
      icol = OTupleGetColumnIndex(This,*itema);
      if(icol!=NotFound)    
        {
	  cols[coli] = icol;
          coli++;
	  itema++;
        }
      else if(OStatementGetFunctionIdentifier(*itema)!=NULL) 
	{
	  CListRemoveEntry ((CList)vars,*itema);
	}
      else
        {
	  ReportError    ("CutTupleWithOperation",CErrorFormat);
	  CMemoryFreeBlock (cols);
	  CMemoryFreeBlock (vars);
	  CMemoryFreeBlock (varb);
	  return;
	}
    }
  varn = coli;

  SetCuts (This,a_cutn,a_cuts); /* convert col name in col number in cuts */
/* compute result and put it in a_to OTuple at column a_col */

  ostmt = OStatementCreate (a_string,varn,vars);

  row   = 0;
  if(ostmt!=NULL) 
    {
      double* vals;
      vals    = (double*)CMemoryAllocateBlock(varn * sizeof(double));  
      for(count=0;count<rows;count++)
        { 
          if(IsRowAccepted(This,count,a_cutn,a_cuts)==1)
	    {
/* set variables */
	      double value;
	      for(coli=0;coli<varn;coli++)
		{
		  vals[coli]     = OTupleGetValueCareless(This,count,cols[coli]);
		}
	      value      = OStatementEvaluate(ostmt,varn,vals);
	      OTupleFill (a_to,row,a_col,value);
	      row++;
	    }
        }
      CMemoryFreeBlock(vals);

  } /* endif stmt */

  OStatementDelete       (ostmt);

  OTupleLimitRows   (a_to,row);

  CMemoryFreeBlock          (cols);
  CMemoryFreeBlock          (vars);
  CMemoryFreeBlock          (varb);

}
/***************************************************************************/
static int IsRowAccepted (
 OTuple This 
,int    a_row   
,int    a_cutn
,OCut*  a_cuts  
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  int     cols;
  double* row; 
/*.........................................................................*/
  if(a_cuts==NULL) return 1;
  row         = OTupleGetRowPointer(This,a_row);
  if(row==NULL)    return 0;
  cols        = OTupleGetNumberOfColumns(This);
  for(count=0;count<a_cutn;count++)
    {
      if(OCutIsRowAccepted(a_cuts[count],cols,row)==0) return 0;
    }
  return 1;
}
/***************************************************************************/
static void SetCuts (
 OTuple This 
,int a_cutn
,OCut* a_cuts 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  char** labels;
  int    nlabel;
/*.........................................................................*/
  if(a_cuts==NULL) return;
  labels = OTupleGetLabels (This,&nlabel);
  for(count=0;count<a_cutn;count++)  
    { 
      OCutSetColumns (a_cuts[count],nlabel,labels);
    }
}
/***************************************************************************/
static int IsOperation (
 char*   This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 0;
  if(strstr(This,"+")!=NULL) return 1;
  if(strstr(This,"-")!=NULL) return 1;
  if(strstr(This,"*")!=NULL) return 1;
  if(strstr(This,"/")!=NULL) return 1;
  if(strstr(This,"(")!=NULL) return 1;
  if(strstr(This,")")!=NULL) return 1;
  return 0;
}
/****************************************************************************/
static char* GetVariables (
 char*   a_string 
,int*    a_linen
,char*** a_lines
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  char*  string;
  int    length;
  char*  token;
  int    linen = 0;
  char** lines = NULL;
  int    in_string;
#define LEX_ALPHA(a_char)  ( (CCharacterIsAlphanumeric(a_char)==1) || (a_char=='_')  ? 1:0 )
/*.........................................................................*/
  if(a_linen!=NULL) *a_linen       = 0;
  if(a_lines!=NULL) *a_lines       = NULL;
  if(a_string==NULL)  return NULL;
  length         = strlen(a_string);
  if(length<=0)  return NULL;

  string         = CStringDuplicate(a_string);
  if(string==NULL)    return NULL;

  token      = NULL;
  in_string  = 0;
  for(count=0;count<length+1;count++) 
    { char ch;
      ch   = string[count];

      if( LEX_ALPHA(ch) && (in_string==0) ) /* start of variable */
        {
          in_string = 1;
          token     = string + count;
        }
      else if( ( (!LEX_ALPHA(ch))&&(CCharacterIsDigit(ch)==0)) && (in_string==1) ) /* end of variable or end of string */
        { 
          in_string       = 0;
          string[count]   = 0;
          if( (token!=NULL) && (*token!='\0')) 
	    {
	      CListAddEntry ((CList*)&lines,token);
	      linen++;
	    }
          token           = NULL;
        }
    }

  if(a_linen!=NULL) *a_linen = linen;
  if(a_lines!=NULL) *a_lines = lines;
  
  return   string;
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
  CErrorHandle ("Ho","Ho",a_name,a_code);
}
