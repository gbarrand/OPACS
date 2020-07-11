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
#include <CString.h>
#include <CText.h>
#include <OShell.h>

#include <XWidget.h>

#include <HF77.h>
#include <HBook.h>
#include <HFile.h>
#include <XzPlotter.h>
#include <XzTypes.h>
#include <Kuip.h>

#include <XzCommands.h>
/***************************************************************************/
void XzAddCommands (
 OShell a_osh
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(OCommandGetIdentifier("hplot")!=NULL) return; /*done*/

  XzSetTypes();

  OShellAddNewCommand (a_osh,"Xz/hplot"   ,XzExecute_hplot);
  OShellAddNewCommand (a_osh,"Xz/KUIP"    ,XzExecute_KUIP);

}
/***************************************************************************/
int XzExecute_hplot (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> hplot <Widget>  clear
  osh> hplot <Widget>  page_title        <string>
  osh> hplot <Widget>  title             <string>
  osh> hplot <Widget>  [parameter,param] <string> <value>
  osh> hplot <Widget>  [option,opt]      <string>
  osh> hplot <Widget>  [plot,hist]       <ident>
  osh> hplot <Widget>  lego              <ident>
  osh> hplot <Widget>  lego              <ident> <theta> <phi>
  osh> hplot <Widget>  1or2              <ident>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget widget;
/*.........................................................................*/
  if(a_argn<3) 
    {
      CWarnF ("More than 2 arguments expected in :\n%s\n",OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  widget = XWidgetGetIdentifier (a_args[1]);
  if(widget==NULL)
    {
      CWarnF ("Unknown widget %s in :\n%s\n",a_args[1],OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  if( (widget!=NULL) && !XtIsSubclass(widget,xzPlotterWidgetClass) ) 
    {
      CWarnF ("Widget %s is not an XzPlotter in :\n%s\n",a_args[1],OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

       if(a_argn==3)
    {
      XzPlotterClear (widget);
    }
  else
    {
      char*  string;
      int    count;
      string = CStringCreateF (strlen(a_args[2])+1,"%s=",a_args[2]);
      for(count=3;count<a_argn;count++)
	{
	  CStringConcatenate (&string,a_args[count]);
	  if(count!=a_argn-1) CStringConcatenate (&string,",");
	}
      XzPlotterAddCommand (widget,string);
      CStringDelete       (string);
    }

  return        EXIT_SUCCESS;
}
/***************************************************************************/
int XzExecute_KUIP (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> KUIP <string>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_argn!=2) 
    {
      CWarnF ("One argument expected in :\n%s\n",OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }
  KuipExecuteScript (a_args[1]);
  return            EXIT_SUCCESS;
}

