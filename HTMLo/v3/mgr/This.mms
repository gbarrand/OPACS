!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!  File produced by the Co/omake tool
! using file Make.odb.
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!  The soft linked packages has been taken
! from the property 'has' of the 
! 'Make' object of file Make.odb.
! It had the value :
!    X11 Xext Xt Xmu Xm HTMLoXm
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
HAS_CCFLAGS = \
$(HTMLo_CCFLAGS)\
$(X11_CCFLAGS)\
$(Xext_CCFLAGS)\
$(Xt_CCFLAGS)\
$(Xmu_CCFLAGS)\
$(Xm_CCFLAGS)\
$(HTMLoXm_CCFLAGS)
HAS_CXXFLAGS = \
$(HTMLo_CXXFLAGS)\
$(X11_CXXFLAGS)\
$(Xext_CXXFLAGS)\
$(Xt_CXXFLAGS)\
$(Xmu_CXXFLAGS)\
$(Xm_CXXFLAGS)\
$(HTMLoXm_CXXFLAGS)
HAS_CPPINCLUDES = \
 $(HTMLo_CPPINCLUDES)\
,$(X11_CPPINCLUDES)\
,$(Xext_CPPINCLUDES)\
,$(Xt_CPPINCLUDES)\
,$(Xmu_CPPINCLUDES)\
,$(Xm_CPPINCLUDES)\
,$(HTMLoXm_CPPINCLUDES)
HAS_CPPDEFINES = \
 $(HTMLo_CPPDEFINES)\
,$(X11_CPPDEFINES)\
,$(Xext_CPPDEFINES)\
,$(Xt_CPPDEFINES)\
,$(Xmu_CPPDEFINES)\
,$(Xm_CPPDEFINES)\
,$(HTMLoXm_CPPDEFINES)
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

libHTMLo_target = $(bin)libHTMLo.$(olb_or_exe)
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
all :: \
$(libHTMLo_target) \
$(bin)htmlT.exe 
	@ write sys$output "HTMLo : all ok." 

libs :: \
$(libHTMLo_target) 
	@ write sys$output "HTMLo : libs ok." 

apps :: \
$(bin)htmlT.exe 
	@ write sys$output "HTMLo : apps ok." 
!.............................................
rmexeo :
	if f$search("$(bin)htmlT.obj;*") .nes. "" then del $(bin)htmlT.obj;*
!.............................................
htmlT : $(bin)htmlT.exe
	@ write sys$output "HTMLo : htmlT ok."
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!.............................................
$(bin)htmlT.exe : $(bin)htmlT.obj \
$(bin)CFile.obj \
$(libHTMLo_target) 
	$(CCLD)$(CCLDFLAGS)/exe=$(bin)htmlT.exe $(bin)htmlT.obj\
,$(bin)CFile.obj\
,$(libHTMLo)\
,$(libXm)\
,$(libXmu)\
,$(libXt)\
,$(libXext)\
,$(libX11)\
,$(libm)\
$(CCLDEND)

$(bin)htmlT.obj   : $(app)htmlT.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES),$(APP_CPPINCLUDES))/define=($(APP_CCDEFINES),$(HAS_CPPDEFINES))/obj=$(bin)htmlT.obj $(app)htmlT.c 
$(bin)libHTMLo.exe : $(bin)libHTMLo.olb 
	$(make_so) libHTMLo HTMLo $(CONFIG)
	if f$search("$(bin)libHTMLo.opt;*") .nes. "" then delete $(bin)libHTMLo.opt;*
	open/write file $(bin)libHTMLo.opt
	write file "HTMLOROOT:[$(CONFIG)]libHTMLo/share"
	close file

$(bin)libHTMLo.olb : \
$(bin)libHTMLo.olb(HTMLWidget) \
$(bin)libHTMLo.olb(DrawingArea) \
$(bin)libHTMLo.olb(HTMLformat) \
$(bin)libHTMLo.olb(HTMLlists) \
$(bin)libHTMLo.olb(HTMLPSformat) \
$(bin)libHTMLo.olb(HTMLimages) \
$(bin)libHTMLo.olb(HTMLparse) \
$(bin)libHTMLo.olb(HTMLjot) \
$(bin)libHTMLo.olb(HTMLtable) \
$(bin)libHTMLo.olb(HTMLwidgets) \
$(bin)libHTMLo.olb(list) \
$(bin)libHTMLo.olb(medcut) \
$(bin)libHTMLo.olb(gifread) \
$(bin)libHTMLo.olb(xpmhash) 
	if f$search("$(bin)libHTMLo.opt;*") .nes. "" then delete $(bin)libHTMLo.opt;*
	open/write file $(bin)libHTMLo.opt
	write file "HTMLOROOT:[$(CONFIG)]libHTMLo/lib"
	close file
