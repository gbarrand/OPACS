/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef GLoNoH_h
#define GLoNoH_h

#include <stdio.h>
#include <stdarg.h>

#ifdef HAS_X
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#endif /*HAS_X*/

/*This permits to avoid the inclusion of C, X11, Xt, Xm include files.*/

#define NO_C_H
#define NO_X_H

#endif  /*GLoNoH_h*/
