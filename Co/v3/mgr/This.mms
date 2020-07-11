!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!  File produced by the Co/omake tool
! using file Make.odb.
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
HAS_CCFLAGS = \
$(Co_CCFLAGS)
HAS_CXXFLAGS = \
$(Co_CXXFLAGS)
HAS_CPPINCLUDES = \
 $(Co_CPPINCLUDES)
HAS_CPPDEFINES = \
 $(Co_CPPDEFINES)
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

libCo_target = $(bin)libCo.$(olb_or_exe)
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
all :: \
$(bin)mapana.exe \
$(libCo_target) \
$(bin)omake.exe \
$(bin)CoT.exe \
$(bin)oshCo.exe \
$(bin)opatch.exe 
	@ write sys$output "Co : all ok." 

libs :: \
$(libCo_target) 
	@ write sys$output "Co : libs ok." 

apps :: \
$(bin)omake.exe \
$(bin)CoT.exe \
$(bin)oshCo.exe \
$(bin)mapana.exe \
$(bin)opatch.exe \
$(bin)oupper.exe \
$(bin)ohtxTohtml.exe \
$(bin)odoc.exe \
$(bin)stdargT.exe \
$(bin)CDefineT.exe \
$(bin)OBatchT.exe \
$(bin)CFITST.exe \
$(bin)GIF_To_PS.exe \
$(bin)CGIFT.exe \
$(bin)CLibraryT.exe \
$(bin)COptionT.exe \
$(bin)NoHT.exe 
	@ write sys$output "Co : apps ok." 
!.............................................
rmexeo :
	if f$search("$(bin)omake.obj;*") .nes. "" then del $(bin)omake.obj;*
	if f$search("$(bin)CoT.obj;*") .nes. "" then del $(bin)CoT.obj;*
	if f$search("$(bin)oshCo.obj;*") .nes. "" then del $(bin)oshCo.obj;*
	if f$search("$(bin)mapana.obj;*") .nes. "" then del $(bin)mapana.obj;*
	if f$search("$(bin)opatch.obj;*") .nes. "" then del $(bin)opatch.obj;*
	if f$search("$(bin)oupper.obj;*") .nes. "" then del $(bin)oupper.obj;*
	if f$search("$(bin)ohtxTohtml.obj;*") .nes. "" then del $(bin)ohtxTohtml.obj;*
	if f$search("$(bin)odoc.obj;*") .nes. "" then del $(bin)odoc.obj;*
	if f$search("$(bin)stdargT.obj;*") .nes. "" then del $(bin)stdargT.obj;*
	if f$search("$(bin)CDefineT.obj;*") .nes. "" then del $(bin)CDefineT.obj;*
	if f$search("$(bin)OBatchT.obj;*") .nes. "" then del $(bin)OBatchT.obj;*
	if f$search("$(bin)CFITST.obj;*") .nes. "" then del $(bin)CFITST.obj;*
	if f$search("$(bin)GIF_To_PS.obj;*") .nes. "" then del $(bin)GIF_To_PS.obj;*
	if f$search("$(bin)CGIFT.obj;*") .nes. "" then del $(bin)CGIFT.obj;*
	if f$search("$(bin)CLibraryT.obj;*") .nes. "" then del $(bin)CLibraryT.obj;*
	if f$search("$(bin)COptionT.obj;*") .nes. "" then del $(bin)COptionT.obj;*
	if f$search("$(bin)NoHT.obj;*") .nes. "" then del $(bin)NoHT.obj;*
!.............................................
omake : $(bin)omake.exe
	@ write sys$output "Co : omake ok."
CoT : $(bin)CoT.exe
	@ write sys$output "Co : CoT ok."
oshCo : $(bin)oshCo.exe
	@ write sys$output "Co : oshCo ok."
mapana : $(bin)mapana.exe
	@ write sys$output "Co : mapana ok."
opatch : $(bin)opatch.exe
	@ write sys$output "Co : opatch ok."
oupper : $(bin)oupper.exe
	@ write sys$output "Co : oupper ok."
