/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef WoWo_h
#define WoWo_h

#define HAS_WO    /*Required.*/

#ifdef HAS_WO

/*Co*/
#include <CSystem.h>
#include <CBatch.h>
#include <CFile.h>
#include <OInterpreter.h>
#include <OShell.h>
#include <CoCommands.h>

/*Xx*/
#include <XxCommands.h>

/*Wo*/
#include <OInterface.h>
#include <WoCommands.h>
#include <Wo.h>

#endif /*HAS_WO*/

#endif /*WoWo_h*/
