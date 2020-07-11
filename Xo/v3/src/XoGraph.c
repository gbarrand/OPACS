/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#include          <stdio.h>
#include          <stdlib.h>
#include          <string.h>
#include          <math.h>

#include          <X11/cursorfont.h>
#include	  <X11/StringDefs.h>
#include	  <X11/Intrinsic.h>

#include          <CMemory.h>
#include          <CList.h>
#include          <CLkStat.h>

/*Private.*/
#include	  <X11/IntrinsicP.h>
#include	  <X11/CoreP.h>
#include  	  <X11/CompositeP.h>
#include	  <X11/ConstrainP.h>

#include	  <XoGraphP.h>

#ifdef __cplusplus
extern "C"{
#endif
static void    InitializeClass            (void);
static void    InitializeWidget           (Widget, Widget, ArgList, Cardinal*);
static void    DestroyWidget              (Widget);
static void    InitializeConstraint       (Widget, Widget, ArgList, Cardinal*);
static Boolean SetConstraintValues        (Widget, Widget, Widget, ArgList, Cardinal*);
static void    DrawWidget                 (Widget, XEvent*, Region);
static Boolean SetValues                  (Widget, Widget, Widget, ArgList, Cardinal*);
static void    DestroyConstraint          (Widget);
static XtGeometryResult GeometryManager   (Widget,XtWidgetGeometry*,XtWidgetGeometry*);

static void    UpdateLinkedItems          (Widget);
static XoLink  CreateLink                 (Widget,Widget);
static void    DeleteLink                 (XoLink);
static void    DrawLink                   (XoLink);
static void    ManageLink                 (XoGraphWidget, Widget, Widget, XoLink);
static void    UnmanageLink               (XoGraphWidget, Widget, Widget, XoLink);
static XoLink  GetLinkFromFather          (LkEntry);
static XoLink  GetLinkFromSon             (LkEntry);
static void    ConvertStringToGraphPolicy (XrmValuePtr, int*, XrmValuePtr, XrmValuePtr);
static void    ConvertGraphPolicyToString (XrmValuePtr, int*, XrmValuePtr, XrmValuePtr);
static void    CreateLinkAction           (Widget,XEvent*, String* ,Cardinal*);
static void    DestroyLinkAction          (Widget,XEvent*, String* ,Cardinal*);

static void    GetLinkInfos               (XoLink,int*,int*,short*,short*,short*,short*);
static int     GetLinkPoints              (int,short,short,short,short,XPoint*);
static int     GetBeginLinkPoints         (int,short,short,XPoint*);
static int     GetEndLinkPoints           (int,short,short,XPoint*);
static Widget  SearchNeightbour           (Widget,int,int);
static void    ShowNeightbour             (Widget,Widget);
static void    DrawFatherToCursor         (Widget,Widget,short,short);
static XoLink  SearchLink                 (Widget,int,int);
static Boolean IsLinkGood                 (XoLink,int,int);
static Boolean IsOnLink                   (int,int,int,short,short,short,short);
static Boolean IsOnSegment                (int,int,short,short,short,short);
static void    HighlightLink              (XoLink);
#ifdef __cplusplus
}
#endif

#define athis           ((XoGraphWidget)This)->graph
#define acur            ((XoGraphWidget)a_current)->graph
#define IFLMOD(a_field) if(!CListCompare((CList)athis.a_field,(CList)acur.a_field))
#define BORDER          4
#define ARROW_SIZE      4

static struct 
{
  XoGraphIsValidWidgetFunction isValidWidgetFunction;
  Cursor cursor_sb_up_arrow;
  Cursor cursor_top_left_arrow;
} Class = {NULL,0L,0L};


static XtResource resources[] = {
  {XoNlinksVisible, "LinksVisible", XtRBoolean, sizeof(Boolean), 
       XtOffset(XoGraphWidget, graph.linksVisible), XtRImmediate, (XtPointer) True},
  {XoNlinkingPolicy, "LinkingPolicy", XoRLinkingPolicy, sizeof(int), 
       XtOffset(XoGraphWidget, graph.linkingPolicy), XtRImmediate, (XtPointer) XoBOTTOM_TO_TOP_NICE},
  {XoNlinkColor, "LinkColor", XtRPixel, sizeof(Pixel), 
       XtOffset(XoGraphWidget, graph.link_color), XtRString, "Black"},
  {XoNlinkWidth, "LinkWidth", XtRInt, sizeof(int), 
       XtOffset(XoGraphWidget, graph.link_width), XtRImmediate, (XtPointer) 1},
  {XoNarrowSize, "ArrowSize", XtRInt, sizeof(int), 
       XtOffset(XoGraphWidget, graph.arrow_size), XtRImmediate, (XtPointer) 4},
  {XoNlinkSpreadType, "LinkSpreadType", XtRBoolean, sizeof(Boolean), 
       XtOffset(XoGraphWidget, graph.linkSpreadType), XtRImmediate, (XtPointer) True},
  {XoNcreateLinkCallback, "CreateLinkCallback",XtRCallback,sizeof(XtCallbackList),
       XtOffset(XoGraphWidget,graph.createLinkCallback),XtRImmediate,(XtPointer)NULL},
  {XoNdestroyLinkCallback,"DestroyLinkCallback",XtRCallback,sizeof(XtCallbackList),
       XtOffset(XoGraphWidget,graph.destroyLinkCallback),XtRImmediate,(XtPointer)NULL},
  {XoNselectedLink ,"SelectedLink" , XtRPointer    , sizeof(XtPointer)     ,
       XtOffset(XoGraphWidget,graph.selectedLink)  ,XtRImmediate,NULL},
  {XoNselectedFather ,"SelectedFather" , XtRWidget    , sizeof(Widget)     ,
       XtOffset(XoGraphWidget,graph.selectedFather)  ,XtRImmediate,NULL},
  {XoNselectedSon    ,"SelectedSon" , XtRWidget    , sizeof(Widget)     ,
       XtOffset(XoGraphWidget,graph.selectedSon)  ,XtRImmediate,NULL},
  {XoNlinkedWidgets,"LinkedWidgets", XtRWidgetList, sizeof(WidgetList) ,
       XtOffset(XoGraphWidget, graph.linkedWidgets) ,XtRImmediate,NULL}
};