ohtxTohtml : $(bin)ohtxTohtml.exe
	@ write sys$output "Co : ohtxTohtml ok."
odoc : $(bin)odoc.exe
	@ write sys$output "Co : odoc ok."
stdargT : $(bin)stdargT.exe
	@ write sys$output "Co : stdargT ok."
CDefineT : $(bin)CDefineT.exe
	@ write sys$output "Co : CDefineT ok."
OBatchT : $(bin)OBatchT.exe
	@ write sys$output "Co : OBatchT ok."
CFITST : $(bin)CFITST.exe
	@ write sys$output "Co : CFITST ok."
GIF_To_PS : $(bin)GIF_To_PS.exe
	@ write sys$output "Co : GIF_To_PS ok."
CGIFT : $(bin)CGIFT.exe
	@ write sys$output "Co : CGIFT ok."
CLibraryT : $(bin)CLibraryT.exe
	@ write sys$output "Co : CLibraryT ok."
COptionT : $(bin)COptionT.exe
	@ write sys$output "Co : COptionT ok."
NoHT : $(bin)NoHT.exe
	@ write sys$output "Co : NoHT ok."
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!.............................................
$(bin)omake.exe : $(bin)omake.obj \
$(libCo_target) 
	$(CCLD)$(CCLDFLAGS)/exe=$(bin)omake.exe $(bin)omake.obj\
,$(libCo)\
,$(libm)\
$(CCLDEND)

$(bin)omake.obj   : $(app)omake.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES),$(APP_CPPINCLUDES))/define=($(APP_CCDEFINES),$(HAS_CPPDEFINES))/obj=$(bin)omake.obj $(app)omake.c 
!.............................................
$(bin)CoT.exe : $(bin)CoT.obj \
$(libCo_target) 
	$(CCLD)$(CCLDFLAGS)/exe=$(bin)CoT.exe $(bin)CoT.obj\
,$(libCo)\
,$(libm)\
$(CCLDEND)

$(bin)CoT.obj   : $(app)CoT.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES),$(APP_CPPINCLUDES))/define=($(APP_CCDEFINES),$(HAS_CPPDEFINES))/obj=$(bin)CoT.obj $(app)CoT.c 
!.............................................
$(bin)oshCo.exe : $(bin)oshCo.obj \
$(libCo_target) 
	$(CCLD)$(CCLDFLAGS)/exe=$(bin)oshCo.exe $(bin)oshCo.obj\
,$(libCo)\
,$(libm)\
$(CCLDEND)

$(bin)oshCo.obj   : $(app)oshCo.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES),$(APP_CPPINCLUDES))/define=($(APP_CCDEFINES),$(HAS_CPPDEFINES))/obj=$(bin)oshCo.obj $(app)oshCo.c 
!.............................................
$(bin)mapana.exe : $(bin)mapana.obj 
	$(CCLD)$(CCLDFLAGS)/exe=$(bin)mapana.exe $(bin)mapana.obj\
$(CCLDEND)

$(bin)mapana.obj   : $(app)mapana.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES),$(APP_CPPINCLUDES))/define=($(APP_CCDEFINES),$(HAS_CPPDEFINES))/obj=$(bin)mapana.obj $(app)mapana.c 
!.............................................
$(bin)opatch.exe : $(bin)opatch.obj \
$(libCo_target) 
	$(CCLD)$(CCLDFLAGS)/exe=$(bin)opatch.exe $(bin)opatch.obj\
,$(libCo)\
,$(libm)\
$(CCLDEND)

$(bin)opatch.obj   : $(app)opatch.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES),$(APP_CPPINCLUDES))/define=($(APP_CCDEFINES),$(HAS_CPPDEFINES))/obj=$(bin)opatch.obj $(app)opatch.c 
!.............................................
$(bin)oupper.exe : $(bin)oupper.obj 
	$(CCLD)$(CCLDFLAGS)/exe=$(bin)oupper.exe $(bin)oupper.obj\
$(CCLDEND)

