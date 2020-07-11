/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef OPieceP_h
#define OPieceP_h

#include <OPiece.h>

typedef struct _OPieceRecord
{
   char*     name;
   OPiece    parent;
   char*     stringChildren;
   int       resn;
   char**    rnms;
   char**    rvls;
   int       nchild;
   OPiece*   children;
   char*     type;
   void*     oui;
   char*     dataBaseName;
   int       volatilChild,volatil;
   OPiece    duplicate;
   void*     xtWidget;
} OPieceRecord;


#endif /*OPieceP_h*/
