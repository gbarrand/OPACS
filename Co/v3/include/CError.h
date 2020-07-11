/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef CError_h
#define CError_h

typedef void(*CErrorHandlerFunction)(char*,char*,char*,int);

#define CErrorValue          1
#define CErrorFailure        2
#define CErrorHas            3
#define CErrorFlow           4
#define CErrorIO             5
#define CErrorCase           6
#define CErrorSystem         7
#define CErrorHandler        8
#define CErrorFormat         9
#define CErrorName          10
#define CErrorExisting      11
#define CErrorConversion    12

#ifdef __cplusplus
extern "C"{
#endif
CErrorHandlerFunction CErrorSetHandler (CErrorHandlerFunction);
void                  CErrorHandle     (char*,char*,char*,int);
#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindCError(0); /**/
#endif

#endif  /*CError_h*/