$(bin)oupper.obj   : $(app)oupper.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES),$(APP_CPPINCLUDES))/define=($(APP_CCDEFINES),$(HAS_CPPDEFINES))/obj=$(bin)oupper.obj $(app)oupper.c 
!.............................................
$(bin)ohtxTohtml.exe : $(bin)ohtxTohtml.obj \
$(libCo_target) 
	$(CCLD)$(CCLDFLAGS)/exe=$(bin)ohtxTohtml.exe $(bin)ohtxTohtml.obj\
,$(libCo)\
,$(libm)\
$(CCLDEND)

$(bin)ohtxTohtml.obj   : $(app)ohtxTohtml.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES),$(APP_CPPINCLUDES))/define=($(APP_CCDEFINES),$(HAS_CPPDEFINES))/obj=$(bin)ohtxTohtml.obj $(app)ohtxTohtml.c 
!.............................................
$(bin)odoc.exe : $(bin)odoc.obj \
$(libCo_target) 
	$(CCLD)$(CCLDFLAGS)/exe=$(bin)odoc.exe $(bin)odoc.obj\
,$(libCo)\
,$(libm)\
$(CCLDEND)

$(bin)odoc.obj   : $(app)odoc.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES),$(APP_CPPINCLUDES))/define=($(APP_CCDEFINES),$(HAS_CPPDEFINES))/obj=$(bin)odoc.obj $(app)odoc.c 
!.............................................
$(bin)stdargT.exe : $(bin)stdargT.obj 
	$(CCLD)$(CCLDFLAGS)/exe=$(bin)stdargT.exe $(bin)stdargT.obj\
$(CCLDEND)

$(bin)stdargT.obj   : $(app)stdargT.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES),$(APP_CPPINCLUDES))/define=($(APP_CCDEFINES),$(HAS_CPPDEFINES))/obj=$(bin)stdargT.obj $(app)stdargT.c 
!.............................................
$(bin)CDefineT.exe : $(bin)CDefineT.obj 
	$(CCLD)$(CCLDFLAGS)/exe=$(bin)CDefineT.exe $(bin)CDefineT.obj\
$(CCLDEND)

$(bin)CDefineT.obj   : $(app)CDefineT.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES),$(APP_CPPINCLUDES))/define=($(APP_CCDEFINES),$(HAS_CPPDEFINES))/obj=$(bin)CDefineT.obj $(app)CDefineT.c 
!.............................................
$(bin)OBatchT.exe : $(bin)OBatchT.obj \
$(libCo_target) 
	$(CCLD)$(CCLDFLAGS)/exe=$(bin)OBatchT.exe $(bin)OBatchT.obj\
,$(libCo)\
,$(libm)\
$(CCLDEND)

$(bin)OBatchT.obj   : $(app)OBatchT.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES),$(APP_CPPINCLUDES))/define=($(APP_CCDEFINES),$(HAS_CPPDEFINES))/obj=$(bin)OBatchT.obj $(app)OBatchT.c 
!.............................................
$(bin)CFITST.exe : $(bin)CFITST.obj \
$(libCo_target) 
	$(CCLD)$(CCLDFLAGS)/exe=$(bin)CFITST.exe $(bin)CFITST.obj\
,$(libCo)\
,$(libm)\
$(CCLDEND)

$(bin)CFITST.obj   : $(app)CFITST.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES),$(APP_CPPINCLUDES))/define=($(APP_CCDEFINES),$(HAS_CPPDEFINES))/obj=$(bin)CFITST.obj $(app)CFITST.c 
!.............................................
$(bin)GIF_To_PS.exe : $(bin)GIF_To_PS.obj \
$(libCo_target) 
	$(CCLD)$(CCLDFLAGS)/exe=$(bin)GIF_To_PS.exe $(bin)GIF_To_PS.obj\
,$(libCo)\
,$(libm)\
$(CCLDEND)

