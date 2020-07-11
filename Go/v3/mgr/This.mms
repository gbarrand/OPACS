!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!  File produced by the Co/omake tool
! using file Make.odb.
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!  The soft linked packages has been taken
! from the property 'has' of the 
! 'Make' object of file Make.odb.
! It had the value :
!    X11 Xext GL
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
HAS_CCFLAGS = \
$(Go_CCFLAGS)\
$(X11_CCFLAGS)\
$(Xext_CCFLAGS)\
$(GL_CCFLAGS)
HAS_CXXFLAGS = \
$(Go_CXXFLAGS)\
$(X11_CXXFLAGS)\
$(Xext_CXXFLAGS)\
$(GL_CXXFLAGS)
HAS_CPPINCLUDES = \
 $(Go_CPPINCLUDES)\
,$(X11_CPPINCLUDES)\
,$(Xext_CPPINCLUDES)\
,$(GL_CPPINCLUDES)
HAS_CPPDEFINES = \
 $(Go_CPPDEFINES)\
,$(X11_CPPDEFINES)\
,$(Xext_CPPDEFINES)\
,$(GL_CPPDEFINES)
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

libGo_target = $(bin)libGo.$(olb_or_exe)
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
all :: \
$(libGo_target) \
$(bin)GoT.exe 
	@ write sys$output "Go : all ok." 

libs :: \
$(libGo_target) 
	@ write sys$output "Go : libs ok." 

apps :: \
$(bin)GoT.exe \
$(bin)oshGo.exe \
$(bin)o3dToodb.exe \
$(bin)ograd.exe \
$(bin)NoHT.exe 
	@ write sys$output "Go : apps ok." 
!.............................................
rmexeo :
	if f$search("$(bin)GoT.obj;*") .nes. "" then del $(bin)GoT.obj;*
	if f$search("$(bin)oshGo.obj;*") .nes. "" then del $(bin)oshGo.obj;*
	if f$search("$(bin)o3dToodb.obj;*") .nes. "" then del $(bin)o3dToodb.obj;*
	if f$search("$(bin)ograd.obj;*") .nes. "" then del $(bin)ograd.obj;*
	if f$search("$(bin)NoHT.obj;*") .nes. "" then del $(bin)NoHT.obj;*
!.............................................
GoT : $(bin)GoT.exe
	@ write sys$output "Go : GoT ok."
oshGo : $(bin)oshGo.exe
	@ write sys$output "Go : oshGo ok."
o3dToodb : $(bin)o3dToodb.exe
	@ write sys$output "Go : o3dToodb ok."
ograd : $(bin)ograd.exe
	@ write sys$output "Go : ograd ok."
NoHT : $(bin)NoHT.exe
	@ write sys$output "Go : NoHT ok."
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!.............................................
$(bin)GoT.exe : $(bin)GoT.obj \
$(libGo_target) 
	$(CCLD)$(CCLDFLAGS)/exe=$(bin)GoT.exe $(bin)GoT.obj\
,$(libGo)\
,$(libGLX)\
,$(libGLU)\
,$(libGL)\
,$(libXext)\
,$(libX11)\
,$(libCo)\
,$(libm)\
$(CCLDEND)

$(bin)GoT.obj   : $(app)GoT.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES),$(APP_CPPINCLUDES))/define=($(APP_CCDEFINES),$(HAS_CPPDEFINES))/obj=$(bin)GoT.obj $(app)GoT.c 
!.............................................
$(bin)oshGo.exe : $(bin)oshGo.obj \
$(libGo_target) 
	$(CCLD)$(CCLDFLAGS)/exe=$(bin)oshGo.exe $(bin)oshGo.obj\
,$(libGo)\
,$(libGLU)\
,$(libGL)\
,$(libXext)\
,$(libX11)\
,$(libCo)\
,$(libm)\
$(CCLDEND)

$(bin)oshGo.obj   : $(app)oshGo.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES),$(APP_CPPINCLUDES))/define=($(APP_CCDEFINES),$(HAS_CPPDEFINES))/obj=$(bin)oshGo.obj $(app)oshGo.c 
!.............................................
$(bin)o3dToodb.exe : $(bin)o3dToodb.obj \
$(libGo_target) 
	$(CCLD)$(CCLDFLAGS)/exe=$(bin)o3dToodb.exe $(bin)o3dToodb.obj\
,$(libGo)\
,$(libGLU)\
,$(libGL)\
,$(libXext)\
,$(libX11)\
,$(libCo)\
,$(libm)\
$(CCLDEND)

$(bin)o3dToodb.obj   : $(app)o3dToodb.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES),$(APP_CPPINCLUDES))/define=($(APP_CCDEFINES),$(HAS_CPPDEFINES))/obj=$(bin)o3dToodb.obj $(app)o3dToodb.c 
!.............................................
$(bin)ograd.exe : $(bin)ograd.obj 
	$(CCLD)$(CCLDFLAGS)/exe=$(bin)ograd.exe $(bin)ograd.obj\
,$(libCo)\
,$(libm)\
$(CCLDEND)

$(bin)ograd.obj   : $(app)ograd.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES),$(APP_CPPINCLUDES))/define=($(APP_CCDEFINES),$(HAS_CPPDEFINES))/obj=$(bin)ograd.obj $(app)ograd.c 
!.............................................
$(bin)NoHT.exe : $(bin)NoHT.obj 
	$(CCLD)$(CCLDFLAGS)/exe=$(bin)NoHT.exe $(bin)NoHT.obj\
$(CCLDEND)

