/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef OCutP_h
#define OCutP_h

#include <OCut.h>

#define MAX_CUTS 20
typedef struct _OCutRecord
{
  char*       name;
  char*       script;
  OStatement  ostmt;    /*if script is a Ci function*/
  int         cutn;
  struct {
    char*     col_name;
    int       column;
    OCutType  type;
    double    value;
  } cuts[MAX_CUTS];
} OCutRecord;

#endif /*OCutP_h*/
