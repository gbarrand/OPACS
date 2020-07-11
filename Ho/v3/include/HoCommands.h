/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef HoCommands_h
#define HoCommands_h

#include <OShell.h>
#ifdef __cplusplus
extern "C"{
#endif
void HoAddCommands                    (OShell);
int  HoExecute_dumpTuple              (int,char**,OProcess);
int  HoExecute_putInHistogram         (int,char**,OProcess);
int  HoExecute_fillHistogramFromTuple (int,char**,OProcess);
int  HoExecute_operateHistograms      (int,char**,OProcess);
int  HoExecute_fitHistogram           (int,char**,OProcess);
#ifdef __cplusplus
}
#endif

#endif  /*HoCommands_h*/



