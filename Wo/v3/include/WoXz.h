/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef WoXz_h
#define WoXz_h

#ifdef HAS_XZ

#define HAS_KUIP

/*Xz*/
#include <XzPlotter.h>
#include <XzCommands.h>
#ifdef HAS_KUIP
#include <Kuip.h>
#endif 

/*Wo*/
#include <OClass.h>
#include <OInterface.h>

#endif /*HAS_XZ*/

#endif /*WoXz_h*/
