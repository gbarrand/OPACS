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
"Xz_CPPFLAGS   = -I$${COROOT}/include -I$${XXROOT}/include -I../include" \
"APP_CCFLAGS   = -DHAS_X" \
"libXz         = -L../$${CONFIG} -lXz" \
"libXzPAWC     = -L../$${CONFIG} -lXzPAWC" \
"libCo         = -L$${COROOT}/$${CONFIG} -lCo" \
"libXx         = -L$${XXROOT}/$${CONFIG} -lXx" \
"libgraflib    = -L$${CERNLIB} -lgraflib" \
"libgrafX11    = -L$${CERNLIB} -lgrafX11" \
"libpacklib    = -L$${CERNLIB} -lpacklib" \
"libmathlib    = -L$${CERNLIB} -lmathlib" \
"libpawlib     = -L$${CERNLIB} -lpawlib" \
"XzCm_CPPFLAGS = -DHAS_CM -I$${CSETROOT}/src -I$${CMROOT}/src" \
"libXzCm       = -L../$${CONFIG} -lXzCm"

include ${COROOT}/mgr/Config.mk

This_needed :
	@if test "${COROOT}"    = "" ; then echo "COROOT  not set" ; exit 1 ; else exit 0 ; fi
	@if test "${XXROOT}"    = "" ; then echo "XXROOT  not set" ; exit 1 ; else exit 0 ; fi
	@if test "${CERNLIB}"   = "" ; then echo "CERNLIB not set" ; exit 1 ; else exit 0 ; fi



