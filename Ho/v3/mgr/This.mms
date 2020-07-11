!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!  File produced by the Co/omake tool
! using file Make.odb.
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
HAS_CCFLAGS = \
$(Ho_CCFLAGS)
HAS_CXXFLAGS = \
$(Ho_CXXFLAGS)
HAS_CPPINCLUDES = \
 $(Ho_CPPINCLUDES)
HAS_CPPDEFINES = \
 $(Ho_CPPDEFINES)
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

libHo_target = $(bin)libHo.$(olb_or_exe)
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
all :: \
$(libHo_target) \
$(bin)oshHo.exe \
$(bin)HoT.exe 
	@ write sys$output "Ho : all ok." 

libs :: \
$(libHo_target) 
	@ write sys$output "Ho : libs ok." 

apps :: \
$(bin)oshHo.exe \
$(bin)HoT.exe \
$(bin)gauss.exe \
$(bin)HoF77T.exe \
$(bin)NoHT.exe 
	@ write sys$output "Ho : apps ok." 
!.............................................
rmexeo :
	if f$search("$(bin)oshHo.obj;*") .nes. "" then del $(bin)oshHo.obj;*
	if f$search("$(bin)HoT.obj;*") .nes. "" then del $(bin)HoT.obj;*
	if f$search("$(bin)gauss.obj;*") .nes. "" then del $(bin)gauss.obj;*
	if f$search("$(bin)HoF77T.obj;*") .nes. "" then del $(bin)HoF77T.obj;*
	if f$search("$(bin)NoHT.obj;*") .nes. "" then del $(bin)NoHT.obj;*
!.............................................
oshHo : $(bin)oshHo.exe
	@ write sys$output "Ho : oshHo ok."
HoT : $(bin)HoT.exe
	@ write sys$output "Ho : HoT ok."
gauss : $(bin)gauss.exe
	@ write sys$output "Ho : gauss ok."
HoF77T : $(bin)HoF77T.exe
	@ write sys$output "Ho : HoF77T ok."
NoHT : $(bin)NoHT.exe
	@ write sys$output "Ho : NoHT ok."
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!.............................................
$(bin)oshHo.exe : $(bin)oshHo.obj \
$(libHo_target) 
	$(CCLD)$(CCLDFLAGS)/exe=$(bin)oshHo.exe $(bin)oshHo.obj\
,$(libHo)\
,$(libCo)\
,$(libm)\
$(CCLDEND)

$(bin)oshHo.obj   : $(app)oshHo.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES),$(APP_CPPINCLUDES))/define=($(APP_CCDEFINES),$(HAS_CPPDEFINES))/obj=$(bin)oshHo.obj $(app)oshHo.c 
!.............................................
$(bin)HoT.exe : $(bin)HoT.obj \
$(libHo_target) 
	$(CCLD)$(CCLDFLAGS)/exe=$(bin)HoT.exe $(bin)HoT.obj\
,$(libHo)\
,$(libCo)\
,$(libm)\
$(CCLDEND)

$(bin)HoT.obj   : $(app)HoT.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES),$(APP_CPPINCLUDES))/define=($(APP_CCDEFINES),$(HAS_CPPDEFINES))/obj=$(bin)HoT.obj $(app)HoT.c 
!.............................................
$(bin)gauss.exe : $(bin)gauss.obj 
	$(CCLD)$(CCLDFLAGS)/exe=$(bin)gauss.exe $(bin)gauss.obj\
,$(libCo)\
,$(libm)\
$(CCLDEND)

$(bin)gauss.obj   : $(app)gauss.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES),$(APP_CPPINCLUDES))/define=($(APP_CCDEFINES),$(HAS_CPPDEFINES))/obj=$(bin)gauss.obj $(app)gauss.c 
!.............................................
$(bin)HoF77T.exe : $(bin)HoF77T.obj \
$(libHo_target) 
	$(F77LD)$(F77LDFLAGS)/exe=$(bin)HoF77T.exe $(bin)HoF77T.obj\
,$(libHo)\
,$(libCo)\
,$(libm)\
$(F77LDEND)

$(bin)HoF77T.obj   : $(app)HoF77T.f
	$(F77)$(F77FLAGS)/obj=$(bin)HoF77T.obj $(app)HoF77T.f
!.............................................
$(bin)NoHT.exe : $(bin)NoHT.obj 
	$(CCLD)$(CCLDFLAGS)/exe=$(bin)NoHT.exe $(bin)NoHT.obj\
$(CCLDEND)

