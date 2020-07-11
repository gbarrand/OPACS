!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!  File produced by the Co/omake tool
! using file Make.odb.
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
HAS_CCFLAGS = \
$(W3o_CCFLAGS)
HAS_CXXFLAGS = \
$(W3o_CXXFLAGS)
HAS_CPPINCLUDES = \
 $(W3o_CPPINCLUDES)
HAS_CPPDEFINES = \
 $(W3o_CPPDEFINES)
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
all   ::
	@ if f$search("[-]$(CONFIG).DIR") .eqs. "" then cre/dir $(bin)
clean ::
	if f$search("$(bin)*.obj;*") .nes. "" then delete $(bin)*.obj;*
	if f$search("$(bin)*.exe;*") .nes. "" then delete $(bin)*.exe;*
	if f$search("$(bin)*.olb;*") .nes. "" then delete $(bin)*.olb;*
	if f$search("$(bin)*.opt;*") .nes. "" then delete $(bin)*.opt;*
rmlib ::
	if f$search("$(bin)*.olb;*") .nes. "" then delete $(bin)*.olb;*
rmexe ::
	if f$search("$(bin)*.exe;*") .nes. "" then delete $(bin)*.exe;*

libW3o_target = $(bin)libW3o.$(olb_or_exe)
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
all :: \
$(libW3o_target) 
	@ write sys$output "W3o : all ok." 

libs :: \
$(libW3o_target) 
	@ write sys$output "W3o : libs ok." 

apps :: \
$(bin)W3oT.exe 
	@ write sys$output "W3o : apps ok." 
!.............................................
rmexeo :
	if f$search("$(bin)W3oT.obj;*") .nes. "" then del $(bin)W3oT.obj;*
!.............................................
W3oT : $(bin)W3oT.exe
	@ write sys$output "W3o : W3oT ok."
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!.............................................
$(bin)W3oT.exe : $(bin)W3oT.obj \
$(libW3o_target) 
	$(CCLD)$(CCLDFLAGS)/exe=$(bin)W3oT.exe $(bin)W3oT.obj\
,$(libW3o)\
$(CCLDEND)

$(bin)W3oT.obj   : $(app)W3oT.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES),$(APP_CPPINCLUDES))/define=($(APP_CCDEFINES),$(HAS_CPPDEFINES))/obj=$(bin)W3oT.obj $(app)W3oT.c 
$(bin)libW3o.exe : $(bin)libW3o.olb 
	$(make_so) libW3o W3o $(CONFIG)
	if f$search("$(bin)libW3o.opt;*") .nes. "" then delete $(bin)libW3o.opt;*
	open/write file $(bin)libW3o.opt
	write file "W3OROOT:[$(CONFIG)]libW3o/share"
	close file