static char translations[] =
            "\
                  Ctrl <Btn1Down>: GraphDestroyLink()\n\
                    Ctrl <Btn1Up>: GraphDestroyLink()\n\
                       <Btn1Down>: GraphCreateLink()\n\
         ~Ctrl Button1 <PtrMoved>: GraphCreateLink()\n\
                         <Btn1Up>: GraphCreateLink()";

static XtActionsRec actions[] = 
{
  { "GraphCreateLink"      , CreateLinkAction},
  { "GraphDestroyLink"     , DestroyLinkAction}
};

XoGraphClassRec xoGraphClassRec = {
{
/* core_class fields  */
    (WidgetClass) &constraintClassRec,
    "XoGraph",                        /* class_name         */
    sizeof(XoGraphRec),               /* widget_size        */
    InitializeClass,                       /* class_initialize   */
    NULL,                             /* class_part_init    */
    FALSE,                            /* class_inited       */	
    InitializeWidget,                      /* initialize         */
    NULL,                             /* initialize_hook    */	
    XtInheritRealize,                 /* realize            */
    actions,     		      /* actions            */
    XtNumber(actions),                /* num_actions        */	
    resources,                        /* resources          */
    XtNumber(resources),              /* num_resources      */
    NULLQUARK,                        /* xrm_class          */
    TRUE,                             /* compress_motion    */	
    TRUE,                             /* compress_exposure  */	
    TRUE,                             /* compress_enterleave*/	
    TRUE,                             /* visible_interest   */
    DestroyWidget,	              /* destroy            */
    XtInheritResize,                  /* resize             */
    DrawWidget,                        /* expose             */
    SetValues,                        /* SetValues         */
    NULL,                             /* SetValues_hook    */	
    XtInheritSetValuesAlmost,         /* SetValues_almost  */
    NULL,                             /* get_values_hook    */	
    XtInheritAcceptFocus,             /* accept_focus       */
    XtVersion,                        /* version            */	
    NULL,                             /* callback_private   */
    translations,	              /* state machine      */
    XtInheritQueryGeometry,           /* preferred geometry */
    XtInheritDisplayAccelerator,      /* your accelerator   */
    NULL                              /* extension          */
},
{
/* composite_class fields */
    GeometryManager,                 /* geometry children   */
    XtInheritChangeManaged,          /* change_managed      */
    XtInheritInsertChild,            /* insert_child        */	
    XtInheritDeleteChild,            /* delete_child        */	
    NULL                             /* extension           */
},
{ 
/* constraint_class fields */
   NULL,                             /* subresources        */
   0,                                /* subresource_count   */
   sizeof(GraphConstraintsRec),      /* constraint_size     */
   InitializeConstraint,             /* initialize          */
   DestroyConstraint,                /* destroy             */
   SetConstraintValues,              /* SetValues          */
   NULL                              /* extension           */
},
{
/* Graph class fields */
    NULL
}
};
WidgetClass xoGraphWidgetClass = (WidgetClass) &xoGraphClassRec;
/***************************************************************************/
static void InitializeClass (
 void
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  XtAddConverter (XtRString,XoRLinkingPolicy,(XtConverter)ConvertStringToGraphPolicy,NULL,0);
  XtAddConverter (XoRLinkingPolicy,XtRString,(XtConverter)ConvertGraphPolicyToString,NULL,0);
}
/***************************************************************************/
static void InitializeWidget (
 Widget    a_request
,Widget    This
,ArgList   foo
,Cardinal* dummy
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  XGCValues values;
  XtGCMask  valueMask = GCForeground | GCBackground | GCLineWidth;
/*.........................................................................*/
  if (a_request->core.width  <= 0)    This->core.width  = 50;
  if (a_request->core.height <= 0)    This->core.height = 50;

  values.foreground = athis.link_color;
  values.line_width = athis.link_width;
  values.background = This->core.background_pixel;
  athis.gc          =  XtGetGC (This, valueMask, &values);
  LkListInit        (&(athis.links));

  if(athis.linkedWidgets!=NULL)
    { 
      int                    count,nwidget;
      athis.linkedWidgets    = (WidgetList)CListDuplicate((CList)athis.linkedWidgets);
      LkListClear            (&(athis.links),(LkFunction)DeleteLink);
      nwidget                = CListGetSize ((CList)athis.linkedWidgets);
      for(count=0;count<nwidget;count+=2)
        {
          CreateLink (athis.linkedWidgets[count],athis.linkedWidgets[count+1]);
        }
    }

  foo   = NULL;
  dummy = NULL;
} 
/***************************************************************************/
static void DestroyWidget (
 Widget This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  LkListClear           (&(athis.links),(LkFunction)DeleteLink);
  XtRemoveAllCallbacks  (This, XoNcreateLinkCallback);
  XtRemoveAllCallbacks  (This, XoNdestroyLinkCallback);
  CListDelete            ((CList)athis.linkedWidgets);
  athis.linkedWidgets   = NULL;
}
/***************************************************************************/
static void InitializeConstraint (
 Widget request
,Widget New
,ArgList foo
,Cardinal* dummy
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  GraphConstraints graph_const;
/*.........................................................................*/
  if(New==NULL)    return;
  graph_const = GRAPH_CONSTRAINT(New);
  LkListInit  (&graph_const->graph.fathers);
  LkListInit  (&graph_const->graph.sons);
  foo         = NULL;
  dummy       = NULL;
  request     = NULL;
} 
/***************************************************************************/
static Boolean SetConstraintValues (
 Widget current
,Widget request
,Widget This
,ArgList foo
,Cardinal* dummy
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if (current->core.x!=This->core.x || current->core.y!=This->core.y)
    {
      if (XtIsRealized(This->core.parent)) 
         XClearWindow((Display*) XtDisplay(This->core.parent),XtWindow(This->core.parent));
      XtMoveWidget (current, This->core.x, This->core.y);
      foo          = NULL;
      dummy        = NULL;
    }
 request = NULL;
 return  False;
}
/***************************************************************************/
static Boolean SetValues (
 Widget    a_current
,Widget    a_request
,Widget    This
,ArgList   args
,Cardinal* narg
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  unsigned int  i;
  Boolean       rv = False;
  XGCValues     values;
  XtGCMask      valueMask;
  LkIterator	links;
  XoLink   link;
  XoGraphWidget current;
/*.........................................................................*/
  current       = (XoGraphWidget) a_current;

  IFLMOD(linkedWidgets)
    { 
      int                    nwidget,count;
      CListDelete             ((CList)acur.linkedWidgets);
      athis.linkedWidgets    = (WidgetList)CListDuplicate((CList)athis.linkedWidgets);
      LkListClear            (&(athis.links),(LkFunction)DeleteLink);
      nwidget                = CListGetSize ((CList)athis.linkedWidgets);
      for(count=0;count<nwidget;count+=2)
        {
          CreateLink (athis.linkedWidgets[count],athis.linkedWidgets[count+1]);
        }
      rv = True;
    }
  else {athis.linkedWidgets = acur.linkedWidgets;}

  if (XtIsRealized(This))
  {
    if (*narg!=0)
    {
      if (athis.link_color != current->graph.link_color ||
          athis.link_width != current->graph.link_width ||
          This->core.background_pixel != current->core.background_pixel)
      {
        valueMask = GCForeground | GCBackground | GCLineWidth;
        values.foreground = athis.link_color;
        values.line_width = athis.link_width;
        values.background = This->core.background_pixel;
        athis.gc          =  XtGetGC (This, valueMask, &values);
        links             = LkIteratorNew (&(athis.links));
        while ((link=(XoLink)LkIteratorNext(links))!=NULL)
        {
          if (link->gc == current->graph.gc)
	    link->gc = athis.gc;
        }
        LkIteratorDelete(links);
        rv = True;
      }

      for (i=0;i<*narg;i++)
      {
        if ( (strcmp(args[i].name, XoNlinksVisible)==0)  || 
             (strcmp(args[i].name, XoNlinkingPolicy)==0) || 
             (strcmp(args[i].name, XoNarrowSize)==0)     || 
             (strcmp(args[i].name, XoNlinkSpreadType)==0)  
	    )
        {
          XClearWindow((Display*) XtDisplay(This),XtWindow(This));
          rv = True;
          break;
        }
      }
    }
  }
  a_request = NULL;
  return    rv;
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
/***************************************************************************/
/***************************************************************************/
XoGraphIsValidWidgetFunction XoGraphSetIsValidWidgetFunction (
 XoGraphIsValidWidgetFunction a_isValidWidgetFunction
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  XoGraphIsValidWidgetFunction old;
/*.........................................................................*/
  old    = Class.isValidWidgetFunction;
  Class.isValidWidgetFunction = a_isValidWidgetFunction;
  return old;
}
/***************************************************************************/
char** XoGraphGetResourceOptions (
 char* a_type 
,int* a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_number!=NULL) *a_number = 0;
  if(a_type==NULL)  return NULL;
  if(strcmp(a_type,XoRLinkingPolicy)==0)
    {
      static char* graph_policy[2] = {
"bottom_to_top_direct",
"bottom_to_top_nice"
};
      if(a_number!=NULL) *a_number = 2;
      return       graph_policy;
    }
  return NULL;
}
/***************************************************************************/
void XoGraphDraw (
 Widget This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  if(!XtIsSubclass(This,xoGraphWidgetClass))  return;
  DrawWidget (This, NULL, NULL);
}
/***************************************************************************/
XoLink XoGraphGetSelectedLink (
 Widget This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)                                   return NULL;
  if(!XtIsSubclass(This,xoGraphWidgetClass))  return NULL;
  return (XoLink)athis.selectedLink;
}
/***************************************************************************/
Widget XoGraphGetSelectedFather (
 Widget This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)                                   return NULL;
  if(!XtIsSubclass(This,xoGraphWidgetClass))  return NULL;
  return athis.selectedFather;
}
/***************************************************************************/
Widget XoGraphGetSelectedSon (
 Widget This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)                                   return NULL;
  if(!XtIsSubclass(This,xoGraphWidgetClass))  return NULL;
  return athis.selectedSon;
}
/***************************************************************************/
WidgetList XoGraphGetNodeSons (
 Widget This
,int* a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  GraphConstraints wconst;
  LkList           links;
  int              number;
  LkEntry          son;
  WidgetList       list;
  LkIterator       iterator;
/*.........................................................................*/
  if(a_number!=NULL) *a_number = 0;
  if(This==NULL) return NULL;
  if( (XtParent(This)==NULL) || 
      !XtIsSubclass(XtParent(This),xoGraphWidgetClass)
    ) return NULL;

  wconst    = GRAPH_CONSTRAINT(This);
  links     = (LkList) (&(wconst->graph.sons));
  number    = LkListCount (links);
  list      = (WidgetList)CListCreate (number);
  number    = 0;
  iterator  = LkIteratorNew (links);
  while ((son=LkIteratorNext(iterator))!=NULL)
    {
       list[number] = GetLinkFromSon(son)->sonNode;
       number++;
    }
  LkIteratorDelete (iterator);
  if(a_number!=NULL) *a_number        = number;
  return           list;
}
/***************************************************************************/
WidgetList XoGraphGetNodeFathers (
 Widget This
,int* a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  GraphConstraints wconst;
  LkList           links;
  int              number;
  LkEntry          father;
  WidgetList       list;
  LkIterator       iterator;
/*.........................................................................*/
  if(a_number!=NULL) *a_number = 0;
  if(This==NULL) return NULL;
  if( (XtParent(This)==NULL) || 
      !XtIsSubclass(XtParent(This),xoGraphWidgetClass)
     ) return NULL;

  wconst    = GRAPH_CONSTRAINT(This);
  links     = (LkList)    (&(wconst->graph.fathers));
  number    = LkListCount (links);
  list      = (WidgetList)CListCreate (number);
  number    = 0;
  iterator  = LkIteratorNew (links);
  while ((father=LkIteratorNext(iterator))!=NULL)
    {
       list[number] = GetLinkFromFather(father)->fatherNode;
       number++;
    }
  LkIteratorDelete (iterator);
  if(a_number!=NULL) *a_number        = number;
  return           list;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static void UpdateLinkedItems (
 Widget This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  WidgetList  list = NULL;
  LkIterator  links;
  XoLink link;
/*.........................................................................*/
  if(This==NULL) return;
  links = LkIteratorNew (&(athis.links));
  while ((link=(XoLink)LkIteratorNext(links))!=NULL)
    { 
      CListAddEntry((CList*)&list,link->fatherNode);
      CListAddEntry((CList*)&list,link->sonNode);
    }
  LkIteratorDelete       (links);
  CListDelete             ((CList)athis.linkedWidgets);
  athis.linkedWidgets    = list;
}
/***************************************************************************/
static void DestroyConstraint (
 Widget This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  GraphConstraints wconst;
  LkEntry          son, father;
  LkList           links;
/*.........................................................................*/
  if(This==NULL) return;
  wconst = GRAPH_CONSTRAINT(This);
  links = (LkList) (&(wconst->graph.sons));
  while ((son=LkListFirstEntry(links))!=NULL)
    {
      DeleteLink (GetLinkFromSon (son));
      CMemoryFreeBlock    (GetLinkFromSon (son));
    }

  links = (LkList) (&(wconst->graph.fathers));
  while ((father=LkListFirstEntry(links))!=NULL)
    {
      DeleteLink (GetLinkFromFather (father));
      CMemoryFreeBlock    (GetLinkFromFather (father));
    }
 {Widget          graph = This->core.parent;
  if (XtIsRealized(graph)) 
    XClearWindow(XtDisplay(graph),XtWindow(graph));
  DrawWidget         (graph, NULL, NULL);}
}
/***************************************************************************/
static XoLink GetLinkFromFather (
 LkEntry This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*  y0 = NULL;
  char*  x0;
  int    offset; 
/*.........................................................................*/
  x0     = (char*) & ((XoLink)y0)->father;
  offset = x0-y0;
  return ((XoLink) ((char*) This - offset ));
} 
/***************************************************************************/
static XoLink GetLinkFromSon (
 LkEntry This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*  y0 = NULL;
  char*  x0;
  int    offset; 
/*.........................................................................*/
  x0     = (char*) & ((XoLink)y0)->son;
  offset = x0-y0;
  return ((XoLink) ((char*) This - offset ));
}
/***************************************************************************/
static void DrawWidget (
 Widget w
,XEvent* event
,Region region
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  XoGraphWidget This = (XoGraphWidget) w;
/*.........................................................................*/
  if (XtIsRealized(w)) XClearWindow((Display*)XtDisplay(w),XtWindow(w));

  if ((This->core.visible==True) && (This->graph.linksVisible==True))
    {
      LkIterator  links;
      XoLink link;
      links = LkIteratorNew (&(This->graph.links));
      while ((link=(XoLink)LkIteratorNext(links))!=NULL)
	{
	  DrawLink (link);
	}
      LkIteratorDelete (links);
    }
  event  = NULL;
  region = NULL;
}
/***************************************************************************/
static void DrawLink (
 XoLink This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget graph, sonNode, fatherNode;
  int    graph_policy,arrow_size;
  short  fatherx, fathery, sonx, sony;
  XPoint points[6];
  int    npoint;
/*.........................................................................*/
  if(This==NULL)  return;
  sonNode    = This->sonNode;
  fatherNode = This->fatherNode;
  graph      = sonNode->core.parent;
  
  if(((XoGraphWidget)graph)->graph.linksVisible==False) return;
  if(!XtIsRealized(fatherNode)) return;
  if(!XtIsRealized(sonNode))    return;

 {XWindowAttributes watbs;
  XGetWindowAttributes (XtDisplay(fatherNode),XtWindow(fatherNode),&watbs);
  if(watbs.map_state!=IsViewable) return;
  XGetWindowAttributes (XtDisplay(sonNode),XtWindow(sonNode),&watbs);
  if(watbs.map_state!=IsViewable) return;}

  GetLinkInfos   (This,&graph_policy,&arrow_size,&fatherx,&fathery,&sonx,&sony);

  npoint       = GetBeginLinkPoints (arrow_size,fatherx,fathery,points);

  XFillPolygon (XtDisplay(graph), XtWindow(graph),
	       This->gc,
	       points,
	       npoint,
               Convex,
	       CoordModeOrigin);

  npoint      = GetEndLinkPoints (arrow_size,sonx,sony,points);

  XFillPolygon (XtDisplay(graph), XtWindow(graph),
	       This->gc,
	       points,
	       npoint,
               Convex,
	       CoordModeOrigin);

  npoint      = GetLinkPoints (graph_policy,fatherx,fathery,sonx,sony,points);

  XDrawLines  (XtDisplay(graph), XtWindow(graph),
	       This->gc,
	       points,
	       npoint,
	       CoordModeOrigin);
}
/***************************************************************************/
static void GetLinkInfos (
 XoLink This
,int*   a_policy
,int*   a_arrow_size
,short* a_father_x
,short* a_father_y
,short* a_son_x
,short* a_son_y
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget graph, sonNode, fatherNode;
  short  fatherx, fathery, sonx, sony;
  int    pos;
  float  prop;
/*.........................................................................*/
  sonNode    = This->sonNode;
  fatherNode = This->fatherNode;
  graph      = sonNode->core.parent;
  if (( (XoGraphWidget) graph )->graph.linkSpreadType==True)
    prop = ( 1.0 + (float) LkEntryPosition(&This->son) ) /
           ( 1.0 + (float) LkListCount(LkEntryFather((&(This->son))) ) ) ;
  else
    prop = 0.5;
  pos     = (int) ( ( (float) fatherNode->core.width ) * prop );
  fatherx = fatherNode->core.x + pos;
  fathery = fatherNode->core.y + fatherNode->core.height;

  if ( ( (XoGraphWidget) graph )->graph.linkSpreadType==True)
    prop = ( 1.0 + (float) LkEntryPosition(&This->father) ) /
	   ( 1.0 + (float) LkListCount(LkEntryFather((&(This->father))) ) ) ;
  else
    prop = 0.5;
  pos  = (int) ( ( (float) sonNode->core.width ) * prop );
  sonx = sonNode->core.x + pos;
  sony = sonNode->core.y;

  *a_policy     = ((XoGraphWidget)graph)->graph.linkingPolicy;
  *a_arrow_size = ((XoGraphWidget)graph)->graph.arrow_size;
  *a_father_x   = fatherx;
  *a_father_y   = fathery;
  *a_son_x      = sonx;
  *a_son_y      = sony;
}
/***************************************************************************/
static int GetLinkPoints (
 int a_policy
,short a_father_x
,short a_father_y
,short a_son_x
,short a_son_y
,XPoint* points
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  short  delta;
  int    point_count = 0;
/*.........................................................................*/
  switch (a_policy)
  {
    case XoBOTTOM_TO_TOP_DIRECT:
      points[point_count].x = a_father_x;
      points[point_count].y = a_father_y;
      point_count++;
      points[point_count].x = a_son_x;
      points[point_count].y = a_son_y;
      point_count++;
      return point_count;
    case XoBOTTOM_TO_TOP_NICE:
      points[point_count].x = a_father_x;
      points[point_count].y = a_father_y;
      point_count++;
      delta = a_son_y-a_father_y;
      if (delta>0)
      {
         points[point_count].x = a_father_x;
	 points[point_count].y = a_father_y+(delta/2);
 	 point_count++;
	 points[point_count].x = a_son_x;
	 points[point_count].y = a_father_y+(delta/2);
	 point_count++;
	 points[point_count].x = a_son_x;
	 points[point_count].y = a_son_y;
	 point_count++;
       }
       else
       {
         delta = a_son_x - a_father_x;
         points[point_count].x = a_father_x;
	 points[point_count].y = a_father_y+20;
         point_count++;
         points[point_count].x = a_father_x+(delta/2);
	 points[point_count].y = a_father_y+20;
         point_count++;
	 points[point_count].x = a_father_x+(delta/2);
	 points[point_count].y = a_son_y-20;
	 point_count++;
	 points[point_count].x = a_son_x;
	 points[point_count].y = a_son_y-20;
	 point_count++;
	 points[point_count].x = a_son_x;
	 points[point_count].y = a_son_y;
	 point_count++;
       }
      return point_count;
  }
  return 0;
}
/***************************************************************************/
static int GetBeginLinkPoints (
 int a_arrow_size
,short a_x
,short a_y
,XPoint* points
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int    point_count = 0;
/*.........................................................................*/
  if(a_arrow_size<=0) return 0;
  points[point_count].x = a_x - a_arrow_size;
  points[point_count].y = a_y;
  point_count++;
  points[point_count].x = a_x + a_arrow_size;
  points[point_count].y = a_y;
  point_count++;
  points[point_count].x = a_x;
  points[point_count].y = a_y + 2 * a_arrow_size;
  point_count++;
  return point_count;
}
/***************************************************************************/
static int GetEndLinkPoints (
 int a_arrow_size
,short a_x
,short a_y
,XPoint* points
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int    point_count = 0;
/*.........................................................................*/
  if(a_arrow_size<=0) return 0;
/*
  points[point_count].x = a_x - a_arrow_size;
  points[point_count].y = a_y;
  point_count++;
  points[point_count].x = a_x;
  points[point_count].y = a_y - 2 * a_arrow_size;
  point_count++;
  points[point_count].x = a_x + a_arrow_size;
  points[point_count].y = a_y;
  point_count++;
*/
  points[point_count].x = a_x - a_arrow_size;
  points[point_count].y = a_y - 2 * a_arrow_size;
  point_count++;
  points[point_count].x = a_x + a_arrow_size;
  points[point_count].y = a_y - 2 * a_arrow_size;
  point_count++;
  points[point_count].x = a_x;
  points[point_count].y = a_y;
  point_count++;

  return point_count;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
XoLink XoLinkGetIdentifierFromFatherAndSon (
 Widget This
,Widget a_father
,Widget a_son
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  LkIterator	  links;
  XoLink     link;
/*.........................................................................*/
  if(This==NULL) return NULL;
  links = LkIteratorNew (&(((XoGraphWidget)This)->graph.links));
  while ((link=(XoLink)LkIteratorNext(links))!=NULL)
    {
       if( (link->fatherNode==a_father) && 
	   (link->sonNode   ==a_son) )  {LkIteratorDelete (links);return link;}
    }
  LkIteratorDelete (links);
  return NULL;
}
/***************************************************************************/
XoLink XoLinkCreate (
 Widget w1
,Widget w2
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  XoLink link;
/*.........................................................................*/
  link                  = CreateLink (w1,w2);
  UpdateLinkedItems   (XoLinkGetGraph(link));
  return                link;
}
/***************************************************************************/
void XoLinkDelete (
 XoLink This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget                  graph;
/*.........................................................................*/
  if(This==NULL)               return;
  graph                   = This->fatherNode!=NULL ? (This->fatherNode)->core.parent : (Widget)NULL;
  DeleteLink             (This);
  CMemoryFreeBlock                (This);
  if(graph==NULL)              return;
  UpdateLinkedItems     (graph);
  if (XtIsRealized(graph)) XClearWindow(XtDisplay(graph),XtWindow(graph));
  DrawWidget               (graph, NULL, NULL);
}
/***************************************************************************/
Widget XoLinkGetGraph (
 XoLink This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->fatherNode!=NULL ? (This->fatherNode)->core.parent : (Widget)NULL;
}
/***************************************************************************/
Widget XoLinkGetFather (
 XoLink This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->fatherNode;
}
/***************************************************************************/
Widget XoLinkGetSon (
 XoLink This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->sonNode;
}
/***************************************************************************/
static XoLink CreateLink (
 Widget w1
,Widget w2
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  XoLink   This;
  XoGraphWidget graph;
/*.........................................................................*/
  if(Class.isValidWidgetFunction!=NULL)
    {
      if(Class.isValidWidgetFunction(w1)==False) return NULL;
      if(Class.isValidWidgetFunction(w2)==False) return NULL;
    }

  if (w1->core.parent != w2->core.parent) return NULL;
  graph = (XoGraphWidget) w1->core.parent;

  This  = (XoLink) CMemoryAllocateBlock(sizeof(XoLinkRecord));
  if(This==NULL) return NULL;

  LkEntryInit (&(This->managed), NULL);
  LkEntryInit (&(This->son)    , NULL);
  LkEntryInit (&(This->father) , NULL);

  This->sonNode    = w2;
  This->fatherNode = w1;
  This->gc         = graph->graph.gc;

  ManageLink  (graph, w1, w2, This);

  if (XtIsRealized(w1->core.parent))
    {
      if (graph->graph.linkSpreadType==True)
	{
	  XClearWindow((Display*) XtDisplay(graph),XtWindow(graph));
	  DrawWidget((Widget) graph, NULL, NULL);
	}
      else  
	DrawLink (This);
    }
  return This;
}
/***************************************************************************/
static void DeleteLink (
 XoLink This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)               return;
  if(This->fatherNode!=NULL)
    {
      XoGraphWidget graph;
      graph         = (XoGraphWidget)((This->fatherNode)->core.parent);
      UnmanageLink  (graph,This->fatherNode,This->sonNode, This);
    }
  This->gc          = NULL;
  This->fatherNode  = NULL;
  This->sonNode     = NULL;
}
/***************************************************************************/
static void ManageLink (
 XoGraphWidget w
,Widget w1
,Widget w2
,XoLink This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  if(w!=NULL) LkListConnectEntry (&(w->graph.links)        , &This->managed);
  if(w1!=NULL)
    {
      GraphConstraints w1const;
      w1const = GRAPH_CONSTRAINT(w1);
      if(w1const!=NULL) LkListConnectEntry (&(w1const->graph.sons)   , &This->son);
    }
  if(w2!=NULL)
    {
      GraphConstraints w2const;
      w2const = GRAPH_CONSTRAINT(w2);
      if(w2const!=NULL) LkListConnectEntry (&(w2const->graph.fathers), &This->father);
    }
}
/***************************************************************************/
static void UnmanageLink (
 XoGraphWidget w
,Widget w1
,Widget w2
,XoLink This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  LkEntryDisconnect (&This->managed);
  LkEntryDisconnect (&This->son);
  LkEntryDisconnect (&This->father);
  w  = NULL;
  w1 = NULL;
  w2 = NULL;
}
/***************************************************************************/
static void ConvertGraphPolicyToString (
 XrmValuePtr a_args
,int*        a_argn
,XrmValuePtr a_source
,XrmValuePtr a_dest
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  static char string[32];
/*.........................................................................*/
  switch (*((int *)a_source->addr))
  {
    case XoBOTTOM_TO_TOP_DIRECT:
      sprintf      (string,"bottom_to_top_direct");
      break;
    case XoBOTTOM_TO_TOP_NICE:
      sprintf      (string,"bottom_to_top_nice");
      break;
    default:
      sprintf      (string,"bottom_to_top_nice");
      break;
  }
  a_dest->size = (unsigned int)(strlen(string)+1);  
  a_dest->addr = (XPointer)string;      
  a_args       = NULL; 
  a_argn       = 0;
}
/***************************************************************************/
static void ConvertStringToGraphPolicy (
 XrmValuePtr a_args
,int*        a_argn
,XrmValuePtr a_source
,XrmValuePtr a_dest
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  static int value = XoBOTTOM_TO_TOP_NICE;
/*.........................................................................*/
  if(a_source->addr==NULL) return;
  if(strcmp(a_source->addr,"bottom_to_top_direct")==0) value = XoBOTTOM_TO_TOP_DIRECT;
  else                                                 value = XoBOTTOM_TO_TOP_NICE;
  a_dest->addr = (char *)&value;
  a_dest->size = sizeof(int);
  a_args = NULL;
  a_argn = 0;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static void CreateLinkAction (
 Widget This
,XEvent* a_event
,String* a_args
,Cardinal* a_argn
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  static  struct  
  {
    Widget father,son;
    short  endx,endy;
    short  father_x,father_y;
    short  son_x,son_y;
  } pointer = {NULL,NULL,0,0,0,0,0,0};
  Widget widget;
/*.........................................................................*/
  if(This==NULL) return;
  switch(a_event->type)
    {
      case ButtonPress:
            if(Class.cursor_sb_up_arrow==0L)    
	      Class.cursor_sb_up_arrow    = XCreateFontCursor(XtDisplay(This),XC_sb_up_arrow);
            if(Class.cursor_top_left_arrow==0L) 
	      Class.cursor_top_left_arrow = XCreateFontCursor(XtDisplay(This),XC_top_left_arrow);
            XDefineCursor    (XtDisplay(This),XtWindow(This),Class.cursor_sb_up_arrow) ;
            pointer.father   = NULL;
            pointer.son      = NULL;
            pointer.endx     = 0;
            pointer.endy     = 0;
            pointer.father_x = 0;
            pointer.father_y = 0;
            pointer.son_x    = 0;
            pointer.son_y    = 0;
            break;
      case ButtonRelease:
            DrawFatherToCursor (This,pointer.father,pointer.endx,pointer.endy);
            ShowNeightbour       (This,pointer.son);
            ShowNeightbour       (This,pointer.father);
            if( (pointer.father!=NULL) && (pointer.son!=NULL) )
              { 
                if(((XoGraphWidget)This)->graph.createLinkCallback!=NULL)
                  { 
		    XoGraphCallbackStruct   value;
                    value.reason            = XoCR_CREATE_LINK;
                    value.event             = a_event;
                    value.link              = NULL;
                    value.father            = pointer.father;
                    value.son               = pointer.son;
                    ((XoGraphWidget)This)->graph.selectedLink   = NULL;
                    ((XoGraphWidget)This)->graph.selectedFather = pointer.father;
                    ((XoGraphWidget)This)->graph.selectedSon    = pointer.son;
                    XtCallCallbacks         (This, XoNcreateLinkCallback,(XtPointer)&value);
                  }
                else
                  XoLinkCreate (pointer.father,pointer.son);
              }
            pointer.father     = NULL;
            pointer.son        = NULL;
            pointer.endx       = 0;
            pointer.endy       = 0;
            pointer.father_x   = 0;
            pointer.father_y   = 0;
            pointer.son_x      = 0;
            pointer.son_y      = 0;
            XDefineCursor      (XtDisplay(This),XtWindow(This),Class.cursor_top_left_arrow) ;
            break;
      case MotionNotify:
            /* search a neightbour child widget */
            DrawFatherToCursor (This,pointer.father,pointer.endx,pointer.endy);
            widget                = SearchNeightbour (This,a_event->xmotion.x,a_event->xmotion.y);
            if(widget!=NULL) 
              {
                if(pointer.father==NULL)
                  {
                    pointer.father   = widget;
                    pointer.father_x = a_event->xmotion.x;
                    pointer.father_y = a_event->xmotion.y;
                    ShowNeightbour  (This,pointer.father);
                  }
                else 
                  {
                    if(pointer.son==NULL)
                      {
                        pointer.son   = widget;
                        pointer.son_x = a_event->xmotion.x;
                        pointer.son_y = a_event->xmotion.y;
                        if(pointer.son!=pointer.father) ShowNeightbour (This,pointer.son);
                      }
                  }
              }
            else /* out of any child */
              {
                if(pointer.son!=NULL) /* unselect son */
                  {
                    if(pointer.son!=pointer.father) ShowNeightbour (This,pointer.son);
                    pointer.son     = NULL;
                  }
              }
            pointer.endx          = a_event->xmotion.x;
            pointer.endy          = a_event->xmotion.y;
            DrawFatherToCursor (This,pointer.father,pointer.endx,pointer.endy);
            break;
    }
  a_args = NULL; /* to remove warning of c++ compiler */
  a_argn = 0; /* to remove warning of c++ compiler */
}
/***************************************************************************/
static Widget SearchNeightbour (
 Widget This
,int a_x
,int a_y
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int        number;
  WidgetList list;
   int count;
/*.........................................................................*/
  number     = ((CompositeWidget)This)->composite.num_children;
  list       = ((CompositeWidget)This)->composite.children;
  if( (number==0) || (list==NULL) ) return NULL;
  for(count=0;count<number;count++) 
    { Widget widget;
      widget = list[count];
      if ( (widget->core.x-BORDER<=a_x) && (a_x<(widget->core.x+(Position)widget->core.width +BORDER)) &&
           (widget->core.y-BORDER<=a_y) && (a_y<(widget->core.y+(Position)widget->core.height+BORDER)) 
         )
        return widget;
     }
  return NULL;
}
/***************************************************************************/
static void ShowNeightbour (
 Widget This
,Widget a_widget
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  XGCValues arggc;
  GC        xgc;
  Display*  xdisp;
  Window    xwindow;
/*.........................................................................*/
  if(a_widget==NULL)  return;
  xdisp          = XtDisplay (This);
  xwindow        = XtWindow  (This);

  arggc.function       = GXxor;
  arggc.background     = XWhitePixelOfScreen(XDefaultScreenOfDisplay(xdisp));
  arggc.foreground     = 0xffffffff;
  arggc.plane_mask     = XWhitePixelOfScreen(XDefaultScreenOfDisplay(xdisp)) ^ 
                         XBlackPixelOfScreen(XDefaultScreenOfDisplay(xdisp));
  arggc.subwindow_mode = IncludeInferiors;
  xgc                  = (GC) XCreateGC (xdisp,
                                            XDefaultRootWindow(xdisp),
                                            GCFunction|GCForeground|GCBackground|GCPlaneMask|GCSubwindowMode,&arggc);

  XFillRectangle   (xdisp,xwindow,xgc,
                    a_widget->core.x-BORDER,
                    a_widget->core.y-BORDER,
                    a_widget->core.width +2*BORDER,
                    a_widget->core.height+2*BORDER);

  XFreeGC (xdisp,xgc);
}
/***************************************************************************/
static void DrawFatherToCursor (
 Widget This
,Widget a_father
,short a_x
,short a_y
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  XGCValues arggc;
  GC        xgc;
  Display*  xdisp;

  short     fatherx, fathery;
  int       pos;
  float     prop;

  XPoint    points[6];
  int       npoint;
/*.........................................................................*/
  if(a_father==NULL) return;
  xdisp      = XtDisplay (This);

  arggc.function       = GXxor;
  arggc.background     = XWhitePixelOfScreen(XDefaultScreenOfDisplay(xdisp));
  arggc.foreground     = 0xffffffff;
  arggc.plane_mask     = XWhitePixelOfScreen(XDefaultScreenOfDisplay(xdisp)) ^ 
                         XBlackPixelOfScreen(XDefaultScreenOfDisplay(xdisp));
  arggc.subwindow_mode = IncludeInferiors;
  xgc                  = (GC) XCreateGC (xdisp,
                                            XDefaultRootWindow(xdisp),
                                            GCFunction|GCForeground|GCBackground|GCPlaneMask|GCSubwindowMode,&arggc);

  prop      = 0.5;
  pos       = (int) ( ( (float) a_father->core.width ) * prop );
  fatherx   = a_father->core.x + pos;
  fathery   = a_father->core.y + a_father->core.height;

  npoint    = GetLinkPoints (((XoGraphWidget)This)->graph.linkingPolicy,fatherx,fathery,a_x,a_y,points);

  XDrawLines  (XtDisplay(This), XtWindow(This),
	       xgc,
	       points,
	       npoint,
	       CoordModeOrigin);

  XFreeGC (xdisp,xgc);
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static void DestroyLinkAction (
 Widget This
,XEvent* a_event
,String* a_args
,Cardinal* a_argn
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  static  struct  
  {
    XoLink link;
  } pointer = {NULL};
/*.........................................................................*/
  if(This==NULL) return;
  switch(a_event->type)
    {
      case ButtonPress:
            if(Class.cursor_sb_up_arrow==0L)    
	      Class.cursor_sb_up_arrow    = XCreateFontCursor(XtDisplay(This),XC_sb_up_arrow);
            if(Class.cursor_top_left_arrow==0L) 
	      Class.cursor_top_left_arrow = XCreateFontCursor(XtDisplay(This),XC_top_left_arrow);
            XDefineCursor     (XtDisplay(This),XtWindow(This),Class.cursor_sb_up_arrow) ;
            pointer.link      = SearchLink (This,a_event->xbutton.x,a_event->xbutton.y);
            HighlightLink    (pointer.link);
            break;
      case ButtonRelease:
            HighlightLink    (pointer.link);
            if( (pointer.link!=NULL) && 
	        (pointer.link == SearchLink (This,a_event->xbutton.x,a_event->xbutton.y)))
              {
                if(((XoGraphWidget)This)->graph.destroyLinkCallback!=NULL)
                  { 
		    XoGraphCallbackStruct   value;
                    value.reason            = XoCR_DESTROY_LINK;
                    value.event             = a_event;
                    value.link              = pointer.link;
                    value.father            = pointer.link->fatherNode;
                    value.son               = pointer.link->sonNode;
                    ((XoGraphWidget)This)->graph.selectedLink   = (XtPointer)pointer.link;
                    ((XoGraphWidget)This)->graph.selectedFather = pointer.link->fatherNode;
                    ((XoGraphWidget)This)->graph.selectedSon    = pointer.link->sonNode;
                    XtCallCallbacks         (This, XoNdestroyLinkCallback,(XtPointer)&value);
                  }
                else
		  {
		    XoLinkDelete       (pointer.link);
		  }
              }
            pointer.link          = NULL;
            XDefineCursor         (XtDisplay(This),XtWindow(This),Class.cursor_top_left_arrow) ;
            break;
    }
  a_args = NULL; /* to remove warning of c++ compiler */
  a_argn = 0; /* to remove warning of c++ compiler */
}
/***************************************************************************/
static XoLink SearchLink (
 Widget This
,int a_x
,int a_y
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  LkIterator	  links;
  XoLink     link;
/*.........................................................................*/
  if(This==NULL) return NULL;
  links = LkIteratorNew (&(((XoGraphWidget)This)->graph.links));
  while ((link=(XoLink)LkIteratorNext(links))!=NULL)
    {
       if(IsLinkGood(link,a_x,a_y)==True) 
	 {
	   LkIteratorDelete (links);
	   return link;
	 }
    }
  LkIteratorDelete (links);
  return NULL;
}
/***************************************************************************/
static Boolean IsLinkGood (
 XoLink This
,int a_x
,int a_y
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int    graph_policy,arrow_size;
  short  fatherx, fathery, sonx, sony;
  XPoint points[6];
  int    npoint;
   int count;
/*.........................................................................*/
  if(This==NULL) return False;

  GetLinkInfos  (This,&graph_policy,&arrow_size,&fatherx,&fathery,&sonx,&sony);

  npoint      = GetLinkPoints (graph_policy,fatherx,fathery,sonx,sony,points);
  for(count=0;count<npoint-1;count++)
    {
      if(IsOnSegment(a_x,a_y,points[count].x,points[count].y,
		     points[count+1].x,points[count+1].y)==True) return True;
    }
  return False;
}
/***************************************************************************/
static Boolean IsOnSegment (
 int a_x
,int a_y
,short a_beg_x
,short a_beg_y
,short a_end_x
,short a_end_y
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  short  vx,vy;
  double lx,ly;
#define EPSIL 2
/*.........................................................................*/
  vx = a_end_x - a_beg_x;  
  vy = a_end_y - a_beg_y;  
  if((vx==0) && (vy==0)) return False; 
  if(vx==0) 
    {
      if (abs(a_x-a_beg_x)>EPSIL) return False;
      ly     = ((double)(a_y-a_beg_y))/((double)vy);
      return ( (ly>=0.) && (ly<=1.)  ? True : False );
    }
  if(vy==0) 
    {
      if (abs(a_y-a_beg_y)>EPSIL) return False;
      lx     = ((double)(a_x-a_beg_x))/((double)vx);
      return ( (lx>=0.) && (lx<=1.)  ? True : False );
    }
  lx = ((double)(a_x-a_beg_x))/((double)vx);
  ly = ((double)(a_y-a_beg_y))/((double)vy);

  return  ( (fabs(lx-ly) < 0.1) && (lx>=0.) && (lx<=1.) && (ly>=0.) && (ly<=1.) ? True : False );
}
/***************************************************************************/
static void HighlightLink (
 XoLink This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  XGCValues arggc;
  GC        xgc;
  Display*  xdisp;
  Widget    graph, sonNode;
  int       graph_policy,arrow_size;
  short     fatherx, fathery, sonx, sony;
  XPoint    points[6];
  int       npoint;
/*.........................................................................*/
  if(This==NULL) return;

  sonNode    = This->sonNode;
  graph      = sonNode->core.parent;
  
  xdisp      = XtDisplay (graph);

  arggc.function       = GXxor;
  arggc.background     = XWhitePixelOfScreen(XDefaultScreenOfDisplay(xdisp));
  arggc.foreground     = 0xffffffff;
  arggc.plane_mask     = XWhitePixelOfScreen(XDefaultScreenOfDisplay(xdisp)) ^ 
                         XBlackPixelOfScreen(XDefaultScreenOfDisplay(xdisp));
  arggc.subwindow_mode = IncludeInferiors;
  xgc                  = (GC) XCreateGC (xdisp,
					 XDefaultRootWindow(xdisp),
					 GCFunction|GCForeground|GCBackground|GCPlaneMask|GCSubwindowMode,&arggc);

  GetLinkInfos  (This,&graph_policy,&arrow_size,&fatherx,&fathery,&sonx,&sony);

  npoint      = GetBeginLinkPoints (arrow_size,fatherx,fathery,points);

  XFillPolygon (XtDisplay(graph), XtWindow(graph),
	       xgc,
	       points,
	       npoint,
               Convex,
	       CoordModeOrigin);

  npoint      = GetEndLinkPoints (arrow_size,sonx,sony,points);

  XFillPolygon (XtDisplay(graph), XtWindow(graph),
	       xgc,
	       points,
	       npoint,
               Convex,
	       CoordModeOrigin);

  npoint      = GetLinkPoints (graph_policy,fatherx,fathery,sonx,sony,points);

  XDrawLines  (XtDisplay(graph), XtWindow(graph),
	       xgc,
	       points,
	       npoint,
	       CoordModeOrigin);

  XFreeGC (xdisp,xgc);
}
#include <CLkStat.ic>