!.............................................
! libHTMLo dependencies 
!.............................................
$(bin)libHTMLo.olb(HTMLWidget) : $(src)HTMLWidget.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)HTMLWidget.obj $(src)HTMLWidget.c
	$(inlib) $(bin)libHTMLo.olb $(bin)HTMLWidget.obj
	del $(bin)HTMLWidget.obj;*

$(bin)libHTMLo.olb(DrawingArea) : $(src)DrawingArea.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)DrawingArea.obj $(src)DrawingArea.c
	$(inlib) $(bin)libHTMLo.olb $(bin)DrawingArea.obj
	del $(bin)DrawingArea.obj;*

$(bin)libHTMLo.olb(HTMLformat) : $(src)HTMLformat.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)HTMLformat.obj $(src)HTMLformat.c
	$(inlib) $(bin)libHTMLo.olb $(bin)HTMLformat.obj
	del $(bin)HTMLformat.obj;*

$(bin)libHTMLo.olb(HTMLlists) : $(src)HTMLlists.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)HTMLlists.obj $(src)HTMLlists.c
	$(inlib) $(bin)libHTMLo.olb $(bin)HTMLlists.obj
	del $(bin)HTMLlists.obj;*

$(bin)libHTMLo.olb(HTMLPSformat) : $(src)HTMLPSformat.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)HTMLPSformat.obj $(src)HTMLPSformat.c
	$(inlib) $(bin)libHTMLo.olb $(bin)HTMLPSformat.obj
	del $(bin)HTMLPSformat.obj;*

$(bin)libHTMLo.olb(HTMLimages) : $(src)HTMLimages.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)HTMLimages.obj $(src)HTMLimages.c
	$(inlib) $(bin)libHTMLo.olb $(bin)HTMLimages.obj
	del $(bin)HTMLimages.obj;*

$(bin)libHTMLo.olb(HTMLparse) : $(src)HTMLparse.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)HTMLparse.obj $(src)HTMLparse.c
	$(inlib) $(bin)libHTMLo.olb $(bin)HTMLparse.obj
	del $(bin)HTMLparse.obj;*

$(bin)libHTMLo.olb(HTMLjot) : $(src)HTMLjot.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)HTMLjot.obj $(src)HTMLjot.c
	$(inlib) $(bin)libHTMLo.olb $(bin)HTMLjot.obj
	del $(bin)HTMLjot.obj;*

$(bin)libHTMLo.olb(HTMLtable) : $(src)HTMLtable.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)HTMLtable.obj $(src)HTMLtable.c
	$(inlib) $(bin)libHTMLo.olb $(bin)HTMLtable.obj
	del $(bin)HTMLtable.obj;*

$(bin)libHTMLo.olb(HTMLwidgets) : $(src)HTMLwidgets.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)HTMLwidgets.obj $(src)HTMLwidgets.c
	$(inlib) $(bin)libHTMLo.olb $(bin)HTMLwidgets.obj
	del $(bin)HTMLwidgets.obj;*

$(bin)libHTMLo.olb(list) : $(src)list.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)list.obj $(src)list.c
	$(inlib) $(bin)libHTMLo.olb $(bin)list.obj
	del $(bin)list.obj;*

$(bin)libHTMLo.olb(medcut) : $(src)medcut.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)medcut.obj $(src)medcut.c
	$(inlib) $(bin)libHTMLo.olb $(bin)medcut.obj
	del $(bin)medcut.obj;*

$(bin)libHTMLo.olb(gifread) : $(src)gifread.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)gifread.obj $(src)gifread.c
	$(inlib) $(bin)libHTMLo.olb $(bin)gifread.obj
	del $(bin)gifread.obj;*

$(bin)libHTMLo.olb(xpmhash) : $(src)xpmhash.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)xpmhash.obj $(src)xpmhash.c
	$(inlib) $(bin)libHTMLo.olb $(bin)xpmhash.obj
	del $(bin)xpmhash.obj;*

$(bin)CFile.obj : $(app)CFile.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)CFile.obj $(app)CFile.c