$(bin)libW3o.olb : \
$(bin)libW3o.olb(HTAABrow) \
$(bin)libW3o.olb(HTAAUtil) \
$(bin)libW3o.olb(HTAccess) \
$(bin)libW3o.olb(HTAlert) \
$(bin)libW3o.olb(HTAnchor) \
$(bin)libW3o.olb(HTAssoc) \
$(bin)libW3o.olb(HTAtom) \
$(bin)libW3o.olb(HTChunk) \
$(bin)libW3o.olb(HTCompressed) \
$(bin)libW3o.olb(HTFTP) \
$(bin)libW3o.olb(HTFWriter) \
$(bin)libW3o.olb(HTFile) \
$(bin)libW3o.olb(HTFormat) \
$(bin)libW3o.olb(HTGopher) \
$(bin)libW3o.olb(HTIcon) \
$(bin)libW3o.olb(HTInit) \
$(bin)libW3o.olb(HTList) \
$(bin)libW3o.olb(HTMIME) \
$(bin)libW3o.olb(HTMLNew) \
$(bin)libW3o.olb(HTMLDTD) \
$(bin)libW3o.olb(HTMLGen) \
$(bin)libW3o.olb(HTMosaicHTML) \
$(bin)libW3o.olb(HTNews) \
$(bin)libW3o.olb(HTParse) \
$(bin)libW3o.olb(HTPlain) \
$(bin)libW3o.olb(HTSort) \
$(bin)libW3o.olb(HTString) \
$(bin)libW3o.olb(HTTCP) \
$(bin)libW3o.olb(HTTP) \
$(bin)libW3o.olb(HTTelnet) \
$(bin)libW3o.olb(HTUU) \
$(bin)libW3o.olb(HTWAIS) \
$(bin)libW3o.olb(HTWSRC) \
$(bin)libW3o.olb(HTWriter) \
$(bin)libW3o.olb(SGML) \
$(bin)libW3o.olb(HTMailto) \
$(bin)libW3o.olb(kcms) \
$(bin)libW3o.olb(moHText) \
$(bin)libW3o.olb(momo) \
$(bin)libW3o.olb(ioctl) 
	if f$search("$(bin)libW3o.opt;*") .nes. "" then delete $(bin)libW3o.opt;*
	open/write file $(bin)libW3o.opt
	write file "W3OROOT:[$(CONFIG)]libW3o/lib"
	close file
!.............................................
! libW3o dependencies 
!.............................................
$(bin)libW3o.olb(HTAABrow) : $(src)HTAABrow.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)HTAABrow.obj $(src)HTAABrow.c
	$(inlib) $(bin)libW3o.olb $(bin)HTAABrow.obj
	del $(bin)HTAABrow.obj;*

$(bin)libW3o.olb(HTAAUtil) : $(src)HTAAUtil.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)HTAAUtil.obj $(src)HTAAUtil.c
	$(inlib) $(bin)libW3o.olb $(bin)HTAAUtil.obj
	del $(bin)HTAAUtil.obj;*

$(bin)libW3o.olb(HTAccess) : $(src)HTAccess.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)HTAccess.obj $(src)HTAccess.c
	$(inlib) $(bin)libW3o.olb $(bin)HTAccess.obj
	del $(bin)HTAccess.obj;*

$(bin)libW3o.olb(HTAlert) : $(src)HTAlert.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)HTAlert.obj $(src)HTAlert.c
	$(inlib) $(bin)libW3o.olb $(bin)HTAlert.obj
	del $(bin)HTAlert.obj;*

$(bin)libW3o.olb(HTAnchor) : $(src)HTAnchor.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)HTAnchor.obj $(src)HTAnchor.c
	$(inlib) $(bin)libW3o.olb $(bin)HTAnchor.obj
	del $(bin)HTAnchor.obj;*

$(bin)libW3o.olb(HTAssoc) : $(src)HTAssoc.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)HTAssoc.obj $(src)HTAssoc.c
	$(inlib) $(bin)libW3o.olb $(bin)HTAssoc.obj
	del $(bin)HTAssoc.obj;*

$(bin)libW3o.olb(HTAtom) : $(src)HTAtom.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)HTAtom.obj $(src)HTAtom.c
	$(inlib) $(bin)libW3o.olb $(bin)HTAtom.obj
	del $(bin)HTAtom.obj;*

$(bin)libW3o.olb(HTChunk) : $(src)HTChunk.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)HTChunk.obj $(src)HTChunk.c
	$(inlib) $(bin)libW3o.olb $(bin)HTChunk.obj
	del $(bin)HTChunk.obj;*

$(bin)libW3o.olb(HTCompressed) : $(src)HTCompressed.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)HTCompressed.obj $(src)HTCompressed.c
	$(inlib) $(bin)libW3o.olb $(bin)HTCompressed.obj
	del $(bin)HTCompressed.obj;*

$(bin)libW3o.olb(HTFTP) : $(src)HTFTP.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)HTFTP.obj $(src)HTFTP.c
	$(inlib) $(bin)libW3o.olb $(bin)HTFTP.obj
	del $(bin)HTFTP.obj;*