$(bin)GIF_To_PS.obj   : $(app)GIF_To_PS.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES),$(APP_CPPINCLUDES))/define=($(APP_CCDEFINES),$(HAS_CPPDEFINES))/obj=$(bin)GIF_To_PS.obj $(app)GIF_To_PS.c 
!.............................................
$(bin)CGIFT.exe : $(bin)CGIFT.obj \
$(libCo_target) 
	$(CCLD)$(CCLDFLAGS)/exe=$(bin)CGIFT.exe $(bin)CGIFT.obj\
,$(libCo)\
,$(libm)\
$(CCLDEND)

$(bin)CGIFT.obj   : $(app)CGIFT.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES),$(APP_CPPINCLUDES))/define=($(APP_CCDEFINES),$(HAS_CPPDEFINES))/obj=$(bin)CGIFT.obj $(app)CGIFT.c 
!.............................................
$(bin)CLibraryT.exe : $(bin)CLibraryT.obj \
$(libCo_target) 
	$(CCLD)$(CCLDFLAGS)/exe=$(bin)CLibraryT.exe $(bin)CLibraryT.obj\
,$(libCo)\
,$(libm)\
$(CCLDEND)

$(bin)CLibraryT.obj   : $(app)CLibraryT.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES),$(APP_CPPINCLUDES))/define=($(APP_CCDEFINES),$(HAS_CPPDEFINES))/obj=$(bin)CLibraryT.obj $(app)CLibraryT.c 
!.............................................
$(bin)COptionT.exe : $(bin)COptionT.obj \
$(libCo_target) 
	$(CCLD)$(CCLDFLAGS)/exe=$(bin)COptionT.exe $(bin)COptionT.obj\
,$(libCo)\
,$(libm)\
$(CCLDEND)

$(bin)COptionT.obj   : $(app)COptionT.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES),$(APP_CPPINCLUDES))/define=($(APP_CCDEFINES),$(HAS_CPPDEFINES))/obj=$(bin)COptionT.obj $(app)COptionT.c 
!.............................................
$(bin)NoHT.exe : $(bin)NoHT.obj 
	$(CCLD)$(CCLDFLAGS)/exe=$(bin)NoHT.exe $(bin)NoHT.obj\
$(CCLDEND)

$(bin)NoHT.obj   : $(app)NoHT.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES),$(APP_CPPINCLUDES))/define=($(APP_CCDEFINES),$(HAS_CPPDEFINES))/obj=$(bin)NoHT.obj $(app)NoHT.c 
$(bin)libCo.exe : $(bin)libCo.olb 
	$(make_so) libCo Co $(CONFIG)
	if f$search("$(bin)libCo.opt;*") .nes. "" then delete $(bin)libCo.opt;*
	open/write file $(bin)libCo.opt
	write file "COROOT:[$(CONFIG)]libCo/share"
	close file

$(bin)libCo.olb : \
$(bin)libCo.olb(CoCommands) \
$(bin)libCo.olb(CManager) \
$(bin)libCo.olb(OModule) \
$(bin)libCo.olb(OTouch) \
$(bin)libCo.olb(CoTypes) \
$(bin)libCo.olb(OType) \
$(bin)libCo.olb(OProperty) \
$(bin)libCo.olb(OProcedureL) \
$(bin)libCo.olb(OProcedure) \
$(bin)libCo.olb(OProcess) \
$(bin)libCo.olb(OHandle) \
$(bin)libCo.olb(OShell) \
$(bin)libCo.olb(OCommand) \
$(bin)libCo.olb(OMatrix) \
$(bin)libCo.olb(OMatrix33) \
$(bin)libCo.olb(OLUT) \
$(bin)libCo.olb(CoF77) \
$(bin)libCo.olb(OBatch) \
$(bin)libCo.olb(FString) \
$(bin)libCo.olb(OHTML) \
$(bin)libCo.olb(OColor) \
$(bin)libCo.olb(CFITS) \
$(bin)libCo.olb(CPS) \
$(bin)libCo.olb(CGIF) \
$(bin)libCo.olb(CTIFF) \
$(bin)libCo.olb(CXPM) \
$(bin)libCo.olb(CFile) \
$(bin)libCo.olb(CStream) \
$(bin)libCo.olb(CText) \
$(bin)libCo.olb(CString) \
$(bin)libCo.olb(CCharacter) \
$(bin)libCo.olb(CColor) \
$(bin)libCo.olb(CMath) \
$(bin)libCo.olb(CVector) \
$(bin)libCo.olb(CList) \
$(bin)libCo.olb(CLibrary) \
$(bin)libCo.olb(CMemory) \
$(bin)libCo.olb(COption) \
$(bin)libCo.olb(CError) \
$(bin)libCo.olb(CPrinter) \
$(bin)libCo.olb(CTime) \
$(bin)libCo.olb(CJPEG) \
$(bin)libCo.olb(CImageshort) \
$(bin)libCo.olb(CSystem) \
$(bin)libCo.olb(CBatch) \
$(bin)libCo.olb(CoBinds) 
	if f$search("$(bin)libCo.opt;*") .nes. "" then delete $(bin)libCo.opt;*
	open/write file $(bin)libCo.opt
	write file "COROOT:[$(CONFIG)]libCo/lib"
	close file
