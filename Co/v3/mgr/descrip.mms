#/* +---------------------- Copyright notice -------------------------------+ */
#/* | Copyright (C) 1995, Guy Barrand, LAL Orsay. (barrand@lalcls.in2p3.fr) | */
#/* |   Permission to use, copy, modify, and distribute this software       | */
#/* |   and its documentation for any purpose and without fee is hereby     | */
#/* |   granted, provided that the above copyright notice appear in all     | */
#/* |   copies and that both that copyright notice and this permission      | */
#/* |   notice appear in supporting documentation.  This software is        | */
#/* |   provided "as is" without express or implied warranty.               | */
#/* +---------------------- Copyright notice -------------------------------+ */
#
# The environment variable COROOT must be defined with something like :
#     VMS$ define/nolog/translation=(concealed) COROOT   <disk>:[<root>.Co.v3.]
# Macros for this package are in macros.mms.
#
This_macros = macros

.include Config.mms

This_needed :
	@ if f$trnlnm ("COROOT") .eqs. "" then write sys$output "COROOT not set"


