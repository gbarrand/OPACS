/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifdef HAS_XZ

#include <math.h>
#include <string.h>

#include <HF77.h>

#include <CMemory.h>
#include <CPrinter.h>
#include <CString.h>
#include <CText.h>
#include <FString.h>

#include <HBook.h>
#include <HFile.h>

#include <OFit.h>
#include <OHistogram.h>
#include <OTuple.h>

#include <HoXz.h>

/* C calling f77 */
#ifndef HAS_NOT_U
#define omnfit omnfit_
#define oxfval oxfval_
#endif

#ifdef __cplusplus
extern "C"{
#endif
void  omnfit (int*, int*, double*, double*, double*, int*, double*, double*);
void  oxfval (double*,int*,double*,double*);
#ifdef __cplusplus
}
#endif

#define NotFound      (-1)

static struct 
{
  OFit   fit;
} Class = {NULL};
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
void* HoXzConvert (
 char* a_string
)
/***************************************************************************/
/*
  Convert the name of an HBOOK object and convert it
 to an Ho object.
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int    id;
  char*  title;
  int    ncol = 0;
  int    nrow = 0;
  char** tags;
/*.........................................................................*/
  id = HoXzGetIdentifier (a_string);
  if(id==0) return NULL;

  HBookGetTupleInformations (id,&title,&ncol,&nrow,&tags);

       if(ncol>0)
    {  /* tuple */
      OTuple       otuple;
      int          icol;

      otuple = OTupleGetIdentifier(a_string);
      if(otuple!=NULL) OTupleDelete(otuple);

      otuple        = OTupleCreate(a_string,ncol,nrow,title,0,NULL);

      for(icol=0;icol<ncol;icol++)
        {
          OTupleSetIthColumnLabel (otuple,icol,tags[icol]);
        }
      CTextDelete (ncol,tags);

     {int    nidn,ie,ierror;
      float* data;
      data  = (float*)CMemoryAllocateBlock( (ncol+10) * sizeof(float));
      if(data==NULL) 
	{
	  OTupleDelete(otuple);
	  return NULL;
	}
      nidn  = 0;
      ie    = 0;
      while(1)
        {
          ie++;
          hgn (&id,&nidn,&ie,data,&ierror);
          if  (ierror<0) break; /* exit if end of file */
          OTupleFillRowWithFloats (otuple,data);
        }
      CMemoryFreeBlock (data);}

      OTupleEndFilling (otuple);

      return        otuple;
    }
  else 
    {
      int    nx,ny;
      double xmi,xma,ymi,yma;

      CStringDelete  (title);
      HBookGetHistogramInformations (id,&title,&nx,&xmi,&xma,&ny,&ymi,&yma);

      if( ny!=0 )
	{  /* Histo 2D */
	  int          nent;
	  OHistogram        hist;
	  int          ix,iy;
	  int          iax;
	  float        mean,stddev;
	  
          hist = OHistogramGetIdentifier(a_string);
          if(hist!=NULL) OHistogramDelete(hist);

	  hist               = OHistogramCreate (a_string,2,"",nx,xmi,xma,"",ny,ymi,yma);
	  OHistogramSetTitle (hist,title);
	  CStringDelete      (title);
	  
	  hnoent             (&id,&nent);
	  OHistogramSetNumberOfEntries    (hist,nent);
	  OHistogramSetSumOfWeights       (hist,(double)nent);  /*Assume weights are one.*/
	  iax                = OAxisX;
	  ohstat             (&id ,&iax,&mean,&stddev);
	  OHistogramSetIthAxisSumOfWeightedCoordinates        (hist,OAxisX,(double)(mean * nent));
	  OHistogramSetIthAxisSumOfWeightedSquaredCoordinates (hist,OAxisX,(double)((stddev * stddev + mean * mean) * nent) );
	  iax                = OAxisY;
	  ohstat             (&id ,&iax,&mean,&stddev);
	  OHistogramSetIthAxisSumOfWeightedCoordinates        (hist,OAxisY,(double)(mean * nent));
	  OHistogramSetIthAxisSumOfWeightedSquaredCoordinates (hist,OAxisY,(double)((stddev * stddev + mean * mean) * nent) );

	  for(iy=1;iy<=ny;iy++)
	    for(ix=1;ix<=nx;ix++)
	      { float              val,sig;
		val                = hij ( &id, &ix, &iy);
		nent               = (int)val;
		sig                = val;
		OHistogramSetBinSumOfWeights        (hist,ix-1,iy-1,(double)val);
		OHistogramSetBinNumberOfEntries     (hist,ix-1,iy-1,(int)val);
		OHistogramSetBinSumOfSquaredWeights (hist,ix-1,iy-1,(double)sig);
		/* unable to have bin error from HBOOK for a 2D histo */
	      }
	  
	  return        hist;
	}
      else
	{  /* Histo 1D */
	  int          nent;
	  OHistogram   hist;
	  int          ix;
	  int          iax;
	  float        val,mean,stddev;

          hist = OHistogramGetIdentifier(a_string);
          if(hist!=NULL) OHistogramDelete(hist);

	  hist               = OHistogramCreate (a_string,1,"",nx,xmi,xma);
	  OHistogramSetTitle (hist,title);
	  CStringDelete      (title);
	  
	  hnoent             (&id,&nent);
	  OHistogramSetNumberOfEntries    (hist,nent);
	  OHistogramSetSumOfWeights       (hist,(double)nent);  /*Assume weights are one.*/

	  iax                  = -1;
	  ohstat               (&id ,&iax,&mean,&stddev);
	  OHistogramSetIthAxisSumOfWeightedCoordinates        (hist,OAxisX,(double)(mean * nent));
	  OHistogramSetIthAxisSumOfWeightedSquaredCoordinates (hist,OAxisX,(double)((stddev * stddev + mean * mean) * nent) );
	  ix                   = 0;
	  val                  = hi  ( &id, &ix);
	  OHistogramSetIthAxisUnderflow (hist,OAxisX,(int)val);
	  ix                   = nx+1;
	  val                  = hi  ( &id, &ix);
	  OHistogramSetIthAxisOverflow  (hist,OAxisX,(int)val);

	  for(ix=1;ix<=nx;ix++)
	    { 
	      float              sig;
	      val                = hi  ( &id, &ix);
	      nent               = (int)val;
	      sig                = val;
	      OHistogramSetBinSumOfWeights        (hist,ix-1,(double)val);
	      OHistogramSetBinNumberOfEntries     (hist,ix-1,(int)val);
	      OHistogramSetBinSumOfSquaredWeights (hist,ix-1,(double)sig);
	    }
	  return        hist;
	}
      
    }
}
/***************************************************************************/
void HoXzBuildHBOOK_Object (
 void* This
,int a_ident
)
/***************************************************************************/
/*
  Convert an Ho object (OHistogram, OTuple) and convert it
 to an HBOOK object. The HBOOK identifier is given by the user.
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int    ident;
/*.........................................................................*/
  if(This==NULL) return;

  HBookInitialize (0);

  ident = a_ident;
  if(hexist(&ident)!=0) hdelet (&ident);  

  if(OHistogramIsValid((OHistogram)This)==1) /*It is an OHistogram.*/
    {
      int    dim;

      dim = OHistogramGetDimension ((OHistogram)This);

      if(dim==1)
	{
	  int    nx;
	  double xmi,xma;
	  int    ix;

	  OHistogramGetIthAxisAttributesF    ((OHistogram)This,OAxisX,"nmx",&nx,&xmi,&xma);

	  HBookCreateOneDimensionalHistogram (ident,OHistogramGetTitle((OHistogram)This),nx,xmi,xma);

/*Loose under/overflow.*/

	  for(ix=0;ix<nx;ix++)
	    { 
	      float  x,y,val;
	      x      = (float)OHistogramGetIthAxisBinMiddle    ((OHistogram)This,OAxisX,ix);
	      y      = 0.;
	      val    = (float)OHistogramGetBinSumOfWeights     ((OHistogram)This,ix);
	      hfill  (&ident,&x,&y,&val);           
	    }
	  
	}
      else if(dim==2)
	{
	  int    nx,ny;
	  double xmi,xma,ymi,yma;
	  int    ix,iy;
	  
	  OHistogramGetIthAxisAttributesF    ((OHistogram)This,OAxisX,"nmx",&nx,&xmi,&xma);
	  OHistogramGetIthAxisAttributesF    ((OHistogram)This,OAxisY,"nmx",&ny,&ymi,&yma);

	  HBookCreateTwoDimensionalHistogram (ident,OHistogramGetTitle((OHistogram)This),nx,xmi,xma,ny,ymi,yma);

/*Loose under/overflow.*/

	  for(iy=0;iy<ny;iy++)
	    for(ix=0;ix<nx;ix++)
	      { 
		float  x,y,val;
		x      = (float)OHistogramGetIthAxisBinMiddle  ((OHistogram)This,OAxisX,ix);
		y      = (float)OHistogramGetIthAxisBinMiddle  ((OHistogram)This,OAxisY,iy);
		val    = (float)OHistogramGetBinSumOfWeights   ((OHistogram)This,ix,iy);
		hfill  (&ident,&x,&y,&val);           
	      }

	}

    }
  else if(OTupleIsValid((OTuple)This)==1) /*It is an OTuple.*/
    {
      int    coln,rown,labeln,row;
      char*  title;
      char** labels;
      float* vals;
      coln   = OTupleGetNumberOfColumns ((OTuple)This);
      rown   = OTupleGetNumberOfRows    ((OTuple)This);
      if(coln<=0) return;
      title  = OTupleGetTitle  ((OTuple)This);
      labels = OTupleGetLabels ((OTuple)This,&labeln);
      HBookCreateTuple (ident,title,coln,rown,labels);
      if(hexist(&ident)==0) 
	{
	  CWarnF ("HBOOK tuple %d not created.\n",ident);
	  return;
	}
      vals = (float*)CMemoryAllocateBlock (coln * sizeof(float));
      if(vals==NULL) return;
      for(row=0;row<rown;row++)
	{
	  int     coli;
	  double* dvals;
	  dvals   = OTupleGetRowPointer ((OTuple)This,row);
	  for(coli=0;coli<coln;coli++) vals[coli] = (float)dvals[coli];
	  hfn     (&ident,vals);
	}
      CMemoryFreeBlock (vals);
    }

}
/***************************************************************************/
int HoXzConvertToHBOOK (
 char* a_string
)
/***************************************************************************/
/*
  Take the name of an Ho object (OHistogram, OTuple) and convert it
 to an HBOOK object. The HBOOK identifier is produced automatically.
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OHistogram ohist;
  OTuple     otuple;
/*.........................................................................*/
  ohist = OHistogramGetIdentifier(a_string);
  if(ohist!=NULL)
    {
      int    ident;
      ident  = HBookBuildIdentifier (0);
      HoXzBuildHBOOK_Object (ohist,ident);
      return ident;
    }
  otuple = OTupleGetIdentifier(a_string);
  if(otuple!=NULL)
    {
      int    ident;
      ident  = HBookBuildIdentifier (0);
      HoXzBuildHBOOK_Object (otuple,ident);
      return ident;
    }
  return NotFound;
}
/***************************************************************************/
void HoXzProduceRZ_File (
 char* a_fname
,CList a_objs
,int   a_userData
)
/***************************************************************************/
/*
  Put the a list of Ho objects and put then in an HBOOK RZ file.
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int        objn;
  int        unit = 11;
  FString    fstr,fdname;
  int        lstr,ldname,status;
  int        mode = 1;
/*.........................................................................*/
  if( (a_fname==NULL) || (*a_fname=='\0') ) return;
  objn = CListGetSize(a_objs);
  if(objn<=0)  return;
  HBookInitialize (0);
  fstr   = FStringCreateFromC (a_fname,&lstr);
  fdname = FStringCreateFromC ("HOXZ",&ldname);
  status = ohopen (&unit,fstr,&lstr,fdname,&ldname,&mode);
  if(status==0)          
    {
      CWarnF ("%s: can't open %s file.\n",a_fname);
    }
  else
    {
      int count;
      int ident = 0;
      for(count=0;count<objn;count++)
	{
	  OHistogram ohist;
	  ohist = (OHistogram)(a_objs[count]);
	  if(a_userData==1)
            {
	      int      userData;
	      userData = (int)OHistogramGetUserData (ohist);
	      if(userData==0)
 	        ident  = HBookBuildIdentifier (ident);
	      else
		ident  = userData;
	    }
	  else
	    {
	      ident  = HBookBuildIdentifier (ident);
	    }
	  HoXzBuildHBOOK_Object (ohist,ident);
	  ohrout (&ident);
	  hdelet (&ident);
	}
    }
  ohrend        (&unit,fdname,&ldname);
  FStringDelete (fstr);
  FStringDelete (fdname);
}
/***************************************************************************/
int HoXzGetIdentifier (
 char* a_string
)
/***************************************************************************/
/*
   The given string is of the form "//fname/directory/ident".
  Do also the import and return the id of object stored in PAWC.
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*  dname;
  char*  pos;
  int    status;
  int    id;
  HFile  file;
/*.........................................................................*/
  if( (a_string==NULL) || (*a_string=='\0') ) return 0;

