/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#define CInfoF printf

int main()
{
  CInfoF("        int:%d\n",(int)sizeof(int));
  CInfoF("       long:%d\n",(int)sizeof(long));
  CInfoF("      float:%d\n",(int)sizeof(float));
  CInfoF("     double:%d\n",(int)sizeof(double));
  CInfoF("long double:%d\n",(int)sizeof(long double));

#ifdef c__plusplus
  CInfoF("c__plusplus\n");
#endif
#ifdef __cplusplus
  CInfoF("__cplusplus\n");
#endif
#ifdef unix
  CInfoF("unix\n");
#endif
#ifdef _AIX
  CInfoF("_AIX\n");
#endif
#ifdef __sgi
  CInfoF("__sgi\n");
#endif
#ifdef ultrix
  CInfoF("ultrix\n");
#endif
#ifdef __hpux
  CInfoF("__hpux\n");
#endif
#ifdef __alpha
  CInfoF("__alpha\n");
#endif
#ifdef __vax
  CInfoF("__vax\n");
#endif
#ifdef VAX
  CInfoF("VAX\n");
#endif
#if defined(vms) && defined(vax)
  CInfoF("vms vax\n");
#endif
#if defined(vms) && defined(__alpha)
  CInfoF("vms alpha\n");
#endif
#ifdef _WIN32
  CInfoF("_WIN32\n");
#endif
#ifdef __STDC__
  CInfoF("__STDC__\n");
#endif
#ifdef __GNUG__
  CInfoF("__GNUG__\n");
#endif
#ifdef sparc
  CInfoF ("sparc\n");
#endif
#ifdef __svr4__ 
  CInfoF ("__svr4__\n");
#endif
#ifdef __GCC_NEW_VARARGS__ 
  CInfoF ("__GCC_NEW_VARARGS__\n");
#endif
#ifdef __USE_BSD
  CInfoF ("__USE_BSD\n");
#endif
#ifdef __USE_POSIX
  CInfoF ("__USE_POSIX\n");
#endif
#ifdef __linux__
  CInfoF("__linux__\n");
#endif

  return 1;
}
