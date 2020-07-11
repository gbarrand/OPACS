/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifdef HAS_XM

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>
#include <limits.h>  /*USHRT_MAX*/

#include <X11/StringDefs.h>

#include <X11/Intrinsic.h>
#include <X11/Shell.h>   

#include <Xm/TextF.h>
#include <Xm/Text.h>
#include <Xm/MessageB.h> 
#include <Xm/PushB.h>      
#include <Xm/RowColumn.h>      
#include <Xm/List.h>      
#include <Xm/Form.h>      
#include <Xm/SelectioB.h>      
#include <Xm/DialogS.h> 
#include <Xm/Frame.h> 
#include <Xm/Label.h> 
#include <Xm/FileSB.h> 

#include <CPrinter.h>
#include <CMemory.h>
#include <CStream.h>
#include <CFile.h>
#include <CString.h>
#include <CText.h>
#include <CList.h>
#include <CPS.h>
#include <CError.h>

#include <XDisplay.h>
#include <XWidget.h>

#include <XmTools.h>

typedef unsigned char        Uchar;
typedef unsigned long        Ulong;
typedef unsigned int         Uint;

typedef struct _OFontListElemRec
{
  char*        name;
  XFontStruct* fontStruct;
  XmFontList   fontList;
} OFontListElemRec;

#ifdef __cplusplus
extern "C"{
#endif
static char*   GetResource                       (Widget,char*,char*,XtArgVal*);
static Boolean ManageToMap                       (Widget);
static void    ClearConsoleCallback              (Widget,XtPointer,XtPointer);
static void    ActivateConsoleCallback           (Widget,XtPointer,XtPointer);
static void    EndConsole                        (Widget);
static Widget  GetParent                         ();
static void    AskCallback                       (Widget,XtPointer,XtPointer);
static int     TreatEvents                       ();
static void    PasswordModifyCallback            (Widget,XtPointer,XtPointer);
static void    ReportError                       (char*,int);
static void    ConvertUnknown                    (XrmValuePtr,Cardinal*,XrmValuePtr,XrmValuePtr);
static void    ConvertXmRCharXtRString           (XrmValuePtr,Cardinal*,XrmValuePtr,XrmValuePtr);
static void    ConvertXmRXmStringXtRString       (XrmValuePtr,Cardinal*,XrmValuePtr,XrmValuePtr);
static void    ConvertXmRAlignmentXtRString      (XrmValuePtr,Cardinal*,XrmValuePtr,XrmValuePtr);
static void    ConvertXmRArrowDirectionXtRString (XrmValuePtr,Cardinal*,XrmValuePtr,XrmValuePtr);
static void    ConvertXtRStringXmRAttachment     (XrmValuePtr,Cardinal*,XrmValuePtr,XrmValuePtr);
static void    ConvertXmRAttachmentXtRString     (XrmValuePtr,Cardinal*,XrmValuePtr,XrmValuePtr);
static void    ConvertXmRDefButtonTypeXtRString  (XrmValuePtr,Cardinal*,XrmValuePtr,XrmValuePtr);
static void    ConvertXmRDialogStyleXtRString    (XrmValuePtr,Cardinal*,XrmValuePtr,XrmValuePtr);
static void    ConvertXmRDialogTypeXtRString     (XrmValuePtr,Cardinal*,XrmValuePtr,XrmValuePtr);
static void    ConvertXmRIndicatorTypeXtRString  (XrmValuePtr,Cardinal*,XrmValuePtr,XrmValuePtr);
static void    ConvertXmRLabelTypeXtRString      (XrmValuePtr,Cardinal*,XrmValuePtr,XrmValuePtr);
static void    ConvertXmRListSizePolicyXtRString (XrmValuePtr,Cardinal*,XrmValuePtr,XrmValuePtr);
static void    ConvertXmRPackingXtRString        (XrmValuePtr,Cardinal*,XrmValuePtr,XrmValuePtr);
static void    ConvertXmRRowColumnTypeXtRString  (XrmValuePtr,Cardinal*,XrmValuePtr,XrmValuePtr);
static void    ConvertXmRProcDirectionXtRString  (XrmValuePtr,Cardinal*,XrmValuePtr,XrmValuePtr);
static void    ConvertXmRResizePolicyXtRString   (XrmValuePtr,Cardinal*,XrmValuePtr,XrmValuePtr);
static void    ConvertXmRSelectPolicyXtRString   (XrmValuePtr,Cardinal*,XrmValuePtr,XrmValuePtr);
static void    ConvertXmRSeparatorTypeXtRString  (XrmValuePtr,Cardinal*,XrmValuePtr,XrmValuePtr);
static void    ConvertXmRShadowTypeXtRString     (XrmValuePtr,Cardinal*,XrmValuePtr,XrmValuePtr);
static void    ConvertXmRStringDirecXtRString    (XrmValuePtr,Cardinal*,XrmValuePtr,XrmValuePtr);
static void    ConvertXmRUnitTypeXtRString       (XrmValuePtr,Cardinal*,XrmValuePtr,XrmValuePtr);
static void    ConvertXmRVisualPolicyXtRString   (XrmValuePtr,Cardinal*,XrmValuePtr,XrmValuePtr);
static void    ConvertXmRSBarDispPolicyXtRString (XrmValuePtr,Cardinal*,XrmValuePtr,XrmValuePtr);
static void    ConvertXmRSBarPlacementXtRString  (XrmValuePtr,Cardinal*,XrmValuePtr,XrmValuePtr);
static void    ConvertXmRScrollPolicyXtRString   (XrmValuePtr,Cardinal*,XrmValuePtr,XrmValuePtr);
static void    ConvertXmRWhichButtonXtRString    (XrmValuePtr,Cardinal*,XrmValuePtr,XrmValuePtr);
static void    ConvertXmROrientationXtRString    (XrmValuePtr,Cardinal*,XrmValuePtr,XrmValuePtr);
static void    ConvertXmREditModeXtRString       (XrmValuePtr,Cardinal*,XrmValuePtr,XrmValuePtr);
#if XmVERSION >= 2
static void    ConvertXmRSetXtRString            (XrmValuePtr,Cardinal*,XrmValuePtr,XrmValuePtr);
#endif
#ifdef __cplusplus
}
#endif

#define NotFound          (-1)
#define AXARGVAL(a_type)  (*((a_type *)((a_from)->addr)))
#define MAXIMUM(a,b)      ((a)>(b)?a:b)

#define MAX_STRING 128
static struct 
{
  int                     fontListn;
  OFontListElemRec*       fontLists;
  XtAppContext            askContext;
  XmGetWidgetFunction     firstShellProc;
  XmProcessEventsFunction mainLoopProc;
  XmVisitWidgetFunction   termEndProc;
  Widget                  xmterm_shell;
  Widget                  xmterm;
  char*                   password;
  XmBalloonShape          ballon_shape;
  Widget                  ballonShell,ballonShape,ballonLabel;
  Widget                  file,file_text,file_label;
  Widget                  text,text_text,text_label;
  Widget                  list,list_list;
  Widget                  passForm,passLabel,passText;
  Widget                  caution,yesno,prompt;
  char                    string[MAX_STRING];
} Class = {
0,NULL,
NULL,
NULL,TreatEvents,EndConsole,
NULL,NULL,
NULL,
XmBalloonShapeRectangular,
NULL,NULL,NULL, /*ballon*/
NULL,NULL,NULL, /*file*/
NULL,NULL,NULL, /*text*/
NULL,NULL,      /*list*/
NULL,NULL,NULL, /*pass*/
NULL,NULL,NULL
};
/***************************************************************************/
void XmInitializeClass (
 Display* a_display
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  XtAppContext context;
/*.........................................................................*/
  if(a_display==NULL) return;

  XDisplayPutFileInResourceDatabase (a_display,"Xm.xrm");

  context = XtDisplayToApplicationContext(a_display);

  XtAppAddConverter (context,XmREditMode               ,XtRString,ConvertXmREditModeXtRString,NULL,0);
  XtAppAddConverter (context,XmROrientation            ,XtRString,ConvertXmROrientationXtRString,NULL,0);
  XtAppAddConverter (context,"XmOrientation"           ,XtRString,ConvertXmROrientationXtRString,NULL,0);

  XtAppAddConverter (context,XtRString                 ,XmRAttachment ,ConvertXtRStringXmRAttachment,NULL,0);
  XtAppAddConverter (context,XmRChar                   ,XtRString,ConvertXmRCharXtRString,NULL,0);
  XtAppAddConverter (context,XmRXmString               ,XtRString,ConvertXmRXmStringXtRString,NULL,0);
  XtAppAddConverter (context,XmRAlignment              ,XtRString,ConvertXmRAlignmentXtRString,NULL,0);
  XtAppAddConverter (context,XmRArrowDirection         ,XtRString,ConvertXmRArrowDirectionXtRString,NULL,0);
  XtAppAddConverter (context,XmRAttachment             ,XtRString,ConvertXmRAttachmentXtRString,NULL,0);
  XtAppAddConverter (context,XmRDefaultButtonType      ,XtRString,ConvertXmRDefButtonTypeXtRString,NULL,0);
  XtAppAddConverter (context,XmRDialogStyle            ,XtRString,ConvertXmRDialogStyleXtRString,NULL,0);
  XtAppAddConverter (context,XmRDialogType             ,XtRString,ConvertXmRDialogTypeXtRString,NULL,0);
  XtAppAddConverter (context,XmRIndicatorType          ,XtRString,ConvertXmRIndicatorTypeXtRString,NULL,0);
  XtAppAddConverter (context,XmRLabelType              ,XtRString,ConvertXmRLabelTypeXtRString,NULL,0);
  XtAppAddConverter (context,XmRListSizePolicy         ,XtRString,ConvertXmRListSizePolicyXtRString,NULL,0);
  XtAppAddConverter (context,XmRPacking                ,XtRString,ConvertXmRPackingXtRString,NULL,0);
  XtAppAddConverter (context,XmRRowColumnType          ,XtRString,ConvertXmRRowColumnTypeXtRString,NULL,0);
  XtAppAddConverter (context,XmRProcessingDirection    ,XtRString,ConvertXmRProcDirectionXtRString,NULL,0);
  XtAppAddConverter (context,XmRResizePolicy           ,XtRString,ConvertXmRResizePolicyXtRString,NULL,0);
  XtAppAddConverter (context,XmRSelectionPolicy        ,XtRString,ConvertXmRSelectPolicyXtRString,NULL,0);
  XtAppAddConverter (context,XmRSeparatorType          ,XtRString,ConvertXmRSeparatorTypeXtRString,NULL,0);
  XtAppAddConverter (context,XmRShadowType             ,XtRString,ConvertXmRShadowTypeXtRString,NULL,0);
  XtAppAddConverter (context,XmRStringDirection        ,XtRString,ConvertXmRStringDirecXtRString,NULL,0);
  XtAppAddConverter (context,XmRUnitType               ,XtRString,ConvertXmRUnitTypeXtRString,NULL,0);
  XtAppAddConverter (context,XmRVisualPolicy           ,XtRString,ConvertXmRVisualPolicyXtRString,NULL,0);
  XtAppAddConverter (context,XmRWhichButton            ,XtRString,ConvertXmRWhichButtonXtRString,NULL,0);
  XtAppAddConverter (context,XmRScrollBarDisplayPolicy ,XtRString,ConvertXmRSBarDispPolicyXtRString,NULL,0);
  XtAppAddConverter (context,XmRScrollBarPlacement     ,XtRString,ConvertXmRSBarPlacementXtRString,NULL,0);
  XtAppAddConverter (context,XmRScrollingPolicy        ,XtRString,ConvertXmRScrollPolicyXtRString,NULL,0);
#if XmVERSION >= 2
  XtAppAddConverter (context,XmRSet                    ,XtRString,ConvertXmRSetXtRString,NULL,0);
#endif

/*
  XtAppAddConverter (context,XmRPrimForegroundPixmap   ,XtRString,ConvertUnknown,NULL,0);
  XtAppAddConverter (context,XmRPrimHighlightPixmap    ,XtRString,ConvertUnknown,NULL,0);
  XtAppAddConverter (context,XmRPrimTopShadowPixmap    ,XtRString,ConvertUnknown,NULL,0);
  XtAppAddConverter (context,XmRPrimBottomShadowPixmap ,XtRString,ConvertUnknown,NULL,0);
  XtAppAddConverter (context,XmRManTopShadowPixmap     ,XtRString,ConvertUnknown,NULL,0);
  XtAppAddConverter (context,XmRManBottomShadowPixmap  ,XtRString,ConvertUnknown,NULL,0);
  XtAppAddConverter (context,XmRManHighlightPixmap     ,XtRString,ConvertUnknown,NULL,0);
  XtAppAddConverter (context,XmRManForegroundPixmap    ,XtRString,ConvertUnknown,NULL,0);
  XtAppAddConverter (context,XmRBackgroundPixmap       ,XtRString,ConvertUnknown,NULL,0);

  XtAppAddConverter (context,XmRFontList               ,XtRString,ConvertUnknown,NULL,0);
*/

  XtAppAddConverter (context,XmRProc                   ,XtRString,ConvertUnknown,NULL,0);
  XtAppAddConverter (context,XmRSizePolicy             ,XtRString,ConvertUnknown,NULL,0);
  XtAppAddConverter (context,XmRListSpacing            ,XtRString,ConvertUnknown,NULL,0);
  XtAppAddConverter (context,XmRListMarginWidth        ,XtRString,ConvertUnknown,NULL,0);
  XtAppAddConverter (context,XmRListMarginHeight       ,XtRString,ConvertUnknown,NULL,0);
  XtAppAddConverter (context,XmRDoubleClickInterval    ,XtRString,ConvertUnknown,NULL,0);
}
/***************************************************************************/
void XmClearClass (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
/*.........................................................................*/
  for(count=0;count<Class.fontListn;count++)
    {
      CStringDelete  (Class.fontLists[count].name);
      XmFontListFree (Class.fontLists[count].fontList);
      XFreeFontInfo  (NULL,Class.fontLists[count].fontStruct,1);
    }
  CMemoryFreeBlock   (Class.fontLists);
  Class.fontLists    = NULL;
  Class.fontListn    = 0;
  if(Class.password!=NULL) free (Class.password);
  Class.password     = NULL;

  Class.askContext   = NULL;
  Class.ballonShell  = NULL;
  Class.ballonShape  = NULL;
  Class.ballonLabel  = NULL;
  Class.file         = NULL;
  Class.file_text    = NULL;
  Class.file_label   = NULL;

  Class.text         = NULL;
  Class.text_text    = NULL;
  Class.text_label   = NULL;

  Class.list         = NULL;
  Class.list_list    = NULL;

  Class.passForm     = NULL;
  Class.passLabel    = NULL;
  Class.passText     = NULL;

  Class.caution      = NULL;
  Class.yesno        = NULL;
  Class.prompt        = NULL;
}
/***************************************************************************/
char* XmConvertResourceValueToString (
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
      (strcmp(a_name,XmNvalue)==0) 
     )    /* XmText/XmNvalue returned value has to be freed.*/
    {
      /*printf  ("debug:Xm:%s.%s(%s):%s\n",XtName(a_widget),a_name,a_type,*((char**)a_value)); */
      return  *((char**)a_value);
    }

  if(strcmp(a_type,XmRXmString)==0)
    { 
      char*    string;
      string   = XmConvertCompoundStringToString (*((XmString*)a_value),0);
      return   (string!=NULL ? string : (char*)XtNewString(""));
    }

  if( (strcmp(a_type,"XmFontList")==0)  || 
      (strcmp(a_type,"FontList")==0) 
     ) /*OResourceIsFontList*/
    { 
      char*       string;
      string      = (char*)XtNewString(XmConvertFontListToString(*((XmFontList*)a_value)));
      return       (string!=NULL ? string : (char*)XtNewString(""));
    }

  if( (strcmp(a_name,XmNitemCount)==0)         || 
      (strcmp(a_name,XmNlistItemCount)==0)     || 
      (strcmp(a_name,XmNselectedItemCount)==0) || 
      (strcmp(a_name,XmNhistoryItemCount)==0)
     )
    { /* MOTIF converter for xxxItems is not sensible to \n: then xxxItemCount must be set to 1 in .odb files */
      return                           (char*)XtNewString("1");
    }
  if(strcmp(a_name,XmNitems)==0)
    { int                              item_count;
      char*                            string;
      Arg                              args[1];
      XtSetArg                         (args[0],XmNitemCount,&item_count);
      XtGetValues                      (a_widget,args,1);
      string                           = XmStringTableConvertToString (*((XmStringTable*)a_value),item_count);
      return                           (string!=NULL ? string : (char*)XtNewString(""));
    }
  if(strcmp(a_name,XmNlistItems)==0)
    { int                              item_count;
      char*                            string;
      Arg                              args[1];
      XtSetArg                         (args[0],XmNlistItemCount,&item_count);
      XtGetValues                      (a_widget,args,1);
      string                           = XmStringTableConvertToString (*((XmStringTable*)a_value),item_count);
      return                           (string!=NULL ? string : (char*)XtNewString(""));
    }
  if(strcmp(a_name,XmNselectedItems)==0)
    { int                              item_count;
      char*                            string;
      Arg                              args[1];
      XtSetArg                         (args[0],XmNselectedItemCount,&item_count);
      XtGetValues                      (a_widget,args,1);
      string                           = XmStringTableConvertToString (*((XmStringTable*)a_value),item_count);
      return                           (string!=NULL ? string : (char*)XtNewString(""));
    }
  if(strcmp(a_name,XmNhistoryItems)==0)
    { int                              item_count;
      char*                            string;
      Arg                              args[1];
      XtSetArg                         (args[0],XmNhistoryItemCount,&item_count);
      XtGetValues                      (a_widget,args,1);
      string                           = XmStringTableConvertToString (*((XmStringTable*)a_value),item_count);
      return                           (string!=NULL ? string : (char*)XtNewString(""));
    }

  return NULL;
}
/***************************************************************************/
int XmConvertStringToResourceChange (
 Widget a_widget 
,char* a_name  
,char* a_type  
,char* a_value 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_widget==NULL) return 0;
  if(a_name==NULL)   return 0;
  if(a_type==NULL)   return 0;
  if(a_value==NULL)  return 0;
  if(XResourceChangesSetWidget(a_widget)==0) return 0;

  if( (strcmp(a_type,XtRString)==0) &&
      (strcmp(a_name,XmNvalue)==0) 
     )
    {
/* 
   printf                   ("debug:Xm:%s.%s(%s):%s\n",XtName(a_widget),a_name,a_type,a_value); 
*/
      XResourceChangesAddEntry (a_name,(XtArgVal)a_value,NULL);
      XResourceChangesFlush    ();
      return                   1;
    }

  if( (strcmp(a_name,XmNitemCount)==0)         || 
      (strcmp(a_name,XmNlistItemCount)==0)     || 
      (strcmp(a_name,XmNselectedItemCount)==0) || 
      (strcmp(a_name,XmNhistoryItemCount)==0)
     )
    { 
      return        1;
    }