$(bin)NoHT.obj   : $(app)NoHT.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES),$(APP_CPPINCLUDES))/define=($(APP_CCDEFINES),$(HAS_CPPDEFINES))/obj=$(bin)NoHT.obj $(app)NoHT.c 
$(bin)libHo.exe : $(bin)libHo.olb 
	$(make_so) libHo Ho $(CONFIG)
	if f$search("$(bin)libHo.opt;*") .nes. "" then delete $(bin)libHo.opt;*
	open/write file $(bin)libHo.opt
	write file "HOROOT:[$(CONFIG)]libHo/share"
	close file

$(bin)libHo.olb : \
$(bin)libHo.olb(HoCommands) \
$(bin)libHo.olb(HoTypes) \
$(bin)libHo.olb(Ho) \
$(bin)libHo.olb(HoF77) \
$(bin)libHo.olb(OTuple) \
$(bin)libHo.olb(OCut) \
$(bin)libHo.olb(OFunction) \
$(bin)libHo.olb(OFit) \
$(bin)libHo.olb(OHistogram) \
$(bin)libHo.olb(OScatter) \
$(bin)libHo.olb(OAxis) \
$(bin)libHo.olb(OStatement) \
$(bin)libHo.olb(HoBinds) 
	if f$search("$(bin)libHo.opt;*") .nes. "" then delete $(bin)libHo.opt;*
	open/write file $(bin)libHo.opt
	write file "HOROOT:[$(CONFIG)]libHo/lib"
	close file
!.............................................
! libHo dependencies 
!.............................................
$(bin)libHo.olb(HoCommands) : $(src)HoCommands.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)HoCommands.obj $(src)HoCommands.c
	$(inlib) $(bin)libHo.olb $(bin)HoCommands.obj
	del $(bin)HoCommands.obj;*

$(bin)libHo.olb(HoTypes) : $(src)HoTypes.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)HoTypes.obj $(src)HoTypes.c
	$(inlib) $(bin)libHo.olb $(bin)HoTypes.obj
	del $(bin)HoTypes.obj;*

$(bin)libHo.olb(Ho) : $(src)Ho.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)Ho.obj $(src)Ho.c
	$(inlib) $(bin)libHo.olb $(bin)Ho.obj
	del $(bin)Ho.obj;*

$(bin)libHo.olb(HoF77) : $(src)HoF77.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)HoF77.obj $(src)HoF77.c
	$(inlib) $(bin)libHo.olb $(bin)HoF77.obj
	del $(bin)HoF77.obj;*

$(bin)libHo.olb(OTuple) : $(src)OTuple.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)OTuple.obj $(src)OTuple.c
	$(inlib) $(bin)libHo.olb $(bin)OTuple.obj
	del $(bin)OTuple.obj;*

$(bin)libHo.olb(OCut) : $(src)OCut.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)OCut.obj $(src)OCut.c
	$(inlib) $(bin)libHo.olb $(bin)OCut.obj
	del $(bin)OCut.obj;*

$(bin)libHo.olb(OFunction) : $(src)OFunction.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)OFunction.obj $(src)OFunction.c
	$(inlib) $(bin)libHo.olb $(bin)OFunction.obj
	del $(bin)OFunction.obj;*

$(bin)libHo.olb(OFit) : $(src)OFit.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)OFit.obj $(src)OFit.c
	$(inlib) $(bin)libHo.olb $(bin)OFit.obj
	del $(bin)OFit.obj;*

$(bin)libHo.olb(OHistogram) : $(src)OHistogram.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)OHistogram.obj $(src)OHistogram.c
	$(inlib) $(bin)libHo.olb $(bin)OHistogram.obj
	del $(bin)OHistogram.obj;*

$(bin)libHo.olb(OScatter) : $(src)OScatter.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)OScatter.obj $(src)OScatter.c
	$(inlib) $(bin)libHo.olb $(bin)OScatter.obj
	del $(bin)OScatter.obj;*

$(bin)libHo.olb(OAxis) : $(src)OAxis.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)OAxis.obj $(src)OAxis.c
	$(inlib) $(bin)libHo.olb $(bin)OAxis.obj
	del $(bin)OAxis.obj;*

$(bin)libHo.olb(OStatement) : $(src)OStatement.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)OStatement.obj $(src)OStatement.c
	$(inlib) $(bin)libHo.olb $(bin)OStatement.obj
	del $(bin)OStatement.obj;*

$(bin)libHo.olb(HoBinds) : $(src)HoBinds.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)HoBinds.obj $(src)HoBinds.c
	$(inlib) $(bin)libHo.olb $(bin)HoBinds.obj
	del $(bin)HoBinds.obj;*


