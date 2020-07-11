/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */

#ifdef HAS_XAW

#include <stdio.h>

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>

#include <Xaw/Form.h>
#include <Xaw/List.h>
#include <Xaw/Dialog.h>

/*Co*/
#include <CString.h>
#include <CPrinter.h>

/*Xx*/
#include <XDisplay.h>
#include <XWidget.h>

#include <XawTools.h>

#ifdef __cplusplus
extern "C"{
#endif
static char* GetResource                     (Widget,char*,char*,XtArgVal*);
static void  ConvertXtREdgeTypeXtRString     (XrmValuePtr,Cardinal*,XrmValuePtr,XrmValuePtr);
#ifdef __cplusplus
}
#endif

#define XtREdgeType "EdgeType"

#define AXARGVAL(a_type)    (*((a_type *)((a_from)->addr)))

#define MAX_STRING 128
static struct 
{
  char string[MAX_STRING];
} Class;
/***************************************************************************/
void XawInitializeClass (
 Display* a_display
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  XtAppContext context;
/*.........................................................................*/
  if(a_display==NULL) return;

  XDisplayPutFileInResourceDatabase (a_display,"Xaw.xrm");

  context = XtDisplayToApplicationContext(a_display);

  XtAppAddConverter (context,XtREdgeType             ,XtRString,ConvertXtREdgeTypeXtRString,NULL,0);

}
/***************************************************************************/
char* XawConvertResourceValueToString (
 Widget    a_widget 
,char*     a_name 
,char*     a_type 
,XtArgVal* a_value  
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* xtstring;
  char* string;
/*.........................................................................*/
  xtstring      = GetResource (a_widget,a_name,a_type,a_value);
  if(xtstring==NULL) return NULL;
  string        = CStringDuplicate(xtstring);
  XtFree        (xtstring);
  return        string;
}
/***************************************************************************/
static char* GetResource (
 Widget    a_widget 
,char*     a_name 
,char*     a_type 
,XtArgVal* a_value  
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_widget==NULL) return NULL;
  if(a_name==NULL)   return NULL;
  if(a_type==NULL)   return NULL;
  if(a_value==NULL)  return NULL;

  if( (strcmp(a_type,XtRString)==0) &&
      (strcmp(a_name,XtNvalue)==0)  &&
      (XtIsSubclass(a_widget,dialogWidgetClass))
     )    /* Dialog widget. Resource name clash with XmText/XmNvalue.*/
    {
      return  (char*)XtNewString(*((char**)a_value));
    }

  return NULL;
}
/***************************************************************************/
char* XawGetCallbackData (
 Widget This  
,void* a_data 
,char* a_what
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* cname;
  char  string[64];
/*.........................................................................*/
  if(This==NULL) return NULL;
  cname     = XWidgetGetClassName(This);
       if(strcmp(cname,"List")==0)
    { 
      if(a_data==NULL)           return NULL;
      if( (a_what!=NULL) && (strcmp(a_what,"index")==0) )  
	{
	  sprintf     (string,"%d",((XawListReturnStruct*)a_data)->list_index);
	  return      (char*)XtNewString(string);
	}
      else /*assume string*/         	         
	{
	  return (char*)XtNewString(((XawListReturnStruct*)a_data)->string);
	}
    }
  else if(strcmp(cname,"Scrollbar")==0)
    { 
           if( (a_what!=NULL) && (strcmp(a_what,"jump")==0) )  
	     {
	       if(a_data==NULL) return NULL;
	       sprintf     (string,"%g",*((float*)a_data));
	     }
      else if( (a_what!=NULL) && (strcmp(a_what,"thumb")==0) ) 
	{
          sprintf (string,"%ld",(long)a_data);  /*float -> long ???*/
	}
      else /*Scroll by button2.*/ 
	{
	  sprintf (string,"%d",(int)a_data);
	}
      return                (char*)XtNewString(string);
    }
  else if(strcmp(cname,"StripChart")==0)
    { 
      if(a_data==NULL) return NULL;
      if(a_what!=NULL)
	{
	  double      value;
	  int         status;
	  value       = CStringConvertToDouble(a_what,&status); 
	  if(status==0) 
	    {
	      CWarnF ("%s: %s not a number.\n",a_what);
	      return NULL;
	    }
	  *((double*)a_data) = value;
	  return      NULL;
	}
    }
  else if(strcmp(cname,"Toggle")==0)
    { 
      sprintf               (string,"%d",(int)a_data);
      return                (char*)XtNewString(string);
    }
  return NULL;
}
/***************************************************************************/
static void ConvertXtREdgeTypeXtRString (
 XrmValuePtr a_args 
,Cardinal* a_argn
,XrmValuePtr a_from
,XrmValuePtr a_to
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  unsigned char from;
/*.........................................................................*/
  from  = AXARGVAL(unsigned char);
  Class.string[0] = '\0';
       if(from==XawChainTop)    sprintf    (Class.string,"chaintop");
  else if(from==XawChainBottom) sprintf    (Class.string,"chainbottom");
  else if(from==XawChainLeft)   sprintf    (Class.string,"chainleft");
  else if(from==XawChainRight)  sprintf    (Class.string,"chainright");
  else if(from==XawRubber)      sprintf    (Class.string,"rubber");
  a_to->size = (unsigned int)(strlen(Class.string)+1);  
  a_to->addr = (XPointer)Class.string;      
  a_argn = 0;
  a_args  = NULL; 
}

#else  /*HAS_XAW*/

#include <stdlib.h>
#include <CPrinter.h>
#include <XawTools.h>
/***************************************************************************/
void XawInitializeClass (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CWarn ("Wo/XawTools.c/XawInitializeClass : dummy.\n");
}
/***************************************************************************/
char* XawConvertResourceValueToString (
 Widget    a_widget 
,char*     a_name 
,char*     a_type 
,XtArgVal* a_value  
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CWarn ("Wo/XawTools.c/XawConvertResourceValueToString : dummy.\n");
  This    = NULL;
  a_name  = NULL;
  a_type  = NULL;
  a_value = NULL;
  return  NULL;
}
/***************************************************************************/
char* XawGetCallbackData (
 Widget This  
,void* a_data 
,char* a_what
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CWarn ("Wo/XawTools.c/XawGetCallbackData : dummy.\n");
  This   = NULL;
  a_data = NULL;
  a_what = NULL;
  return NULL;
}

#endif /*HAS_XAW*/
