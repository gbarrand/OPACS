/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C"{
#endif
void s  (int,...);
void sV (va_list);
#ifdef __cplusplus
}
#endif

#define CInfoF printf
int main()
{
  s      (333,14,3.14,53);
  return 1;
}
void s(int a,...)
{
   int      count = 0;
   va_list  args;
   va_start (args,a);
   for(count=0;count<10;count++)  sV (args);
   va_end   (args);
}
void sV(va_list args)
{
   double   d = 0.;
   int      i = 0,j = 0;
   i        = va_arg(args,int);
   d        = va_arg(args,double);
   j        = va_arg(args,int);
   CInfoF   ("%d %g %d\n",i,d,j); 
}
