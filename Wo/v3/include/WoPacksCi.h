/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef WoPacksCi_h
#define WoPacksCi_h

#ifdef HAS_CI

#include <Ci.h>

#include <OCi.h>

#include <CoBind.h>
#include <XxBind.h>
#include <WoBind.h>

#ifdef HAS_XM
#include <XmBind.h>
#include <XmTools.h> /*Wo*/
#endif /*HAS_XM*/

#ifdef HAS_XO
#include <HoBind.h>     /*Ho*/
#include <OStatement.h> /*Ho*/
#include <GoBind.h>     /*Go*/
#include <XoBind.h>     /*Xo*/
#include <GL_Bind.h>    /*GL*/
#endif /*HAS_XO*/

#ifdef HAS_XZ
#include <XzBind.h>
#endif /*HAS_XZ*/

#ifdef HAS_G3O
#include <G3oBind.h>
#endif /*HAS_G3O*/

#endif /*HAS_CI*/

#endif /*WoPacksCi_h*/