$(bin)libW3o.olb(HTFWriter) : $(src)HTFWriter.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)HTFWriter.obj $(src)HTFWriter.c
	$(inlib) $(bin)libW3o.olb $(bin)HTFWriter.obj
	del $(bin)HTFWriter.obj;*

$(bin)libW3o.olb(HTFile) : $(src)HTFile.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)HTFile.obj $(src)HTFile.c
	$(inlib) $(bin)libW3o.olb $(bin)HTFile.obj
	del $(bin)HTFile.obj;*

$(bin)libW3o.olb(HTFormat) : $(src)HTFormat.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)HTFormat.obj $(src)HTFormat.c
	$(inlib) $(bin)libW3o.olb $(bin)HTFormat.obj
	del $(bin)HTFormat.obj;*

$(bin)libW3o.olb(HTGopher) : $(src)HTGopher.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)HTGopher.obj $(src)HTGopher.c
	$(inlib) $(bin)libW3o.olb $(bin)HTGopher.obj
	del $(bin)HTGopher.obj;*

$(bin)libW3o.olb(HTIcon) : $(src)HTIcon.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)HTIcon.obj $(src)HTIcon.c
	$(inlib) $(bin)libW3o.olb $(bin)HTIcon.obj
	del $(bin)HTIcon.obj;*

$(bin)libW3o.olb(HTInit) : $(src)HTInit.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)HTInit.obj $(src)HTInit.c
	$(inlib) $(bin)libW3o.olb $(bin)HTInit.obj
	del $(bin)HTInit.obj;*

$(bin)libW3o.olb(HTList) : $(src)HTList.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)HTList.obj $(src)HTList.c
	$(inlib) $(bin)libW3o.olb $(bin)HTList.obj
	del $(bin)HTList.obj;*

$(bin)libW3o.olb(HTMIME) : $(src)HTMIME.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)HTMIME.obj $(src)HTMIME.c
	$(inlib) $(bin)libW3o.olb $(bin)HTMIME.obj
	del $(bin)HTMIME.obj;*

$(bin)libW3o.olb(HTMLNew) : $(src)HTMLNew.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)HTMLNew.obj $(src)HTMLNew.c
	$(inlib) $(bin)libW3o.olb $(bin)HTMLNew.obj
	del $(bin)HTMLNew.obj;*

$(bin)libW3o.olb(HTMLDTD) : $(src)HTMLDTD.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)HTMLDTD.obj $(src)HTMLDTD.c
	$(inlib) $(bin)libW3o.olb $(bin)HTMLDTD.obj
	del $(bin)HTMLDTD.obj;*

$(bin)libW3o.olb(HTMLGen) : $(src)HTMLGen.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)HTMLGen.obj $(src)HTMLGen.c
	$(inlib) $(bin)libW3o.olb $(bin)HTMLGen.obj
	del $(bin)HTMLGen.obj;*

$(bin)libW3o.olb(HTMosaicHTML) : $(src)HTMosaicHTML.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)HTMosaicHTML.obj $(src)HTMosaicHTML.c
	$(inlib) $(bin)libW3o.olb $(bin)HTMosaicHTML.obj
	del $(bin)HTMosaicHTML.obj;*

$(bin)libW3o.olb(HTNews) : $(src)HTNews.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)HTNews.obj $(src)HTNews.c
	$(inlib) $(bin)libW3o.olb $(bin)HTNews.obj
	del $(bin)HTNews.obj;*

$(bin)libW3o.olb(HTParse) : $(src)HTParse.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)HTParse.obj $(src)HTParse.c
	$(inlib) $(bin)libW3o.olb $(bin)HTParse.obj
	del $(bin)HTParse.obj;*

