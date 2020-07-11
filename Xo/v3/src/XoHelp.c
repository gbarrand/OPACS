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
*/

#include <stdio.h>

#include <CPrinter.h>
#include <CText.h>
#include <CString.h>
#include <OHTML.h>
#include <CMemory.h> 
#include <CList.h>

#ifdef HAS_XM
#include <Xm/Form.h>
#include <Xm/List.h>
#include <Xm/Text.h>
#include <Xm/Label.h>
#include <Xm/PushB.h>
#include <XmTools.ic>
#endif /*HAS_XM*/

#ifdef HAS_XAW
#include <X11/Intrinsic.h>
#include <Xaw/Form.h>
#include <Xaw/List.h>
#include <Xaw/AsciiText.h>
#include <Xaw/Label.h>
#include <Xaw/Command.h>
#endif /*HAS_XAW*/

/*Private.*/
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/CoreP.h>
#include <X11/CompositeP.h>

#include <XoHelpP.h>

#ifdef __cplusplus
extern "C"{
#endif
static void             InitializeClass  (void);
static void             InitializeWidget (Widget, Widget,ArgList,Cardinal*);
static Boolean          SetValues        (Widget,Widget,Widget,ArgList,Cardinal *);
static void             RealizeWidget    (Widget, XtValueMask*, XSetWindowAttributes*);
static void             ChangeWidgetSize (Widget);
static XtGeometryResult GeometryManager  (Widget,XtWidgetGeometry*,XtWidgetGeometry*);

static void             GoBackCallback   (Widget,XtPointer,XtPointer);
static void             ListCallback     (Widget,XtPointer,XtPointer);
#ifdef __cplusplus
}
#endif

#define athis   ((XoHelpWidget)This)->help
#define acur    ((XoHelpWidget)a_current)->help
#define IFMOD(a_field)  if(athis.a_field != acur.a_field)

XoHelpClassRec  xoHelpClassRec = {
/* Core Class Part */
{
    (WidgetClass) &compositeClassRec, /* pointer to superclass ClassRec   */
    "XoHelp",         		/* widget resource class name       */
    sizeof(XoHelpRec),	        /* size in bytes of widget record   */
    InitializeClass,            /* class_initialize                 */
    NULL, 			/* dynamic initialization           */
    FALSE,       		/* has class been initialized?      */
    InitializeWidget,           /* initialize                       */
    NULL,			/* notify that initialize called    */
    RealizeWidget,              /* XCreateWindow for widget         */
    NULL,     		        /* widget semantics name to proc map*/
    0, 			        /* number of entries in actions     */
    NULL,                       /* resources for subclass fields    */
    0,                          /* number of entries in resources   */
    NULLQUARK,		        /* resource class quarkified        */
    TRUE,			/* compress MotionNotify for widget */
    TRUE,			/* compress Expose events for widget*/
    TRUE,			/* compress enter and leave events  */
    TRUE,			/* select for VisibilityNotify      */
    NULL,                       /* destroy                          */
    ChangeWidgetSize,           /* geom manager changed widget size */
    XtInheritExpose,            /* expose                           */
    SetValues,	                /* set subclass resource values     */
    NULL,			/* notify that SetValues called     */
    XtInheritSetValuesAlmost,   /* SetValues got "Almost" geo reply */
    NULL,		        /* notify that get_values called    */
    XtInheritAcceptFocus,       /* assign input focus to widget     */
    XtVersion,	                /* version of intrinsics used       */
    NULL,		        /* list of callback offsets         */
    XtInheritTranslations,      /* translations                     */	
    XtInheritQueryGeometry,     /* return preferred geometry        */
    XtInheritDisplayAccelerator,/* display your accelerator         */
    NULL 		        /* pointer to extension record      */
},
/* Composite Class Part */
{
    GeometryManager,            /* geometry manager for children   */
    XtInheritChangeManaged,     /* change managed state of child   */
    XtInheritInsertChild,       /* physically add child to parent  */
    XtInheritDeleteChild,       /* physically remove child         */
    NULL 		        /* pointer to extension record     */
},
/* XoHelp */
{
   NULL
}
   
};

WidgetClass xoHelpWidgetClass = (WidgetClass) &xoHelpClassRec;
/*class*/ /*XoHelp*/
/***************************************************************************/
static void InitializeClass (
 void
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
}
/***************************************************************************/
static void InitializeWidget (
 Widget a_request
,Widget This
,ArgList a_args
,Cardinal* a_argn
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_request->core.width<=0)  This->core.width  = 100;
  if(a_request->core.height<=0) This->core.height = 100;

  athis.form   = NULL;
  athis.text   = NULL;
  athis.list   = NULL;