!.............................................
! libCo dependencies 
!.............................................
$(bin)libCo.olb(CoCommands) : $(src)CoCommands.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)CoCommands.obj $(src)CoCommands.c
	$(inlib) $(bin)libCo.olb $(bin)CoCommands.obj
	del $(bin)CoCommands.obj;*

$(bin)libCo.olb(CManager) : $(src)CManager.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)CManager.obj $(src)CManager.c
	$(inlib) $(bin)libCo.olb $(bin)CManager.obj
	del $(bin)CManager.obj;*

$(bin)libCo.olb(OModule) : $(src)OModule.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)OModule.obj $(src)OModule.c
	$(inlib) $(bin)libCo.olb $(bin)OModule.obj
	del $(bin)OModule.obj;*

$(bin)libCo.olb(OTouch) : $(src)OTouch.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)OTouch.obj $(src)OTouch.c
	$(inlib) $(bin)libCo.olb $(bin)OTouch.obj
	del $(bin)OTouch.obj;*

$(bin)libCo.olb(CoTypes) : $(src)CoTypes.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)CoTypes.obj $(src)CoTypes.c
	$(inlib) $(bin)libCo.olb $(bin)CoTypes.obj
	del $(bin)CoTypes.obj;*

$(bin)libCo.olb(OType) : $(src)OType.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)OType.obj $(src)OType.c
	$(inlib) $(bin)libCo.olb $(bin)OType.obj
	del $(bin)OType.obj;*

$(bin)libCo.olb(OProperty) : $(src)OProperty.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)OProperty.obj $(src)OProperty.c
	$(inlib) $(bin)libCo.olb $(bin)OProperty.obj
	del $(bin)OProperty.obj;*

$(bin)libCo.olb(OProcedureL) : $(src)OProcedureL.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)OProcedureL.obj $(src)OProcedureL.c
	$(inlib) $(bin)libCo.olb $(bin)OProcedureL.obj
	del $(bin)OProcedureL.obj;*

$(bin)libCo.olb(OProcedure) : $(src)OProcedure.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)OProcedure.obj $(src)OProcedure.c
	$(inlib) $(bin)libCo.olb $(bin)OProcedure.obj
	del $(bin)OProcedure.obj;*

$(bin)libCo.olb(OProcess) : $(src)OProcess.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)OProcess.obj $(src)OProcess.c
	$(inlib) $(bin)libCo.olb $(bin)OProcess.obj
	del $(bin)OProcess.obj;*

$(bin)libCo.olb(OHandle) : $(src)OHandle.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)OHandle.obj $(src)OHandle.c
	$(inlib) $(bin)libCo.olb $(bin)OHandle.obj
	del $(bin)OHandle.obj;*

$(bin)libCo.olb(OShell) : $(src)OShell.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)OShell.obj $(src)OShell.c
	$(inlib) $(bin)libCo.olb $(bin)OShell.obj
	del $(bin)OShell.obj;*