/* assume that widget will do a private copy of stable, cps */
  if(strcmp(a_name,XmNitems)==0)
    { 
      XmString*     stable;
      int           nitem;
      stable        = XmStringTableCreateFromString    (a_value);
      nitem         = XmStringTableGetSize (stable);
      XResourceChangesAddEntry (XmNitems            ,(XtArgVal)stable,(XResourceChangeFunction)XmStringTableDelete);
      XResourceChangesAddEntry (XmNitemCount        ,(XtArgVal)nitem ,NULL);
      XResourceChangesFlush ();
      return        1;
    }
  if(strcmp(a_name,XmNlistItems)==0)
    { 
      XmString*     stable;
      int           nitem;
      stable        = XmStringTableCreateFromString    (a_value);
      nitem         = XmStringTableGetSize (stable);
      XResourceChangesAddEntry (XmNlistItems        ,(XtArgVal)stable,(XResourceChangeFunction)XmStringTableDelete);
      XResourceChangesAddEntry (XmNlistItemCount    ,(XtArgVal)nitem,NULL);
      XResourceChangesFlush ();
      return        1;
    }
  if(strcmp(a_name,XmNselectedItems)==0) 
    { 
      XmString*     stable;
      int           nitem;
      stable        = XmStringTableCreateFromString    (a_value);
      nitem         = XmStringTableGetSize (stable);
      XResourceChangesAddEntry (XmNselectedItems    ,(XtArgVal)stable,(XResourceChangeFunction)XmStringTableDelete);
      XResourceChangesAddEntry (XmNselectedItemCount,(XtArgVal)nitem ,NULL);
      XResourceChangesFlush ();
      return        1;
    }
  if(strcmp(a_name,XmNhistoryItems)==0)
    { 
      XmString*     stable;
      int           nitem;
      stable        = XmStringTableCreateFromString    (a_value);
      nitem         = XmStringTableGetSize (stable);
      XResourceChangesAddEntry (XmNhistoryItems    ,(XtArgVal)stable,(XResourceChangeFunction)XmStringTableDelete);
      XResourceChangesAddEntry (XmNhistoryItemCount,(XtArgVal)nitem ,NULL);
      XResourceChangesFlush ();
      return        1;
    }

  if(strcmp(a_type,XmRXmString)==0)
    { XmString       cps;   /*assume that a local copy of cps is done by the widget*/
      cps            = XmStringLtoRCreate(a_value,XmSTRING_DEFAULT_CHARSET);
      XResourceChangesAddEntry  (a_name,(XtArgVal)cps,(XResourceChangeFunction)XmStringFree);
      XResourceChangesFlush  ();
      return         1;
    }

  if( (strcmp(a_type,"XmFontList")==0)  || 
      (strcmp(a_type,"FontList")==0)
     ) /*OResourceIsFontList*/
    { 
      XmFontList   fontlist;
      fontlist     = XmConvertStringToFontList (a_value,XtDisplay(a_widget));
      if(fontlist!=NULL)
        {
          XResourceChangesAddEntry  (a_name,(XtArgVal)fontlist,NULL);
          XResourceChangesFlush  ();
        }
      return 1;
    }

  return 0;
}
/***************************************************************************/
char** XmGetResourceOptions (
 char* a_type 
,int* a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_number!=NULL) *a_number = 0;
  if(a_type==NULL) return NULL;

       if(strcmp(a_type,XmREditMode)==0)
  {
    static char* edit_mode[2] = {
"multi_line_edit",
"single_line_edit"
};
    if(a_number!=NULL) *a_number = 2;
    return    edit_mode;
  }
  else if(strcmp(a_type,XmRAlignment)==0)
  {
    static char* alignment[3] = {
"alignment_beginning",
"alignment_center",
"alignment_end"
};
    if(a_number!=NULL) *a_number = 3;
    return    alignment;
  }
  else if(strcmp(a_type,XmRArrowDirection)==0)
  {
    static char* arrow_direction[4] = {
"arrow_up",
"arrow_down",
"arrow_left",
"arrow_right"
};
    if(a_number!=NULL) *a_number = 4;
    return    arrow_direction;
  }
  else if(strcmp(a_type,XmRAttachment)==0)
  {
    static char* attachment[7] = {
"attach_none",
"attach_form",
"attach_opposite_form",
"attach_widget",
"attach_opposite_widget",
"attach_position",
"attach_self"
};
    if(a_number!=NULL) *a_number = 7;
    return    attachment;
  }
  else if(strcmp(a_type,XmRDefaultButtonType)==0)
  {
    static char* default_button_type[3] = {
"dialog_ok_button",
"dialog_cancel_button",
"dialog_help_button"
};
    if(a_number!=NULL) *a_number = 3;
    return    default_button_type;
  }
  else if(strcmp(a_type,XmRDialogStyle)==0)
  {
    static char* dialog_style[4] = {
"dialog_work_area",
"dialog_modeless",
"dialog_application_modal",
"dialog_system_modal"
};
    if(a_number!=NULL) *a_number = 4;
    return    dialog_style;
  }
  else if(strcmp(a_type,XmRDialogType)==0)
  {
    static char* dialog_type[9] = {
"dialog_error",
"dialog_information",
"dialog_message",
"dialog_question",
"dialog_warning",
"dialog_working",
"dialog_prompt",
"dialog_selection",
"dialog_command"
};
    if(a_number!=NULL) *a_number = 9;
    return    dialog_type;
  }
  else if(strcmp(a_type,XmRIndicatorType)==0)
  {
    static char* indicator_type[2] = {
"n_of_many",
"one_of_many"
};
    if(a_number!=NULL) *a_number = 2;
    return    indicator_type;
  }
  else if(strcmp(a_type,XmRLabelType)==0)
  {
    static char* label_type[2] = {
"string",
"pixmap"
};
    if(a_number!=NULL) *a_number = 2;
    return    label_type;
  }
  else if(strcmp(a_type,XmRListSizePolicy)==0)
  {
    static char* list_size_policy[3] = {
"constant",
"variable",
"resize_if_possible"
};
    if(a_number!=NULL) *a_number = 3;
    return    list_size_policy;
  }
  else if(strcmp(a_type,XmRPacking)==0)
  {
    static char* packing[3] = {
"pack_none",
"pack_tight",
"pack_column"
};
    if(a_number!=NULL) *a_number = 3;
    return    packing;
  }
  else if(strcmp(a_type,XmRRowColumnType)==0)
  {
    static char* row_column_type[5] = {
"work_area",
"menu_bar",
"menu_popup",
"menu_pulldown",
"menu_option"
};
    if(a_number!=NULL) *a_number = 5;
    return    row_column_type;
  }
  else if(strcmp(a_type,XmRProcessingDirection)==0)
  {
    static char* processing_direction[4] = {
"max_on_top",
"max_on_bottom",
"max_on_left",
"max_on_right"
};
    if(a_number!=NULL) *a_number = 4;
    return    processing_direction;
  }
  else if(strcmp(a_type,XmRResizePolicy)==0)
  {
    static char* resize_policy[3] = {
"resize_none",
"resize_grow",
"resize_any"
};
    if(a_number!=NULL) *a_number = 3;
    return    resize_policy;
  }
  else if(strcmp(a_type,XmRSelectionPolicy)==0)
  {
    static char* selection_policy[4] = {
"single_select",
"multiple_select",
"extended_select",
"browse_select"
};
    if(a_number!=NULL) *a_number = 4;
    return    selection_policy;
  }
  else if(strcmp(a_type,XmRSeparatorType)==0)
  {
    static char* separator_type[7] = {
"no_line",
"single_line",
"double_line",
"single_dashed_line",
"double_dashed_line",
"shadow_etched_out",
"shadow_etched_in"
};
    if(a_number!=NULL) *a_number = 7;
    return    separator_type;
  }
  else if(strcmp(a_type,XmRShadowType)==0)
  {
    static char* shadow_type[4] = {
"shadow_etched_in",
"shadow_etched_out",
"shadow_in",
"shadow_out",
};
    if(a_number!=NULL) *a_number = 4;
    return    shadow_type;
  }
  else if(strcmp(a_type,XmRStringDirection)==0)
  {
    static char* string_direction[2] = {
"string_direction_l_to_r",
"string_direction_r_to_l"
};
    if(a_number!=NULL) *a_number = 2;
    return    string_direction;
  }
  else if(strcmp(a_type,XmRUnitType)==0)
  {
    static char* unit_type[5] = {
"pixels",
"100th_millimeters",
"1000th_inches",
"100th_points",
"100th_font_units"
};
    if(a_number!=NULL) *a_number = 5;
    return    unit_type;
  }
  else if(strcmp(a_type,XmRVisualPolicy)==0)
  {
    static char* visual_policy[2] = {
"variable",
"constant"
};
    if(a_number!=NULL) *a_number = 2;
    return    visual_policy;
  }
  else if(strcmp(a_type,XmRWhichButton)==0)
  {
    static char* which_button[5] = {
"button1",
"button2",
"button3",
"button4",
"button5"
};
    if(a_number!=NULL) *a_number = 5;
    return    which_button;
  }
  else if(strcmp(a_type,XmRScrollBarDisplayPolicy)==0)
  {
    static char* scroll_bar_display_policy[2] = {
"as_needed",
"static"
};
    if(a_number!=NULL) *a_number = 2;
    return    scroll_bar_display_policy;
  }
  else if(strcmp(a_type,XmRScrollBarPlacement)==0)
  {
    static char* scroll_bar_placement[4] = {
"top_left",
"top_right",
"bottom_left",
"bottom_right"
};
    if(a_number!=NULL) *a_number = 4;
    return    scroll_bar_placement;
  }
  else if(strcmp(a_type,XmRScrollingPolicy)==0)
  {
    static char* scrolling_policy[2] = {
"automatic",
"application_defined"
};
    if(a_number!=NULL) *a_number = 2;
    return    scrolling_policy;
  }
 return NULL;
}
/***************************************************************************/
char* XmGetCallbackData (
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
       if(strcmp(cname,"XmScrollBar")==0)
    { 
      if(a_data==NULL)           return NULL;
      sprintf               (string,"%d",((XmScrollBarCallbackStruct*)a_data)->value);
      return                (char*)XtNewString(string);
    }
  else if(strcmp(cname,"XmScale")==0)
    { 
      if(a_data==NULL)           return NULL;
      sprintf               (string,"%d",((XmScaleCallbackStruct*)a_data)->value);
      return                (char*)XtNewString(string);
    }
  else if(strcmp(cname,"XmText")==0)
    { 
      return               XmTextGetString (This);
    }
  else if(strcmp(cname,"XmTextField")==0)
    { 
      return               XmTextFieldGetString (This);
    }
  else if(strcmp(cname,"XmCommand")==0)
    { 
      if(a_data==NULL)          return NULL;
      return               XmConvertCompoundStringToString (((XmCommandCallbackStruct*)a_data)->value,0);
    }
  else if(strcmp(cname,"XmList")==0)
    { XmListCallbackStruct *data;
      if(a_data==NULL)          return NULL;
      data                 = (XmListCallbackStruct*)a_data;
           if(data->reason==XmCR_SINGLE_SELECT)
          return XmConvertCompoundStringToString (data->item,0);
      else 
          return NULL;
    }
  else if(strcmp(cname,"XmFileSelectionBox")==0)
    { XmFileSelectionBoxCallbackStruct *data;
      if(a_data==NULL)          return NULL;
      data                 = (XmFileSelectionBoxCallbackStruct*)a_data;
           if(data->reason==XmCR_OK)
          return XmConvertCompoundStringToString (data->value,0);
      else if(data->reason==XmCR_APPLY)
          return XmConvertCompoundStringToString (data->value,0);
      else 
          return NULL;
    }
  else if(strcmp(cname,"XmSelectionBox")==0)
    { XmSelectionBoxCallbackStruct *data;
      if(a_data==NULL)          return NULL;
      data                 = (XmSelectionBoxCallbackStruct*)a_data;
           if(data->reason==XmCR_OK)
          return XmConvertCompoundStringToString (data->value,0);
      else if(data->reason==XmCR_APPLY)
          return XmConvertCompoundStringToString (data->value,0);
      else 
          return NULL;
    }
  a_what = NULL;
  return NULL;
}
/***************************************************************************/
XEvent* XmGetCallbackEvent (
 XtPointer a_data 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_data==NULL) return NULL;
  if((((XmAnyCallbackStruct*)a_data)->event)==NULL) return NULL;
  return ((XmAnyCallbackStruct*)a_data)->event;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