#ifdef HAS_XM
 {Arg     args[6];
  Widget  label,back;
  athis.form         = XmCreateForm (This,"HELP_FORM",args,0);
  XtManageChild      (athis.form);

  athis.text         = XmCreateScrolledText (athis.form,"HELP_TEXT",args,0);
  XtManageChild      (athis.text);

  XtSetArg           (args[0],XmNleftAttachment        ,XmATTACH_FORM);
  XtSetArg           (args[1],XmNrightAttachment       ,XmATTACH_FORM);
  XtSetArg           (args[2],XmNtopAttachment         ,XmATTACH_FORM);
  XtSetArg           (args[3],XmNbottomAttachment      ,XmATTACH_POSITION);
  XtSetArg           (args[4],XmNbottomPosition        ,60);
  XtSetValues        (XtParent(athis.text),args,5);

  XtSetArg           (args[0],XmNleftAttachment        ,XmATTACH_FORM);
  XtSetArg           (args[1],XmNrightAttachment       ,XmATTACH_FORM);
  XtSetArg           (args[2],XmNtopAttachment         ,XmATTACH_WIDGET);
  XtSetArg           (args[3],XmNtopWidget             ,XtParent(athis.text));
  label              = XmCreateLabel (athis.form,"HELP_LABEL",args,4);
  XtManageChild      (label);

  XtSetArg           (args[0],XmNleftAttachment        ,XmATTACH_FORM);
  XtSetArg           (args[1],XmNrightAttachment       ,XmATTACH_FORM);
  XtSetArg           (args[2],XmNbottomAttachment      ,XmATTACH_FORM);
  back               = XmCreatePushButton (athis.form,"HELP_BACK",args,3);
  XtManageChild      (back);

  athis.list         = XmCreateScrolledList (athis.form,"HELP_LIST",args,0);
  XtManageChild      (athis.list);

  XtSetArg           (args[0],XmNleftAttachment        ,XmATTACH_FORM);
  XtSetArg           (args[1],XmNrightAttachment       ,XmATTACH_FORM);
  XtSetArg           (args[2],XmNtopAttachment         ,XmATTACH_WIDGET);
  XtSetArg           (args[3],XmNtopWidget             ,label);
  XtSetArg           (args[4],XmNbottomAttachment      ,XmATTACH_WIDGET);
  XtSetArg           (args[5],XmNbottomWidget          ,back);
  XtSetValues        (XtParent(athis.list),args,6);

  XtAddCallback      (back        ,XmNactivateCallback       ,GoBackCallback ,(XtPointer)This);
  XtAddCallback      (athis.list  ,XmNsingleSelectionCallback,ListCallback   ,(XtPointer)This);
  }
#endif /*HAS_XM*/

#ifdef HAS_XAW
 {Arg     args[6];
  Widget  label,back;
  athis.form         = XtCreateWidget ("HELP_FORM",formWidgetClass,This,args,0);
  XtManageChild      (athis.form);

  XtSetArg           (args[0],XtNleft                  ,XawChainLeft);
  XtSetArg           (args[1],XtNright                 ,XawChainRight);
  XtSetArg           (args[2],XtNtop                   ,XawChainTop);
  XtSetArg           (args[3],XtNscrollHorizontal      ,XawtextScrollWhenNeeded);
  XtSetArg           (args[4],XtNscrollVertical        ,XawtextScrollWhenNeeded);
  athis.text         = XtCreateWidget ("HELP_TEXT",asciiTextWidgetClass,athis.form,args,5);
  XtManageChild      (athis.text);

  XtSetArg           (args[0],XtNleft                  ,XawChainLeft);
  XtSetArg           (args[1],XtNright                 ,XawChainRight);
  XtSetArg           (args[2],XtNfromVert              ,athis.text);
  XtSetArg           (args[3],XtNvertDistance          ,2);
  XtSetArg           (args[4],XtNheight                ,1);
  label              = XtCreateWidget ("HELP_LABEL",labelWidgetClass,athis.form,args,5);
  XtManageChild      (label);

  XtSetArg           (args[0],XtNleft                  ,XawChainLeft);
  XtSetArg           (args[1],XtNright                 ,XawChainRight);
  XtSetArg           (args[2],XtNfromVert              ,label);
  XtSetArg           (args[3],XtNvertDistance          ,2);
  athis.list         = XtCreateWidget ("HELP_LIST",listWidgetClass,athis.form,args,4);
  XtManageChild      (athis.list);

  XtSetArg           (args[0],XtNleft                  ,XawChainLeft);
  XtSetArg           (args[1],XtNright                 ,XawChainRight);
  XtSetArg           (args[2],XtNbottom                ,XawChainBottom);
  XtSetArg           (args[3],XtNfromVert              ,athis.list); 
  XtSetArg           (args[4],XtNvertDistance          ,2);
  XtSetArg           (args[5],XtNheight                ,1);
  back               = XtCreateWidget ("HELP_BACK",commandWidgetClass,athis.form,args,6);
  XtManageChild      (back);

  XtAddCallback      (back        ,XtNcallback       ,GoBackCallback ,(XtPointer)This);
  XtAddCallback      (athis.list  ,XtNcallback       ,ListCallback   ,(XtPointer)This);
  }
