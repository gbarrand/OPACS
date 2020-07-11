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
#undef HAS_XM
#define HAS_XAW
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <X11/Intrinsic.h>

#ifdef HAS_XM
#include <Xm/PushB.h>
#include <Xm/Label.h>
#include <Xm/RowColumn.h>
#include <Xm/PushBG.h>
#endif /*HAS_XM*/

#ifdef HAS_XAW
#include <Xaw/Command.h>
#include <Xaw/Label.h>
#include <Xaw/SimpleMenu.h>   
#include <Xaw/SmeBSB.h>   
#include <Xaw/SmeLine.h>  
#endif /*HAS_XAW*/

#ifdef HAS_HTML
#include <HTML.h>
#include <momo.h>
#include <mowww.h>
#endif /*HAS_HTML*/

#include <CPrinter.h>
#include <CString.h>
#include <CError.h>
#include <CFile.h>
#include <CSystem.h>
#include <CList.h>
#include <OHTML.h>

#include <XWidget.h>

/*Private.*/
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/CoreP.h>
#include <X11/CompositeP.h>

#include <XoMosaicP.h>

#define mo_status int

typedef void(*HTGetMailtoKludgeInfoFunction)(char**,char**);

#ifdef __cplusplus
extern "C"{
#endif
void HTSetGetMailtoKludgeInfoProc (HTGetMailtoKludgeInfoFunction);
void GetMailtoKludgeInfo          (char**,char**);
static mo_status mo_load_window_text (Widget,char*);

static void             InitializeClass     (void);
static void             InitializeWidget    (Widget, Widget,ArgList,Cardinal*);
static void             DestroyWidget       (Widget);
static void             RealizeWidget       (Widget, XtValueMask*, XSetWindowAttributes*);
static void             ChangeWidgetSize    (Widget);
static void             DrawWidget          (Widget, XEvent*, Region);
static Boolean          SetValues           (Widget,Widget,Widget,ArgList,Cardinal *);
static XtGeometryResult GeometryManager     (Widget,XtWidgetGeometry*,XtWidgetGeometry*);
static void             SetChildrenGeometry (Widget);
static void             AnchorCbk           (Widget,XtPointer,XtPointer);
static void             GoBackCallback      (Widget,XtPointer,XtPointer);
static void             StartNotify         (char*);
static char*            DuplicateString     (char*);
static int              GetFontFamily       (char*);
static void             NotifyProc          (char*);

static void             PopupAction         (Widget,XEvent*,String*,Cardinal*);
static void             CreatePopup         (Widget);
static void             ActivateCallback    (Widget,XtPointer,XtPointer);
static char*            GetMenuText         (Widget,int*,int*);
#ifdef __cplusplus
}
#endif

extern int binary_transfer;
extern int interrupted;
extern int HTML_Print_Header;
extern int HTML_Print_Header_Top;
extern int HTML_Print_Header_Left;

static struct {
 Widget This;
 char*  string;
 Widget popup;
 XEvent This_event;
} Class = {NULL,NULL,NULL};

#define athis   ((XoMosaicWidget)This)->mosaic
#define acur    ((XoMosaicWidget)a_current)->mosaic

#define IFMOD(a_field)  if(athis.a_field != acur.a_field)
#define IFSMOD(a_field) if(CStringCompare(athis.a_field,acur.a_field)==0)
#define IFLMOD(a_field) if(CListCompare((CList)athis.a_field,(CList)acur.a_field)==0)

static char translations[] =
            "\
                      <Btn3Down>: MosaicPopup()";

static XtActionsRec actions[] = 
{
  { "MosaicPopup"     ,PopupAction}
};

static XtResource resources [] ={
  {XoNhomeDocument ,"HomeDocument"  ,XtRString,sizeof(String),
    XtOffset(XoMosaicWidget,mosaic.homeDocument),XtRImmediate,NULL},
  {XoNcontrolsVisible ,"ControlsVisible"  ,XtRBoolean,sizeof(Boolean),
   XtOffset(XoMosaicWidget,mosaic.controlsVisible),XtRImmediate,(XtPointer)True},
  {XoNcurrentURL ,"CurrentURL"  ,XtRString,sizeof(String),
   XtOffset(XoMosaicWidget,mosaic.currentURL),XtRImmediate,NULL},
  {XoNmenuFont      , "MenuFont"      , XtRFontStruct   , sizeof(XFontStruct*),
   XtOffset(XoMosaicWidget, mosaic.menuFont),XtRString,XtDefaultFont}
};