XmFontList XmConvertStringToFontList (
 char* a_string
,Display* a_display
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  XFontStruct*   fontStruct = NULL;
  XmFontList     fontList;
/*.........................................................................*/
  if(a_string==NULL)  return NULL;
  if(a_display==NULL) return NULL;

 { int count;  /*Look if already loaded.*/
  for(count=0;count<Class.fontListn;count++)
    {
      if(strcmp(a_string,Class.fontLists[count].name)==0) 
	return Class.fontLists[count].fontList;
    }}

  fontStruct      = XLoadQueryFont (a_display,a_string);
  if(fontStruct==NULL) return NULL;

  fontList        = XmFontListCreate (fontStruct,XmSTRING_DEFAULT_CHARSET);
  
  if(fontList!=NULL)
    {
      Class.fontLists = Class.fontListn==0 ? 
                      (OFontListElemRec*)CMemoryAllocateBlock(sizeof(OFontListElemRec)) :
                      (OFontListElemRec*)CMemoryChangeBlockSize((char *)(Class.fontLists),(Class.fontListn+1)*sizeof(OFontListElemRec));
      if(Class.fontLists==NULL) {Class.fontListn = 0;return NULL;}
      Class.fontLists[Class.fontListn].name       = CStringDuplicate(a_string);
      Class.fontLists[Class.fontListn].fontStruct = fontStruct;
      Class.fontLists[Class.fontListn].fontList   = fontList;
      Class.fontListn++;
    }

  return fontList;
}
/***************************************************************************/
char* XmConvertFontListToString (
 XmFontList a_font_list
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
/*.........................................................................*/
  if(a_font_list==NULL) return NULL;

  for(count=0;count<Class.fontListn;count++)
    {
      if(a_font_list == Class.fontLists[count].fontList) 
	return Class.fontLists[count].name;
    }

  return NULL;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
char* XmConvertCompoundStringToString (
 XmString a_cps 
,int a_number 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*             string;
   int      icount;
  XmStringContext   context;
  Boolean           done;
/*.........................................................................*/
  if(a_cps==NULL)           return NULL;
  string               = NULL;
  XmStringInitContext  (&context,a_cps);
  icount               = 0;
  done                 = False;
  while(done==False)
    {  char*             text    = NULL;
       XmStringCharSet   charset = NULL;
       XmStringDirection direct;
       Boolean           sep;
       if(XmStringGetNextSegment(context,&text,&charset,&direct,&sep)==True) 
         {
           XtFree               (charset);
           if(sep==True)        done = True;
           if(icount==a_number) 
             { 
               string = text;
               break;
             }
           icount               ++;
           XtFree               (text);
         }
       else
         done = True;
    }
  XmStringFreeContext  (context);
  return               string;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
char* XmStringTableConvertToString (
 XmStringTable This
,int a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  char*  sret;
  int    lret;
/*.........................................................................*/
  if(This==NULL) return NULL;
  sret   = NULL;
  lret   = 0;
  for(count=0;count<a_number;count++)
    { char*        string;
      int          length;
      string       = XmConvertCompoundStringToString (This[count],0);
      if(string!=NULL)
        {
	  length       = strlen(string);
	  sret         =  sret!=NULL ? 
	                  XtRealloc (sret, (lret+length+2+1) * sizeof(char)) :
	                  XtMalloc  (      (lret+length+2+1) * sizeof(char)) ;
	  if(sret==NULL)    
	    {
	      XtFree(string);
	      return NULL;
	    }      
	  sprintf      (sret+lret,"%s\\n",string);
	  lret        += length + 2;
	}        
      XtFree       (string);
    }
  if(sret!=NULL)
    { /* remove last \n */
      sret[lret-2] = 0;
    }
  return sret;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/******** XmSTable routine to handle null ended list of XmString ***********/
/***************************************************************************/
/***************************************************************************/
XmString* XmStringTableCreateFromString (
 char* a_string
)
/***************************************************************************/
/*
  String is a list separated by "\n" or "\\n".
 Free with: XmStringTableDelete.
 Same algo then CTextCreateFromString.
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  CList  items  = NULL;
  char*  string;
  int    length,pos,count;
/*.........................................................................*/
  if(a_string==NULL)  return NULL;
  length           = strlen(a_string);
  if(length<=0)    return NULL;

  string           = CStringDuplicate(a_string);
  if(string==NULL) return NULL;

  pos              = 0;
  items            = NULL;
  length           ++;
  for(count=0;count<length;count++) 
    {
      if( ( (string[count]=='\\') && (string[count+1]=='n') ) || (string[count]=='\n') || (string[count]=='\0') ) 
        { 
	  char           shift_one;
          char*          item;
          shift_one      = (string[count]=='\n' ? 1 : 0);
          string[count]  = '\0';
          item           = string+pos;
	  if(shift_one==1)
	    {
	      pos    = count+1;
	    }
	  else
	    {
	      pos    = count+2;
	      count ++;
	    }
          if(item!=NULL)
            {
	      XmString      xmstring;
	      xmstring      = XmStringLtoRCreate(item,XmSTRING_DEFAULT_CHARSET);
	      if(xmstring!=NULL) CListAddEntry (&items,xmstring);
            }
        }
    }
  CMemoryFreeBlock (string);
  return           (XmString*)items;
}
/***************************************************************************/
XmString* XmStringTableCreateFromText (
 int a_number
,char** a_list
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  XmString*  table;
   int count,icount;
/*.........................................................................*/
  if(a_list==NULL)     return NULL; 
  if(a_number<=0) return NULL; 
  table          = (XmString*) CMemoryAllocateBlock((a_number+1)*sizeof(XmString));
  if(table==NULL)     return NULL;
  icount         = 0;
  for(count=0;count<a_number;count++)
    { XmString     xmstring;
      xmstring     = XmStringLtoRCreate(a_list[count],XmSTRING_DEFAULT_CHARSET);
      if(xmstring!=NULL)
        {
	  table[icount] = xmstring;
	  icount++;
	}
    }
  table[icount]  = NULL; /*Null ended list*/
  return         table;
}
/***************************************************************************/
int XmStringTableGetSize (
 XmString* This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return CListGetSize ((CList)This);
}
/***************************************************************************/
void XmStringTableDelete (
 XmString* This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
/*.........................................................................*/
  if(This==NULL) return;
  count = 0;
  while(This[count]!=NULL)
    {
      XmStringFree (This[count]);
      count++;
    }
  CListDelete ((CList)This);
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
void XmMapWidget (
 Widget This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
       if(ManageToMap(This)==True)
    { 
/* strange logic due to problem in the following sequence: 
  - dialog child of an other dialog
  - manage parent   -> parent mapped
  - manage child    -> child  mapped
  - unmanage parent -> parent,child are unmapped 
  - manage parent   -> parent mapped 
  - manage child    -> child never mapped !!!!!!!!!
*/
      if(XWidgetIsVisible(This)==False) 
	{
          XtUnmanageChild (This);
          XtManageChild   (This);
        }
    }
  else if(XtIsSubclass(This,xmDialogShellWidgetClass))
    {WidgetList list;
     list       = XWidgetGetChildren (This,NULL);
     if( (list!=NULL) && (list[0]!=NULL) ) XtManageChild (list[0]);
     else            	 XWidgetMapWindow (This);
     CListDelete ((CList)list);
    }
  else
    XWidgetMapWindow (This);
}
/***************************************************************************/
void XmUnmapWidget (
 Widget This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
       if(ManageToMap(This)==True)
    {
      XtUnmanageChild (This);
    }
  else if(XtIsSubclass(This,xmDialogShellWidgetClass))
    {WidgetList list;
     list       = XWidgetGetChildren (This,NULL);
     if( (list!=NULL) && (list[0]!=NULL) )
       XtUnmanageChild (list[0]);
     else
       XWidgetUnmapWindow (This);
     CListDelete ((CList)list);
    }
  else
    XWidgetUnmapWindow (This);
}
/***************************************************************************/
void XmRaiseWidget (
 Widget This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(ManageToMap(This)==True)
    XWidgetRaise (XtParent(This));
  else 
    XWidgetRaise (This);
}
/***************************************************************************/
void XmLowerWidget (
 Widget This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(ManageToMap(This)==True)
    XWidgetLower (XtParent(This));
  else 
    XWidgetLower (This);
}
/***************************************************************************/
void XmMapAndRaiseWidget (
 Widget This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  XmMapWidget   (This);
  XmRaiseWidget (This);
}
/***************************************************************************/
static Boolean ManageToMap (
 Widget This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return False;
  if(XtIsShell(This)) return False;  
  if(XtIsSubclass(XtParent(This),xmDialogShellWidgetClass)) return True;
  return False;
}
/***************************************************************************/
void XmSetCompoundStringResourceFromString (
 Widget This
,char* a_name
,char* a_string
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Arg       args[1];
  XmString  cps;
/*.........................................................................*/
  if(This==NULL)     return;
  if(a_name==NULL)   return;
  if(a_string==NULL) return;
  cps          = XmStringLtoRCreate(a_string,XmSTRING_DEFAULT_CHARSET);
  XtSetArg     (args[0],a_name,cps);
  XtSetValues  (This,args,1);
  XmStringFree (cps);
}
/***************************************************************************/
void XmSetSideAttachment (
 Widget This 
,String a_side   
,String a_mode   
,int a_offset  
,void* a_what
,XmMarkFunction a_markProc
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget   parent;
  char     swhat[15];    /* largest is bottomPosition */
  char     soffset[13];  /* largest is bottomOffset */
  char     sattach[17];  /* largest is bottomAttachment */
  Arg      args[3];
/*.........................................................................*/
  if(This==NULL)   return;
  parent      = XtParent(This);
  if(parent==NULL) return;
  if(!XtIsSubclass(parent,xmFormWidgetClass)) return;
  if(a_side==NULL) return;
  if(a_mode==NULL) return;
  sprintf     (soffset,"%sOffset",a_side);
  sprintf     (sattach,"%sAttachment",a_side);
       if(strcmp(a_mode,"attach_none")==0)
    {
      XtSetArg               (args[0], sattach  ,XmATTACH_NONE);
      XtSetArg               (args[1], soffset  ,a_offset);
      XtSetValues            (This,args,2);
      if(a_markProc!=NULL)   a_markProc (This,sattach);
    }
  else if(strcmp(a_mode,"attach_self")==0)
    {
      XtSetArg              (args[0], sattach  ,XmATTACH_SELF);
      XtSetArg              (args[1], soffset  ,a_offset);
      XtSetValues           (This,args,2);
      if(a_markProc!=NULL)  a_markProc (This,sattach);
    }
  else if(strcmp(a_mode,"attach_position")==0)
    { int                   iwhat = 0;
      iwhat                 = a_what!=NULL ? *((int*)a_what) : 0;

      sprintf               (swhat,"%sPosition",a_side);
/* Motif involve crash if we try to set position and offset at same moment */
      XtSetArg              (args[0], sattach   ,XmATTACH_POSITION);
      XtSetArg              (args[1], swhat     ,iwhat);
      XtSetValues           (This,args,2);
      if(a_markProc!=NULL)  a_markProc (This,sattach);
      if(a_markProc!=NULL)  a_markProc (This,swhat);
    }
  else if(strcmp(a_mode,"attach_form")==0)
    {
      XtSetArg              (args[0], sattach  ,XmATTACH_FORM);
      XtSetArg              (args[1], soffset  ,a_offset);
      XtSetValues           (This,args,2);
      if(a_markProc!=NULL)  a_markProc (This,sattach);
    }
  else if(strcmp(a_mode,"attach_opposite_form")==0)
    {
      XtSetArg              (args[0], sattach  ,XmATTACH_OPPOSITE_FORM);
      XtSetArg              (args[1], soffset  ,a_offset);
      XtSetValues           (This,args,2);
      if(a_markProc!=NULL)  a_markProc (This,sattach);
    }
  else if(strcmp(a_mode,"attach_widget")==0)
    { Widget                wwhat;
      wwhat                 = a_what!=NULL ? *((Widget*)a_what) : (Widget)NULL;
      if(wwhat==NULL)       return;

      sprintf               (swhat,"%sWidget",a_side);
      XtSetArg              (args[0], sattach  ,XmATTACH_WIDGET);
      XtSetArg              (args[1], swhat    ,wwhat);
      XtSetArg              (args[2], soffset  ,a_offset);
      XtSetValues           (This,args,3);
      if(a_markProc!=NULL)  a_markProc (This,sattach);
      if(a_markProc!=NULL)  a_markProc (This,swhat);
    }
  else if(strcmp(a_mode,"attach_opposite_widget")==0)
    { Widget                wwhat;
      wwhat                 = a_what!=NULL ? *((Widget*)a_what) : (Widget)NULL;
      if(wwhat==NULL)       return;

      sprintf               (swhat,"%sWidget",a_side);
      XtSetArg              (args[0], sattach  ,XmATTACH_OPPOSITE_WIDGET);
      XtSetArg              (args[1], swhat    ,wwhat);
      XtSetArg              (args[2], soffset  ,a_offset);
      XtSetValues           (This,args,3);
      if(a_markProc!=NULL)  a_markProc (This,sattach);
      if(a_markProc!=NULL)  a_markProc (This,swhat);
    }
  if(a_markProc==NULL) return;
  if((a_offset!=0)&&(a_offset!=USHRT_MAX)) a_markProc (This,soffset);
  if( (a_side!=NULL) && (strcmp(a_side,"right")==0) ) a_markProc (This,"leftAttachment");
  if( (a_side!=NULL) && (strcmp(a_side,"bottom")==0)) a_markProc (This,"topAttachment");
}
/***************************************************************************/
void XmGlueWidget (
 Widget This 
,XmMarkFunction a_markProc
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget    parent;
  Position  wx,wy,px,py;
  Dimension ww,wh,pw,ph;
  int       top, bottom,left,right;
/*.........................................................................*/
  if(This==NULL) return;
  parent      = XtParent(This);
  if(parent==NULL) return;
  if(!XtIsSubclass(parent,xmFormWidgetClass)) return;
  XWidgetGetPosition       (This  ,&wx,&wy);
  XWidgetGetDimension      (This  ,&ww,&wh);
  XWidgetGetPosition       (parent,&px,&py);
  XWidgetGetDimension      (parent,&pw,&ph);
  top                    = 100 *  wy/((int)ph);
  bottom                 = 100 * (wy+(int)wh)/(int)ph;
  left                   = 100 *  wx/(int)pw;
  right                  = 100 * (wx+(int)ww)/(int)pw;
  XmAttachWidgetAtPositionAndMark (This,top, bottom,left,right,a_markProc);
}
/***************************************************************************/
void XmAttachWidgetAtPositionAndMark (
 Widget This 
,int  a_top    
,int  a_bottom 
,int  a_left   
,int  a_right  
,XmMarkFunction a_markProc
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget parent;
  Arg    args[9];
/*.........................................................................*/
  if(This==NULL) return;
  parent      = XtParent(This);
  if(parent==NULL) return;
  if(!XtIsSubclass(parent,xmFormWidgetClass)) return;
  XtSetArg (args[0], XmNresizable        ,True); 
  XtSetArg (args[1], XmNtopAttachment    ,XmATTACH_POSITION); 
  XtSetArg (args[2], XmNbottomAttachment ,XmATTACH_POSITION); 
  XtSetArg (args[3], XmNleftAttachment   ,XmATTACH_POSITION); 
  XtSetArg (args[4], XmNrightAttachment  ,XmATTACH_POSITION); 
  XtSetArg (args[5], XmNtopPosition      ,a_top); 
  XtSetArg (args[6], XmNbottomPosition   ,a_bottom);
  XtSetArg (args[7], XmNleftPosition     ,a_left); 
  XtSetArg (args[8], XmNrightPosition    ,a_right);
  XtSetValues (This, args,9);

  if(a_markProc==NULL) return;

  a_markProc (This, XmNresizable);
  a_markProc (This, XmNtopAttachment);
  a_markProc (This, XmNbottomAttachment);
  a_markProc (This, XmNleftAttachment);
  a_markProc (This, XmNrightAttachment);
  a_markProc (This, XmNtopPosition);
  a_markProc (This, XmNbottomPosition);
  a_markProc (This, XmNleftPosition);
  a_markProc (This, XmNrightPosition);

}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
void XmLabelSetLabelString (
 Widget This 
,char* a_string
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Arg       args[1];
  XmString  cps;
/*.........................................................................*/
  if(This==NULL)     return;
  if(a_string==NULL) return;
  cps          = XmStringLtoRCreate(a_string,XmSTRING_DEFAULT_CHARSET);
  XtSetArg     (args[0],XmNlabelString,cps);
  XtSetValues  (This,args,1);
  XmStringFree (cps);
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
void XmListSetItemsFromText (
 Widget        This 
,int           a_linen
,char**        a_lines
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  XmString* xmss;
  int       xmsn;
/*.........................................................................*/
  if(This==NULL)                                 return;
  if(!XtIsSubclass(This,xmListWidgetClass)) return;
  xmss                   = XmStringTableCreateFromText  (a_linen,a_lines);
  xmsn                   = XmStringTableGetSize    (xmss);
  XmListDeleteAllItems   (This);
  XmListDeselectAllItems (This);
  XmListAddItems         (This,xmss,xmsn,0);
  XmStringTableDelete         (xmss);
}
/***************************************************************************/
void XmListSetSelectedItem (
 Widget        This 
,char*         a_string
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  XmString               cps;
/*.........................................................................*/
  if(This==NULL)                            return;
  if(!XtIsSubclass(This,xmListWidgetClass)) return;
  cps                    = XmStringLtoRCreate(a_string,XmSTRING_DEFAULT_CHARSET);
  XmListDeselectAllItems (This);
  XmListSelectItem       (This,cps,False);
  XmStringFree           (cps);
}
/***************************************************************************/
char* XmListGetSelectedItem (
 Widget        This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  XmStringTable items = NULL;
  int           itemn = 0;
  Arg           args[2];
/*.........................................................................*/
  if(This==NULL)                                 return NULL;
  if(!XtIsSubclass(This,xmListWidgetClass)) return NULL;
  XtSetArg     (args[0],XmNselectedItems    ,&items);
  XtSetArg     (args[1],XmNselectedItemCount,&itemn);
  XtGetValues  (This,args,2);
  return       XmStringTableConvertToString (items,itemn);
}
/***************************************************************************/
void XmListSetItemsFromFile (
 Widget This
,char* a_fname
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  FILE*  file;
/*.........................................................................*/
  if(This==NULL)                                 return;
  if(!XtIsSubclass(This,xmListWidgetClass)) return;
  file            = CFileOpenForReading (a_fname,NULL);
  XmListSetItemsFromStream  (This,file);
  fclose          (file);
}
/***************************************************************************/
void XmListSetItemsFromStream (
 Widget This
,FILE* a_file
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*  lineb;
  char** lines;
  int    linen;
/*.........................................................................*/
  if(This==NULL)                            return;
  if(!XtIsSubclass(This,xmListWidgetClass)) return;
  if(a_file==NULL)                          return; 
  lineb                  = CStreamGetStrings (a_file,'\0',&linen,&lines);
  XmListSetItemsFromText (This,linen,lines);
  CMemoryFreeBlock       (lines);
  CMemoryFreeBlock       (lineb);
}
/***************************************************************************/
void XmListAppendString (
 Widget This
,char* a_string
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  XmString               cps;
/*.........................................................................*/
  if(This==NULL)                            return;
  if(!XtIsSubclass(This,xmListWidgetClass)) return;
  if(a_string==NULL)                        return;
  cps                     = XmStringLtoRCreate(a_string,XmSTRING_DEFAULT_CHARSET);
  XmListAddItemUnselected (This,cps,0);
  XmStringFree            (cps);
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
void XmLabelSetShapeAccordingToPixmap (
 Widget This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  XWidgetSetShape (This,XShapePixmap,XWidgetGetResource(This,XmNlabelPixmap));
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
void XmTextSetNumberOfRowsAndColumns (
 Widget        This 
,int           a_rows
,int           a_cols
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Arg          args[2];
/*.........................................................................*/
  if(This==NULL) return;
  if(!XtIsSubclass(This,xmTextWidgetClass)) return;
  XtSetArg    (args[0],XmNrows   ,a_rows);
  XtSetArg    (args[1],XmNcolumns,a_cols);
  XtSetValues (This,args,2);
}
/***************************************************************************/
char* XmTextGetValue (
 Widget This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*        string = NULL;
  Arg          args[1];
/*.........................................................................*/
  if(This==NULL)    return NULL;
  XtSetArg     (args[0],XmNvalue,&string);
  XtGetValues  (This,args,1);
  return       string;
}
/***************************************************************************/
Widget XmTextConvertValueToWidget (
 Widget        This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)                                  return NULL;
  if(!XtIsSubclass(This,xmTextWidgetClass)) return NULL;
  return  XWidgetGetAndConvertStringResourceToWidget (This,XmNvalue);
}
/***************************************************************************/
int XmTextConvertValueToBoolean (
 Widget        This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)                                  return 0;
  if(!XtIsSubclass(This,xmTextWidgetClass)) return 0;
  return                               XWidgetGetAndConvertStringResourceToBoolean (This,XmNvalue);
}
/***************************************************************************/
double XmTextConvertValueToDouble (
 Widget        This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)                                  return 0.;
  if(!XtIsSubclass(This,xmTextWidgetClass)) return 0.;
  return                               XWidgetGetAndConvertStringResourceToDouble (This,XmNvalue);
}
/***************************************************************************/
int XmTextConvertValueToInteger (
 Widget        This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)                                  return 0;
  if(!XtIsSubclass(This,xmTextWidgetClass)) return 0;
  return                               XWidgetGetAndConvertStringResourceToInteger (This,XmNvalue);
}
/***************************************************************************/
void XmTextSetValueFromWidgetName (
 Widget This 
,Widget a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)                                  return;
  if(!XtIsSubclass(This,xmTextWidgetClass)) return;
  if(!XWidgetIsValid(a_value))              return;
  XmTextSetString                           (This,XtName(a_value));
}
/***************************************************************************/
void XmTextSetValueF (
 Widget This
,int   a_number
,char* a_format 
,...
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  va_list args;
  char*   string;
/*.........................................................................*/
  if(This==NULL)                            return;
  if(!XtIsSubclass(This,xmTextWidgetClass)) return;
  if(a_number<0)     return;
  if(a_format==NULL) return;
  va_start         (args,a_format);
  string           = CStringCreateV (a_number,a_format,args);
  va_end           (args);
  if(string==NULL) return;
  XmTextSetString  (This,string);
  CStringDelete    (string);
}
/***************************************************************************/
void XmTextAppendValueV (
 Widget This
,int   a_number
,char* a_format 
,va_list a_args
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*              string;
/*.........................................................................*/
  if(a_number<0)     return;
  string             = CStringCreateV (a_number,a_format,a_args);
  XmTextAppendString (This,string);
  CStringDelete      (string);
}
/***************************************************************************/
void XmTextAppendValueF (
 Widget This
,int   a_number
,char* a_format 
,...
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  va_list            args;
/*.........................................................................*/
  if(a_format==NULL) return;
  va_start           (args,a_format);
  XmTextAppendValueV (This,a_number,a_format,args);
  va_end             (args);
}
/***************************************************************************/
void XmTextAppendString (
 Widget This
,char* a_string
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  XmTextPosition  lastpos;
/*.........................................................................*/
  if(This==NULL)                            return;
  if(!XtIsSubclass(This,xmTextWidgetClass)) return;
  if(a_string==NULL)                        return;
  lastpos                    = XmTextGetLastPosition(This);
  XmTextReplace              (This,lastpos,lastpos,a_string);
  XmTextSetInsertionPosition (This,XmTextGetLastPosition(This));
}
/***************************************************************************/
void XmTextSetValueFromFile (
 Widget This
,char* a_fname
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  FILE*  file;
/*.........................................................................*/
  if(This==NULL)                                  return;
  if(!XtIsSubclass(This,xmTextWidgetClass)) return;
  file            = CFileOpenForReading (a_fname,NULL);
  XmTextSetValueFromStream  (This,file);
  fclose          (file);
}
/***************************************************************************/
void XmTextSetValueFromStream (
 Widget This
,FILE* a_file
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*  string;
  unsigned long size;
/*.........................................................................*/
  if(This==NULL)                                  return;
  if(!XtIsSubclass(This,xmTextWidgetClass)) return;
  if(a_file==NULL)     return; 
  string          = CStreamRead (a_file,&size);
  XmTextSetString (This,string);
  CStringDelete   (string);
}
/***************************************************************************/
void XmTextPutPixmapInPS_Stream (
 Widget This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Pixmap    pixmap;
  XImage*   image;
  Dimension ww,wh;
  double    red,green,blue;
/*.........................................................................*/
  if(This==NULL)              return;
  if(!XtIsSubclass(This,xmTextWidgetClass)) return;

  pixmap                 = XWidgetGetPixmap  (This);
  image                  = XDisplayGetPixmapImage (XtDisplay(This),pixmap);
  XDisplayPutImageInPS_Stream (XtDisplay(This),image);
  if(image!=NULL)             XDestroyImage (image);
  if(pixmap!=0L)              XFreePixmap (XtDisplay(This),pixmap);

/* border not in widget pixmap */
  XWidgetGetDimension      (This,&ww,&wh);
  XDisplayGetPixelRGB          (XtDisplay(This),XWidgetGetPixelResource(This,"borderColor"),&red,&green,&blue);
  if(XWidgetGetIntegerResource(This,"highlightThickness")!=0)
    CPS_PutFrameInStream (red,green,blue,(double)ww,(double)wh);

}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
int XmSelectionBoxConvertTextToInteger (
 Widget        This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  XmString     cps    = NULL;
  char*        string = NULL;
  Arg          args[1];
/*.........................................................................*/
  if(This==NULL)   return 0;
  if(!XtIsSubclass(This,xmSelectionBoxWidgetClass)) return 0;
  XtSetArg    (args[0],XmNtextString,&cps);
  XtGetValues (This,args,1);
  string      = XmConvertCompoundStringToString (cps,0);
  if(string==NULL) return 0;
  return      CStringConvertToLong(string,NULL);
}
/***************************************************************************/
double XmSelectionBoxConvertTextToDouble (
 Widget        This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  XmString     cps    = NULL;
  char*        string = NULL;
  Arg          args[1];
/*.........................................................................*/
  if(This==NULL)    return 0.;
  if(!XtIsSubclass(This,xmSelectionBoxWidgetClass)) return 0.;
  XtSetArg     (args[0],XmNtextString,&cps);
  XtGetValues  (This,args,1);
  string       = XmConvertCompoundStringToString (cps,0);
  if(string==NULL)  return 0.; /* a_name not a resource of widget */
  return       CStringConvertToDouble (string,NULL);
}
/***************************************************************************/
void XmSelectionBoxInitialize (
 Widget        This 
,int           a_itemCount
,XmStringTable a_items
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Arg          args[3];
/*.........................................................................*/
  if(This==NULL) return;
/* check we have an XmList */
  if(!XtIsSubclass(This,xmSelectionBoxWidgetClass)) return;
  XtSetArg    (args[0],XmNlistItemCount    ,a_itemCount);
  XtSetArg    (args[1],XmNlistItems        ,a_items);
  XtSetValues (This,args,2);
}
/***************************************************************************/
void XmSelectionBoxSetText (
 Widget        This 
,int           a_linen
,char**        a_lines
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  XmString*  xmss;
  int        xmsn;
/*.........................................................................*/
  xmss                = XmStringTableCreateFromText       (a_linen,a_lines);
  xmsn                = XmStringTableGetSize         (xmss);
  XmSelectionBoxInitialize  (This,xmsn,xmss);
  XmStringTableDelete      (xmss);
}
/***************************************************************************/
/***************************************************************************/
void XmOptionInitialize (
 Widget This 
,int a_number
,char** a_text 
,char* a_default 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget        wpull,button,whistory;
  Arg           args[1];
  unsigned char value;
   int  count;
  XmString      cps;
/*.........................................................................*/
  if(This==NULL) return;
/* check we have an option menu */
  if(!XtIsSubclass(This,xmRowColumnWidgetClass)) return;
  XtSetArg    (args[0],XmNrowColumnType,&value);
  XtGetValues (This,args,1);
  if(value!=XmMENU_OPTION) return;
/* get pulldown */
  XtSetArg    (args[0],XmNsubMenuId,&wpull);
  XtGetValues (This,args,1);
  if(XWidgetIsValid(wpull)==False) return;
/* set history to null */
  XtSetArg    (args[0],XmNmenuHistory,NULL);
  XtSetValues (This,args,1);
/* kill existing pull children */
 {WidgetList items,itema;
  items = XWidgetGetChildren (wpull,NULL);
  for(itema=items;(itema!=NULL) && (*itema!=NULL);itema++) XtDestroyWidget (*itema);
  CListDelete((CList)items);}
/* if no option given create a "blank" push button */
  if((a_number<=0) || (a_text==NULL) )
    { 
      char*                 name;
      cps                   = XmStringLtoRCreate(" ",XmSTRING_DEFAULT_CHARSET);
      XtSetArg              (args[0],XmNlabelString,cps);
      name                  = CStringCreateF(strlen(XtName(This))+2,"%s_1",XtName(This));
      button                = XtCreateManagedWidget (name,xmPushButtonWidgetClass,wpull,args,1);
      CStringDelete         (name);
      XmStringFree          (cps);
      XtSetArg              (args[0],XmNmenuHistory,button);
      XtSetValues           (This,args,1);
      return;
    }
/* creaet new pull children */
  whistory  = NULL;
  for(count=0;count<a_number;count++)
    { 
      char*                 name;
      cps                   = XmStringLtoRCreate(a_text[count],XmSTRING_DEFAULT_CHARSET);
      XtSetArg              (args[0],XmNlabelString,cps);
      name                  = CStringCreateF(strlen(XtName(This))+32,"%s_%d",XtName(This),count+1);
      button                = XtCreateManagedWidget (name,xmPushButtonWidgetClass,wpull,args,1);
      CStringDelete         (name);
      if(whistory==NULL)    whistory = button;  /* history = first option */
      XmStringFree          (cps);
      if( (a_default!=NULL)     && 
	  (a_text[count]!=NULL) && 
	  (strcmp(a_default,a_text[count])==0) ) whistory = button;
    }
  if(whistory==NULL) return;
/* set history */
  XtSetArg       (args[0],XmNmenuHistory,whistory);
  XtSetValues    (This,args,1);
}
/***************************************************************************/
void XmOptionSetOption (
 Widget This 
,char* a_string  
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget        wpull;
  WidgetList    items,itema;
  Arg           args[1];
  unsigned char value;
/*.........................................................................*/
  if(This==NULL)     return;
/* check we have an option menu */
  if(!XtIsSubclass(This,xmRowColumnWidgetClass)) return;
  XtSetArg    (args[0],XmNrowColumnType,&value);
  XtGetValues (This,args,1);
  if(value!=XmMENU_OPTION) return;
/* get pulldown */
  XtSetArg    (args[0],XmNsubMenuId,&wpull);
  XtGetValues (This,args,1);
  if(XWidgetIsValid(wpull)==False) return;
/* get option items */
  items = XWidgetGetChildren (wpull,NULL);
  for(itema=items;(itema!=NULL) && (*itema!=NULL);itema++)
    { 
      XmString       cps = NULL;
      char*          string;
      XtSetArg       (args[0],XmNlabelString,&cps);
      XtGetValues    (*itema,args,1);
      string         = XmConvertCompoundStringToString (cps,0);
      XmStringFree   (cps);
      if( (string!=NULL) && (a_string!=NULL) && (strcmp(string,a_string)==0) )
	{
	  XtSetArg       (args[0],XmNmenuHistory,*itema);
	  XtSetValues    (This,args,1);
	  XtFree         (string);
	  break;
	}
      XtFree (string);
    }
  CListDelete((CList)items);
}
/***************************************************************************/
char* XmOptionGetOption (
 Widget This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget        whistory;
  Arg           args[1];
  unsigned char value;
  XmString      cps = NULL;
  char*         string;
/*.........................................................................*/
  if(This==NULL) return NULL;
/* check we have an option menu */
  if(!XtIsSubclass(This,xmRowColumnWidgetClass)) return NULL;
  XtSetArg     (args[0],XmNrowColumnType,&value);
  XtGetValues  (This,args,1);
  if(value!=XmMENU_OPTION) return NULL;
  XtSetArg     (args[0],XmNmenuHistory,&whistory);
  XtGetValues  (This,args,1);
  if(XWidgetIsValid(whistory)==False) return NULL;
  XtSetArg     (args[0],XmNlabelString,&cps);
  XtGetValues  (whistory,args,1);
  string       = XmConvertCompoundStringToString (cps,0);
  XmStringFree (cps);
  return       string;
}
/***************************************************************************/
void XmOptionVisit (
 Widget This 
,XmVisitWidgetFunction a_proc
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  unsigned char value = 0;
  Widget     wpull;
  Arg        args[1];
  WidgetList items,itema;
/*.........................................................................*/
  if(This==NULL) return;
/* check we have an option menu */
  if(!XtIsSubclass(This,xmRowColumnWidgetClass)) return;
  XtSetArg    (args[0],XmNrowColumnType,&value);
  XtGetValues (This,args,1);
  if(value!=XmMENU_OPTION) return;
/* get pulldown */
  XtSetArg    (args[0],XmNsubMenuId,&wpull);
  XtGetValues (This,args,1);
  if(XWidgetIsValid(wpull)==False) return;
  if(a_proc==NULL) return;
  items = XWidgetGetChildren (wpull,NULL);
  for(itema=items;(itema!=NULL) && (*itema!=NULL);itema++) a_proc (*itema);
  CMemoryFreeBlock (items);
}
/***************************************************************************/
void XmFormBuildGrid (
 Widget This
,char* a_name
,XmCreateWidgetFunction a_proc
,int  a_rows
,int  a_cols
,Boolean a_adapt
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int    toCreate,count,row,col;
  int    number;
  Arg    args[1];
  int    width,height;
  int    top, bottom, left, right;
  int    border = 0;
  char** names;
  int*   tops;
  int*   bottoms;
  int*   lefts;
  int*   rights;
  WidgetList items,itema;
/*.........................................................................*/
  if(This==NULL) return;
  if(!XtIsSubclass(This,xmFormWidgetClass)) 
    {
      CWarnF("XmFormBuildGrid: %s is not an XmForm.\n",XtName(This));
      return;
    }
  if(a_rows<=0)    return;
  if(a_cols<=0)    return;

  toCreate = 1;
  if(a_adapt==True) 
    {
      int rows,cols;
      XmFormGetNumberOfRowsAndColumns (This,&rows,&cols);
      if( (a_rows<=rows) && (a_cols<=cols) ) 
	{
	  toCreate = 0;
	}
    }

  if(toCreate==1) /* kill existing children */
    {
      items = XWidgetGetChildren (This,NULL);
      for(itema=items;(itema!=NULL) && (*itema!=NULL);itema++) XtDestroyWidget (*itema);
      CListDelete((CList)items);
    }

  XtSetArg    (args[0], XmNfractionBase     ,1000);
  XtSetValues (This, args,1);

  number      = a_rows * a_cols;

  height      = (1000 / a_rows) - 2 * border;
  width       = (1000 / a_cols) - 2 * border;

  top    = border;
  bottom = top + height;

  names   = (char**)CMemoryAllocateBlock(number * sizeof(char*));
  tops    = (int*)  CMemoryAllocateBlock(number * sizeof(int));
  bottoms = (int*)  CMemoryAllocateBlock(number * sizeof(int));
  lefts   = (int*)  CMemoryAllocateBlock(number * sizeof(int));
  rights  = (int*)  CMemoryAllocateBlock(number * sizeof(int));
  if( (names==NULL) || (tops==NULL) || (bottoms==NULL) || (lefts==NULL) || (rights==NULL) )
    {
      CTextDelete        (number,names);
      CMemoryFreeBlock   (tops);
      CMemoryFreeBlock   (bottoms);
      CMemoryFreeBlock   (lefts);
      CMemoryFreeBlock   (rights);
      return;
    }

  count  = 0;  
  for (row=0;row<a_rows;row++)
    {
      left  = border;
      right = left + width;
      for (col=0;col<a_cols;col++)
        { 
          char*                name;
          if(count>=number)    return;
	  if(a_name!=NULL)
	    {
	      name = CStringCreateF(strlen(a_name)+16+16+2,"%s_%d_%d",a_name,row+1,col+1);
	    }
	  else if(XtName(This)!=NULL)
	    {
	      name = CStringCreateF(strlen(XtName(This))+16+16+2,"%s_%d_%d",XtName(This),row+1,col+1);
	    }
	  else
	    {
	      name = CStringCreateF(5+16+16+2,"xxxx_%d_%d",row+1,col+1);
	    }
          names[count]         = CStringDuplicate(name);
          CStringDelete        (name);
          tops[count]          = top;
          bottoms[count]       = bottom;
          lefts[count]         = left;
          rights[count]        = right;
          count               ++;
          left                 = right + 2 * border;
          right                = left  + width;
        }
      top     = bottom + 2 * border;
      bottom  = top    + height;
    }

  if(toCreate==1) 
    {
      if (a_proc!=NULL) 
	{
	  for(count=number-1;count>=0;count--)
	    {
	      Widget                   child;
	      child = a_proc(This,names[count],args,0);
	      XmAttachWidgetAtPosition (child , tops[count], bottoms[count], lefts[count], rights[count]);
	    }
	}
    }
  else
    {
      items = XWidgetGetChildren (This,NULL);
      for(itema=items;(itema!=NULL) && (*itema!=NULL);itema++) 
	{
	  Widget child;
          char*  name;
	  int    found;
	  child  = *itema;
	  name   = XtName(child);
	  found  = 0;
	  for(count=number-1;count>=0;count--)
	    {
	      if( (name!=NULL) && (names[count]!=NULL) && (strcmp(name,names[count])==0) )
		{
		  XtManageChild            (child);
		  XmAttachWidgetAtPosition (child , tops[count], bottoms[count], lefts[count], rights[count]);
		  found                    = 1;
		}
	    }
	  if(found==0) XtUnmanageChild (child);
	}
      CListDelete((CList)items);
    }

  CTextDelete        (number,names);
  CMemoryFreeBlock   (tops);
  CMemoryFreeBlock   (bottoms);
  CMemoryFreeBlock   (lefts);
  CMemoryFreeBlock   (rights);
}
/***************************************************************************/
void XmFormGetNumberOfRowsAndColumns (
 Widget This
,int*  a_rows
,int*  a_cols
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int        rows,cols;
  WidgetList items,itema;
/*.........................................................................*/
  if(a_rows!=NULL) *a_rows = 0;
  if(a_cols!=NULL) *a_cols = 0;
  if(This==NULL) return;
  if(!XtIsSubclass(This,xmFormWidgetClass)) 
    {
      CWarnF("XmFormBuildGrid: %s is not an XmForm.\n",XtName(This));
      return;
    }

  rows  = 0;
  cols  = 0;
  items = XWidgetGetChildren (This,NULL);
  for(itema=items;(itema!=NULL) && (*itema!=NULL);itema++) 
    {
      char* name;
      name  = CStringDuplicate(XtName(*itema));
      if( (XtName(*itema)!=NULL) && (name!=NULL) )
	{
	  char* posc;
	  int   row,col,status;
	  posc  = strrchr(name,'_');
	  if(posc!=NULL) 
	    {
	      char* posr;
	      *posc = '\0';
	      posr  = strrchr(name,'_');
	      if(posr!=NULL) 
		{
		  *posr = '\0';
		  row   = (int)CStringConvertToLong(posr+1,&status);
		  if(status==0)  
		    {
		      CWarnF ("XmFormGetNumberOfRowsAndColumns : %s not a number in :\n%s\n",posr+1,XtName(*itema));
		    }
		  else
		    {
		      rows = MAXIMUM(rows,row);
		    }
		  col = (int)CStringConvertToLong(posc+1,&status);
		  if(status==0)  
		    {
		      CWarnF ("XmFormGetNumberOfRowsAndColumns : %s not a number in :\n%s\n",posc+1,XtName(*itema));
		    }
		  else
		    {
		      cols = MAXIMUM(cols,col);
		    }
		}
	    }
	}
      CStringDelete (name);
    }
  CListDelete((CList)items);

  if(a_rows!=NULL) *a_rows = rows;
  if(a_cols!=NULL) *a_cols = cols;
}
/***************************************************************************/
void XmFormAlignChildren (
 Widget This
,int  a_rows
,int  a_cols
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int           count,row,col;
  WidgetList    childs;
  int           nchild;
  int           number;
  Arg           args[1];
  int           width,height;
  int           top, bottom, left, right;
  int           border = 0;
/*.........................................................................*/
  if(This==NULL)                            return;
  if(!XtIsSubclass(This,xmFormWidgetClass)) return;
  if(a_rows<=0) return;
  if(a_cols<=0) return;

  childs = XWidgetGetChildren (This,NULL);
  nchild = CListGetSize((CList)childs);
  if(nchild==0) 
    {
      CListDelete((CList)childs);
      return;
    }

  number = a_rows * a_cols;
  if(nchild>number) 
    {
      ReportError      ("XmFormAlignChildren",CErrorFlow);
      CListDelete ((CList)childs);
      return;
    }

  XtSetArg    (args[0], XmNfractionBase     ,1000);
  XtSetValues (This, args,1);

  height      = (1000 / a_rows) - 2 * border;
  width       = (1000 / a_cols) - 2 * border;

  top    = border;
  bottom = top + height;

  count  = 0;  
  for (row=0;row<a_rows;row++)
    {
      left  = border;
      right = left + width;
      for (col=0;col<a_cols;col++)
        { 
          if(count>=nchild)    
	    {
	      CListDelete((CList)childs);
	      return;
	    }
          XmAttachWidgetAtPosition    (childs[count], top, bottom, left, right);
          count               ++;
          left                 = right + 2 * border;
          right                = left  + width;
        }
      top     = bottom + 2 * border;
      bottom  = top    + height;
    }

  CListDelete((CList)childs);
}
/***************************************************************************/
void XmAttachWidgetAtPosition (
 Widget This 
,int  a_top    
,int  a_bottom 
,int  a_left   
,int  a_right  
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Arg args[13];
  int offset = 2; 
/*.........................................................................*/
  if(This==NULL)           return;
  if(XtParent(This)==NULL) return;
  if(!XtIsSubclass(XtParent(This),xmFormWidgetClass)) return;
  XtSetArg    (args[0]  , XmNresizable        ,True); 
  XtSetArg    (args[1]  , XmNtopAttachment    ,XmATTACH_POSITION); 
  XtSetArg    (args[2]  , XmNbottomAttachment ,XmATTACH_POSITION); 
  XtSetArg    (args[3]  , XmNleftAttachment   ,XmATTACH_POSITION); 
  XtSetArg    (args[4]  , XmNrightAttachment  ,XmATTACH_POSITION); 
  XtSetArg    (args[5]  , XmNtopPosition      ,a_top); 
  XtSetArg    (args[6]  , XmNbottomPosition   ,a_bottom);
  XtSetArg    (args[7]  , XmNleftPosition     ,a_left); 
  XtSetArg    (args[8]  , XmNrightPosition    ,a_right);

  XtSetArg    (args[9]  , XmNtopOffset        ,offset); 
  XtSetArg    (args[10] , XmNbottomOffset     ,offset);
  XtSetArg    (args[11] , XmNleftOffset       ,offset); 
  XtSetArg    (args[12] , XmNrightOffset      ,offset);
  XtSetValues (This,args,13);
}
/***************************************************************************/
void XmMenuPopAside (
 Widget  This
,Widget  a_near
,XEvent* a_event
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Position               x = 0,y = 0;
  Dimension              w,h;
/*.........................................................................*/
  if(This==NULL)               return;
  if(!XtIsSubclass(This,xmRowColumnWidgetClass)) return;
  if(a_near==NULL)             return;
  if(a_event==NULL)            return;
  if(a_event->type!=ButtonPress) return;
  XWidgetGetDimension       (a_near,&w,&h);
  XWidgetGetRootPosition  (a_near,&x,&y);
  a_event->xbutton.x_root = x + w;
  a_event->xbutton.y_root = y;
  XmMenuPosition          (This,(XButtonPressedEvent*)a_event);
  XtManageChild           (This);
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
Widget XmCreateShellText (
 Widget a_parent
,char* a_name
,ArgList a_list
,Cardinal a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int        lname;
  char*      name;
  Widget     shell,form,clear,hide,frame,end,text;
  Arg        args[6];
/*.........................................................................*/
  if(a_parent==NULL)          return NULL;
  if(a_name==NULL)            return NULL;

  lname                  = strlen(a_name);

  name                   = CStringCreateF (lname+5,"%sSHELL",a_name);

  shell                  = XtAppCreateShell   (name,"Wo",topLevelShellWidgetClass,XtDisplay(a_parent),args,0);
  CStringDelete          (name);

  form                   = XmCreateForm   (shell,"SHELL_TEXT_FORM",args,0);
  XtManageChild          (form);

  XtSetArg    (args[0],XmNleftAttachment    ,XmATTACH_FORM);
  XtSetArg    (args[1],XmNrightAttachment   ,XmATTACH_FORM);
  XtSetArg    (args[2],XmNbottomAttachment  ,XmATTACH_FORM);
  end                    = XmCreatePushButton   (form,"SHELL_TEXT_END",args,3);
  XtManageChild          (end);

  XtSetArg    (args[0],XmNleftAttachment    ,XmATTACH_FORM);
  XtSetArg    (args[1],XmNrightAttachment   ,XmATTACH_FORM);
  XtSetArg    (args[2],XmNbottomAttachment  ,XmATTACH_WIDGET);
  XtSetArg    (args[3],XmNbottomWidget      ,end);
  clear                  = XmCreatePushButton   (form,"SHELL_TEXT_CLEAR",args,4);
  XtManageChild          (clear);

  XtSetArg    (args[0],XmNleftAttachment    ,XmATTACH_FORM);
  XtSetArg    (args[1],XmNrightAttachment   ,XmATTACH_FORM);
  XtSetArg    (args[2],XmNtopAttachment     ,XmATTACH_FORM);
  hide                   = XmCreatePushButton   (form,"SHELL_TEXT_HIDE",args,3);
  XtManageChild          (hide);

  XtSetArg    (args[0],XmNleftAttachment    ,XmATTACH_FORM);
  XtSetArg    (args[1],XmNrightAttachment   ,XmATTACH_FORM);
  XtSetArg    (args[2],XmNtopAttachment     ,XmATTACH_WIDGET);
  XtSetArg    (args[3],XmNtopWidget         ,hide);
  XtSetArg    (args[4],XmNbottomAttachment  ,XmATTACH_WIDGET);
  XtSetArg    (args[5],XmNbottomWidget      ,clear);
  frame                  = XmCreateFrame  (form,"SHELL_TEXT_FRAME",args,6);
  XtManageChild          (frame);

  text                   = XmCreateScrolledText (frame,a_name,args,0);
  XtManageChild          (text);

  XtAddCallback          (clear  ,XmNactivateCallback    ,ClearConsoleCallback,(XtPointer)text);
  XtAddCallback          (hide   ,XmNactivateCallback    ,ActivateConsoleCallback,(XtPointer)1);
  XtAddCallback          (end    ,XmNactivateCallback    ,ActivateConsoleCallback,(XtPointer)2);

  a_list      = NULL;
  a_number    = 0;

  return      text;
}
/***************************************************************************/
static void ClearConsoleCallback (
 Widget This 
,XtPointer a_tag
,XtPointer a_data
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  XmTextSetString((Widget)a_tag,"");
  This   = NULL;
  a_data = NULL;
}
/***************************************************************************/
static void ActivateConsoleCallback (
 Widget This 
,XtPointer a_tag
,XtPointer a_data
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if((long)a_tag==1) 
    {
      XWidgetIconify (XWidgetGetShell(This));
    }
  else if((long)a_tag==2)
    {
      if(Class.termEndProc!=NULL) Class.termEndProc(This);
    }
  a_data = NULL;
}
/***************************************************************************/
static void EndConsole (
 Widget This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  XtDestroyApplicationContext  (XtWidgetToApplicationContext(This));
  printf                       ("%s\n",CMemoryGetStatus());
  exit                         (EXIT_SUCCESS);
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
XmGetWidgetFunction XmDialogSetGetFirstShellFunction (
 XmGetWidgetFunction a_proc
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  XmGetWidgetFunction  old;
/*.........................................................................*/
  old                  = Class.firstShellProc;
  Class.firstShellProc = a_proc;
  return               old;
}
/***************************************************************************/
void XmDialogSetLocalLoopFunction (
 XmProcessEventsFunction a_mainLoopProc
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  Class.mainLoopProc = a_mainLoopProc;
}
/***************************************************************************/
char* XmDialogAskForBufferedPassword (
 char* a_prompt
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  static char pass[32];
  char* string = NULL;
/*.........................................................................*/
  sprintf     (pass,"%s","");
  string      = XmDialogAskForPassword (a_prompt);
  if(string!=NULL)  sprintf (pass,string);
  XtFree      (string);
  return      pass;
}
/***************************************************************************/
char* XmDialogAskForPassword (
 char* a_prompt 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Arg      args[10];
  int      reason = NotFound;
  char*    password;
/*.........................................................................*/
  if(Class.passForm==NULL)
    { 
      Widget               parent,ok,cancel;

      parent               = GetParent();
      if(parent==NULL)     return NULL;
      Class.passForm       = XmCreateFormDialog (parent,"xmask_password",args,0);
      if(Class.passForm==NULL)       return NULL;

      Class.passLabel      = XmCreateLabel         (Class.passForm,"xmask_password_label" ,args,0);
      XtManageChild        (Class.passLabel);
      Class.passText       = XmCreateText          (Class.passForm,"xmask_password_text"  ,args,0);
      XtManageChild        (Class.passText);
      ok                   = XmCreatePushButton    (Class.passForm,"xmask_password_ok"    ,args,0);
      XtManageChild        (ok);
      cancel               = XmCreatePushButton    (Class.passForm,"xmask_password_cancel",args,0);
      XtManageChild        (cancel);

      XtSetArg             (args[0],XmNleftAttachment,XmATTACH_FORM);
      XtSetArg             (args[1],XmNrightAttachment,XmATTACH_FORM);
      XtSetValues          (Class.passLabel,args,2);

      XtSetArg             (args[0],XmNtopAttachment  ,XmATTACH_WIDGET);
      XtSetArg             (args[1],XmNtopWidget      ,Class.passLabel);
      XtSetArg             (args[2],XmNrightAttachment,XmATTACH_FORM);
      XtSetArg             (args[3],XmNleftAttachment,XmATTACH_FORM);
      XtSetValues          (Class.passText,args,4);

      XtSetArg             (args[0],XmNtopAttachment  ,XmATTACH_WIDGET);
      XtSetArg             (args[1],XmNtopWidget      ,Class.passText);
      XtSetArg             (args[2],XmNrightAttachment,XmATTACH_POSITION);
      XtSetArg             (args[3],XmNrightPosition  ,50);
      XtSetArg             (args[4],XmNleftAttachment,XmATTACH_FORM);
      XtSetValues          (ok,args,5);

      XtSetArg             (args[0],XmNtopAttachment  ,XmATTACH_WIDGET);
      XtSetArg             (args[1],XmNtopWidget      ,Class.passText);
      XtSetArg             (args[2],XmNleftAttachment ,XmATTACH_WIDGET);
      XtSetArg             (args[3],XmNleftWidget     ,ok);
      XtSetArg             (args[4],XmNrightAttachment,XmATTACH_FORM);
      XtSetValues          (cancel,args,5);

      XmLabelSetLabelString     (ok    ,"ok");
      XmLabelSetLabelString     (cancel,"cancel");

      XtAddCallback        (ok    ,XmNactivateCallback    ,AskCallback,(XtPointer)XmCR_OK);
      XtAddCallback        (cancel,XmNactivateCallback    ,AskCallback,(XtPointer)XmCR_CANCEL);
      XtAddCallback        (Class.passText  ,XmNactivateCallback    ,AskCallback,(XtPointer)XmCR_OK);
      XtAddCallback        (Class.passText  ,XmNmodifyVerifyCallback,PasswordModifyCallback,NULL);

    }

  if(a_prompt!=NULL) XmLabelSetLabelString     (Class.passLabel,a_prompt);

  XtSetArg             (args[0],XmNvalue,"");
  XtSetValues          (Class.passText,args,1);

  XDisplayRingBell     (XtDisplay(Class.passForm),1);

  XtManageChild        (Class.passForm);

  if(Class.password!=NULL) free (Class.password);
  Class.password     = NULL;

  if(Class.mainLoopProc!=NULL) 
    reason = Class.mainLoopProc();
  else 
    ReportError  ("XmDialogAskForPassword",CErrorHandler);

  XtUnmanageChild      (Class.passForm);

  if(reason==XmCR_CANCEL) 
    {
      if(Class.password!=NULL) free (Class.password);
      Class.password     = NULL;
      return NULL;
    }

  password           = (char*)XtNewString(Class.password);

  if(Class.password!=NULL) free (Class.password);
  Class.password     = NULL;

  return             password;
}
/***************************************************************************/
char* XmDialogAskForString (
 char* a_prompt 
,char* a_def 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Arg      args[2];
  XmString cps;
  char*    string;
  int      reason = NotFound;
/*.........................................................................*/
  if(Class.prompt==NULL) 
    { 
      Widget               parent;

      parent               = GetParent();
      if(parent==NULL)     return NULL;
      Class.prompt         = XmCreatePromptDialog (parent,"xmask_prompt",args,0);
      if(Class.prompt==NULL)          return NULL;

      XtAddCallback        (Class.prompt,XmNokCallback    ,AskCallback,NULL);
      XtAddCallback        (Class.prompt,XmNapplyCallback ,AskCallback,NULL);
      XtAddCallback        (Class.prompt,XmNcancelCallback,AskCallback,NULL);

    }
  if(a_prompt!=NULL) 
    {
      XmString             cps_prompt;
      cps_prompt           = XmStringLtoRCreate(a_prompt,XmSTRING_DEFAULT_CHARSET);
      XtSetArg             (args[0],XmNselectionLabelString,cps_prompt);
      XtSetValues          (Class.prompt,args,1);
      XmStringFree         (cps_prompt);
    }
  if(a_def!=NULL)
    {
      XmString             cps_def;
      cps_def              = XmStringLtoRCreate(a_def   ,XmSTRING_DEFAULT_CHARSET);
      XtSetArg             (args[0],XmNtextString          ,cps_def);
      XtSetValues          (Class.prompt,args,1);
      XmStringFree         (cps_def);
    }

  XDisplayRingBell            (XtDisplay(Class.prompt),1);

  XtManageChild        (Class.prompt);

  if(Class.mainLoopProc!=NULL)   
    reason = Class.mainLoopProc();
  else
    ReportError ("XmDialogAskForString",CErrorHandler);

  XtUnmanageChild      (Class.prompt);

  if(reason==XmCR_CANCEL) return NULL;

  XtSetArg             (args[0],XmNtextString ,&cps);
  XtGetValues          (Class.prompt,args,1);

  string               = XmConvertCompoundStringToString (cps,0);

  return               string;
}
/***************************************************************************/
int XmDialogAskForInteger (
 char*  a_prompt 
,int    a_def    
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  static char sdef[64];
  char*  string;
  int    value;
/*.........................................................................*/
  sprintf       (sdef,"%d",a_def);
  string        = XmDialogAskForString (a_prompt,sdef);
  if( (string==NULL) || (sscanf(string,"%d",&value)!=1)
    ) value = a_def;
  XtFree        (string);
  return        value;
}
/***************************************************************************/
double XmDialogAskForDouble (
 char*  a_prompt 
,double a_def  
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  static char sdef[64];
  char*  string;
  double value;
/*.........................................................................*/
  sprintf       (sdef,"%g",a_def);
  string        = XmDialogAskForString (a_prompt,sdef);
  if( (string==NULL) || (sscanf(string,"%le",&value)!=1)
    ) value = a_def;
  XtFree        (string);
  return        value;
}
/***************************************************************************/
Boolean   XmDialogAskForBoolean (
 char* a_prompt 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Arg      args[1];
  int      reason = NotFound;
/*.........................................................................*/
  if(Class.yesno==NULL) 
    { Widget               parent;

      parent                = GetParent();
      if(parent==NULL)      return 0;
      Class.yesno           = XmCreateQuestionDialog (parent,"xmask_yesno",args,0);
      if(Class.yesno==NULL) return 0;

      XtAddCallback        (Class.yesno,XmNokCallback    ,AskCallback,NULL);
      XtAddCallback        (Class.yesno,XmNcancelCallback,AskCallback,NULL);
    }

  if(a_prompt!=NULL)
    {
      XmString cps;
      cps                  = XmStringLtoRCreate(a_prompt,XmSTRING_DEFAULT_CHARSET);
      XtSetArg             (args[0],XmNmessageString,cps);
      XtSetValues          (Class.yesno,args,1);
      XmStringFree         (cps);
    }

  XDisplayRingBell            (XtDisplay(Class.yesno),1);

  XtManageChild        (Class.yesno);

  if(Class.mainLoopProc!=NULL) 
    reason = Class.mainLoopProc();
  else 
    ReportError ("XmDialogAskForBoolean",CErrorHandler);

  XtUnmanageChild      (Class.yesno);

  return               (reason == XmCR_OK ? True : False);
}
/***************************************************************************/
char* XmDialogAskForList (
 char* a_prompt
,int a_number
,char** a_list
,Boolean a_sort 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Arg           args[5];
  char*         string;
  XmString      cps1,cps;
  XmString*     table;
  int           number;
  int           reason = NotFound;
/*.........................................................................*/
  if(a_list==NULL)     return NULL;
  if(a_number<=0)      return NULL;
  if(a_sort==True)     CTextSort(a_number,a_list);

  if(Class.list==NULL) 
    { Widget               parent;

      parent               = GetParent();
      if(parent==NULL)     return NULL;
      Class.list           = XmCreateSelectionDialog (parent,"xmask_list",args,0);
      if(Class.list==NULL) return NULL;

      Class.list_list      = XmSelectionBoxGetChild (Class.list,XmDIALOG_LIST);

      XtAddCallback        (Class.list,XmNokCallback    ,AskCallback,NULL);
      XtAddCallback        (Class.list,XmNapplyCallback ,AskCallback,NULL);
      XtAddCallback        (Class.list,XmNcancelCallback,AskCallback,NULL);
    }

  if(a_prompt!=NULL)
    {
      cps                  = XmStringLtoRCreate (a_prompt,XmSTRING_DEFAULT_CHARSET);
      XtSetArg             (args[0],XmNselectionLabelString ,cps);
      XtSetValues          (Class.list,args,1);
      XmStringFree         (cps);
    }

  table                = XmStringTableCreateFromText (a_number,a_list);
  number               = XmStringTableGetSize        (table);
  cps1                 = XmStringLtoRCreate          ("",XmSTRING_DEFAULT_CHARSET);
  XtSetArg             (args[0],XmNlistItems            ,table);
  XtSetArg             (args[1],XmNlistItemCount        ,number);
  XtSetArg             (args[2],XmNtextString           ,cps1);
  XtSetValues          (Class.list,args,3);
  XmStringTableDelete  (table);
  XmStringFree         (cps1);

  XtSetArg             (args[0],XmNselectedItemCount,0);
  XtSetValues          (Class.list_list,args,1);

  XDisplayRingBell     (XtDisplay(Class.list),1);

  XtManageChild        (Class.list);

  if(Class.mainLoopProc!=NULL) 
    reason = Class.mainLoopProc();
  else
    ReportError ("XmDialogAskForList",CErrorHandler);

  XtUnmanageChild      (Class.list);

  if(reason==XmCR_CANCEL) return NULL;

  XtSetArg             (args[0],XmNtextString ,&cps);
  XtGetValues          (Class.list,args,1);

  string               = XmConvertCompoundStringToString (cps,0);
 
  return               string;
}
/***************************************************************************/
char* XmDialogAskForText (
 char* a_prompt
,char* a_string 
,int a_rows
,int a_cols
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Arg           args[6];
  int           reason = NotFound;
/*.........................................................................*/
  if(Class.text==NULL) 
    { 
      Widget               parent;
      Widget               frame,text_ok,text_cancel;

      parent               = GetParent();
      if(parent==NULL)     return NULL;
      Class.text           = XmCreateFormDialog (parent,"xmask_text",args,0);
      if(Class.text==NULL) return NULL;

      Class.text_label     = XmCreateLabel        (Class.text,"xmask_text_label" ,args,0);
      frame                = XmCreateFrame        (Class.text,"xmask_text_frame" ,args,0);
      text_cancel          = XmCreatePushButton   (Class.text,"xmask_text_cancel",args,0);
      text_ok              = XmCreatePushButton   (Class.text,"xmask_text_ok"    ,args,0);

      XtSetArg             (args[0],XmNtopAttachment  ,XmATTACH_FORM);
      XtSetArg             (args[1],XmNleftAttachment ,XmATTACH_FORM);
      XtSetArg             (args[2],XmNrightAttachment,XmATTACH_FORM);
      XtSetValues          (Class.text_label,args,3);

      XtSetArg             (args[0],XmNbottomAttachment ,XmATTACH_FORM);
      XtSetArg             (args[1],XmNleftAttachment   ,XmATTACH_FORM);
      XtSetArg             (args[2],XmNrightAttachment  ,XmATTACH_POSITION);
      XtSetArg             (args[3],XmNrightPosition    ,50);
      XtSetValues          (text_ok,args,4);

      XtSetArg             (args[0],XmNbottomAttachment ,XmATTACH_FORM);
      XtSetArg             (args[1],XmNleftAttachment   ,XmATTACH_POSITION);
      XtSetArg             (args[2],XmNleftPosition     ,50);
      XtSetArg             (args[3],XmNrightAttachment  ,XmATTACH_FORM);
      XtSetValues          (text_cancel,args,4);

      XtSetArg             (args[0],XmNtopAttachment    ,XmATTACH_WIDGET);
      XtSetArg             (args[1],XmNtopWidget        ,Class.text_label);
      XtSetArg             (args[2],XmNbottomAttachment ,XmATTACH_WIDGET);
      XtSetArg             (args[3],XmNbottomWidget     ,text_ok);
      XtSetArg             (args[4],XmNleftAttachment   ,XmATTACH_FORM);
      XtSetArg             (args[5],XmNrightAttachment  ,XmATTACH_FORM);
      XtSetValues          (frame,args,6);

      Class.text_text      = XmCreateScrolledText (frame      ,"xmask_text_text"  ,args,0);

      XtManageChild        (Class.text_label);
      XtManageChild        (frame);
      XtManageChild        (text_cancel);
      XtManageChild        (text_ok);
      XtManageChild        (Class.text_text);

      XtAddCallback        (text_cancel,XmNactivateCallback ,AskCallback,(XtPointer)XmCR_CANCEL);
      XtAddCallback        (text_ok    ,XmNactivateCallback ,AskCallback,(XtPointer)XmCR_OK);
    }

  if( (a_rows!=0) && (a_cols!=0) )
    {
      XtSetArg             (args[0],XmNrows   ,a_rows);
      XtSetArg             (args[1],XmNcolumns,a_cols);
      XtSetValues          (Class.text_text,args,2);
    }

  if(a_prompt!=NULL) 
    {
      XmString             cps;
      cps                  = XmStringLtoRCreate   (a_prompt,XmSTRING_DEFAULT_CHARSET);
      XtSetArg             (args[0],XmNlabelString,cps);
      XtSetValues          (Class.text_label,args,1);
      XmStringFree         (cps);
    }

  if(a_string!=NULL)    XmTextSetString (Class.text_text,a_string);

  XDisplayRingBell     (XtDisplay(Class.text),1);

  XtManageChild        (Class.text);

  if(Class.mainLoopProc!=NULL) 
    reason = Class.mainLoopProc();
  else
    ReportError ("XmDialogAskForText",CErrorHandler);

  XtUnmanageChild      (Class.text);

  if(reason==XmCR_CANCEL) return NULL;

  return               XmTextGetString (Class.text_text);
}
/***************************************************************************/
char* XmDialogAskForFile (
 char* a_prompt
,char* a_mask
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int           reason = NotFound;
  Arg           args[6];
/*.........................................................................*/
  if(Class.file==NULL) 
    { Widget               parent;

      parent               = GetParent();
      if(parent==NULL)     return NULL;
      Class.file           = XmCreateFileSelectionDialog (parent,"xmask_file",args,0);
      if(Class.file==NULL) return NULL;

      Class.file_text      = XmFileSelectionBoxGetChild (Class.file,XmDIALOG_TEXT);
      Class.file_label     = XmFileSelectionBoxGetChild (Class.file,XmDIALOG_SELECTION_LABEL);

      XtAddCallback        (Class.file,XmNokCallback    ,AskCallback,NULL);
      XtAddCallback        (Class.file,XmNcancelCallback,AskCallback,NULL);
    }

  if(a_prompt!=NULL)
    {
      XmString             cps;
      cps                  = XmStringLtoRCreate   (a_prompt,XmSTRING_DEFAULT_CHARSET);
      XtSetArg             (args[0],XmNlabelString,cps);
      XtSetValues          (Class.file_label,args,1);
      XmStringFree         (cps);
    }

  if(a_mask!=NULL)
    {
      XmString             cps;
      cps                  = XmStringLtoRCreate   (a_mask,XmSTRING_DEFAULT_CHARSET);
      XtSetArg             (args[0],XmNdirMask,cps);
      XtSetValues          (Class.file,args,1);
      XmStringFree         (cps);
    }

  XDisplayRingBell     (XtDisplay(Class.file),1);

  XtManageChild        (Class.file);

  if(Class.mainLoopProc!=NULL) 
    reason = Class.mainLoopProc();
  else
    ReportError ("XmDialogAskForFile",CErrorHandler);

  XtUnmanageChild      (Class.file);

  if(reason==XmCR_CANCEL) return NULL;

  return               XmTextGetString (Class.file_text);
}
/***************************************************************************/
void XmDialogWarn (
 char* a_string
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
#define MAX_STR    2048
  Arg        args[1];
/*.........................................................................*/
  if(Class.caution==NULL) 
    { Widget               parent;

      parent                  = GetParent();
      if(parent==NULL)        return;
      Class.caution           = XmCreateWarningDialog (parent,"xmask_caution",args,0);
      if(Class.caution==NULL) return;

      XtAddCallback        (Class.caution,XmNokCallback    ,AskCallback,NULL);
      XtAddCallback        (Class.caution,XmNcancelCallback,AskCallback,NULL);
    }

  if(a_string!=NULL) 
    {
      XmString     cps;
      cps          = XmStringLtoRCreate(a_string,XmSTRING_DEFAULT_CHARSET);
      XtSetArg     (args[0],XmNmessageString,cps);
      XtSetValues  (Class.caution,args,1);
      XmStringFree (cps);
    }

  XDisplayRingBell     (XtDisplay(Class.caution),1);

  XtManageChild        (Class.caution);

  if(Class.mainLoopProc!=NULL) 
    Class.mainLoopProc();
  else
    ReportError ("XmDialogWarn",CErrorHandler);

  XtUnmanageChild      (Class.caution);

}
/***************************************************************************/
static Widget GetParent (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget parent;
/*.........................................................................*/
  parent = Class.firstShellProc!=NULL ? Class.firstShellProc() : XWidgetGetTop();
  if(parent==NULL)
    {
      CWarn ("XmTools : can't get a parent widget.\n");
      return NULL;
    }
  Class.askContext = XtWidgetToApplicationContext(parent);
  return parent;
}
/***************************************************************************/
static void AskCallback (
 Widget This 
,XtPointer a_tag
,XtPointer a_data
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  long reason = 0L;
/*.........................................................................*/
  if(a_tag!=NULL) 
    {
      reason = (long)a_tag;
    }
  else if(a_data!=NULL)
    {
      reason = ((XmAnyCallbackStruct *)a_data)->reason;
    }
  XWidgetSendMessage (This,XMSG_CALLBACK,(long)reason);
  This = NULL;
}
/***************************************************************************/
static int TreatEvents (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.askContext==NULL) return 0;
  while(1)
    { 
      XEvent          xevent;
      XtAppNextEvent  (Class.askContext,&xevent);

      XtDispatchEvent (&xevent);        

      if ( (xevent.xany.type==ClientMessage) && 
           (xevent.xclient.data.l[0]==XMSG_CALLBACK)
         )
        return xevent.xclient.data.l[1];

    }
}
/***************************************************************************/
static void PasswordModifyCallback (
 Widget w
,XtPointer client_data
,XtPointer call_data
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  XmTextVerifyCallbackStruct *tv =(XmTextVerifyCallbackStruct *)call_data;
  int i, len;
  Boolean ModifyIgnore = False;
/*.........................................................................*/
	/*
	 * by default accept nothing
	tv->doit = False;
	 */

	/*
	 * Ignore when ModifyIgnore is true
	 */
	if (ModifyIgnore == True)
	{
		return;
	}

	/*
	 * only accept text modification of password fields
	 */
	if (tv->reason != XmCR_MODIFYING_TEXT_VALUE)
	{
		return;
	}

	/*
	 * Deletion.
	 */
	if (tv->text->ptr == NULL)
	{
		tv->doit = True;

		/*
		 * Only can delete if we have stuff to delete.
		 */
		if ((Class.password != NULL)&&(Class.password[0] != '\0'))
		{
			int start;
			char *tptr;

			len = strlen(Class.password);
			/*
			 * Find the start of the chunk of text to
			 * delete.
			 */
			if (tv->startPos < len)
			{
				start = (int)tv->startPos;
			}
			else
			{
				start = len - 1;
			}

			/*
			 * might be more stuff after the end that we
			 * want to move up
			 */
			if (tv->endPos > len)
			{
				tptr = &(Class.password[len]);
			}
			else
			{
				tptr = &(Class.password[tv->endPos]);
			}
			Class.password[start] = '\0';
			strcat(Class.password, tptr);
		}
	}
	/*
	 * Else insert character.
	 */
	else if (tv->text->length >= 1)
	{
		int maxlength, plen;
		Cardinal argcnt;
		Arg arg[5];

		/*
		 * No insertion if it makes you exceed maxLength
		 */
		if (Class.password == NULL)
		{
			plen = 0;
		}
		else
		{
			plen = strlen(Class.password);
		}
		maxlength = 1000000;
		argcnt = 0;
		XtSetArg(arg[argcnt], XmNmaxLength, &maxlength); argcnt++;
		XtGetValues(w, arg, argcnt);
		if ((plen + tv->text->length) > maxlength)
		{
			return;
		}

		if (Class.password == NULL)
		{
			Class.password = (char *)malloc(tv->text->length + 1);
			for (i=0; i < tv->text->length; i++)
			{
				Class.password[i] = tv->text->ptr[i];
			}
			Class.password[tv->text->length] = '\0';
		}
		/*
		 * else insert a char somewhere.
		 * Make a new buffer.  Put everything from before the insert
		 * postion into it.  Now insert the character.
		 * Finally append any remaining text.
		 */
		else
		{
			char *buf;
			char *tptr;
			char tchar;
			int start;

			len = strlen(Class.password);
			if (tv->startPos < len)
			{
				start =(int) tv->startPos;
			}
			else
			{
				start = len;
			}
			tptr = &(Class.password[start]);
			tchar = *tptr;
			*tptr = '\0';
			buf = (char *)malloc(len + tv->text->length + 1);
			buf [len + tv->text->length] = '\0';
			strcpy(buf, Class.password);
			for (i=0; i < tv->text->length; i++)
			{
				buf[start + i] = tv->text->ptr[i];
			}
			buf[start + tv->text->length] = '\0';
			*tptr = tchar;
			strcat(buf, tptr);
			free           (Class.password);
			Class.password = buf;
		}

		tv->doit = True;
		/*
		 * make a '*' show up instead of what they typed
		 */
		for (i=0; i < tv->text->length; i++)
		{
			tv->text->ptr[i] = '*';
		}
	}
  client_data = NULL;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
void XmSetBalloonShape (
 XmBalloonShape a_shape
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  Class.ballon_shape = a_shape;
}
/***************************************************************************/
void XmPopBalloon (
 Widget This
,char* a_string
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Arg        args[4];
  Position   xn,yn; 
  Position   xs,ys; 
  Dimension  wn,hn;
  Dimension  wb,hb;
  Dimension  ws,hs;
  Uint       wd,hd;
/*.........................................................................*/
  if(This==NULL)       return;  
  if(a_string==NULL)   return;  
  if(XDisplayHasExtension(XtDisplay(This),"SHAPE")==0)
    {
      CPrinterPutF ("%s\n",a_string);
      return;
    }

  if(Class.ballonLabel==NULL)
    { 
      Widget               parent;
      parent               = XWidgetGetTop();
      if(parent==NULL)     return;
/*name xmask_ballon appears in OWidget.c */
      XtSetArg                     (args[0],XtNgeometry,"100x100+0+0");
      Class.ballonShell            = XtCreatePopupShell    ("xmask_ballon",overrideShellWidgetClass,parent,args,1);
      if(Class.ballonShell==NULL)  return;
      Class.ballonShape            = XmCreateBulletinBoard (Class.ballonShell,"xmask_ballon_shape",args,0);
      if(Class.ballonShape==NULL)  return;
      XtSetArg                     (args[0],"marginWidth"       ,0);
      XtSetArg                     (args[1],"marginHeight"      ,0);
      XtSetArg                     (args[2],"highlightThickness",0);
      XtSetArg                     (args[3],"shadowThickness"   ,0);
      Class.ballonLabel            = XmCreateLabel (Class.ballonShape,"xmask_ballon_label",args,4);
      if(Class.ballonLabel==NULL)  return;

      XtManageChild        (Class.ballonShell);
      XtManageChild        (Class.ballonShape);
      XtManageChild        (Class.ballonLabel);

    }

  XtUnmanageChild        (Class.ballonLabel);
  XmLabelSetLabelString  (Class.ballonLabel,a_string);
  XWidgetGetDimension    (Class.ballonLabel,&wb,&hb);
  XtManageChild          (Class.ballonLabel);

  XWidgetGetDimension    (This,&wn,&hn);

  if(Class.ballon_shape==XmBalloonShapeOval)
    {
      if(wb>=hb)
        {
          hs = (Dimension) (hb * 1.2);
          ws = (Dimension) (((double)wb) / sqrt (1. - ((double)hb * (double)hb) /((double)hs * (double)hs)));
        }
      else
        {
          ws = (Dimension) (wb * 1.2);
          hs = (Dimension) (((double)hb) / sqrt (1. - ((double)wb * (double)wb) /((double)ws * (double)ws)));
        }
      XWidgetSetDimension   (Class.ballonShell,ws,hs);
      XWidgetSetDimension   (Class.ballonShape,ws,hs);
      XWidgetMove           (Class.ballonLabel,(Position)(((Position)ws-(Position)wb)/2),
			                       (Position)(((Position)hs-(Position)hb)*0.4));
    }
  else
    {
      ws                     = wb;
      hs                     = (Dimension)(hb * 1.2);
      XWidgetSetDimension   (Class.ballonShell,ws,hs);
      XWidgetSetDimension   (Class.ballonShape,ws,hs);
      XWidgetMove           (Class.ballonLabel,0, 0);
    }

  xn                    = 0;
  yn                    = 0;
  XWidgetGetRootPosition (This,&xn,&yn);

  XDisplayGetSize              (XtDisplay(Class.ballonShell),&wd,&hd);
  if(xn+wn+ws>=(Position)wd)
    {
      xs                     = wd - ws;
      ys                     = yn - hs;
    }
  else 
    {
      xs                     = xn + wn;
      ys                     = yn - hs;
    }

  if(Class.ballon_shape==XmBalloonShapeOval)
    {
      XWidgetSetShape       (Class.ballonShape,XShapeBallonArcXY,(int)(xn+wn/2-xs),(int)(hs+hn/2));
    }
  else
    {
      XWidgetSetShape       (Class.ballonShape,XShapeBallonRectXY,(int)(xn+wn/2-xs),(int)(hs+hn/2));
    }

  XWidgetMove           (Class.ballonShell,xs,ys);

  XWidgetRaise          (Class.ballonShell);
  XWidgetMap            (Class.ballonShell);

}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
void XmConsoleSetExitFunction (
 XmVisitWidgetFunction a_proc
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  Class.termEndProc = a_proc;
}
/***************************************************************************/
void XmConsolePrintF (
 char*   a_format 
,...
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  va_list       args;
/*.........................................................................*/
  if(a_format==NULL) return;
  va_start        (args,a_format);
  XmConsolePutV   (1024,a_format,args);
  va_end          (args);
}
/***************************************************************************/
void XmConsolePrintV (
 char*   a_format 
,va_list a_args
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  XmConsolePutV   (1024,a_format,a_args);
}
/***************************************************************************/
void XmConsolePutF (
 int     a_number
,char*   a_format 
,...
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  va_list       args;
/*.........................................................................*/
  if(a_format==NULL) return;
  va_start        (args,a_format);
  XmConsolePutV   (a_number,a_format,args);
  va_end          (args);
}
/***************************************************************************/
void XmConsolePutV (
 int     a_number
,char*   a_format 
,va_list a_args
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*         string;
/*.........................................................................*/
  string         = CStringCreateV(a_number,a_format,a_args);
  XmConsolePrint (string);
  CStringDelete  (string);
}
/***************************************************************************/
void XmConsolePrint (
 char*   a_string
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_string==NULL)     return;
  if(Class.xmterm_shell==NULL)
    { 
      Widget parent;
      parent = XWidgetGetTop();
      if(parent!=NULL)
	{
	  Arg                args[1];
	  Class.xmterm       = XmCreateShellText (parent,"xmterm",args,0);
	  Class.xmterm_shell = XWidgetGetShell   (Class.xmterm);
	  if(Class.xmterm!=NULL)
	    {
	      XtRealizeWidget                     (Class.xmterm_shell);
	      XWidgetSetWindowManagerCloseFunction (Class.xmterm_shell,NULL);
	    }
	}
    }
  if( (Class.xmterm_shell!=NULL) && (Class.xmterm!=NULL) )
    {
      XmTextAppendString (Class.xmterm,a_string);
      XWidgetRaise       (Class.xmterm_shell);
      XWidgetMap         (Class.xmterm_shell);
    }
  else
    {
      printf ("%s",a_string);
    }
}
/***************************************************************************/
void XmConsoleDestroy (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.xmterm_shell!=NULL)    
    {
      XtDestroyWidget       (Class.xmterm_shell);
      Class.xmterm_shell   = NULL;
      Class.xmterm         = NULL;
    }
}
/***************************************************************************/
Widget XmConsoleGetTextWidget (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return Class.xmterm;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static void ConvertUnknown (
 XrmValuePtr a_args 
,Cardinal* a_argn
,XrmValuePtr a_from
,XrmValuePtr a_to
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  a_to->size = 0;
  a_to->addr = NULL;
  a_from     = NULL; /* to remove warning of c++ compiler */
  a_args     = NULL;  
  a_argn     = 0;
}
/***************************************************************************/
static void ConvertXmRCharXtRString (
 XrmValuePtr a_args 
,Cardinal* a_argn
,XrmValuePtr a_from
,XrmValuePtr a_to
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  sprintf    (Class.string,"%d",AXARGVAL(char));
  a_to->size = (unsigned int)(strlen(Class.string)+1);             
  a_to->addr = (XPointer)Class.string;      
  a_argn = 0;
  a_args  = NULL; 
}
/***************************************************************************/
static void ConvertXmRXmStringXtRString (
 XrmValuePtr a_args 
,Cardinal* a_argn
,XrmValuePtr a_from
,XrmValuePtr a_to
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* string;
/*.........................................................................*/
  string     = XmConvertCompoundStringToString (AXARGVAL(XmString),0);
  sprintf    (Class.string,"%s","");
  if(string!=NULL) 
    {
      if(strlen(string)>MAX_STRING-1) 
        {
          ReportError ("ConvertXmRXmStringXtRString",CErrorFlow);
          string[MAX_STRING-1] = 0;
        }  
      strcpy       (Class.string,string);
    }                           
  a_to->size = (unsigned int)(strlen(Class.string)+1);             
  a_to->addr = (XPointer)Class.string;      
  XtFree     (string);
  a_argn = 0;
  a_args  = NULL; 
}
/***************************************************************************/
static void ConvertXmRAlignmentXtRString (
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
  sprintf(Class.string,"%s",""); 
       if(from==XmALIGNMENT_BEGINNING) sprintf    (Class.string,"alignment_beginning");
  else if(from==XmALIGNMENT_CENTER)    sprintf    (Class.string,"alignment_center");
  else if(from==XmALIGNMENT_END)       sprintf    (Class.string,"alignment_end");
  a_to->size = (unsigned int)(strlen(Class.string)+1);  
  a_to->addr = (XPointer)Class.string;      
  a_argn = 0;
  a_args  = NULL; 
}
/***************************************************************************/
static void ConvertXmRArrowDirectionXtRString (
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
  sprintf(Class.string,"%s",""); 
       if(from==XmARROW_UP)        sprintf    (Class.string,"arrow_up");
  else if(from==XmARROW_DOWN)      sprintf    (Class.string,"arrow_down");
  else if(from==XmARROW_LEFT)      sprintf    (Class.string,"arrow_left");
  else if(from==XmARROW_RIGHT)     sprintf    (Class.string,"arrow_right");
  a_to->size = (unsigned int)(strlen(Class.string)+1);  
  a_to->addr = (XPointer)Class.string;      
  a_argn = 0;
  a_args  = NULL; 
}
/***************************************************************************/
static void ConvertXtRStringXmRAttachment (
 XrmValuePtr a_args 
,Cardinal* a_argn
,XrmValuePtr a_from
,XrmValuePtr a_to
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  static unsigned char value;
/*.........................................................................*/
  if(a_from->addr==NULL) return;
       if(strcmp(a_from->addr,"attach_none")==0)             value = XmATTACH_NONE;
  else if(strcmp(a_from->addr,"attach_form")==0)             value = XmATTACH_FORM;
  else if(strcmp(a_from->addr,"attach_opposite_form")==0)    value = XmATTACH_OPPOSITE_FORM;
  else if(strcmp(a_from->addr,"attach_widget")==0)           value = XmATTACH_WIDGET;
  else if(strcmp(a_from->addr,"attach_opposite_widget")==0)  value = XmATTACH_OPPOSITE_WIDGET;
  else if(strcmp(a_from->addr,"attach_position")==0)         value = XmATTACH_POSITION;
  else if(strcmp(a_from->addr,"attach_self")==0)             value = XmATTACH_SELF;
  else           
    {
      a_to->size = 0;
      a_to->addr = NULL;
      return;
    }
  a_to->size = (unsigned int)(sizeof(unsigned char));  
  a_to->addr = (XPointer)&value;      
  a_argn = 0;
  a_args = NULL; 
}
/***************************************************************************/
static void ConvertXmRAttachmentXtRString (
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
  sprintf(Class.string,"%s",""); 
       if(from==XmATTACH_NONE)            sprintf    (Class.string,"attach_none");
  else if(from==XmATTACH_FORM)            sprintf    (Class.string,"attach_form");
  else if(from==XmATTACH_OPPOSITE_FORM)   sprintf    (Class.string,"attach_opposite_form");
  else if(from==XmATTACH_WIDGET)          sprintf    (Class.string,"attach_widget");
  else if(from==XmATTACH_OPPOSITE_WIDGET) sprintf    (Class.string,"attach_opposite_widget");
  else if(from==XmATTACH_POSITION)        sprintf    (Class.string,"attach_position");
  else if(from==XmATTACH_SELF)            sprintf    (Class.string,"attach_self");
  a_to->size = (unsigned int)(strlen(Class.string)+1);  
  a_to->addr = (XPointer)Class.string;      
  a_argn = 0;
  a_args = NULL; 
}
/***************************************************************************/
static void ConvertXmRDefButtonTypeXtRString (
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
  sprintf(Class.string,"%s",""); 
       if(from==XmDIALOG_OK_BUTTON)        sprintf    (Class.string,"dialog_ok_button");
  else if(from==XmDIALOG_CANCEL_BUTTON)    sprintf    (Class.string,"dialog_cancel_button");
  else if(from==XmDIALOG_HELP_BUTTON)      sprintf    (Class.string,"dialog_help_button");
  a_to->size = (unsigned int)(strlen(Class.string)+1);  
  a_to->addr = (XPointer)Class.string;      
  a_argn = 0;
  a_args = NULL; 
}
/***************************************************************************/
static void ConvertXmRDialogStyleXtRString (
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
  sprintf(Class.string,"%s",""); 
       if(from==XmDIALOG_WORK_AREA)         sprintf    (Class.string,"dialog_work_area");
  else if(from==XmDIALOG_MODELESS)          sprintf    (Class.string,"dialog_modeless");
  else if(from==XmDIALOG_APPLICATION_MODAL) sprintf    (Class.string,"dialog_application_modal");
  else if(from==XmDIALOG_SYSTEM_MODAL)      sprintf    (Class.string,"dialog_system_modal");
  a_to->size = (unsigned int)(strlen(Class.string)+1);  
  a_to->addr = (XPointer)Class.string;      
  a_argn = 0;
  a_args = NULL; 
}
/***************************************************************************/
static void ConvertXmRDialogTypeXtRString (
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
  sprintf(Class.string,"%s",""); 
       if(from==XmDIALOG_ERROR)        sprintf    (Class.string,"dialog_error");
  else if(from==XmDIALOG_INFORMATION)  sprintf    (Class.string,"dialog_information");
  else if(from==XmDIALOG_MESSAGE)      sprintf    (Class.string,"dialog_message");
  else if(from==XmDIALOG_QUESTION)     sprintf    (Class.string,"dialog_question");
  else if(from==XmDIALOG_WARNING)      sprintf    (Class.string,"dialog_warning");
  else if(from==XmDIALOG_WORKING)      sprintf    (Class.string,"dialog_working");
  else if(from==XmDIALOG_PROMPT)       sprintf    (Class.string,"dialog_prompt");
  else if(from==XmDIALOG_SELECTION)    sprintf    (Class.string,"dialog_selection");
  else if(from==XmDIALOG_COMMAND)      sprintf    (Class.string,"dialog_command");
  a_to->size = (unsigned int)(strlen(Class.string)+1);  
  a_to->addr = (XPointer)Class.string;      
  a_argn = 0;
  a_args = NULL; 
}
/***************************************************************************/
static void ConvertXmRIndicatorTypeXtRString (
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
  sprintf(Class.string,"%s",""); 
       if(from==XmN_OF_MANY)        sprintf    (Class.string,"n_of_many");
  else if(from==XmONE_OF_MANY)      sprintf    (Class.string,"one_of_many");
  a_to->size = (unsigned int)(strlen(Class.string)+1);  
  a_to->addr = (XPointer)Class.string;      
  a_argn = 0;
  a_args = NULL; 
}
/***************************************************************************/
static void ConvertXmRLabelTypeXtRString (
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
  sprintf(Class.string,"%s",""); 
       if(from==XmSTRING)      sprintf    (Class.string,"string");
  else if(from==XmPIXMAP)      sprintf    (Class.string,"pixmap");
  a_to->size = (unsigned int)(strlen(Class.string)+1);  
  a_to->addr = (XPointer)Class.string;      
  a_argn = 0;
  a_args = NULL; 
}
/***************************************************************************/
static void ConvertXmRListSizePolicyXtRString (
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
  sprintf(Class.string,"%s",""); 
       if(from==XmCONSTANT)           sprintf    (Class.string,"constant");
  else if(from==XmVARIABLE)           sprintf    (Class.string,"variable");
  else if(from==XmRESIZE_IF_POSSIBLE) sprintf    (Class.string,"resize_if_possible");
  a_to->size = (unsigned int)(strlen(Class.string)+1);  
  a_to->addr = (XPointer)Class.string;      
  a_argn = 0;
  a_args = NULL; 
}
/***************************************************************************/
static void ConvertXmRPackingXtRString (
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
  sprintf(Class.string,"%s",""); 
       if(from==XmPACK_NONE)       sprintf    (Class.string,"pack_none");
  else if(from==XmPACK_TIGHT)      sprintf    (Class.string,"pack_tight");
  else if(from==XmPACK_COLUMN)     sprintf    (Class.string,"pack_column");
  a_to->size = (unsigned int)(strlen(Class.string)+1);  
  a_to->addr = (XPointer)Class.string;      
  a_argn = 0;
  a_args = NULL; 
}
/***************************************************************************/
static void ConvertXmRRowColumnTypeXtRString (
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
  sprintf(Class.string,"%s",""); 
       if(from==XmWORK_AREA)       sprintf    (Class.string,"work_area");
  else if(from==XmMENU_BAR)        sprintf    (Class.string,"menu_bar");
  else if(from==XmMENU_POPUP)      sprintf    (Class.string,"menu_popup");
  else if(from==XmMENU_PULLDOWN)   sprintf    (Class.string,"menu_pulldown");
  else if(from==XmMENU_OPTION)     sprintf    (Class.string,"menu_option");
  a_to->size = (unsigned int)(strlen(Class.string)+1);  
  a_to->addr = (XPointer)Class.string;      
  a_argn = 0;
  a_args = NULL; 
}
/***************************************************************************/
static void ConvertXmRProcDirectionXtRString (
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
  sprintf(Class.string,"%s",""); 
       if(from==XmMAX_ON_TOP)       sprintf    (Class.string,"max_on_top");
  else if(from==XmMAX_ON_BOTTOM)    sprintf    (Class.string,"max_on_bottom");
  else if(from==XmMAX_ON_LEFT)      sprintf    (Class.string,"max_on_left");
  else if(from==XmMAX_ON_RIGHT)     sprintf    (Class.string,"max_on_right");
  a_to->size = (unsigned int)(strlen(Class.string)+1);  
  a_to->addr = (XPointer)Class.string;      
  a_argn = 0;
  a_args = NULL; 
}
/***************************************************************************/
static void ConvertXmRResizePolicyXtRString (
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
  sprintf(Class.string,"%s",""); 
       if(from==XmRESIZE_NONE)      sprintf    (Class.string,"resize_none");
  else if(from==XmRESIZE_GROW)      sprintf    (Class.string,"resize_grow");
  else if(from==XmRESIZE_ANY)       sprintf    (Class.string,"resize_any");
  a_to->size = (unsigned int)(strlen(Class.string)+1);  
  a_to->addr = (XPointer)Class.string;      
  a_argn = 0;
  a_args = NULL; 
}
/***************************************************************************/
static void ConvertXmRSelectPolicyXtRString (
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
  sprintf(Class.string,"%s",""); 
       if(from==XmSINGLE_SELECT)      sprintf    (Class.string,"single_select");
  else if(from==XmMULTIPLE_SELECT)    sprintf    (Class.string,"multiple_select");
  else if(from==XmEXTENDED_SELECT)    sprintf    (Class.string,"extended_select");
  else if(from==XmBROWSE_SELECT)      sprintf    (Class.string,"browse_select");
  a_to->size = (unsigned int)(strlen(Class.string)+1);  
  a_to->addr = (XPointer)Class.string;      
  a_argn = 0;
  a_args = NULL; 
}
/***************************************************************************/
static void ConvertXmRSeparatorTypeXtRString (
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
  sprintf(Class.string,"%s",""); 
       if(from==XmNO_LINE)            sprintf    (Class.string,"no_line");
  else if(from==XmSINGLE_LINE)        sprintf    (Class.string,"single_line");
  else if(from==XmDOUBLE_LINE)        sprintf    (Class.string,"double_line");
  else if(from==XmSINGLE_DASHED_LINE) sprintf    (Class.string,"single_dashed_line");
  else if(from==XmDOUBLE_DASHED_LINE) sprintf    (Class.string,"double_dashed_line");
  else if(from==XmSHADOW_ETCHED_OUT)  sprintf    (Class.string,"shadow_etched_out");
  else if(from==XmSHADOW_ETCHED_IN)   sprintf    (Class.string,"shadow_etched_in");
  a_to->size = (unsigned int)(strlen(Class.string)+1);  
  a_to->addr = (XPointer)Class.string;      
  a_argn = 0;
  a_args = NULL; 
}
/***************************************************************************/
static void ConvertXmRShadowTypeXtRString (
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
  sprintf(Class.string,"%s",""); 
       if(from==XmSHADOW_ETCHED_IN)  sprintf    (Class.string,"shadow_etched_in");
  else if(from==XmSHADOW_ETCHED_OUT) sprintf    (Class.string,"shadow_etched_out");
  else if(from==XmSHADOW_IN)         sprintf    (Class.string,"shadow_in");
  else if(from==XmSHADOW_OUT)        sprintf    (Class.string,"shadow_out");
  a_to->size = (unsigned int)(strlen(Class.string)+1);  
  a_to->addr = (XPointer)Class.string;      
  a_argn = 0;
  a_args = NULL; 
}
/***************************************************************************/
static void ConvertXmRStringDirecXtRString (
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
  sprintf(Class.string,"%s",""); 
       if(from==XmSTRING_DIRECTION_L_TO_R)      sprintf    (Class.string,"string_direction_l_to_r");
  else if(from==XmSTRING_DIRECTION_R_TO_L)      sprintf    (Class.string,"string_direction_r_to_l");
  a_to->size = (unsigned int)(strlen(Class.string)+1);  
  a_to->addr = (XPointer)Class.string;      
  a_argn = 0;
  a_args = NULL; 
}
/***************************************************************************/
static void ConvertXmRUnitTypeXtRString (
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
  sprintf(Class.string,"%s",""); 
       if(from==XmPIXELS)             sprintf    (Class.string,"pixels");
  else if(from==Xm100TH_MILLIMETERS)  sprintf    (Class.string,"100th_millimeters");
  else if(from==Xm1000TH_INCHES)      sprintf    (Class.string,"1000th_inches");
  else if(from==Xm100TH_POINTS)       sprintf    (Class.string,"100th_points");
  else if(from==Xm100TH_FONT_UNITS)   sprintf    (Class.string,"100th_font_units");
  a_to->size = (unsigned int)(strlen(Class.string)+1);  
  a_to->addr = (XPointer)Class.string;      
  a_argn = 0;
  a_args = NULL; 
}
/***************************************************************************/
static void ConvertXmRVisualPolicyXtRString (
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
  sprintf(Class.string,"%s",""); 
       if(from==XmVARIABLE)      sprintf    (Class.string,"variable");
  else if(from==XmCONSTANT)      sprintf    (Class.string,"constant");
  a_to->size = (unsigned int)(strlen(Class.string)+1);  
  a_to->addr = (XPointer)Class.string;      
  a_argn = 0;
  a_args = NULL; 
}
/***************************************************************************/
static void ConvertXmRSBarDispPolicyXtRString (
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
  sprintf(Class.string,"%s",""); 
       if(from==XmAS_NEEDED)   sprintf    (Class.string,"as_needed");
  else if(from==XmSTATIC)      sprintf    (Class.string,"static");
  a_to->size = (unsigned int)(strlen(Class.string)+1);  
  a_to->addr = (XPointer)Class.string;      
  a_argn = 0;
  a_args = NULL; 
}
/***************************************************************************/
static void ConvertXmRSBarPlacementXtRString (
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
  sprintf(Class.string,"%s",""); 
       if(from==XmTOP_LEFT)          sprintf    (Class.string,"top_left");
  else if(from==XmTOP_RIGHT)         sprintf    (Class.string,"top_right");
  else if(from==XmBOTTOM_LEFT)       sprintf    (Class.string,"bottom_left");
  else if(from==XmBOTTOM_RIGHT)      sprintf    (Class.string,"bottom_right");
  a_to->size = (unsigned int)(strlen(Class.string)+1);  
  a_to->addr = (XPointer)Class.string;      
  a_argn = 0;
  a_args = NULL; 
}
/***************************************************************************/
static void ConvertXmRScrollPolicyXtRString (
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
  sprintf(Class.string,"%s",""); 
       if(from==XmAUTOMATIC)           sprintf    (Class.string,"automatic");
  else if(from==XmAPPLICATION_DEFINED) sprintf    (Class.string,"application_defined");
  a_to->size = (unsigned int)(strlen(Class.string)+1);  
  a_to->addr = (XPointer)Class.string;      
  a_argn = 0;
  a_args = NULL; 
}
/***************************************************************************/
static void ConvertXmRWhichButtonXtRString (
 XrmValuePtr a_args 
,Cardinal* a_argn
,XrmValuePtr a_from
,XrmValuePtr a_to
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int from;
/*.........................................................................*/
  from  = AXARGVAL(int);
  sprintf(Class.string,"%s",""); 
       if(from==Button1)      sprintf    (Class.string,"button1");
  else if(from==Button2)      sprintf    (Class.string,"button2");
  else if(from==Button3)      sprintf    (Class.string,"button3");
  else if(from==Button4)      sprintf    (Class.string,"button4");
  else if(from==Button5)      sprintf    (Class.string,"button5");
  a_to->size = (unsigned int)(strlen(Class.string)+1);  
  a_to->addr = (XPointer)Class.string;      
  a_argn = 0;
  a_args = NULL; 
}
/***************************************************************************/
static void ConvertXmROrientationXtRString (
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
  sprintf(Class.string,"%s",""); 
       if(from==XmVERTICAL)        sprintf    (Class.string,"vertical");
  else if(from==XmHORIZONTAL)      sprintf    (Class.string,"horizontal");
  a_to->size = (unsigned int)(strlen(Class.string)+1);  
  a_to->addr = (XPointer)Class.string;      
  a_argn = 0;
  a_args = NULL; 
}
/***************************************************************************/
static void ConvertXmREditModeXtRString (
 XrmValuePtr a_args 
,Cardinal* a_argn
,XrmValuePtr a_from
,XrmValuePtr a_to
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int from;
/*.........................................................................*/
  from  = AXARGVAL(int);
  sprintf(Class.string,"%s",""); 
       if(from==XmMULTI_LINE_EDIT)  sprintf    (Class.string,"multi_line_edit");
  else if(from==XmSINGLE_LINE_EDIT) sprintf    (Class.string,"single_line_edit");
  a_to->size = (unsigned int)(strlen(Class.string)+1);  
  a_to->addr = (XPointer)Class.string;      
  a_argn = 0;
  a_args = NULL; 
}
#if XmVERSION >= 2
/***************************************************************************/
static void ConvertXmRSetXtRString (
 XrmValuePtr a_args 
,Cardinal* a_argn
,XrmValuePtr a_from
,XrmValuePtr a_to
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  sprintf    (Class.string,"%d",AXARGVAL(char));
  a_to->size = (unsigned int)(strlen(Class.string)+1);             
  a_to->addr = (XPointer)Class.string;      
  a_argn = 0;
  a_args  = NULL; 
}
#endif
/***************************************************************************/
static void ReportError (
 char* a_name
,int a_code
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CErrorHandle ("Xo","XmWidget",a_name,a_code);
}


#else /*HAS_XM*/
#include <stddef.h>
#endif /*HAS_XM*/
