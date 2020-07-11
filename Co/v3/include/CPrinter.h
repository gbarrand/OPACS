/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef CPrinter_h
#define CPrinter_h

#ifndef NO_C_H
#include <stdarg.h>
#endif

#define CWarn  CPrinterPut
#define CInfo  CPrinterPut
#define CDump  CPrinterPut

#define CWarnF CPrinterPutF
#define CInfoF CPrinterPutF
#define CDumpF CPrinterPutF

#ifdef __cplusplus
extern "C"{
#endif
typedef void(*CPrinterPutFunction)(char*);
typedef void(*CPrinterPutV_Function)(char*,va_list);

CPrinterPutFunction   CPrinterSetPutFunction   (CPrinterPutFunction);
void                  CPrinterPut              (char*);
CPrinterPutV_Function CPrinterSetPutV_Function (CPrinterPutV_Function);
void                  CPrinterPutF             (char*,...);
#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindCPrinter(0); /**/
#endif

#endif  /*CPrinter_h*/