XoMosaicClassRec  xoMosaicClassRec = {
/* Core Class Part */
{
    (WidgetClass) &compositeClassRec, /* pointer to superclass ClassRec   */
    "XoMosaic",         	/* widget resource class name       */
    sizeof(XoMosaicRec),	/* size in bytes of widget record   */
    InitializeClass,            /* class_initialize                 */
    NULL, 			/* dynamic initialization           */
    FALSE,       		/* has class been initialized?      */
    InitializeWidget,           /* initialize                       */
    NULL,			/* notify that initialize called    */
    RealizeWidget,              /* XCreateWindow for widget         */
    actions,                    /* widget semantics name to proc map*/
    XtNumber(actions),          /* number of entries in actions     */
    resources,                  /* resources for subclass fields    */
    XtNumber(resources),        /* number of entries in resources   */
    NULLQUARK,		        /* resource class quarkified        */
    TRUE,			/* compress MotionNotify for widget */
    TRUE,			/* compress Expose events for widget*/
    TRUE,			/* compress enter and leave events  */
    TRUE,			/* select for VisibilityNotify      */
    DestroyWidget,	        /* free data for subclass pointers  */
    ChangeWidgetSize,           /* resize                           */
    DrawWidget,                 /* expose                           */
    SetValues,	                /* set subclass resource values     */
    NULL,			/* notify that SetValues called    */
    XtInheritSetValuesAlmost,   /* SetValues got "Almost" geo reply*/
    NULL,		        /* notify that get_values called    */
    XtInheritAcceptFocus,       /* assign input focus to widget     */
    XtVersion,	                /* version of intrinsics used       */
    NULL,		        /* list of callback offsets         */
    translations /*XtInheritTranslations*/,      /* translations                     */	
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
/* XoMosaic */
{
   NULL
}
   
};

WidgetClass xoMosaicWidgetClass = (WidgetClass) &xoMosaicClassRec;
/***************************************************************************/
static void InitializeClass (
 void
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
#ifndef HAS_HTML
  CWarn ("Xo package has been compiled without the -DHAS_HTML option.\n");
#else
  HTSetGetMailtoKludgeInfoProc (GetMailtoKludgeInfo);
  mo_set_notify_proc           (StartNotify);
#endif

#ifdef HAS_XAW
  XtRegisterGrabAction(PopupAction,True,ButtonPressMask | ButtonReleaseMask,GrabModeAsync,GrabModeAsync);
#endif /*HAS_XAW*/
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
  Arg    args[5];
/*.........................................................................*/
  if(a_request->core.width<=0)  This->core.width  = 100;
  if(a_request->core.height<=0) This->core.height = 100;

  athis.html              = NULL;
  athis.tracker           = NULL;
  athis.binary_transfer   = 0;
  athis.currentText       = NULL;
  athis.refi              = 0;

#ifdef HAS_HTML
    
#ifdef HAS_XM
  athis.tracker     = XtCreateWidget ("track",xmLabelWidgetClass,This,args,0);
#endif /*HAS_XM*/
#ifdef HAS_XAW
  athis.tracker   = XtCreateWidget ("track",labelWidgetClass,This,args,0);
#endif /*HAS_XM*/

  athis.html      = XtCreateWidget ("html",htmlWidgetClass,This,args,0);
  (athis.html)->core.width    = This->core.width;
  (athis.html)->core.height   = This->core.height;
  /*  This->core.background_pixel = (athis.html)->core.background_pixel;*/
  XtManageChild   (athis.html);

  XtAddCallback   (athis.html,WbNanchorCallback    ,AnchorCbk   ,(XtPointer)This);

  athis.homeDocument = CStringDuplicate(athis.homeDocument);
  athis.currentURL   = CStringDuplicate(athis.currentURL);

  if( (athis.homeDocument!=NULL) && (*athis.homeDocument!='\0') )
    {
      char*                   href;
      href                    = CStringDuplicate(athis.homeDocument);
      mo_load_window_text     (This,href);
      CStringDelete           (href);
    }

  mo_set_globals (This,athis.html,NotifyProc);

#else /*HAS_HTML*/

  athis.homeDocument = CStringDuplicate(athis.homeDocument);
  athis.currentURL   = CStringDuplicate(athis.currentURL);
  /*todo: Draw something in the window.*/

#endif /*HAS_HTML*/

#ifdef DEBUG
  printf ("debug:controlsVisible:%d\n" ,athis.controlsVisible);
  printf ("debug:homeDocuments:%s\n",athis.homeDocument);
#endif

  if(athis.controlsVisible==True)
    {
      if(athis.tracker!=NULL) XtManageChild (athis.tracker);
    }


  a_request = NULL;
  a_args    = NULL;
  a_argn    = 0; /*avoid C++ warnings*/
}
/***************************************************************************/
static void DestroyWidget (
 Widget This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
/*.........................................................................*/
  CStringDelete      (athis.homeDocument);
  CStringDelete      (athis.currentURL);
  CStringDelete      (athis.currentText);
  athis.homeDocument = NULL;
  athis.currentURL   = NULL;
  athis.currentText  = NULL;
  for(count=0;count<athis.refi;count++)
    {
      CStringDelete     (athis.refs[count]);
      athis.refs[count] = NULL;
    }
  CStringDelete      (Class.string);
  Class.string       = NULL;
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
  if(xoMosaicWidgetClass->core_class.superclass->core_class.realize)
    (xoMosaicWidgetClass->core_class.superclass->core_class.realize)(This, a_mask, a_watbs);

 SetChildrenGeometry (This);
}
/***************************************************************************/
static void ChangeWidgetSize (
 Widget This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(xoMosaicWidgetClass->core_class.superclass->core_class.resize)
    (xoMosaicWidgetClass->core_class.superclass->core_class.resize)(This);

  /*
  if (  (athis.html!=NULL) && (
        ((athis.html)->core.width  != This->core.width)  ||
        ((athis.html)->core.height != This->core.height))
     )
    {
      (athis.html)->core.width  = This->core.width;
      (athis.html)->core.height = This->core.height;
      XWidgetExecuteChangeSizeFunction     (athis.html);
    }
    */

 SetChildrenGeometry (This);
}
/***************************************************************************/
static void DrawWidget (
 Widget  This
,XEvent* a_event
,Region a_region
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Display*     xdisp;
  Window       xwindow;
  GC           xgc;
  char*        string;
  int          width,height,x,y;
/*.........................................................................*/
  if(xoMosaicWidgetClass->core_class.superclass->core_class.expose)
    (xoMosaicWidgetClass->core_class.superclass->core_class.expose)(This,a_event,a_region);

  xdisp             = XtDisplay (This);
  xwindow           = XtWindow  (This);

  xgc               = (GC) XCreateGC (xdisp,XDefaultRootWindow(xdisp),0L,NULL);

  string            = GetMenuText (This,&width,&height);  

  XSetFont          (xdisp,xgc,athis.menuFont->fid);
  XSetForeground    (xdisp,xgc,XBlackPixelOfScreen(XDefaultScreenOfDisplay(xdisp)));
  x                 = (This->core.width - width)/2;
  y                 = 4 + athis.menuFont->max_bounds.ascent;
  XDrawString       (xdisp,xwindow,xgc,x,y,string,strlen(string));

  XFreeGC           (xdisp,xgc);
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
  IFSMOD(currentURL) /*Must be check first. mo_load_window_text could change field currentURL.*/
    {
      CStringDelete           (acur.currentURL);
      athis.currentURL        = CStringDuplicate (athis.currentURL);
     {char*                   href;
      href                    = CStringDuplicate(athis.currentURL);
      mo_load_window_text     (This,href);
      CStringDelete           (href);}
    }
  else {athis.currentURL = acur.currentURL;}

  IFSMOD(homeDocument) 
    {
      CStringDelete           (acur.homeDocument);
      athis.homeDocument      = CStringDuplicate (athis.homeDocument);
     {char*                   href;
      href                    = CStringDuplicate(athis.homeDocument);
      mo_load_window_text     (This,href);
      CStringDelete           (href);}
    }
  else {athis.homeDocument = acur.homeDocument;}

  IFMOD(controlsVisible)
    {
      if(athis.controlsVisible==True)
	{
	  if(athis.tracker!=NULL) 
	    {
	      XtManageChild       (athis.tracker);
	      SetChildrenGeometry (This);
	    }
	}
      else
	{
	  if(athis.tracker!=NULL) 
	    {
	      XtUnmanageChild     (athis.tracker);
	      SetChildrenGeometry (This);
	    }
	}
    }

  a_request = NULL;
  a_args    = NULL;
  a_argn    = 0; /*avoid C++ warnings*/

  return    False;
}
/***************************************************************************/
static XtGeometryResult GeometryManager (
 Widget		   This
,XtWidgetGeometry* a_request
,XtWidgetGeometry* a_reply
)
/***************************************************************************/
/* debug */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  This      = NULL;  
  a_request = NULL;
  a_reply   = NULL;
  return    (XtGeometryYes);
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static void SetChildrenGeometry (
 Widget This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Dimension menuHeight,trackerHeight;
/*.........................................................................*/
  trackerHeight = 0;
 {int           width,height;
  GetMenuText   (This,&width,&height);  
  menuHeight    = height + 2 * 4;}
  if( (athis.tracker!=NULL) && XtIsManaged(athis.tracker) )
    {
      Dimension           width;
      XWidgetGetDimension (athis.tracker,&width,&trackerHeight);
      XtResizeWidget      (athis.tracker,This->core.width,trackerHeight,0);
    }
  if(athis.html!=NULL)
    {
      XtResizeWidget      (athis.html,This->core.width,This->core.height-menuHeight-trackerHeight,0);
      XtMoveWidget        (athis.html,0,menuHeight);
    }
  if( (athis.tracker!=NULL) && XtIsManaged(athis.tracker) )
    {
      XtMoveWidget        (athis.tracker,0,This->core.height-trackerHeight);
    }
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
void XoMosaicLoadWindowText (
 Widget This
,char* a_string
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)                              return;
  if(!XtIsSubclass(This,xoMosaicWidgetClass)) return;
  if(a_string==NULL)                          return;
  mo_load_window_text (This,a_string);
}
/***************************************************************************/
void XoMosaicShowTopic (
 Widget This
,char* a_locator  
)
/***************************************************************************/
/* topic is of the form
      <topic> <filename>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* fileName;
  char* topic;
/*.........................................................................*/
  if(This==NULL)                              return;
  if(!XtIsSubclass(This,xoMosaicWidgetClass)) return;
  if(a_locator==NULL)                         return;

  fileName = OHTML_ParseLocator (a_locator,&topic);

  if(fileName!=NULL)
    {
      char*  fname;
      FILE*  file;
      file   = CFileOpenForReading(fileName,&fname);
      if(file)
        {
	  char*               string;
	  fclose              (file);
	  if(topic==NULL)
	    string = CStringCreateF(strlen(fname)+5,"file:%s",fname);
	  else
	    string = CStringCreateF(strlen(fname)+strlen(topic)+6,"file:%s#%s",fname,topic);
	  CStringDelete       (athis.currentURL); /*So that file in current directory is found.*/
	  athis.currentURL    = NULL;
	  mo_load_window_text (This,string);
          CStringDelete       (string);
	}
      CStringDelete (fname);
    }
  CStringDelete (fileName);
  CStringDelete (topic);
}
/***************************************************************************/
void XoMosaicGoBack (
 Widget This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)                              return;
  if(!XtIsSubclass(This,xoMosaicWidgetClass)) return;
  if(athis.refi>=2) 
    { 
      char*                      href;
      athis.refi               --;
      CStringDelete              (athis.refs[athis.refi]);
      athis.refs[athis.refi]     =  NULL;
      athis.refi               --;
      href                       = CStringDuplicate(athis.refs[athis.refi]);
      CStringDelete              (athis.refs[athis.refi]);
      athis.refs[athis.refi]     =  NULL;
      mo_load_window_text        (This,href);
      CStringDelete              (href);
    }
  This   = NULL;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static void AnchorCbk (
 Widget This 
,XtPointer a_tag
,XtPointer a_data
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
#ifdef HAS_HTML
  char*       href;
/*.........................................................................*/
  if (((WbAnchorCallbackData *)a_data)->href!=NULL)
    href = CStringDuplicate (((WbAnchorCallbackData *)a_data)->href);
  else
    href = CStringDuplicate ("Unlinked");

  mo_convert_newlines_to_spaces (href);

  mo_load_window_text     ((Widget)a_tag,href);

  CStringDelete (href);

#else
  a_tag  = NULL;
  a_data = NULL;
#endif /*HAS_HTML*/

  This = NULL;
}
/***************************************************************************/
static void GoBackCallback (
 Widget This 
,XtPointer a_tag
,XtPointer a_data
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  XoMosaicGoBack ((Widget)a_tag);
  This   = NULL;
  a_data = NULL;
}
/***************************************************************************/
static void StartNotify (
 char* This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  CPrinterPutF ("%s\n",This);
}
/***************************************************************************/
static mo_status mo_load_window_text (
 Widget This
,char* url
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
#ifdef HAS_HTML
  char* newtext       = NULL;
  char* newtexthead   = NULL;
  char* target_anchor = NULL;
/*.........................................................................*/
  Class.This = This;

  mo_busy ();

  target_anchor = mo_url_extract_anchor (url);

#ifdef DEBUG
  printf ("debug:mo_load_window_text:\nurl:%s\nanchor:%s\n",url,target_anchor);
#endif

  /* If we're just referencing an anchor inside a document, do the right thing. */
  if ( (url!=NULL) && (*url == '#'))
    {
      /* Now we make a copy of the current text and make sure we ask
         for a new mo_node and entry in the history list. */
      /* IF we're not dealing with an internal reference. */
      if ( (strncmp (url, "#hdfref;", 8)!=0) &&
           (strncmp (url, "#hdfdtm;", 8)!=0) )
        {
          if (athis.currentText!=NULL)
            {
              newtext     = DuplicateString (athis.currentText);
              newtexthead = newtext;
#ifdef DEBUG
	      printf ("debug:inside a document.\n");
#endif
            }
          else
            {
              newtext     = DuplicateString ("lose");
              newtexthead = newtext;
            }
        }
     {char*            string;
      string           = mo_url_canonicalize_keep_anchor (url, athis.currentText!=NULL ? athis.currentURL : "");
      CStringDelete    (Class.string);
      Class.string     = CStringDuplicate(string);
      if(string!=NULL) free (string);}
      url              = Class.string;
    }
  else
    {
      /* Get a full address for this URL. */
      /* Under some circumstances we may not have a current node yet
         and may wish to just run with it... so check for that. */
      if ( (athis.currentText!=NULL) && 
	   (athis.currentURL!=NULL) 
	  )
        {
          char*            string;
          string           = mo_url_canonicalize_keep_anchor (url, athis.currentURL);
	  CStringDelete    (Class.string);
	  Class.string     = CStringDuplicate(string);
	  if(string!=NULL) free (string);
	  url              = Class.string;
        }
      /* Set binary_transfer as per current window. */
      binary_transfer           = athis.binary_transfer;
/*debug:      mo_set_current_cached_win (win);*/

      {
        char*                     canon;
        canon                     = mo_url_canonicalize (url, "");
        interrupted               = 0;
	CStringDelete             (athis.currentURL);
	athis.currentURL          = CStringDuplicate(canon);
        newtext                   = mo_pull_er_over (canon, &newtexthead);
        if(canon!=NULL)           {free(canon);canon=NULL;}
#ifdef DEBUG
        printf                    ("debug:new node.\n");
#endif
/*New node; update current_node infos.*/
	CStringDelete             (athis.currentText);
	athis.currentText         = CStringDuplicate(newtext);
      }

      {
        /* Check use_this_url_instead from HTAccess.c. */
        extern char *use_this_url_instead;
        if (use_this_url_instead!=NULL)
          {
            mo_here_we_are_son (url);
            url = use_this_url_instead;
            
            /* Go get another target_anchor. */
            if (target_anchor!=NULL) {free(target_anchor);target_anchor=NULL;}
            target_anchor = mo_url_extract_anchor (url);
          }
      }
    }

#ifdef HAVE_HDF
  /* If a target anchor exists, and if it's an HDF reference, then
     go decode the HDF reference and call mo_do_window_text on the
     resulting text. */
  if ( (target_anchor!=NULL) &&
       (strncmp (target_anchor, "hdfref;", 7)==0) &&
       (strlen (target_anchor)>8) 
      )
    {
      char *text;
      text = (char *)mo_decode_hdfref (url, target_anchor);
      {
        /* Check use_this_url_instead from HTAccess.c. */
        extern char *use_this_url_instead;
        if (use_this_url_instead!=NULL)
          {
            mo_here_we_are_son      (url);
            url                     = use_this_url_instead;
            mo_load_window_text     (win, url, ref);
	    if(newtexthead!=NULL)   {free(newtexthead);newtexthead=NULL;}
	    if(target_anchor!=NULL) {free(target_anchor);target_anchor=NULL;}
	    Class.This     = NULL;
            return                  1;
          }
      }
      mo_do_window_text (win, url, text, text, 1, ref);
    }
  /* An hdfdtm reference means that we should blast the referenced
     HDF data object over the output DTM port to Collage.  Currently
     this can only be an image; in the future we'll do SDS's, etc. */
  else if ( (target_anchor!=NULL) &&
            (strncmp (target_anchor, "hdfdtm;", 7)==0) &&
            (strlen (target_anchor)>8) 
	   )
    {
#ifdef HAVE_DTM
      /* We specifically want to make sure that the anchor is allowed
         to stay in the URL, so we don't canonicalize to strip it out. */
      mo_do_hdf_dtm_thang (url, &(target_anchor[7]));
#endif

      if (target_anchor!=NULL) {free(target_anchor);target_anchor=NULL;}

      mo_gui_done_with_icon ();
    }
  /* Assuming we have HDF, the only thing mosaic-internal-reference
     currently can be is pointer to an HDF file. */
  else if ( (newtext!=NULL) && 
	    (strncmp (newtext, "<mosaic-internal-reference", 26)==0)
	   )
    {
      char *text;
      text = mo_decode_internal_reference (url, newtext, target_anchor);
      mo_do_window_text (win, url, text, text, 1, ref);
    }
  else
#endif

  /* Now, if it's a telnet session, there should be no need
     to do anything else.  Also check for override in text itself. */
  if ( (strncmp (url, "telnet:", 7)==0) || 
       (strncmp (url, "tn3270:", 7)==0) ||
       (strncmp (url, "rlogin:", 7)==0) ||
       ( (newtext!=NULL) && (strncmp(newtext,"<mosaic-access-override>", 24)==0) )
      )
    {
      /* We don't need this anymore. */
      free                  (newtexthead);
      newtexthead           = NULL;

      /* We still want a global history entry but NOT a window history entry. */
      mo_here_we_are_son    (url); 
      /* ... and we want to redisplay the current window to 
             get the effect of the history entry today, not tomorrow. */
/*debug:      mo_redisplay_window   (win);*/
      /* We're not busy anymore... */
      mo_gui_done_with_icon ();
    }
  else if (newtext!=NULL)
    {
      /* Not a telnet session and not an override, but text present
         (the "usual" case): */

      /* Set the window text. */
/*debug:      mo_do_window_text (win, url, newtext, newtexthead, 1, ref);*/

#ifdef DEBUG
      printf ("debug:do_window_text\nurl:%s\nanchor:%s\n",url,target_anchor);
#endif

      athis.refs[athis.refi]   = CStringDuplicate(url);
      athis.refi               ++;
      if(athis.html!=NULL) 
	HTMLSetText (athis.html,newtext,NULL,NULL,0,target_anchor,NULL);

    }
  else
    {
      /* No text at all. */
      mo_gui_done_with_icon ();
    }

  mo_not_busy       ();

  if(newtexthead!=NULL)   free (newtexthead);
  if(target_anchor!=NULL) free (target_anchor);
  Class.This     = NULL;
  return  1;

#else /*HAS_HTML*/

  This   = NULL;
  url    = NULL;
  return 0;

#endif /*HAS_HTML*/
}
/***************************************************************************/
static char *DuplicateString (
 char* str
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*  dup;
  int    l;
/*.........................................................................*/
  l      = strlen(str);
  dup    = (char *)malloc (l + 1);
  if(dup==NULL) return NULL;
  dup[l] = '\0'; 
  dup    = strcpy (dup, str);

  return dup;
}
/***************************************************************************/
mo_status mo_not_busy (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.This==NULL) return 0;
  XWidgetSetCursor   (Class.This,NULL);
  return             1;
}
/***************************************************************************/
mo_status mo_busy (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.This==NULL) return 0;
  XWidgetSetCursor   (Class.This,"watch");
  return             1;
}
/***************************************************************************/
static void NotifyProc (
 char* msg
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.This==NULL) return;
#ifdef HAS_XM
 {Arg             args[1];
  XmString        cps;
  cps             = XmStringLtoRCreate(msg,XmSTRING_DEFAULT_CHARSET);
  XtSetArg        (args[0],XmNlabelString,cps);
  XtSetValues     (((XoMosaicWidget)Class.This)->mosaic.tracker,args,1);
  XmStringFree    (cps);}
