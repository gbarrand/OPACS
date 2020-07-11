/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef WoCi_h
#define WoCi_h

#ifdef HAS_CI

#include <Ci.h>
#ifdef __cplusplus
extern "C"{
#endif
CiRoutine CiBindmath      (char*);
CiRoutine CiBindstdarg    (char*);
CiRoutine CiBindstdio     (char*);
CiRoutine CiBindstdlib    (char*);
CiRoutine CiBindstring    (char*);
CiRoutine CiBindtime      (char*);
CiRoutine CiBindCi        (char*);

CiRoutine CiBindXlib      (char*);
CiRoutine CiBindXutil     (char*);
CiRoutine CiBindXresource (char*);
CiRoutine CiBindIntrinsic (char*);
CiRoutine CiBindShell     (char*);
CiRoutine CiBindVendor    (char*);
#ifdef __cplusplus
}
#endif

#include <CoBind.h>
#include <XxBind.h>
#include <WoBind.h>

/*Wo*/
#include <OInterface.h>
#include <OCi.h>
#include <OInterpreter.h>
#include <Wo.h>
#include <CiCommands.h>


#endif /*HAS_CI*/

#endif /*WoCi_h*/
