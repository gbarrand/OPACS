/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
/*
#define DEBUG 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <CPrinter.h>
#include <CString.h>
#include <CText.h>
#include <CFile.h>

#include <XWidget.h>

#include <OWidget.h>
#include <OInterface.h>

#ifdef __cplusplus
extern "C"{
#endif
#include <native.h>
#ifdef __cplusplus
}
#endif

#include <JAVA.h>
/***************************************************************************/
void JAVA_ExecuteScript (
 char* a_string
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if( (a_string==NULL) || (*a_string=='\0') ) 
    {
      Widget             widget;
      char*              name;
      char*              cbkName;
      char*              fileName;
      char*              className;
      char*              methodName;
      widget             = XWidgetGetThisWidget();
      if(widget==NULL)   return;
      cbkName            = OWidgetFindCurrentCallbackName (widget);
      if(cbkName==NULL)  return;
      fileName           = CFileGetName(OInterfaceGetFileName((OInterface)OWidgetGetFile(widget)));
      if(fileName==NULL) return;
      name               = XtName(widget);
      className          = fileName;
      methodName         = CStringCreateF(strlen(name)+strlen(cbkName)+1,"%s_%s",name,cbkName);
#ifdef DEBUG
      printf             ("debug : callback : %s %s\n",className,methodName);
#endif
      JAVA_ExecuteMethod (className,methodName);
      CStringDelete      (fileName);
      CStringDelete      (methodName);
    }
  else
    {
      char**      words;
      int         wordn;
      words       = CStringGetWords (a_string," ",&wordn);
      if(wordn<2) 
	{
	  CWarnF      ("Two words (class method) expected in :\n%s\n",a_string);
	  CTextDelete (wordn,words);
	  return;
	}
      JAVA_ExecuteMethod (words[0],words[1]);
      CTextDelete        (wordn,words);
  }
}
/***************************************************************************/
void JAVA_ExecuteMethod (
 char* a_class
,char* a_method
)
/***************************************************************************/
/*
  From : http://www.jovial.com/javaAndCpp/java_and_cpp.html
  Call a JAVA class (or static) method. 
  On Linux :
   javac <file>.java
 does a :
   java -ms8m sun.tools.javac.Main <file>.java
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  ClassClass* javaClass;
/*.........................................................................*/
  if(a_class==NULL)  return;
  if(a_method==NULL) return;
  javaClass = FindClass(NULL,a_class,TRUE);
  if(javaClass==NULL)
    {
      CWarnF ("JAVA class %s not found.\n",a_class);
      return;
    }
  execute_java_static_method (NULL,javaClass,a_method,"()V");
}