#endif /*HAS_XM*/
#ifdef HAS_XAW
 {Arg             args[1];
  XtSetArg        (args[0],XtNlabel,msg);
  XtSetValues     (((XoMosaicWidget)Class.This)->mosaic.tracker,args,1);}
#endif /*HAS_XAW*/
  XDisplayUpdate  (XtDisplay(Class.This));
  msg             = NULL;
}
/***************************************************************************/
void XoMosaicSave (
 Widget This
,char* a_name
,char* a_saveFormat
,char* a_userFont
)
/***************************************************************************/
/*
  From Mosaic-2.6/src/gui-dialogs.c/ XmxCallback (save_win_cb)
  It uses resources :
 - saveFormat : "plain_text", "formatted_text", "postscript".
 - userFont : "times", "large_helvetica", "newcentury", "lucidabright".
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)                              return;
  if(!XtIsSubclass(This,xoMosaicWidgetClass)) return;

#ifdef HAS_HTML
  mo_busy ();

  HTML_Print_Header      = 0;
  HTML_Print_Header_Top  = 1;
  HTML_Print_Header_Left = 0;

  if (CStringCompare(a_saveFormat,"plain_text")==1)
    {
      char* text;
      text  = HTMLGetText (athis.html, 0, athis.currentURL);
      if (text!=NULL)
	{
	  FILE*          file;
	  file           = CFileOpenForWriting (a_name==NULL?"out.txt":a_name);
	  if(file==NULL) return;
	  fputs          (text, file);
	  fclose         (file);
	  free           (text);
	}
    }
  else if (CStringCompare(a_saveFormat,"formatted_text")==1)
    {
      char* text;
      text = HTMLGetText (athis.html, 1, athis.currentURL);
      if (text!=NULL)
	{
	  FILE*          file;
	  file           = CFileOpenForWriting (a_name==NULL?"out.txt":a_name);
	  if(file==NULL) return;
	  fputs          (text, file);
	  fclose         (file);
	  free           (text);
	}
    }
  else if (CStringCompare(a_saveFormat,"postscript")==1)
    {
      char*       text;
      int         font_family;
      font_family = GetFontFamily (a_userFont);
      text        = HTMLGetText (athis.html, 2 + font_family, athis.currentURL);
      if (text)
	{
	  FILE*          file;
	  file           = CFileOpenForWriting (a_name==NULL?"out.ps":a_name);
	  if(file==NULL) return;
	  fputs          (text, file);
	  fclose         (file);
	  free           (text);
	}
    }
  else if (athis.currentText!=NULL) /* HTML source */
    {
      FILE*          file;
      file           = CFileOpenForWriting (a_name==NULL?"out.txt":a_name);
      if(file==NULL) return;
      fputs          (athis.currentText,file);
      fclose         (file);
    }

  mo_not_busy ();