$(bin)libCo.olb(OCommand) : $(src)OCommand.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)OCommand.obj $(src)OCommand.c
	$(inlib) $(bin)libCo.olb $(bin)OCommand.obj
	del $(bin)OCommand.obj;*

$(bin)libCo.olb(OMatrix) : $(src)OMatrix.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)OMatrix.obj $(src)OMatrix.c
	$(inlib) $(bin)libCo.olb $(bin)OMatrix.obj
	del $(bin)OMatrix.obj;*

$(bin)libCo.olb(OMatrix33) : $(src)OMatrix33.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)OMatrix33.obj $(src)OMatrix33.c
	$(inlib) $(bin)libCo.olb $(bin)OMatrix33.obj
	del $(bin)OMatrix33.obj;*

$(bin)libCo.olb(OLUT) : $(src)OLUT.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)OLUT.obj $(src)OLUT.c
	$(inlib) $(bin)libCo.olb $(bin)OLUT.obj
	del $(bin)OLUT.obj;*

$(bin)libCo.olb(CoF77) : $(src)CoF77.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)CoF77.obj $(src)CoF77.c
	$(inlib) $(bin)libCo.olb $(bin)CoF77.obj
	del $(bin)CoF77.obj;*

$(bin)libCo.olb(OBatch) : $(src)OBatch.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)OBatch.obj $(src)OBatch.c
	$(inlib) $(bin)libCo.olb $(bin)OBatch.obj
	del $(bin)OBatch.obj;*

$(bin)libCo.olb(FString) : $(src)FString.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)FString.obj $(src)FString.c
	$(inlib) $(bin)libCo.olb $(bin)FString.obj
	del $(bin)FString.obj;*

$(bin)libCo.olb(OHTML) : $(src)OHTML.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)OHTML.obj $(src)OHTML.c
	$(inlib) $(bin)libCo.olb $(bin)OHTML.obj
	del $(bin)OHTML.obj;*

$(bin)libCo.olb(OColor) : $(src)OColor.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)OColor.obj $(src)OColor.c
	$(inlib) $(bin)libCo.olb $(bin)OColor.obj
	del $(bin)OColor.obj;*

$(bin)libCo.olb(CFITS) : $(src)CFITS.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)CFITS.obj $(src)CFITS.c
	$(inlib) $(bin)libCo.olb $(bin)CFITS.obj
	del $(bin)CFITS.obj;*

$(bin)libCo.olb(CPS) : $(src)CPS.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)CPS.obj $(src)CPS.c
	$(inlib) $(bin)libCo.olb $(bin)CPS.obj
	del $(bin)CPS.obj;*

$(bin)libCo.olb(CGIF) : $(src)CGIF.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)CGIF.obj $(src)CGIF.c
	$(inlib) $(bin)libCo.olb $(bin)CGIF.obj
	del $(bin)CGIF.obj;*

$(bin)libCo.olb(CTIFF) : $(src)CTIFF.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)CTIFF.obj $(src)CTIFF.c
	$(inlib) $(bin)libCo.olb $(bin)CTIFF.obj
	del $(bin)CTIFF.obj;*

$(bin)libCo.olb(CXPM) : $(src)CXPM.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)CXPM.obj $(src)CXPM.c
	$(inlib) $(bin)libCo.olb $(bin)CXPM.obj
	del $(bin)CXPM.obj;*

$(bin)libCo.olb(CFile) : $(src)CFile.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)CFile.obj $(src)CFile.c
	$(inlib) $(bin)libCo.olb $(bin)CFile.obj
	del $(bin)CFile.obj;*

$(bin)libCo.olb(CStream) : $(src)CStream.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)CStream.obj $(src)CStream.c
	$(inlib) $(bin)libCo.olb $(bin)CStream.obj
	del $(bin)CStream.obj;*

$(bin)libCo.olb(CText) : $(src)CText.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)CText.obj $(src)CText.c
	$(inlib) $(bin)libCo.olb $(bin)CText.obj
	del $(bin)CText.obj;*

