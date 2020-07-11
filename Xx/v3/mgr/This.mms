!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!  File produced by the Co/omake tool
! using file Make.odb.
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!  The soft linked packages has been taken
! from the property 'has' of the 
! 'Make' object of file Make.odb.
! It had the value :
!    X11 Xext Xt Xmu
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
HAS_CCFLAGS = \
$(Xx_CCFLAGS)\
$(X11_CCFLAGS)\
$(Xext_CCFLAGS)\
$(Xt_CCFLAGS)\
$(Xmu_CCFLAGS)
HAS_CXXFLAGS = \
$(Xx_CXXFLAGS)\
$(X11_CXXFLAGS)\
$(Xext_CXXFLAGS)\
$(Xt_CXXFLAGS)\
$(Xmu_CXXFLAGS)
HAS_CPPINCLUDES = \
 $(Xx_CPPINCLUDES)\
,$(X11_CPPINCLUDES)\
,$(Xext_CPPINCLUDES)\
,$(Xt_CPPINCLUDES)\
,$(Xmu_CPPINCLUDES)
HAS_CPPDEFINES = \
 $(Xx_CPPDEFINES)\
,$(X11_CPPDEFINES)\
,$(Xext_CPPDEFINES)\
,$(Xt_CPPDEFINES)\
,$(Xmu_CPPDEFINES)
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

libXx_target = $(bin)libXx.$(olb_or_exe)
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
all :: \
$(libXx_target) 
	@ write sys$output "Xx : all ok." 

libs :: \
$(libXx_target) 
	@ write sys$output "Xx : libs ok." 

apps :: \
$(bin)omail.exe \
$(bin)NoHT.exe 
	@ write sys$output "Xx : apps ok." 
!.............................................
rmexeo :
	if f$search("$(bin)omail.obj;*") .nes. "" then del $(bin)omail.obj;*
	if f$search("$(bin)NoHT.obj;*") .nes. "" then del $(bin)NoHT.obj;*
!.............................................
omail : $(bin)omail.exe
	@ write sys$output "Xx : omail ok."
NoHT : $(bin)NoHT.exe
	@ write sys$output "Xx : NoHT ok."
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!.............................................
$(bin)omail.exe : $(bin)omail.obj \
$(libXx_target) 
	$(CCLD)$(CCLDFLAGS)/exe=$(bin)omail.exe $(bin)omail.obj\
,$(libXx)\
,$(libCo)\
,$(libXmu)\
,$(libXt)\
,$(libXext)\
,$(libX11)\
,$(libm)\
$(CCLDEND)

$(bin)omail.obj   : $(app)omail.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES),$(APP_CPPINCLUDES))/define=($(APP_CCDEFINES),$(HAS_CPPDEFINES))/obj=$(bin)omail.obj $(app)omail.c 
!.............................................
$(bin)NoHT.exe : $(bin)NoHT.obj 
	$(CCLD)$(CCLDFLAGS)/exe=$(bin)NoHT.exe $(bin)NoHT.obj\
$(CCLDEND)

$(bin)NoHT.obj   : $(app)NoHT.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES),$(APP_CPPINCLUDES))/define=($(APP_CCDEFINES),$(HAS_CPPDEFINES))/obj=$(bin)NoHT.obj $(app)NoHT.c 
$(bin)libXx.exe : $(bin)libXx.olb 
	$(make_so) libXx Xx $(CONFIG)
	if f$search("$(bin)libXx.opt;*") .nes. "" then delete $(bin)libXx.opt;*
	open/write file $(bin)libXx.opt
	write file "XXROOT:[$(CONFIG)]libXx/share"
	close file

$(bin)libXx.olb : \
$(bin)libXx.olb(XxCommands) \
$(bin)libXx.olb(XxTypes) \
$(bin)libXx.olb(XxF77) \
$(bin)libXx.olb(XWidget) \
$(bin)libXx.olb(XMailbox) \
$(bin)libXx.olb(XSelection) \
$(bin)libXx.olb(XDisplay) \
$(bin)libXx.olb(LocPix) \
$(bin)libXx.olb(XxBinds) 
	if f$search("$(bin)libXx.opt;*") .nes. "" then delete $(bin)libXx.opt;*
	open/write file $(bin)libXx.opt
	write file "XXROOT:[$(CONFIG)]libXx/lib"
	close file
!.............................................
! libXx dependencies 
!.............................................
$(bin)libXx.olb(XxCommands) : $(src)XxCommands.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)XxCommands.obj $(src)XxCommands.c
	$(inlib) $(bin)libXx.olb $(bin)XxCommands.obj
	del $(bin)XxCommands.obj;*

$(bin)libXx.olb(XxTypes) : $(src)XxTypes.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)XxTypes.obj $(src)XxTypes.c
	$(inlib) $(bin)libXx.olb $(bin)XxTypes.obj
	del $(bin)XxTypes.obj;*

$(bin)libXx.olb(XxF77) : $(src)XxF77.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)XxF77.obj $(src)XxF77.c
	$(inlib) $(bin)libXx.olb $(bin)XxF77.obj
	del $(bin)XxF77.obj;*

$(bin)libXx.olb(XWidget) : $(src)XWidget.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)XWidget.obj $(src)XWidget.c
	$(inlib) $(bin)libXx.olb $(bin)XWidget.obj
	del $(bin)XWidget.obj;*

$(bin)libXx.olb(XMailbox) : $(src)XMailbox.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)XMailbox.obj $(src)XMailbox.c
	$(inlib) $(bin)libXx.olb $(bin)XMailbox.obj
	del $(bin)XMailbox.obj;*

$(bin)libXx.olb(XSelection) : $(src)XSelection.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)XSelection.obj $(src)XSelection.c
	$(inlib) $(bin)libXx.olb $(bin)XSelection.obj
	del $(bin)XSelection.obj;*

$(bin)libXx.olb(XDisplay) : $(src)XDisplay.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)XDisplay.obj $(src)XDisplay.c
	$(inlib) $(bin)libXx.olb $(bin)XDisplay.obj
	del $(bin)XDisplay.obj;*

$(bin)libXx.olb(LocPix) : $(src)LocPix.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)LocPix.obj $(src)LocPix.c
	$(inlib) $(bin)libXx.olb $(bin)LocPix.obj
	del $(bin)LocPix.obj;*

$(bin)libXx.olb(XxBinds) : $(src)XxBinds.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)XxBinds.obj $(src)XxBinds.c
	$(inlib) $(bin)libXx.olb $(bin)XxBinds.obj
	del $(bin)XxBinds.obj;*


