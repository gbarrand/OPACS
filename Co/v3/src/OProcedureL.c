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
#include <stdlib.h>
#include <stdio.h>

#include <CPrinter.h>
#include <CError.h>
#include <CString.h>
#include <CList.h>

#include <OProcedure.h>
#include <OProcedureL.h>

#ifdef __cplusplus
extern "C"{
#endif
static void ReportError (char*,int);
#ifdef __cplusplus
}
#endif

#define NotFound (-1)
/***************************************************************************/
OProcedureList OProcedureListCreate (
 int a_number
,char** a_list
,char* a_token_begin 
,char* a_token_end 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int line;
  int     lbeg,lend,beginloop;
  OProcedureList   list;
/*.........................................................................*/
  if(a_list==NULL)        return NULL;
  if(a_number<=0)         return NULL;
  if(a_token_begin==NULL) return NULL;
  if(a_token_end==NULL)   return NULL;
  lbeg      = strlen(a_token_begin);
  lend      = strlen(a_token_end);
  list      = NULL;
  beginloop = NotFound;
  for(line=0;line<a_number;line++) 
    { char* pline;
      pline = a_list[line];
      if    ( (pline!=NULL) && (strncmp(pline,a_token_begin,lbeg)==0) ) 
        {
          beginloop = line+1;
        } 
      else if( (pline!=NULL) && (strncmp(pline,a_token_end,lend)==0) )
       {
	 if(beginloop==NotFound) 
	   {
	     CWarn ("Bad begin/end balance.\n");
	     /*ReportError ("OProcedureListCreate",CErrorFormat);*/
	   }
	 else
	   { 
	     OProcedure          pcd;
	     char*               word2;
	     word2               = CStringGetSecondWord (a_list[beginloop-1]);
	     
	     pcd                 = (OProcedure)OProcedureCreate (word2);
	     CListAddEntry       ((CList*)&list,pcd);
	     CStringDelete       (word2);
	     
	     OProcedureSetTitlePosition    (pcd,beginloop-1);
	     OProcedureSetBeginPosition    (pcd,beginloop);
	     OProcedureSetEndPosition      (pcd,line - 1);
	     OProcedureSetUserData         (pcd,NULL);
	     beginloop           = -1;
	   }
       }
    }
  if( (beginloop!=NotFound) && (list==NULL)) CWarn ("Bad begin/end balance.\n");
  return list;
}
/***************************************************************************/
void OProcedureListDelete (
 OProcedureList  This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  CListVisit  ((CList)This,(CListVisitEntryFunction)OProcedureDelete);
  CListDelete ((CList)This);
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
  CErrorHandle ("Co","OProcedureList",a_name,a_code);
}