$(bin)libW3o.olb(HTPlain) : $(src)HTPlain.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)HTPlain.obj $(src)HTPlain.c
	$(inlib) $(bin)libW3o.olb $(bin)HTPlain.obj
	del $(bin)HTPlain.obj;*

$(bin)libW3o.olb(HTSort) : $(src)HTSort.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)HTSort.obj $(src)HTSort.c
	$(inlib) $(bin)libW3o.olb $(bin)HTSort.obj
	del $(bin)HTSort.obj;*

$(bin)libW3o.olb(HTString) : $(src)HTString.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)HTString.obj $(src)HTString.c
	$(inlib) $(bin)libW3o.olb $(bin)HTString.obj
	del $(bin)HTString.obj;*

$(bin)libW3o.olb(HTTCP) : $(src)HTTCP.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)HTTCP.obj $(src)HTTCP.c
	$(inlib) $(bin)libW3o.olb $(bin)HTTCP.obj
	del $(bin)HTTCP.obj;*

$(bin)libW3o.olb(HTTP) : $(src)HTTP.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)HTTP.obj $(src)HTTP.c
	$(inlib) $(bin)libW3o.olb $(bin)HTTP.obj
	del $(bin)HTTP.obj;*

$(bin)libW3o.olb(HTTelnet) : $(src)HTTelnet.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)HTTelnet.obj $(src)HTTelnet.c
	$(inlib) $(bin)libW3o.olb $(bin)HTTelnet.obj
	del $(bin)HTTelnet.obj;*

$(bin)libW3o.olb(HTUU) : $(src)HTUU.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)HTUU.obj $(src)HTUU.c
	$(inlib) $(bin)libW3o.olb $(bin)HTUU.obj
	del $(bin)HTUU.obj;*

$(bin)libW3o.olb(HTWAIS) : $(src)HTWAIS.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)HTWAIS.obj $(src)HTWAIS.c
	$(inlib) $(bin)libW3o.olb $(bin)HTWAIS.obj
	del $(bin)HTWAIS.obj;*

$(bin)libW3o.olb(HTWSRC) : $(src)HTWSRC.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)HTWSRC.obj $(src)HTWSRC.c
	$(inlib) $(bin)libW3o.olb $(bin)HTWSRC.obj
	del $(bin)HTWSRC.obj;*

$(bin)libW3o.olb(HTWriter) : $(src)HTWriter.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)HTWriter.obj $(src)HTWriter.c
	$(inlib) $(bin)libW3o.olb $(bin)HTWriter.obj
	del $(bin)HTWriter.obj;*

$(bin)libW3o.olb(SGML) : $(src)SGML.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)SGML.obj $(src)SGML.c
	$(inlib) $(bin)libW3o.olb $(bin)SGML.obj
	del $(bin)SGML.obj;*

$(bin)libW3o.olb(HTMailto) : $(src)HTMailto.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)HTMailto.obj $(src)HTMailto.c
	$(inlib) $(bin)libW3o.olb $(bin)HTMailto.obj
	del $(bin)HTMailto.obj;*

$(bin)libW3o.olb(kcms) : $(src)kcms.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)kcms.obj $(src)kcms.c
	$(inlib) $(bin)libW3o.olb $(bin)kcms.obj
	del $(bin)kcms.obj;*

$(bin)libW3o.olb(moHText) : $(src)moHText.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)moHText.obj $(src)moHText.c
	$(inlib) $(bin)libW3o.olb $(bin)moHText.obj
	del $(bin)moHText.obj;*

$(bin)libW3o.olb(momo) : $(src)momo.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)momo.obj $(src)momo.c
	$(inlib) $(bin)libW3o.olb $(bin)momo.obj
	del $(bin)momo.obj;*

$(bin)libW3o.olb(ioctl) : $(src)ioctl.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)ioctl.obj $(src)ioctl.c
	$(inlib) $(bin)libW3o.olb $(bin)ioctl.obj
	del $(bin)ioctl.obj;*


