#/* +---------------------- Copyright notice -------------------------------+ */
#/* | Copyright (C) 1995, Guy Barrand, LAL Orsay. (barrand@lalcls.in2p3.fr) | */
#/* |   Permission to use, copy, modify, and distribute this software       | */
#/* |   and its documentation for any purpose and without fee is hereby     | */
#/* |   granted, provided that the above copyright notice appear in all     | */
#/* |   copies and that both that copyright notice and this permission      | */
#/* |   notice appear in supporting documentation.  This software is        | */
#/* |   provided "as is" without express or implied warranty.               | */
#/* +---------------------- Copyright notice -------------------------------+ */
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
# Examples :
#  VMS$ mms ALPHA
#  VMS$ mms/macro=t=all   ALPHA
#  VMS$ mms/macro=t=clean ALPHA
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
echo = @ write sys$output

default :
	$(echo) "  Type :"
	$(echo) "     VMS$ mms <config>"
	$(echo) " where, for the distribution, <config> could be :"
	$(echo) "     ALPHA : for DEC ALPHA with VMS."
	$(echo) "     VAX   : for DEC VAX   with VMS."
	$(echo) "  For example : "
	$(echo) "     VMS$ mms ALPHA"
	$(echo) "  If none of the above 'config' matches you environment,"
	$(echo) " edit the [...Co.v3.mgr]Config.mms file."

# ALPHA VMS
ALPHA : This_needed
	@ CONFIG="ALPHA"
	@ mms/desc=This.mms/macro=(\
"olb_or_exe=exe",\
"CC=cc",\
"CFLAGS=/decc/standard=ansi89/opt",\
"CPPDEFINES=VMS,HAS_SHORT_NAME,HAS_NOT_U",\
"CCLD=link",\
"CCLDFLAGS=",\
"CCLDEND=",\
"F77=for",\
"F77FLAGS=",\
"F77LD=link",\
"X11_CPPINCLUDES=DECW$INCLUDE:",\
"X11_CPPDEFINES=NOTHING",\
"Xext_CPPINCLUDES=NOTHING",\
"Xext_CPPDEFINES=NOTHING",\
"Xmu_CPPINCLUDES=NOTHING",\
"Xmu_CPPDEFINES=NOTHING",\
"Xt_CPPINCLUDES=NOTHING",\
"Xt_CPPDEFINES=NOTHING",\
"Xaw_CPPINCLUDES=NOTHING",\
"Xaw_CPPDEFINES=NOTHING",\
"Xm_CPPINCLUDES=NOTHING",\
"Xm_CPPDEFINES=NOTHING",\
$(This_macros)) $(t)

# VAX VMS
VAX : This_needed
	@ CONFIG="VAX"
	@ mms/desc=This.mms/macro=(\
"olb_or_exe=exe",\
"CC=cc",\
"CFLAGS=/decc/standard=ansi89/warnings=(disable=LONGEXTERN)/opt",\
"CPPDEFINES=VMS,HAS_SHORT_NAME,HAS_NOT_U",\
"CCLD=link",\
"CCLDFLAGS=",\
"CCLDEND=",\
"F77=for",\
"F77FLAGS=",\
"F77LD=link",\
"X11_CPPINCLUDES=DECW$INCLUDE:",\
"X11_CPPDEFINES=NOTHING",\
"Xext_CPPINCLUDES=NOTHING",\
"Xext_CPPDEFINES=NOTHING",\
"Xmu_CPPINCLUDES=NOTHING",\
"Xmu_CPPDEFINES=NOTHING",\
"Xt_CPPINCLUDES=NOTHING",\
"Xt_CPPDEFINES=NOTHING",\
"Xaw_CPPINCLUDES=NOTHING",\
"Xaw_CPPDEFINES=NOTHING",\
"Xm_CPPINCLUDES=NOTHING",\
"Xm_CPPDEFINES=NOTHING",\
$(This_macros)) $(t)


