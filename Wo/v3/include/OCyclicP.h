/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef OCyclicP_h
#define OCyclicP_h

#include <OCyclic.h>

typedef struct _OCyclicRecord
{
   char*        name;
   char*        widgetName;
   char*        script;
   XtAppContext context;
   int          delay;
   XtIntervalId ident;
} OCyclicRecord;

#endif /*OCyclicP_h*/