$(bin)libCo.olb(CString) : $(src)CString.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)CString.obj $(src)CString.c
	$(inlib) $(bin)libCo.olb $(bin)CString.obj
	del $(bin)CString.obj;*

$(bin)libCo.olb(CCharacter) : $(src)CCharacter.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)CCharacter.obj $(src)CCharacter.c
	$(inlib) $(bin)libCo.olb $(bin)CCharacter.obj
	del $(bin)CCharacter.obj;*

$(bin)libCo.olb(CColor) : $(src)CColor.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)CColor.obj $(src)CColor.c
	$(inlib) $(bin)libCo.olb $(bin)CColor.obj
	del $(bin)CColor.obj;*

$(bin)libCo.olb(CMath) : $(src)CMath.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)CMath.obj $(src)CMath.c
	$(inlib) $(bin)libCo.olb $(bin)CMath.obj
	del $(bin)CMath.obj;*

$(bin)libCo.olb(CVector) : $(src)CVector.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)CVector.obj $(src)CVector.c
	$(inlib) $(bin)libCo.olb $(bin)CVector.obj
	del $(bin)CVector.obj;*

$(bin)libCo.olb(CList) : $(src)CList.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)CList.obj $(src)CList.c
	$(inlib) $(bin)libCo.olb $(bin)CList.obj
	del $(bin)CList.obj;*

$(bin)libCo.olb(CLibrary) : $(src)CLibrary.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)CLibrary.obj $(src)CLibrary.c
	$(inlib) $(bin)libCo.olb $(bin)CLibrary.obj
	del $(bin)CLibrary.obj;*

$(bin)libCo.olb(CMemory) : $(src)CMemory.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)CMemory.obj $(src)CMemory.c
	$(inlib) $(bin)libCo.olb $(bin)CMemory.obj
	del $(bin)CMemory.obj;*

$(bin)libCo.olb(COption) : $(src)COption.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)COption.obj $(src)COption.c
	$(inlib) $(bin)libCo.olb $(bin)COption.obj
	del $(bin)COption.obj;*

$(bin)libCo.olb(CError) : $(src)CError.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)CError.obj $(src)CError.c
	$(inlib) $(bin)libCo.olb $(bin)CError.obj
	del $(bin)CError.obj;*

$(bin)libCo.olb(CPrinter) : $(src)CPrinter.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)CPrinter.obj $(src)CPrinter.c
	$(inlib) $(bin)libCo.olb $(bin)CPrinter.obj
	del $(bin)CPrinter.obj;*

$(bin)libCo.olb(CTime) : $(src)CTime.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)CTime.obj $(src)CTime.c
	$(inlib) $(bin)libCo.olb $(bin)CTime.obj
	del $(bin)CTime.obj;*

$(bin)libCo.olb(CJPEG) : $(src)CJPEG.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)CJPEG.obj $(src)CJPEG.c
	$(inlib) $(bin)libCo.olb $(bin)CJPEG.obj
	del $(bin)CJPEG.obj;*

$(bin)libCo.olb(CImageshort) : $(src)CImageshort.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)CImageshort.obj $(src)CImageshort.c
	$(inlib) $(bin)libCo.olb $(bin)CImageshort.obj
	del $(bin)CImageshort.obj;*

$(bin)libCo.olb(CSystem) : $(src)CSystem.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)CSystem.obj $(src)CSystem.c
	$(inlib) $(bin)libCo.olb $(bin)CSystem.obj
	del $(bin)CSystem.obj;*

$(bin)libCo.olb(CBatch) : $(src)CBatch.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)CBatch.obj $(src)CBatch.c
	$(inlib) $(bin)libCo.olb $(bin)CBatch.obj
	del $(bin)CBatch.obj;*

$(bin)libCo.olb(CoBinds) : $(src)CoBinds.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)CoBinds.obj $(src)CoBinds.c
	$(inlib) $(bin)libCo.olb $(bin)CoBinds.obj
	del $(bin)CoBinds.obj;*


