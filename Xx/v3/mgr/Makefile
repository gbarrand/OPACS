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
"Xx_CPPFLAGS = -I$${COROOT}/include -DHAS_XEXT -DHAS_XMU -I../include" \
"libXx       = -L../$${CONFIG} -lXx" \
"libCo       = -L$${COROOT}/$${CONFIG} -lCo"

include ${COROOT}/mgr/Config.mk

This_needed :
	@if test "${COROOT}" = "" ; then echo "COROOT not set" ; exit 1 ; else exit 0 ; fi



