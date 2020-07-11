/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef OPartP_h
#define OPartP_h

#include <OPart.h>

typedef struct _OPartRecord
{
   void*          widget;
   char*          type;
   void*          wui_parent;
   char*          createCallback;
   char*          targetChangedCallback;
   char*          eventHandler;
   void*          eventHandlerData;
   int            nbyte;
   unsigned char* resources_changed;
   void*          oui;
   int            volatilChild,volatil;
   void*          duplicate;
} OPartRecord;

#endif  /*OPartP_h*/
