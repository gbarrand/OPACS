/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef OEventP_h
#define OEventP_h

#include <OEvent.h>
 
typedef struct _OEventRecord
{
   int        type;
   Display*   display;
   Widget     widget;
   char*      string;
   char*      value;
   XEvent*    event;
} OEventRecord;

#endif /*OEventP_h*/
