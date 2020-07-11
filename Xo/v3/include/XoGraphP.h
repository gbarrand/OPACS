/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef XoGraphP_h
#define XoGraphP_h

#include <XoGraph.h>

typedef struct _XoGraphClassPart {
    void* extension;
} XoGraphClassPart;

typedef struct _XoGraphClassRec {
    CoreClassPart             core_class;
    CompositeClassPart        composite_class;
    ConstraintClassPart       constraint_class;
    XoGraphClassPart          graph_class;
} XoGraphClassRec;

#ifdef __cplusplus
extern "C"{
#endif
extern XoGraphClassRec xoGraphClassRec;
#ifdef __cplusplus
}
#endif

typedef struct _XoGraphPart {
    LkListRec         links;
    Boolean	      linksVisible;
    int 	      linkingPolicy; 
    Pixel             link_color;
    int               link_width;
    int               arrow_size;
    GC                gc;
    Boolean	      linkSpreadType;
    XtCallbackList    createLinkCallback;
    XtCallbackList    destroyLinkCallback;
    XtPointer         selectedLink;
    Widget            selectedFather;
    Widget            selectedSon;
    WidgetList        linkedWidgets;
} XoGraphPart;

typedef struct _XoGraphRec {
    CorePart            core;
    CompositePart       composite;
    ConstraintPart      constraint;
    XoGraphPart         graph;
}  XoGraphRec;

typedef struct _XoLinkRecord {
    LkEntryRec		managed;
    LkEntryRec		son;
    LkEntryRec		father;
    Widget		sonNode;
    Widget		fatherNode;
    GC			gc;
} XoLinkRecord;

typedef struct _GraphConstraintsPart {
    LkListRec		sons;
    LkListRec		fathers;    
} GraphConstraintsPart;

typedef struct _GraphConstraintsRec {
   GraphConstraintsPart    graph;
} GraphConstraintsRec, *GraphConstraints;

#define GRAPH_CONSTRAINT(w) ((GraphConstraints)((w)->core.constraints))

#endif /*XoGraphP_h*/
