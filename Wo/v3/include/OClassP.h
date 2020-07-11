/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef OClassP_h
#define OClassP_h

#include <OClass.h>

typedef struct _OClassRecord
{
  char*                      group;
  char*                      name;
  WidgetClass                widget_class;
  char*                      create_name;
  OClassCreateWidgetFunction create_proc;
  char*                      inc_pub;
  OClassVisitWidgetFunction  psProc;
  OClassVisitWidgetFunction  targetProc;
  OClassVoidFunction         clearClassProc;
} OClassRecord;

#endif /*OClassP_h*/
