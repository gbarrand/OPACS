#/* +---------------------- Copyright notice -------------------------------+ */
#/* | Copyright (C) 1995, Guy Barrand, LAL Orsay. (barrand@lalcls.in2p3.fr) | */
#/* |   Permission to use, copy, modify, and distribute this software       | */
#/* |   and its documentation for any purpose and without fee is hereby     | */
#/* |   granted, provided that the above copyright notice appear in all     | */
#/* |   copies and that both that copyright notice and this permission      | */
#/* |   notice appear in supporting documentation.  This software is        | */
#/* |   provided "as is" without express or implied warranty.               | */
#/* +---------------------- Copyright notice -------------------------------+ */

This_macros    = \
"bin           = ../$${CONFIG}" \
"mgr           = ../mgr" \
"src           = ../src" \
"app           = ../app" \
"make_so       = sh ${COROOT}/mgr/maklibs.sh" \
"Xo_CPPFLAGS    = -I$${COROOT}/include -I$${HOROOT}/include -I$${GOROOT}/include -I$${XXROOT}/include -I../include" \
"XoXm_CPPFLAGS  = -DHAS_XM" \
"XoXaw_CPPFLAGS = -DHAS_XAW" \
"HTML_CPPFLAGS  = -DHAS_HTML -DHAS_MOSAIC -I$${HTMLOROOT}/include -I$${W3OROOT}/include" \
"GL_CPPFLAGS    = -DHAS_GL -I$${GLOROOT}/usr/include" \
"APP_CCFLAGS    = -DHAS_X" \
"libXo         = -L../$${CONFIG}  -lXo" \
"libCo         = -L$${COROOT}/$${CONFIG} -lCo" \
"libHo         = -L$${HOROOT}/$${CONFIG} -lHo" \
"libXx         = -L$${XXROOT}/$${CONFIG} -lXx" \
"libGo         = -L$${GOROOT}/$${CONFIG}  -lGo" \
"libGL         = -L$${GLOROOT}/$${CONFIG} -lGLo" \
"libGLU        = -L$${GLOROOT}/$${CONFIG} -lGLUo" \
"libGLX        = -L$${GLOROOT}/$${CONFIG} -lGLXo" \
"libwww        = -L$${W3OROOT}/$${CONFIG} -lW3o" \
"libhtmlw      = -L$${HTMLOROOT}/$${CONFIG} -lHTMLo" \
"XoCi_CPPFLAGS = -DHAS_CI -I$${CIROOT}/src -I$${CSETROOT}/src" \
"libCi         = -L$${CIROOT}/$${METHODSCONFIG}  -lCiSwitches -lCi " \
"libCSet       = -L$${CSETROOT}/$${METHODSCONFIG}  -lCSet -lReg"

include ${COROOT}/mgr/Config.mk


This_needed :
	@if test "${COROOT}"    = "" ; then echo "COROOT not set"    ; exit 1 ; else exit 0 ; fi
	@if test "${HOROOT}"    = "" ; then echo "HOROOT not set"    ; exit 1 ; else exit 0 ; fi
	@if test "${XXROOT}"    = "" ; then echo "XXROOT not set"    ; exit 1 ; else exit 0 ; fi
	@if test "${GOROOT}"    = "" ; then echo "GOROOT not set"    ; exit 1 ; else exit 0 ; fi
	@
	@if test "${W3OROOT}"   = "" ; then echo "W3OROOT not set"   ; else exit 0 ; fi
	@if test "${HTMLOROOT}" = "" ; then echo "HTMLOROOT not set" ; else exit 0 ; fi
	@if test "${GLOROOT}"   = "" ; then echo "GLOROOT not set"   ; else exit 0 ; fi


