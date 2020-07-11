/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef OHTMLP_h
#define OHTMLP_h
 
#include <OHTML.h>

typedef struct _OHTML_Record* OHTML;

typedef struct _OHTML_Record
{
  char*   name;
  char*   lineb;
  int     linen;
  char**  lines;
  OProcedureList   pcds;
} OHTML_Record;

#endif  /*OHTMLP_h*/
