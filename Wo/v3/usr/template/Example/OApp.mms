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
# Macros for this package are in macros.mms
#
This_macros = macros

.include COROOT:[mgr]Config.mms

This_needed :
	@ if f$trnlnm ("COROOT")  .eqs. "" then write sys$output "COROOT not set"
	@ if f$trnlnm ("XXROOT")  .eqs. "" then write sys$output "XXROOT not set"
	@ if f$trnlnm ("WOROOT")  .eqs. "" then write sys$output "WOROOT not set"
	@ 
	@ if f$trnlnm ("HOROOT")  .eqs. "" then write sys$output "HOROOT not set"
	@ if f$trnlnm ("GLOROOT")   .eqs. "" then write sys$output "GLOROOT not set"
	@ if f$trnlnm ("GOROOT")  .eqs. "" then write sys$output "GOROOT not set"
	@ if f$trnlnm ("W3OROOT")   .eqs. "" then write sys$output "W3OROOT not set"
	@ if f$trnlnm ("HTMLOROOT") .eqs. "" then write sys$output "HTMLOROOT not set"
	@ if f$trnlnm ("XOROOT")  .eqs. "" then write sys$output "XOROOT not set"