#else
  a_name = NULL;
#endif /*HAS_HTML*/
}
/***************************************************************************/
static int GetFontFamily (
 char* a_userFont
)
/***************************************************************************/
/*
  From Mosaic-2.6/src/gui.c/mo_get_font_size_from_res
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*        lowerfontstr;
  int          fontFamily;
/*.........................................................................*/
  if(a_userFont==NULL) return 0;
  lowerfontstr = CStringDuplicate(a_userFont);
  CStringLower (lowerfontstr);

  fontFamily   = 0;
  if (strstr(lowerfontstr, "times")!=NULL)
    {
      fontFamily = 0;
    }
  else if (strstr(lowerfontstr, "helvetica")!=NULL)
    {
      fontFamily = 1;
    }
  else if (strstr(lowerfontstr, "century")!=NULL)
    {
      fontFamily = 2;
    }
  else if (strstr(lowerfontstr, "lucida")!=NULL)
    {
      fontFamily = 3;
    }
  else
    {
      CWarnF("Can't find font family from %s.\n",a_userFont);
    }
  CStringDelete (lowerfontstr);
  return        fontFamily;
}  
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static void PopupAction (
 Widget    This 
,XEvent*   a_event 
,String*   a_args
,Cardinal* a_argn
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_event==NULL)                    return;
  if(a_event->type!=ButtonPress)       return;
  if(a_event->xbutton.button!=Button3) return;

  CreatePopup (This);

  if(Class.popup!=NULL)
    { 
#ifdef HAS_XM
      XmMenuPosition    (Class.popup,(XButtonPressedEvent*)a_event);
      XtManageChild     (Class.popup);
#endif
#ifdef HAS_XAW
     {Arg                 args[2];
      XtSetArg            (args[0], XtNx, ((XButtonPressedEvent*)a_event)->x_root);
      XtSetArg            (args[1], XtNy, ((XButtonPressedEvent*)a_event)->y_root);
      XtSetValues         (Class.popup, args,2);
      XtPopupSpringLoaded (Class.popup);}
#endif /*HAS_XM*/
      Class.This        = This;
      Class.This_event  = *a_event;
    }
  a_args  = NULL; 
  a_argn  = NULL;
}
/***************************************************************************/
static void CreatePopup (
 Widget This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)        return;
  if(Class.popup!=NULL) return; /*Done.*/

