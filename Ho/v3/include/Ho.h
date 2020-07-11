/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef Ho_h
#define Ho_h

typedef void*(*HoConvertFunction)(char*);

#include <OHistogram.h>
#include <OScatter.h>
#include <OTuple.h>
#include <OCut.h>
#include <OFit.h>
#ifdef __cplusplus
extern "C"{
#endif
void       HoClearClass             ();
void       HoAddConverter           (HoConvertFunction);
void*      HoConvert                (char*);
OHistogram HoFillHistogramFromTuple (OHistogram,OTuple,int,char**,int,OCut*,int);
OScatter   HoFillScatterFromTuple   (OScatter,OTuple,int,char**,int,OCut*);
int        HoFitHistogram           (OHistogram,OFit,int,int*,int*);
#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindHo(0); /**/
#endif

#endif  /*Ho_h*/