$(bin)NoHT.obj   : $(app)NoHT.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES),$(APP_CPPINCLUDES))/define=($(APP_CCDEFINES),$(HAS_CPPDEFINES))/obj=$(bin)NoHT.obj $(app)NoHT.c 
$(bin)libGo.exe : $(bin)libGo.olb 
	$(make_so) libGo Go $(CONFIG)
	if f$search("$(bin)libGo.opt;*") .nes. "" then delete $(bin)libGo.opt;*
	open/write file $(bin)libGo.opt
	write file "GOROOT:[$(CONFIG)]libGo/share"
	close file

$(bin)libGo.olb : \
$(bin)libGo.olb(GoCommands) \
$(bin)libGo.olb(GoTypes) \
$(bin)libGo.olb(GoF77) \
$(bin)libGo.olb(OColormap) \
$(bin)libGo.olb(OCamera) \
$(bin)libGo.olb(OCamVRML) \
$(bin)libGo.olb(OCamDAWN) \
$(bin)libGo.olb(OCamPOV) \
$(bin)libGo.olb(OPick) \
$(bin)libGo.olb(OPointList) \
$(bin)libGo.olb(Go) \
$(bin)libGo.olb(OShape) \
$(bin)libGo.olb(ONode) \
$(bin)libGo.olb(OPrimitive) \
$(bin)libGo.olb(OContextmap) \
$(bin)libGo.olb(OContext) \
$(bin)libGo.olb(GoBinds) 
	if f$search("$(bin)libGo.opt;*") .nes. "" then delete $(bin)libGo.opt;*
	open/write file $(bin)libGo.opt
	write file "GOROOT:[$(CONFIG)]libGo/lib"
	close file
!.............................................
! libGo dependencies 
!.............................................
$(bin)libGo.olb(GoCommands) : $(src)GoCommands.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)GoCommands.obj $(src)GoCommands.c
	$(inlib) $(bin)libGo.olb $(bin)GoCommands.obj
	del $(bin)GoCommands.obj;*

$(bin)libGo.olb(GoTypes) : $(src)GoTypes.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)GoTypes.obj $(src)GoTypes.c
	$(inlib) $(bin)libGo.olb $(bin)GoTypes.obj
	del $(bin)GoTypes.obj;*

$(bin)libGo.olb(GoF77) : $(src)GoF77.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)GoF77.obj $(src)GoF77.c
	$(inlib) $(bin)libGo.olb $(bin)GoF77.obj
	del $(bin)GoF77.obj;*

$(bin)libGo.olb(OColormap) : $(src)OColormap.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)OColormap.obj $(src)OColormap.c
	$(inlib) $(bin)libGo.olb $(bin)OColormap.obj
	del $(bin)OColormap.obj;*

$(bin)libGo.olb(OCamera) : $(src)OCamera.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)OCamera.obj $(src)OCamera.c
	$(inlib) $(bin)libGo.olb $(bin)OCamera.obj
	del $(bin)OCamera.obj;*

$(bin)libGo.olb(OCamVRML) : $(src)OCamVRML.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)OCamVRML.obj $(src)OCamVRML.c
	$(inlib) $(bin)libGo.olb $(bin)OCamVRML.obj
	del $(bin)OCamVRML.obj;*

$(bin)libGo.olb(OCamDAWN) : $(src)OCamDAWN.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)OCamDAWN.obj $(src)OCamDAWN.c
	$(inlib) $(bin)libGo.olb $(bin)OCamDAWN.obj
	del $(bin)OCamDAWN.obj;*

$(bin)libGo.olb(OCamPOV) : $(src)OCamPOV.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)OCamPOV.obj $(src)OCamPOV.c
	$(inlib) $(bin)libGo.olb $(bin)OCamPOV.obj
	del $(bin)OCamPOV.obj;*

$(bin)libGo.olb(OPick) : $(src)OPick.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)OPick.obj $(src)OPick.c
	$(inlib) $(bin)libGo.olb $(bin)OPick.obj
	del $(bin)OPick.obj;*

$(bin)libGo.olb(OPointList) : $(src)OPointList.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)OPointList.obj $(src)OPointList.c
	$(inlib) $(bin)libGo.olb $(bin)OPointList.obj
	del $(bin)OPointList.obj;*

$(bin)libGo.olb(Go) : $(src)Go.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)Go.obj $(src)Go.c
	$(inlib) $(bin)libGo.olb $(bin)Go.obj
	del $(bin)Go.obj;*

$(bin)libGo.olb(OShape) : $(src)OShape.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)OShape.obj $(src)OShape.c
	$(inlib) $(bin)libGo.olb $(bin)OShape.obj
	del $(bin)OShape.obj;*

$(bin)libGo.olb(ONode) : $(src)ONode.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)ONode.obj $(src)ONode.c
	$(inlib) $(bin)libGo.olb $(bin)ONode.obj
	del $(bin)ONode.obj;*

$(bin)libGo.olb(OPrimitive) : $(src)OPrimitive.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)OPrimitive.obj $(src)OPrimitive.c
	$(inlib) $(bin)libGo.olb $(bin)OPrimitive.obj
	del $(bin)OPrimitive.obj;*

$(bin)libGo.olb(OContextmap) : $(src)OContextmap.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)OContextmap.obj $(src)OContextmap.c
	$(inlib) $(bin)libGo.olb $(bin)OContextmap.obj
	del $(bin)OContextmap.obj;*

$(bin)libGo.olb(OContext) : $(src)OContext.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)OContext.obj $(src)OContext.c
	$(inlib) $(bin)libGo.olb $(bin)OContext.obj
	del $(bin)OContext.obj;*

$(bin)libGo.olb(GoBinds) : $(src)GoBinds.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)GoBinds.obj $(src)GoBinds.c
	$(inlib) $(bin)libGo.olb $(bin)GoBinds.obj
	del $(bin)GoBinds.obj;*


