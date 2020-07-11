/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef CTime_h
#define CTime_h

#ifndef NO_C_H
#include <time.h>
#endif

#ifdef __cplusplus
extern "C"{
#endif
void   CTimeStart            ();
int    CTimeGetElapsedPeriod ();
char*  CTimeGetDate          ();
time_t CTimeParseDate        (char*);
char*  CTimeConvertToString  (time_t);
#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindCTime(0); /**/
#endif

#endif  /*CTime_h*/
