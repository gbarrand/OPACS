/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef XoGraph_h
#define XoGraph_h

#ifndef NO_X_H
#include <X11/Intrinsic.h>
#endif

typedef struct _XoGraphClassRec *XoGraphWidgetClass;
typedef struct _XoGraphRec      *XoGraphWidget;
typedef struct _XoLinkRecord  *XoLink;

#define XoBOTTOM_TO_TOP_DIRECT 0
#define XoBOTTOM_TO_TOP_NICE   1

#define XoNcreateLinkCallback    "createLinkCallback"
#define XoNdestroyLinkCallback   "destroyLinkCallback"
#define XoNlinkingPolicy         "linkingPolicy"
#define XoNlinksVisible          "linksVisible"
#define XoNlinkColor             "linkColor"
#define XoNlinkWidth             "linkWidth"
#define XoNlinkSpreadType        "linkSpreadType"
#define XoNselectedLink          "selectedLink"
#define XoNselectedFather        "selectedFather"
#define XoNselectedSon           "selectedSon"
#define XoNlinkedWidgets         "linkedWidgets"
#define XoNarrowSize             "arrowSize"

#define XoRLinkingPolicy         "LinkingPolicy"

typedef struct
{
   int         reason;
   XEvent*     event;
   XoLink link;
   Widget      father;
   Widget      son;
 } XoGraphCallbackStruct;

#define XoCR_NONE        0
#define XoCR_CREATE_LINK  1
#define XoCR_DESTROY_LINK 2

#ifdef __cplusplus
extern "C"{
#endif
typedef Boolean (*XoGraphIsValidWidgetFunction)(Widget);

extern        WidgetClass               xoGraphWidgetClass;

XoGraphIsValidWidgetFunction XoGraphSetIsValidWidgetFunction           (XoGraphIsValidWidgetFunction);
char**        XoGraphGetResourceOptions           (char*,int*);
WidgetList    XoGraphGetNodeSons                  (Widget,int*);
WidgetList    XoGraphGetNodeFathers               (Widget,int*);
void          XoGraphDraw                         (Widget);
XoLink        XoGraphGetSelectedLink              (Widget);
Widget        XoGraphGetSelectedFather            (Widget);
Widget        XoGraphGetSelectedSon               (Widget);

XoLink        XoLinkCreate                        (Widget,Widget);
XoLink        XoLinkGetIdentifierFromFatherAndSon (Widget,Widget,Widget);
void          XoLinkDelete                        (XoLink);
Widget        XoLinkGetGraph                      (XoLink);
Widget        XoLinkGetFather                     (XoLink);
Widget        XoLinkGetSon                        (XoLink);
#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindXoGraph(0); /**/
#endif

#ifdef __CI_SPECIFIC__
#include <XWidget.h> /*thisWidget*/
#define thisSelectedLink           (XoGraphGetSelectedLink(thisWidget))
#define thisSelectedFather         (XoGraphGetSelectedFather(thisWidget))
#define thisSelectedSon            (XoGraphGetSelectedSon(thisWidget))
#endif



#endif /*XoGraph_h*/



