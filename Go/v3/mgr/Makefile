#/* +---------------------- Copyright notice -------------------------------+ */
#/* | Copyright (C) 1995, Guy Barrand, LAL Orsay. (barrand@lalcls.in2p3.fr) | */
#/* |   Permission to use, copy, modify, and distribute this software       | */
#/* |   and its documentation for any purpose and without fee is hereby     | */
#/* |   granted, provided that the above copyright notice appear in all     | */
#/* |   copies and that both that copyright notice and this permission      | */
#/* |   notice appear in supporting documentation.  This software is        | */
#/* |   provided "as is" without express or implied warranty.               | */
#/* +---------------------- Copyright notice -------------------------------+ */

This_macros = \
"bin         = ../$${CONFIG}" \
"mgr         = ../mgr" \
"src         = ../src" \
"app         = ../app" \
"make_so     = sh ${COROOT}/mgr/maklibs.sh" \
"Go_CPPFLAGS = -I$${COROOT}/include -I../include" \
"GL_CPPFLAGS = -DHAS_GL -I$${GLOROOT}/usr/include" \
"APP_CCFLAGS = -DHAS_X" \
"libGo       = -L../$${CONFIG}  -lGo" \
"libCo       = -L$${COROOT}/$${CONFIG}  -lCo" \
"libGL       = -L$${GLOROOT}/$${CONFIG} -lGLo" \
"libGLU      = -L$${GLOROOT}/$${CONFIG} -lGLUo" \
"libGLX      = -L$${GLOROOT}/$${CONFIG} -lGLXo"

include ${COROOT}/mgr/Config.mk

This_needed :
	@if test "${COROOT}"  = "" ; then echo "COROOT not set" ;   exit 1 ; else exit 0 ; fi
	@if test "${GLOROOT}"  = "" ; then echo "GLOROOT not set" ; exit 1 ; else exit 0 ; fi



