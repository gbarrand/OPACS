#/* +---------------------- Copyright notice -------------------------------+ */
#/* | Copyright (C) 1995, Guy Barrand, LAL Orsay. (barrand@lalcls.in2p3.fr) | */
#/* |   Permission to use, copy, modify, and distribute this software       | */
#/* |   and its documentation for any purpose and without fee is hereby     | */
#/* |   granted, provided that the above copyright notice appear in all     | */
#/* |   copies and that both that copyright notice and this permission      | */
#/* |   notice appear in supporting documentation.  This software is        | */
#/* |   provided "as is" without express or implied warranty.               | */
#/* +---------------------- Copyright notice -------------------------------+ */

This_macros   = \
"bin          = ../$${CONFIG}" \
"mgr          = ../mgr" \
"src          = ../src" \
"app          = ../app" \
"make_so      = sh ${COROOT}/mgr/maklibs.sh" \
"G3o_CPPFLAGS = -I$${COROOT}/include -I$${GOROOT}/include -I../include" \
"GL_CPPFLAGS  = -I$${GLOROOT}/usr/include" \
"libG3o       = -L../$${CONFIG} -lG3o" \
"libG3oEXPO   = -L../$${CONFIG} -lG3oEXPO" \
"libCo        = -L$${COROOT}/$${CONFIG} -lCo" \
"libGL        = -L$${GLOROOT}/$${CONFIG} -lGLo" \
"libGLU       = -L$${GLOROOT}/$${CONFIG} -lGLUo" \
"libGo        = -L$${GOROOT}/$${CONFIG} -lGo" \
"libgeant     = -L$${CERNLIB} -lgeant321" \
"libpacklib   = -L$${CERNLIB} -lpacklib"

include ${COROOT}/mgr/Config.mk

This_needed :
	@if test "${COROOT}"    = "" ; then echo "COROOT not set"    ; exit 1 ; else exit 0 ; fi
	@if test "${GOROOT}"    = "" ; then echo "GOROOT not set"    ; exit 1 ; else exit 0 ; fi
	@if test "${GLOROOT}"   = "" ; then echo "GLOROOT not set"   ; exit 1 ; else exit 0 ; fi
	@if test "${CERNLIB}"   = "" ; then echo "CERNLIB not set"   ; exit 1 ; else exit 0 ; fi