#endif /*HAS_XAW*/

  a_request = NULL;
  a_args    = NULL;
  a_argn    = 0; /*avoid C++ warnings*/
}
/***************************************************************************/
static Boolean SetValues (
 Widget a_current
,Widget a_request
,Widget This
,ArgList a_args
,Cardinal* a_argn
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  This      = NULL;
  a_current = NULL;
  a_request = NULL;
  a_args    = NULL;
  a_argn    = 0; /*avoid C++ warnings*/
  return    False;
}
/***************************************************************************/
static void RealizeWidget (
 Widget This
,XtValueMask* a_mask
,XSetWindowAttributes* a_watbs
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
/* Call the Realize procedure (XtInheritRealize) */
  if(xoHelpWidgetClass->core_class.superclass->core_class.realize)
    (xoHelpWidgetClass->core_class.superclass->core_class.realize)(This, a_mask, a_watbs);

  if(athis.form!=NULL)
    XtResizeWidget      (athis.form,This->core.width,This->core.height,0);

}
/***************************************************************************/
static void ChangeWidgetSize (
 Widget This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(xoHelpWidgetClass->core_class.superclass->core_class.resize!=NULL)
    (xoHelpWidgetClass->core_class.superclass->core_class.resize)(This);

  if(athis.form!=NULL)
    XtResizeWidget      (athis.form,This->core.width,This->core.height,0);

}
/***************************************************************************/
static XtGeometryResult GeometryManager (
 Widget		   a_child
,XtWidgetGeometry* a_request
,XtWidgetGeometry* a_reply
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  a_child   = NULL;
  a_request = NULL;
  a_reply   = NULL;
  return    (XtGeometryYes);
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static void GoBackCallback (
 Widget    This 
,XtPointer a_tag
,XtPointer a_data
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget           help;
  char*            string;
/*.........................................................................*/
  help             = (Widget)a_tag;
  string           = OHTML_GoBackward();
  XoHelpShowTopic  (help,string);
  CStringDelete    (string);
  This   = NULL;
  a_data = NULL;
}
/***************************************************************************/
static void ListCallback (
 Widget    This 
,XtPointer a_tag
,XtPointer a_data
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*            item;
  char*            topic = NULL;
  Widget           help;
/*.........................................................................*/
  if(a_data==NULL) return;
  help             = (Widget)a_tag;
#ifdef HAS_XM
  item             = XmConvertCompoundStringToString (((XmListCallbackStruct*)a_data)->item,0);
  topic            = CStringCreateF                  (strlen(item)+1,"#%s",item);
  XtFree           (item);
#endif /*HAS_XM*/
#ifdef HAS_XAW
  item             = ((XawListReturnStruct*)a_data)->string;
  topic            = CStringCreateF                  (strlen(item)+1,"#%s",item);
#endif /*HAS_XAW*/
  XoHelpShowTopic  (help,topic);
  CStringDelete    (topic);
  This             = NULL;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
void XoHelpShowTopic (
 Widget This
,char* a_locator  
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int           linen = 0;
  char**        lines = NULL;
  int           subn  = 0;
  char**        subs  = NULL;
  char*         string;
/*.........................................................................*/
  if(This==NULL)                            return;
  if(!XtIsSubclass(This,xoHelpWidgetClass)) return;
  if(a_locator==NULL)                       return;

/* get topic */
  OHTML_GetTopic         (a_locator,1,&lines,&linen,&subs,&subn);

/* set text widget */
/* set list widget */
  string                 = CTextConvertToString (linen,lines,"\n");
  CTextDelete            (linen,lines);
#ifdef HAS_XM
  XmTextSetString        (athis.text,string);
  XmListSetItemsFromText (athis.list,subn,subs);
#endif /*HAS_XM*/
#ifdef HAS_XAW
 {Arg                    args[2];
  XtSetArg               (args[0],XtNstring,string);
  XtSetValues            (athis.text,args,1);
  if(subn>0) /*The List widget don't do a local copy.*/
    {
      char** items = NULL;
      items  = (char**)XtMalloc (subn * sizeof(char*));
      if(items!=NULL)
	{
	  int                    count;
	  for(count=0;count<subn;count++) items[count] = (char*)XtNewString(subs[count]);
	  XtSetArg               (args[0],XtNlist         ,items); 
	  XtSetArg               (args[1],XtNnumberStrings,subn);
	  XtSetValues            (athis.list,args,2);
	}
    }
  else
    {
      char** items = NULL;
      items  = (char**)XtMalloc (sizeof(char*));
      if(items!=NULL)
	{
	  items[0]               = (char*)XtNewString("No subtopic.");
	  XtSetArg               (args[0],XtNlist         ,items); 
	  XtSetArg               (args[1],XtNnumberStrings,1);
	  XtSetValues            (athis.list,args,2);
	}
    }}
#endif /*HAS_XAW*/

  CTextDelete            (subn,subs);
  CStringDelete          (string);
}
/***************************************************************************/
void XoHelpClearClass (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  OHTML_ClearClass ();
}
