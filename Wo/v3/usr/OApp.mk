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
"bin            = ."\
"mgr            = ."\
"src            = ."\
"app            = ."\
"make_so        = sh ${COROOT}/mgr/maklibs.sh"\
"OApp_CPPFLAGS     = -I."\
"OApp_Wo_CPPFLAGS  = -I$${COROOT}/include -I$${XXROOT}/include -I$${WOROOT}/include"\
"libCo             = -L$${COROOT}/$${CONFIG} -lCo"\
"libXx             = -L$${XXROOT}/$${CONFIG} -lXx"\
"libWo             = -L$${WOROOT}/$${CONFIG} -lWo"\
"OApp_Xm_CPPFLAGS  = -DHAS_XM"\
"libWoXm           = -L$${WOROOT}/$${CONFIG} -lWoXm"\
"OApp_Xaw_CPPFLAGS = -DHAS_XAW"\
"libWoXaw          = -L$${WOROOT}/$${CONFIG} -lWoXaw"\
"OApp_Xo_CPPFLAGS  = -DHAS_XO -I$${XOROOT}/include -I$${GOROOT}/include -I$${GLOROOT}/usr/include -I$${HOROOT}/include"\
"libWoGL           = -L$${WOROOT}/$${CONFIG} -lWoGL"\
"libXo             = -L$${XOROOT}/$${CONFIG}  -lXo"\
"libHo             = -L$${HOROOT}/$${CONFIG} -lHo"\
"libGL             = -L$${GLOROOT}/$${CONFIG} -lGLo"\
"libGLU            = -L$${GLOROOT}/$${CONFIG} -lGLUo"\
"libGLX            = -L$${GLOROOT}/$${CONFIG} -lGLXo"\
"libGo             = -L$${GOROOT}/$${CONFIG}  -lGo"\
"OApp_HTML_CPPFLAGS = -DHAS_HTML"\
"libwww            = -L$${W3OROOT}/$${CONFIG} -lW3o"\
"libhtmlw          = -L$${HTMLOROOT}/$${CONFIG} -lHTMLo"\
"OApp_Xz_CPPFLAGS  = -DHAS_XZ -DHAS_HO -I$${XZROOT}/include -I$${HOROOT}/include"\
"libWoHoXz         = -L$${WOROOT}/$${CONFIG} -lWoHoXz"\
"libXz             = -L$${XZROOT}/$${CONFIG} -lXz"\
"libXzPAWC         = -L$${XZROOT}/$${CONFIG} -lXzPAWC"\
"libgraflib        = -L$${CERNLIB} -lgraflib"\
"libgrafX11        = -L$${CERNLIB} -lgrafX11"\
"libpacklib        = -L$${CERNLIB} -lpacklib"\
"libmathlib        = -L$${CERNLIB} -lmathlib"\
"OApp_PAW_CPPFLAGS = -DHAS_PAW"\
"libWoPAW          = -L$${WOROOT}/$${CONFIG} -lWoPAW"\
"libpawlib         = -L$${CERNLIB} -lpawlib"\
"OApp_G3o_CPPFLAGS = -DHAS_G3O -I$${G3OROOT}/include"\
"libG3o            = -L$${G3OROOT}/$${CONFIG} -lG3o"\
"libG3oEXPO        = -L$${G3OROOT}/$${CONFIG} -lG3oEXPO"\
"libgeant          = -L$${CERNLIB} -lgeant321"\
"OApp_Ci_CPPFLAGS  = -DHAS_CI -I$${CIROOT}/src -I$${CSETROOT}/src"\
"libWoCi           = -L$${WOROOT}/$${CONFIG} -lWoCi"\
"libCi             = -L$${CIROOT}/$${METHODSCONFIG}  -lCiSwitches -lCi"\
"libCSet           = -L$${CSETROOT}/$${METHODSCONFIG}  -lCSet -lReg"\
"OApp_FNAL_CPPFLAGS = -DHAS_FNAL $${HOME}/$${CONFIG}/include/FNAL"\
"libWoFNAL          = -L$${WOROOT}/$${CONFIG} -lWoFNAL"\
"libFNAL            = -L$${HOME}/$${CONFIG}/lib -lPlotW -lNUtil"\
"OApp_tcl_CPPFLAGS  = -DHAS_TCL -I/usr/local/include"\
"libtcl             = -L/usr/local/lib -ltcl"

include ${COROOT}/mgr/Config.mk

This_needed :
	@if test "${COROOT}"    = "" ; then echo "COROOT not set"    ; exit 1 ; else exit 0 ; fi
	@if test "${XXROOT}"    = "" ; then echo "XXROOT not set"    ; exit 1 ; else exit 0 ; fi
	@if test "${WOROOT}"    = "" ; then echo "WOROOT not set"    ; exit 1 ; else exit 0 ; fi
	@
	@if test "${HOROOT}"    = "" ; then echo "HOROOT not set"    ; else exit 0 ; fi
	@if test "${GLOROOT}"   = "" ; then echo "GLOROOT not set"   ; else exit 0 ; fi
	@if test "${GOROOT}"    = "" ; then echo "GOROOT not set"    ; else exit 0 ; fi
	@if test "${W3OROOT}"   = "" ; then echo "W3OROOT not set"   ; else exit 0 ; fi
	@if test "${HTMLOROOT}" = "" ; then echo "HTMLOROOT not set" ; else exit 0 ; fi
	@if test "${XOROOT}"    = "" ; then echo "XOROOT not set"    ; else exit 0 ; fi
	@if test "${CIROOT}"    = "" ; then echo "CIROOT not set"    ; else exit 0 ; fi
	@if test "${CSETROOT}"  = "" ; then echo "CSETROOT not set"  ; else exit 0 ; fi

