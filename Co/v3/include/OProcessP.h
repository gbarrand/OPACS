/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef OProcessP_h
#define OProcessP_h

#include <OProcess.h>

typedef struct _OProcessRecord 
{
  void*     osh;
  char*     command;
  int       set_v;
  int       set_x;
  int       pipeActive;
  OHandle*  pipeHands;
  int       argn;
  char**    args;
  int       varn;
  char**    vars; /*variable name*/
  int       valn;
  char**    vals; /*variable value*/
  int       ifStatus,elseStatus;
} OProcessRecord;

#endif /*OProcessP_h*/