/*find directory and ident*/
  dname = CStringDuplicate(a_string);
  pos   = strrchr(dname,'/');
  if(pos==NULL)           /*no directory specified*/
    {
      id = (int)CStringConvertToLong(dname,&status);
      if(status==0)
        {
	  /*
          CWarnF        ("HoXzGetIdentifier : ident not a number in :\n%s\n",a_string);
	  */
          CStringDelete (dname); 
          return        0;
        }
      CStringDelete (dname);
      dname         = NULL;
    }
  else
    {
      *pos = 0;
      id   = (int)CStringConvertToLong(pos+1,&status);
      if(status==0)
        {
	  /*
          CWarnF        ("HoXzGetIdentifier : ident not a number in :\n%s\n",a_string);
	  */
          CStringDelete (dname); 
          return        0;
        }
    }

  if(dname==NULL) dname = CStringDuplicate ("//PAWC");

  if(strncmp(dname,"//",2)!=0)      
    {
      CWarnF        ("HoXzGetIdentifier : must start by // in :\n%s\n",a_string);
      CStringDelete (dname); 
      return        0;
    }

  file    = HFileGetIdentifier(dname);
  if(file==NULL)
    {
      CWarnF        ("HoXzGetIdentifier : file %s not open.\n",dname);
      CStringDelete (dname); 
      return        0;
    }
       
  HFileBringDirectoryInMemory      (file);
  CStringDelete    (dname);

  if(hexist(&id)==0) return 0;

  return           id;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
void HoXzFitWithMINUIT (
 void* a_fit
,int a_type 
,int   a_npt
,double* a_x    
,double* a_val  
,double* a_err  
,int   a_npar
,double* a_pars 
,double* a_stps 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int        type,npt,npar;
/*.........................................................................*/
  if(a_npt <=0) return;
  if(a_npar<=0) return;
  Class.fit  = (OFit)a_fit;
  type       = a_type;  /* do this for VMCMS */
  npt        = a_npt;
  npar       = a_npar;
  omnfit     (&type,&npt,a_x,a_val,a_err,&npar,a_pars,a_stps);
}
/***************************************************************************/
void oxfval (
 double* a_xx
,int* a_npar
,double* a_pars
,double* a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  *a_value = OFitEvaluateScript  (Class.fit,*a_xx,*a_npar,a_pars);
}

#else /*HAS_XZ*/
#include <stddef.h>
#endif /*HAS_XZ*/