#ifdef HAS_XM
 {Arg         args[1];
  XtSetArg    (args[0],XmNwhichButton,Button3);
  Class.popup = XmCreatePopupMenu (This,"xomosaic_popup",args,1);}
#endif /*HAS_XM*/
#ifdef HAS_XAW
 {Arg         args[1];
  Class.popup = XtCreatePopupShell ("xomosaic_popup",simpleMenuWidgetClass,This,args,0);}
#endif /*HAS_XAW*/

  if(Class.popup==NULL) return;

  XtAddCallback    (Class.popup   ,XtNdestroyCallback ,ActivateCallback,(XtPointer)115);

  /*Default menu.*/
  XoMosaicAddPopupEntry (This,"Back"    ,ActivateCallback,(XtPointer)111);
  XoMosaicAddPopupEntry (This,"Tracker" ,ActivateCallback,(XtPointer)222);
  if(CSystemIsSecured()==0)
    {
  XoMosaicAddPopupEntry (This,"Save/PS"             ,ActivateCallback,(XtPointer)333);
  XoMosaicAddPopupEntry (This,"Save/text"           ,ActivateCallback,(XtPointer)334);
  XoMosaicAddPopupEntry (This,"Save/formatted text" ,ActivateCallback,(XtPointer)335);
    }
}
/***************************************************************************/
static void ActivateCallback (
 Widget    This 
,XtPointer a_tag
,XtPointer a_data
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if((long)a_tag==111)  /*Back*/
    {
      XoMosaicGoBack (Class.This);
    }
  else if((long)a_tag==222)  /*Tracker*/
    {
      Arg          args[1];
      if(((XoMosaicWidget)Class.This)->mosaic.controlsVisible==True)
        XtSetArg     (args[0],XoNcontrolsVisible,False);
      else
        XtSetArg     (args[0],XoNcontrolsVisible,True);
      XtSetValues  (Class.This,args,1);
    }
  else if((long)a_tag==333)  /*Save/PS*/
    {
      XoMosaicSave (Class.This,NULL,"postscript","times");
    }
  else if((long)a_tag==334)  /*Save/text*/
    {
      XoMosaicSave (Class.This,NULL,"plain_text","times");
    }
  else if((long)a_tag==335)  /*Save/formatted text*/
    {
      XoMosaicSave (Class.This,NULL,"formatted_text","times");
    }
  This   = NULL;
  a_data = NULL;
}
/***************************************************************************/
static Widget FindPopupEntry (
 char* a_label
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  WidgetList items,itema;
  char*      name;
  Widget     entry = NULL;
/*.........................................................................*/
  if(a_label==NULL)     return NULL;
  if(Class.popup==NULL) return NULL;
  name  = CStringCreateF (16+strlen(a_label),"xomosaic_popup_%s",a_label);
  items = XWidgetGetChildren (Class.popup,NULL);
  entry = NULL;
  for(itema=items;(itema!=NULL) && (*itema!=NULL);itema++) 
    {
      char*  string;
      string = XtName(*itema);
      if( (string!=NULL) && (strcmp(name,string)==0) ) /*Already exists*/
	{
	  entry = *itema;
          break;
	}
    }
  CListDelete   ((CList)items);
  CStringDelete (name);
  return        entry;
}
/***************************************************************************/
void XoMosaicAddPopupEntry (
 Widget This
,char* a_label
,XtCallbackProc a_proc
,XtPointer a_data
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*            name;
  Arg              args[1];
  Widget           widget= NULL;
/*.........................................................................*/
  if(This==NULL)    return;
  if(a_label==NULL) return;
  if(a_proc==NULL)  return;
  CreatePopup       (This);
  if(Class.popup==NULL) return;
  if(FindPopupEntry(a_label)!=NULL) return;
  name             = CStringCreateF (16+strlen(a_label),"xomosaic_popup_%s",a_label);
#ifdef HAS_XM
  widget           = XmCreatePushButtonGadget (Class.popup,name,args,0);
 {XmString         cps;
  cps              = XmStringLtoRCreate(a_label,XmSTRING_DEFAULT_CHARSET);
  XtSetArg         (args[0],XmNlabelString,cps);
  XtSetValues      (widget,args,1);
  XmStringFree     (cps);}
  XtAddCallback    (widget,XmNactivateCallback,a_proc,(XtPointer)a_data);
#endif /*HAS_XM*/
#ifdef HAS_XAW
  widget           = XtCreateManagedWidget(name,smeBSBObjectClass,Class.popup,args,0);
  XtSetArg         (args[0],XtNlabel,a_label);
  XtSetValues      (widget,args,1);
  XtAddCallback    (widget,XtNcallback,a_proc,(XtPointer)a_data);
#endif /*HAS_XM*/
  if(widget!=NULL) XtManageChild    (widget);
  CStringDelete    (name);
}
/***************************************************************************/
static char* GetMenuText (
 Widget  This
,int* a_width
,int* a_height
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  static char string[] = "Menu (Right Button)";
/*.........................................................................*/
  *a_width          = XTextWidth (athis.menuFont,string,strlen(string));
  *a_height         = athis.menuFont->max_bounds.ascent + athis.menuFont->max_bounds.descent;
  return            string;
}
