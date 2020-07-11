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
#include <stdarg.h>
#include <stdio.h>

#include <math.h>

#include <CPrinter.h>
#include <CMemory.h>
#include <CText.h>
#include <CString.h>
#include <CList.h>
#include <FString.h>

#include <HF77.h>

#include <HBook.h>

typedef unsigned long Ulong;
typedef char*         OString;
#define NotFound      (-1)
#define MAX_IDS       1024
#define MAXIMUM(a,b)  ((a)>(b)?a:b)

static struct {
  int                   init;
  int                   hlimit;
  CList                 extent;
  HBookUserFunction     userFunction;
  HBookConvertFunction* cvts;
  int*                  idents;
} Class = {0,0,NULL,NULL,NULL,NULL};
/***************************************************************************/
void HBookClearClass (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CListDelete        (Class.extent);
  Class.extent       = NULL;
  Class.userFunction = NULL;
  CListDelete        ((CList)Class.cvts);
  Class.cvts         = NULL;
  CMemoryFreeBlock   (Class.idents);
  Class.idents       = NULL;
/*Class.init         = 0;*/ /*Not sure that we can do HLIMIT twice.*/
}
/***************************************************************************/
void HBookInitialize (
 int a_size
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.init==1) return;
  Class.hlimit = a_size;
  ohinit       (&Class.hlimit); /*Class.hlimit could be modified by ohinit.*/
  Class.init   = 1;
}
/***************************************************************************/
HBookUserFunction HBookSetUserFunction (
 HBookUserFunction a_routine
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  HBookUserFunction  old;
/*.........................................................................*/
  old                = Class.userFunction;
  Class.userFunction = a_routine;
  return             old;
}
/***************************************************************************/
char** HBookGetSubDirectories (
 int* a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{ 
  int     linen = 0;
  char**  lines = NULL;
  int     id;
/*.........................................................................*/
  if(a_number!=NULL) *a_number = 0;
  id           = 0;
  while(1)
    {
      char*             chtype;
      char*             chtitl;
      HBookGetNextEntry (&id,&chtype,&chtitl," ");
      if(id==0)         break;
      if( (chtype!=NULL) && (chtype[0]=='D') ) 
	{
	  CTextAddLine      (&linen,&lines,chtitl);
	}
      CStringDelete     (chtype);
      CStringDelete     (chtitl);
    }
  if(a_number!=NULL) *a_number = linen;
  return       lines;
}
/***************************************************************************/
char** HBookGetEntries (
 int*    a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{ 
  int     linen = 0;
  char**  lines = NULL;
  int     id;
/*.........................................................................*/
  if(a_number!=NULL) *a_number = 0;
  id           = 0;
  while(1)
    {
      char*             chtype;
      char*             chtitl;
      HBookGetNextEntry (&id,&chtype,&chtitl," ");
      if(id==0) break;
      if( (chtype!=NULL) && 
	  ( (chtype[0]=='1')  ||
	    (chtype[0]=='2')  ||
	    (chtype[0]=='N') )
	) 
	{ 
	  char*          string;
	  string         = CStringCreateF(strlen(chtype)+strlen(chtitl)+10,"%6d (%s) %s",id,chtype,chtitl);
	  CTextAddLine   (&linen,&lines,string);
	  CStringDelete  (string);
	}
      CStringDelete  (chtype);
      CStringDelete  (chtitl);
    }
  if(a_number!=NULL) *a_number = linen;
  return       lines;
}
/***************************************************************************/
void HBookGetNextEntry (
 int*    a_id
,char**  a_type
,char**  a_title
,char*   a_opt
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  FString    chtype ,chtitl ,chopt;
  int        lchtype,lchtitl,lchopt;
  int        ident;
/*.........................................................................*/
  if(a_type!=NULL)  *a_type  = NULL;
  if(a_title!=NULL) *a_title = NULL;
  if(a_id==NULL)    return;

  HBookInitialize   (0);

  lchtype                = 80;
  chtype                 = FStringCreate(lchtype);
  lchtitl                = 80;
  chtitl                 = FStringCreate(lchtitl);

  chopt                  = FStringCreateFromC(a_opt,&lchopt);
  ident                  = *a_id;
  ohlnxt                 (&ident,chtype,&lchtype,chtitl,&lchtitl,chopt,&lchopt);
  *a_id                  = ident;
  if(a_type!=NULL)       *a_type  = ident==0 ? CStringNULL : FStringConvertToC(chtype,lchtype);
  if(a_title!=NULL)      *a_title = ident==0 ? CStringNULL : FStringConvertToC(chtitl,lchtitl);

  FStringDelete          (chtype);
  FStringDelete          (chtitl);
  FStringDelete          (chopt);
}
/***************************************************************************/
char* HBookGetDirectoryName (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int                    ldname;
  FString                fdname;
  char*                  cdname;
/*.........................................................................*/
  HBookInitialize        (0);
  ldname                 = 80;
  fdname                 = FStringCreate (ldname);

  ohpwd                  (fdname,&ldname);

  cdname                 = FStringConvertToC (fdname,ldname);
  FStringDelete          (fdname);

  return                 cdname;
}
/***************************************************************************/
int HBookSetDirectory (
 char* a_name
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int                    ldname;
  FString                fdname;
  int                    status;
/*.........................................................................*/
  if(a_name==NULL)       return 0;
  HBookInitialize        (0);
  if(strcmp(a_name,"..")==0)
    fdname                 = FStringCreateFromC ("\\",&ldname);
  else
    fdname                 = FStringCreateFromC (a_name,&ldname);
  status                 = ohcd (fdname,&ldname);
  FStringDelete          (fdname);
  return                 status;
}
/***************************************************************************/
void HBookBringDirectoryInMemory (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int     id     = 0;
  int     icycle = 9999;
  int     iofset = 0;
/*.........................................................................*/
  HBookInitialize (0);
  hdelet    (&id);
  hrin      (&id,&icycle,&iofset);
}
/***************************************************************************/
void HBookAddConverter (
 HBookConvertFunction a_proc
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CListAddEntryFunction ((CList*)&Class.cvts,(CEntryFunction)a_proc);
}
/***************************************************************************/
int HBookConvert (
 char* a_string
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  HBookConvertFunction* itema;
  int                   obj;
/*.........................................................................*/
  for(itema=Class.cvts;(itema!=NULL) && (*itema!=NULL);itema++)
    {
       obj = (*itema)(a_string);
       if(obj!=NotFound) return obj;      
    }
  return NotFound;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
void HBookGetHistogramInformations (
 int a_id
,char** a_title
,int* a_nx
,double* a_xmin
,double* a_xmax
,int* a_ny
,double* a_ymin
,double* a_ymax
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int        lstr;
  FString       fstr;
  int        id;

  int        nx,ny;
  float      xmi,xma,ymi,yma;
/*.........................................................................*/
  HBookInitialize     (0);

  lstr          = 80;
  fstr          = FStringCreate(lstr);

  id            = a_id;

  ohgivh        (&id,fstr,&lstr,&nx,&xmi,&xma,&ny,&ymi,&yma);

  if(a_title!=NULL)   *a_title = FStringConvertToC (fstr,lstr);
  if(a_nx!=NULL)      *a_nx    = nx;
  if(a_xmin!=NULL)    *a_xmin  = (double)xmi;
  if(a_xmax!=NULL)    *a_xmax  = (double)xma;
  if(a_ny!=NULL)      *a_ny    = ny;
  if(a_ymin!=NULL)    *a_ymin  = (double)ymi;
  if(a_ymax!=NULL)    *a_ymax  = (double)yma;

  FStringDelete             (fstr);
}
/***************************************************************************/
void HBookGetTupleInformations (
 int a_id
,char** a_title
,int* a_coln
,int* a_rown
,char*** a_tags
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int        lstr;
  FString       fstr;
  int        ltag;
  FString       ftag;

  int        id;
  int        ncol,nrow,count;
  char*      ctag;
  char*      p;
#define LTAG 8
/*.........................................................................*/
  HBookInitialize              (0);

  lstr                   = 80;
  fstr                   = FStringCreate(lstr);

  ltag                   = 500 * LTAG;
  ftag                   = FStringCreate(ltag);

  id                     = a_id;

  ohgivn                 (&id,fstr,&lstr,&ncol,&nrow,ftag,&ltag);

  if(a_title!=NULL)      *a_title = FStringConvertToC (fstr,lstr);
  if(a_coln!=NULL)       *a_coln  = ncol;
  if(a_rown!=NULL)       *a_rown  = nrow;

  ctag                   = FStringConvertToC (ftag,ltag);
  FStringDelete          (ftag);

  if(a_tags!=NULL) 
    {
      *a_tags = ncol!=0 ? (char**)CMemoryAllocateBlock(ncol*sizeof(char*)) : (char**)NULL;
      if( (ncol!=0) && (*a_tags!=NULL) )  
	{
	  p = ctag;
	  for(count=0;count<ncol;count++)
	    {
	      (*a_tags)[count] = CStringCreate(LTAG);
	      strncpy          ((*a_tags)[count],p,LTAG);
	      p            += LTAG;
	    }
	}
    }

  CStringDelete          (ctag);
  FStringDelete             (fstr);
}
/***************************************************************************/
void HBookCreateTuple (
 int a_id
,char* a_title
,int a_coln
,int a_rown
,char** a_tags
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int        lstr;
  FString    fstr;
  char*      ctags;
  int        id,coln,rown,count;
  char*      pos;
  int        ltag;
  FString    ftag;
#define LTAG 8
/*.........................................................................*/
  if(a_title==NULL) return;
  if(a_coln<=0)     return;
  if(a_tags==NULL)  return;

  HBookInitialize        (0);

  fstr                   = FStringCreateFromC (a_title,&lstr);

  id                     = a_id;
  coln                   = a_coln;
  rown                   = a_rown;

  pos = ctags = CStringCreate ( coln * LTAG);
  for(count=0;count<coln;count++)
    {
      strncpy (pos,a_tags[count],strlen(a_tags[count]));
      pos    += LTAG;
    }

  ftag                   = FStringCreateFromC (ctags,&ltag);

  ohbkn                  (&id,fstr,&lstr,&coln,&rown,ftag,&ltag);

  CStringDelete          (ctags);
  FStringDelete          (ftag);
  FStringDelete          (fstr);
}
/***************************************************************************/
void HBookCreateOneDimensionalHistogram (
 int    a_id
,char*  a_title
,int    a_nx
,double a_xmin
,double a_xmax
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int        lstr;
  FString    fstr;
  int        id;
  int        nx;
  float      xmi,xma,vmx;
/*.........................................................................*/
  HBookInitialize        (0);
  
  fstr                   = FStringCreateFromC (a_title,&lstr);

  id                     = a_id;
  nx                     = a_nx;
  xmi                    = (float)a_xmin;
  xma                    = (float)a_xmax;
  vmx                    = 0.;

  ohbk1                  (&id,fstr,&lstr,&nx,&xmi,&xma,&vmx);
  hbarx                  (&id);

  FStringDelete          (fstr);
}
/***************************************************************************/
void HBookCreateTwoDimensionalHistogram (
 int    a_id
,char*  a_title
,int    a_nx
,double a_xmin
,double a_xmax
,int    a_ny
,double a_ymin
,double a_ymax
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int        lstr;
  FString    fstr;
  int        id;
  int        nx,ny;
  float      xmi,xma,ymi,yma,vmx;
/*.........................................................................*/
  HBookInitialize        (0);
  
  fstr                   = FStringCreateFromC (a_title,&lstr);

  id                     = a_id;
  nx                     = a_nx;
  xmi                    = (float)a_xmin;
  xma                    = (float)a_xmax;
  ny                     = a_ny;
  ymi                    = (float)a_ymin;
  yma                    = (float)a_ymax;
  vmx                    = 0.;

  ohbk2                  (&id,fstr,&lstr,&nx,&xmi,&xma,&ny,&ymi,&yma,&vmx);

  FStringDelete          (fstr);
}
/***************************************************************************/
void HBookCreateProfileHistogram (
 int    a_id
,char*  a_title
,int    a_nx
,double a_xmin
,double a_xmax
,double a_ymin
,double a_ymax
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int        lstr;
  FString    fstr;
  int        id;
  int        nx;
  float      xmi,xma,ymi,yma;
  FString    chopt;
  int        lchopt;
/*.........................................................................*/
  HBookInitialize        (0);
  
  fstr                   = FStringCreateFromC (a_title,&lstr);

  id                     = a_id;
  nx                     = a_nx;
  xmi                    = (float)a_xmin;
  xma                    = (float)a_xmax;
  ymi                    = (float)a_ymin;
  yma                    = (float)a_ymax;
  chopt                  = FStringCreateFromC(" ",&lchopt);

  ohbkp                  (&id,fstr,&lstr,&nx,&xmi,&xma,&ymi,&yma,chopt,&lchopt);

  FStringDelete          (chopt);
  FStringDelete          (fstr);
}
/***************************************************************************/
int HBookBuildIdentifier (
 int a_start
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int count;
  int objn,idmax;
/*.........................................................................*/
  idmax                  = a_start<0 ? 0 : a_start;
  HBookInitialize        (0);
  if(Class.idents==NULL) Class.idents = (int*)CMemoryAllocateBlock( MAX_IDS * sizeof(int));
  if(Class.idents==NULL) return idmax + 1;
  hidall                 (Class.idents,&objn);  
  if(objn<=0)            return idmax + 1;
  for(count=0;count<objn;count++) idmax = MAXIMUM (idmax,Class.idents[count]);
  return                 idmax + 1;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
int* HBookGetIdentifiers (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int       objn;
   int count;
/*.........................................................................*/
  HBookInitialize (0);
  if(Class.idents==NULL) Class.idents = (int*)CMemoryAllocateBlock( MAX_IDS * sizeof(int));
  if(Class.idents==NULL) return NULL;
  hidall                 (Class.idents,&objn);  
  if(objn<=0)            return NULL;
  CListDelete            (Class.extent);
  Class.extent           = CListCreate(objn);
  if(Class.extent==NULL) return NULL;
  for(count=0;count<objn;count++) Class.extent[count] = (void*)Class.idents[count];
  return                 (int*)Class.extent;
}
/***************************************************************************/
void HBookDelete (
 int This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int     ident;
/*.........................................................................*/
  HBookInitialize (0);
  if(This==0)     return;
  ident           = This;
  hdelet         (&ident);
}
/***************************************************************************/
int HBookGetDimension (
 int    This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*  title;
  int    xn;
  double xmin,xmax;
  int    yn;
  double ymin,ymax;
/*.........................................................................*/
  HBookGetHistogramInformations (This,&title,&xn,&xmin,&xmax,&yn,&ymin,&ymax);
  CStringDelete                 (title);
  return                        (yn!=0 ? 2 : 1);
}
/***************************************************************************/
void HBookSetTitle (
 char* a_opt
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int           lopt;
  FString       fopt;
/*.........................................................................*/
  HBookInitialize (0);
  fopt            = FStringCreateFromC(a_opt,&lopt);
  ohtit           (fopt,&lopt);
  FStringDelete   (fopt);
}
/***************************************************************************/
void HBookSetOption (
 int a_id
,char* a_opt
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int           id;
  int           lopt;
  FString       fopt;
/*.........................................................................*/
  HBookInitialize (0);
  id            = a_id;
  fopt          = FStringCreateFromC(a_opt,&lopt);
  ohidop        (&id,fopt,&lopt);
  FStringDelete (fopt);
}
/***************************************************************************/
int HBookSetAttribute (
 int    This
,char*  a_name
,void*  a_user
,void*  a_addr 
,int    a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_name==NULL)  return 0;
  if(a_addr==NULL)  return 0; 

  if(This==0)
    {
       if(strcmp(a_name,"directory")==0) 
	 {
	   HBookSetDirectory (*((OString*)a_addr));
	 }
       else if(strcmp(a_name,"list")==0)
	 {
	 }
       else if(strcmp(a_name,"directories")==0)
	 {
	 }
       else if(strcmp(a_name,"objects")==0)
	 {
	 }
       else 
	 {
	   CInfoF ("HBookSetAttribute: %s is not a class property.\n",a_name);
	   return 0;
	 }
       return 1;
    }

       if(strcmp(a_name,"identifier")==0)        {}
  else if(strcmp(a_name,"name")==0)              {}
  else if(strcmp(a_name,"dimension")==0)         {}
  else if(strcmp(a_name,"title")==0)             {}
  else if(strcmp(a_name,"xAxisNumberOfBins")==0) {}
  else if(strcmp(a_name,"xAxisMinimum")==0)      {}
  else if(strcmp(a_name,"xAxisMaximum")==0)      {}
  else if(strcmp(a_name,"yAxisNumberOfBins")==0) {}
  else if(strcmp(a_name,"yAxisMinimum")==0)      {}
  else if(strcmp(a_name,"yAxisMaximum")==0)      {}
  else if(strcmp(a_name,"bins")==0)              {}
  else 
    {
      CInfoF ("HBookSetAttribute: unknown property %s.\n",a_name);
      return 0;
    }

  a_user   = NULL;
  a_number = 0;
  return   1;
}
/***************************************************************************/
int HBookGetAttribute (
 int    This
,char*  a_name
,void*  a_user
,void*  a_addr 
,int*   a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  static char name[32];
  int    identifier;
  int    xn;
  double xmin,xmax;
  int    yn;
  double ymin,ymax;
  char*  title;
/*.........................................................................*/
  if(a_number!=NULL) *a_number = 0;
  if(a_name==NULL)  return 0;
  if(a_addr==NULL)  return 0;  

  if(This==0)
    {
       if(strcmp(a_name,"directory")==0) 
	 {
	   *((char**)a_addr) = HBookGetDirectoryName();
	   return            FREE_BLOCK;
	 }
       else if(strcmp(a_name,"list")==0)
	 {
	   int                linen;
	   char**             lines;
	   int                stringn = 0;
	   char**             strings = NULL;
	   strings            = HBookGetSubDirectories (&stringn);
	   lines              = HBookGetEntries (&linen);
	   CTextConcatenate   (&stringn,&strings,linen,lines);
	   CTextDelete        (linen,lines);
	   if(a_number!=NULL) *a_number = stringn;
	   *((char***)a_addr) = strings;
	   return             FREE_BLOCKS;
	 }
       else if(strcmp(a_name,"directories")==0)
	 {
	   int                stringn = 0;
	   char**             strings = NULL;
	   strings            = HBookGetSubDirectories (&stringn);
	   if(a_number!=NULL) *a_number = stringn;
	   *((char***)a_addr) = strings;
	   return             FREE_BLOCKS;
	 }
       else if(strcmp(a_name,"objects")==0)
	 {
	   int                stringn = 0;
	   char**             strings = NULL;
	   strings            = HBookGetEntries (&stringn);
	   if(a_number!=NULL) *a_number = stringn;
	   *((char***)a_addr) = strings;
	   return             FREE_BLOCKS;
	 }
       else 
	 {
	   CInfoF("HBookGetAttribute: %s is not a class property.\n",a_name);
	   return 0;
	 }
     }

  HBookGetHistogramInformations (This,&title,&xn,&xmin,&xmax,&yn,&ymin,&ymax);
  identifier = This; 

  sprintf        (name,"%d",This);

       if(strcmp(a_name,"identifier")==0)        *((int*)a_addr)    = This;
  else if(strcmp(a_name,"name")==0)              *((char**)a_addr)  = name;
  else if(strcmp(a_name,"dimension")==0)         *((int*)a_addr)    = yn==0 ? 1 : 2;
  else if(strcmp(a_name,"title")==0) 
    {
      *((char**)a_addr)  = title;
      return FREE_BLOCK;
    }
  else if(strcmp(a_name,"xAxisNumberOfBins")==0) *((int*)a_addr)    = xn;
  else if(strcmp(a_name,"xAxisMinimum")==0)      *((double*)a_addr) = xmin;
  else if(strcmp(a_name,"xAxisMaximum")==0)      *((double*)a_addr) = xmax;
  else if(strcmp(a_name,"yAxisNumberOfBins")==0) *((int*)a_addr)    = yn;
  else if(strcmp(a_name,"yAxisMinimum")==0)      *((double*)a_addr) = ymin;
  else if(strcmp(a_name,"yAxisMaximum")==0)      *((double*)a_addr) = ymax;
  else if(strcmp(a_name,"bins")==0) 
    {
      int              dimension;
      int              binNumber;
      int              number = 0;
      double*          doubles= NULL;
      dimension        = yn==0 ? 1 : 2;
      binNumber        = dimension==1 ? xn : xn * yn;
      if(binNumber!=0)
        {
	  doubles = (double*)CMemoryAllocateBlock(binNumber * sizeof(double));
	  if(doubles!=NULL)
            {
	        int count;
	      number    = binNumber;
	      if(dimension==1)
		{
		  int   ix;
		  count = 0;
		  for(ix=1;ix<=xn;ix++)
		    { 
		      float          val;
		      val            = hi  ( &identifier, &ix);
		      doubles[count] = (double)val;
		      count++;
		    }
		}
	      else if(dimension==2)
		{
		  int   ix,iy;
		  count = 0;
		  for(iy=1;iy<=yn;iy++)
		    for(ix=1;ix<=xn;ix++)
		      { 
			float                val;
			val                  = hij ( &identifier, &ix, &iy);
			doubles[count] = (double)val;
			count++;
		      }
		}
	    }
	}
      if(a_number!=NULL) *a_number = number;
      *((double**)a_addr) = doubles;
      CStringDelete  (title);
      return FREE_BLOCK;
    }
  else 
    {
      CInfoF("HBookGetAttribute: unknown property %s.\n",a_name);
      CStringDelete  (title);
      return 0;
    }
  CStringDelete  (title);
  a_user = NULL;
  return 1;
}
/***************************************************************************/
int HBookDoMethod (
 int    This
,char*  a_name
,void*  a_user
,int    a_argn
,char** a_args
,void*  a_addr 
,int*   a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int     identifier;
/*.........................................................................*/
  if(a_number!=NULL) *a_number = 0;
  if(a_name==NULL)  return 0;

  if(This==0)
    {
       if(strcmp(a_name,"bringDirectoryInMemory")==0) 
	 HBookBringDirectoryInMemory();
       else 
	 {
	   CInfoF("HBookDoMethod: %s is not a class routine property.\n",a_name);
	   return 0;
	 }
       return 1;
     }

  identifier = This;
  if(strcmp(a_name,"fill")==0)
    {
      int     dimension;
      int     hasWeight;
      double* values;
      float   xval   = 0.;
      float   yval   = 0.;
      float   weight = 1.;
      int     count;
      dimension = HBookGetDimension (This);
      if(dimension<=0) return 1;
      if( (a_argn!=dimension) && (a_argn!=(dimension+1)) ) 
	{
	  CWarnF  ("%d or %d arguments expected for %s property.\n",dimension,dimension+1,a_name);
	  return  0;
	}
      hasWeight    = a_argn==dimension ? 0 : 1;
      values       = (double*)CMemoryAllocateBlock( dimension * sizeof(double));
      if(values==NULL) return 0;
      for(count=0;count<dimension;count++) 
	{ 
	  int           status;
	  values[count] = CStringConvertToDouble(a_args[count],&status);
	  if(status==0) 
	    {
	      CWarnF           ("Argument %s not a number.\n",a_args[count]);
	      CMemoryFreeBlock (values);
	      return           0;
	    }
	}
      if(hasWeight==1)
	{ 
	  int    status;
	  weight = (float)CStringConvertToDouble(a_args[dimension],&status);
	  if(status==0) 
	    {
	      CWarnF           ("Argument %s not a number.\n",a_args[dimension]);
	      CMemoryFreeBlock (values);
	      return           0;
	    }
	}
      if(dimension==1) 
	{
	  xval  = values[0];
	  yval  = 0.;
	}
      else if(dimension==2) 
	{
	  xval  = values[0];
	  yval  = values[1];
	}
      hfill            (&identifier,&xval,&yval,&weight);
      CMemoryFreeBlock (values);
    }
  else if(strcmp(a_name,"clear")==0)
    {
      HBookInitialize (0);
      ohraz           (&identifier);
    }

  a_user = NULL;
  a_argn = 0;
  a_args = NULL;
  a_addr = NULL;
  return 0;
}
/***************************************************************************/
void* HBookConstruct (
 void*  a_type
,int    a_parn
,char** a_pars
,char** a_vals
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  int    identifier = 0;
  char*  name       = NULL;
  char*  title      = NULL;
  int    dimension  = 0;
  int    xn         = 0;
  double xmin       = 0.;
  double xmax       = 0.;
  int    yn         = 0;
  double ymin       = 0.;
  double ymax       = 0.;
  int    profile    = 0;
/*.........................................................................*/
  if(a_parn==0) return NULL;
  for(count=0;count<a_parn;count++)
    {
      char*  string;
      string = a_pars[count];
            if( (string!=NULL) && (strcmp(string,"identifier")==0) )  
	 {
	   int    status;
	   int    value;
	   value  = (int)CStringConvertToLong(a_vals[count],&status);
	   if(status==0) 
	     {
	       CWarnF ("Can't convert %s to a number.\n",a_vals[count]);
	       return NULL;
	     }
	   identifier = value;          
	 }
      else if( (string!=NULL) && (strcmp(string,"name")==0) )  
	 {
	   name  = a_vals[count];
	 }
      else if( (string!=NULL) && (strcmp(string,"title")==0) )  
	{
	  title = a_vals[count];
	}
      else if( (string!=NULL) && (strcmp(string,"dimension")==0) )
	{
	  int    status;
	  int    value;
	  value  = (int)CStringConvertToLong(a_vals[count],&status);
	  if(status==0) 
	    {
	      CWarnF ("Can't convert %s to a number.\n",a_vals[count]);
	      return NULL;
	    }
          dimension = value;          
	}
      else if( (string!=NULL) && (strcmp(string,"xAxisNumberOfBins")==0) )
	{
	  int    status;
	  int    value;
	  value  = (int)CStringConvertToLong(a_vals[count],&status);
	  if(status==0) 
	    {
	      CWarnF ("Can't convert %s to a number.\n",a_vals[count]);
	      return NULL;
	    }
          xn = value;          
	}
      else if( (string!=NULL) && (strcmp(string,"xAxisMinimum")==0) )
	{
	  int    status;
	  double value;
	  value  = CStringConvertToDouble(a_vals[count],&status);
	  if(status==0) 
	    {
	      CWarnF ("Can't convert %s to a number.\n",a_vals[count]);
	      return NULL;
	    }
          xmin = value;          
	}
      else if( (string!=NULL) && (strcmp(string,"xAxisMaximum")==0) )
	{
	  int    status;
	  double value;
	  value  = CStringConvertToDouble(a_vals[count],&status);
	  if(status==0) 
	    {
	      CWarnF ("Can't convert %s to a number.\n",a_vals[count]);
	      return NULL;
	    }
          xmax = value;          
	}
      else if( (string!=NULL) && (strcmp(string,"yAxisNumberOfBins")==0) )
	{
	  int    status;
	  int    value;
	  value  = (int)CStringConvertToLong(a_vals[count],&status);
	  if(status==0) 
	    {
	      CWarnF ("Can't convert %s to a number.\n",a_vals[count]);
	      return NULL;
	    }
          yn = value;          
	}
      else if( (string!=NULL) && (strcmp(string,"yAxisMinimum")==0) )
	{
	  int    status;
	  double value;
	  value  = CStringConvertToDouble(a_vals[count],&status);
	  if(status==0) 
	    {
	      CWarnF ("Can't convert %s to a number.\n",a_vals[count]);
	      return NULL;
	    }
          ymin = value;          
	}
      else if( (string!=NULL) && (strcmp(string,"yAxisMaximum")==0) )
	{
	  int    status;
	  double value;
	  value  = CStringConvertToDouble(a_vals[count],&status);
	  if(status==0) 
	    {
	      CWarnF ("Can't convert %s to a number.\n",a_vals[count]);
	      return NULL;
	    }
	  ymax = value;          
	}
      else if( (string!=NULL) && (strcmp(string,"profile")==0) )
	{
	  int    status;
	  int    value;
	  value  = (int)CStringConvertToBoolean(a_vals[count],&status);
	  if(status==0) 
	    {
	      CWarnF ("Can't convert %s to a boolean.\n",a_vals[count]);
	      return NULL;
	    }
          profile = value;          
	}
      else if( (string!=NULL) && (strcmp(string,"dataBaseName")==0) ) /*Not used today.*/
	{
	}
      else if( (string!=NULL) && (strcmp(string,"dataBaseFile")==0) ) /*Not used today.*/
	{
	}
      else
        { 
	  CWarnF ("HBookConstruct: %s is not a HBook property.\n",string);
          return NULL;
	}
    }
  
  HBookInitialize   (0);

  if(identifier==0) /*Find one.*/
    {
      identifier = 1;
      while(1)
	{
	  if(hexist(&identifier)==0) break;
	  identifier++;
	}
    }
  else
    {
      if(hexist(&identifier)!=0)
	{
	  CWarnF ("HBookConstruct: histogram %d already exists.\n",identifier);
          return NULL;
	}
    }

  if(dimension==1)
    {
      if(xmin>=xmax)
	{
	  CWarnF ("HBookConstruct: can't create %d : xmin >= xmax.\n",identifier);
          return NULL;
	}
      if(profile==0)
	{
	  HBookCreateOneDimensionalHistogram (identifier,title,xn,xmin,xmax);
	}
      else
	{
	  if(ymin>=ymax)
	    {
	      CWarnF ("HBookConstruct: can't create %d : ymin >= ymax.\n",identifier);
	      return NULL;
	    }
	  HBookCreateProfileHistogram (identifier,title,xn,xmin,xmax,ymin,ymax);
	}
    }
  else if(dimension==2)
    {
      if(xmin>=xmax)
	{
	  CWarnF ("HBookConstruct: can't create %d : xmin >= xmax.\n",identifier);
          return NULL;
	}
      if(ymin>=ymax)
	{
	  CWarnF ("HBookConstruct: can't create %d : xmin >= xmax.\n",identifier);
          return NULL;
	}
      HBookCreateTwoDimensionalHistogram (identifier,title,xn,xmin,xmax,yn,ymin,ymax);
    }
  else
    {
      CWarnF ("HBookConstruct: can't create a %d dimensionnal histogram.\n",dimension);
      return NULL;
    }

  if(Class.userFunction!=NULL) Class.userFunction(identifier,name);

  a_type = NULL;
  return (void*)identifier;
}
