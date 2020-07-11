!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!  File produced by the Co/omake tool
! using file Make.odb.
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!  The soft linked packages has been taken
! from the command line option '-has'.
! It had the value :
!    X11 Xext Xt Xmu Xm WoXm WoGL WoXo WoHTML
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
HAS_CCFLAGS = \
$(Wo_CCFLAGS)\
$(X11_CCFLAGS)\
$(Xext_CCFLAGS)\
$(Xt_CCFLAGS)\
$(Xmu_CCFLAGS)\
$(Xm_CCFLAGS)\
$(WoXm_CCFLAGS)\
$(WoGL_CCFLAGS)\
$(WoXo_CCFLAGS)\
$(WoHTML_CCFLAGS)
HAS_CXXFLAGS = \
$(Wo_CXXFLAGS)\
$(X11_CXXFLAGS)\
$(Xext_CXXFLAGS)\
$(Xt_CXXFLAGS)\
$(Xmu_CXXFLAGS)\
$(Xm_CXXFLAGS)\
$(WoXm_CXXFLAGS)\
$(WoGL_CXXFLAGS)\
$(WoXo_CXXFLAGS)\
$(WoHTML_CXXFLAGS)
HAS_CPPINCLUDES = \
 $(Wo_CPPINCLUDES)\
,$(X11_CPPINCLUDES)\
,$(Xext_CPPINCLUDES)\
,$(Xt_CPPINCLUDES)\
,$(Xmu_CPPINCLUDES)\
,$(Xm_CPPINCLUDES)\
,$(WoXm_CPPINCLUDES)\
,$(WoGL_CPPINCLUDES)\
,$(WoXo_CPPINCLUDES)\
,$(WoHTML_CPPINCLUDES)
HAS_CPPDEFINES = \
 $(Wo_CPPDEFINES)\
,$(X11_CPPDEFINES)\
,$(Xext_CPPDEFINES)\
,$(Xt_CPPDEFINES)\
,$(Xmu_CPPDEFINES)\
,$(Xm_CPPDEFINES)\
,$(WoXm_CPPDEFINES)\
,$(WoGL_CPPDEFINES)\
,$(WoXo_CPPDEFINES)\
,$(WoHTML_CPPDEFINES)
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

libWo_target = $(bin)libWo.$(olb_or_exe)
libWoXm_target = $(bin)libWoXm.$(olb_or_exe)
libWoGL_target = $(bin)libWoGL.$(olb_or_exe)
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
all :: \
$(libWo_target) \
$(libWoXm_target) \
$(libWoGL_target) \
$(bin)oshow.exe \
$(bin)onew.exe 
	@ write sys$output "Wo : all ok." 

libs :: \
$(libWo_target) \
$(libWoXm_target) \
$(libWoGL_target) 
	@ write sys$output "Wo : libs ok." 

apps :: \
$(bin)oshow.exe \
$(bin)onew.exe \
$(bin)oiv.exe \
$(bin)oshWo.exe \
$(bin)WoT.exe \
$(bin)CiT.exe \
$(bin)NoHT.exe \
$(bin)tclT.exe \
$(bin)ouiToodb.exe \
$(bin)ovoid.exe \
$(bin)jo.exe \
$(bin)jolet.exe \
$(bin)JavaT.exe \
$(bin)wof77t.exe 
	@ write sys$output "Wo : apps ok." 
!.............................................
rmexeo :
	if f$search("$(bin)oshow.obj;*") .nes. "" then del $(bin)oshow.obj;*
	if f$search("$(bin)onew.obj;*") .nes. "" then del $(bin)onew.obj;*
	if f$search("$(bin)oiv.obj;*") .nes. "" then del $(bin)oiv.obj;*
	if f$search("$(bin)oshWo.obj;*") .nes. "" then del $(bin)oshWo.obj;*
	if f$search("$(bin)WoT.obj;*") .nes. "" then del $(bin)WoT.obj;*
	if f$search("$(bin)CiT.obj;*") .nes. "" then del $(bin)CiT.obj;*
	if f$search("$(bin)NoHT.obj;*") .nes. "" then del $(bin)NoHT.obj;*
	if f$search("$(bin)tclT.obj;*") .nes. "" then del $(bin)tclT.obj;*
	if f$search("$(bin)ouiToodb.obj;*") .nes. "" then del $(bin)ouiToodb.obj;*
	if f$search("$(bin)ovoid.obj;*") .nes. "" then del $(bin)ovoid.obj;*
	if f$search("$(bin)jo.obj;*") .nes. "" then del $(bin)jo.obj;*
	if f$search("$(bin)jolet.obj;*") .nes. "" then del $(bin)jolet.obj;*
	if f$search("$(bin)JavaT.obj;*") .nes. "" then del $(bin)JavaT.obj;*
	if f$search("$(bin)wof77t.obj;*") .nes. "" then del $(bin)wof77t.obj;*
!.............................................
oshow : $(bin)oshow.exe
	@ write sys$output "Wo : oshow ok."
onew : $(bin)onew.exe
	@ write sys$output "Wo : onew ok."
oiv : $(bin)oiv.exe
	@ write sys$output "Wo : oiv ok."
oshWo : $(bin)oshWo.exe
	@ write sys$output "Wo : oshWo ok."
WoT : $(bin)WoT.exe
	@ write sys$output "Wo : WoT ok."
CiT : $(bin)CiT.exe
	@ write sys$output "Wo : CiT ok."
NoHT : $(bin)NoHT.exe
	@ write sys$output "Wo : NoHT ok."
tclT : $(bin)tclT.exe
	@ write sys$output "Wo : tclT ok."
ouiToodb : $(bin)ouiToodb.exe
	@ write sys$output "Wo : ouiToodb ok."
ovoid : $(bin)ovoid.exe
	@ write sys$output "Wo : ovoid ok."
jo : $(bin)jo.exe
	@ write sys$output "Wo : jo ok."
jolet : $(bin)jolet.exe
	@ write sys$output "Wo : jolet ok."
JavaT : $(bin)JavaT.exe
	@ write sys$output "Wo : JavaT ok."
wof77t : $(bin)wof77t.exe
	@ write sys$output "Wo : wof77t ok."
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!.............................................
$(bin)oshow.exe : $(bin)oshow.obj \
$(libWoGL_target) \
$(libWoXm_target) \
$(libWo_target) 
	$(CCLD)$(CCLDFLAGS)/exe=$(bin)oshow.exe $(bin)oshow.obj\
,$(libWoGL)\
,$(libXo)\
,$(libhtmlw)\
,$(libWoXm)\
,$(libXm)\
,$(libWo)\
,$(libXx)\
,$(libXmu)\
,$(libXt)\
,$(libGo)\
,$(libGLX)\
,$(libGLU)\
,$(libGL)\
,$(libXext)\
,$(libX11)\
,$(libHo)\
,$(libCo)\
,$(libwww)\
,$(libm)\
$(CCLDEND)

$(bin)oshow.obj   : $(app)oshow.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES),$(APP_CPPINCLUDES))/define=($(APP_CCDEFINES),$(HAS_CPPDEFINES))/obj=$(bin)oshow.obj $(app)oshow.c 
!.............................................
$(bin)onew.exe : $(bin)onew.obj \
$(libWoGL_target) \
$(libWoXm_target) \
$(libWo_target) 
	$(CCLD)$(CCLDFLAGS)/exe=$(bin)onew.exe $(bin)onew.obj\
,$(libWoGL)\
,$(libXo)\
,$(libhtmlw)\
,$(libWoXm)\
,$(libXm)\
,$(libWo)\
,$(libXx)\
,$(libXmu)\
,$(libXt)\
,$(libGo)\
,$(libGLX)\
,$(libGLU)\
,$(libGL)\
,$(libXext)\
,$(libX11)\
,$(libHo)\
,$(libCo)\
,$(libwww)\
,$(libm)\
$(CCLDEND)

$(bin)onew.obj   : $(app)onew.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES),$(APP_CPPINCLUDES))/define=($(APP_CCDEFINES),$(HAS_CPPDEFINES))/obj=$(bin)onew.obj $(app)onew.c 
!.............................................
$(bin)oiv.exe : $(bin)oiv.obj \
$(libWoGL_target) \
$(libWoXm_target) \
$(libWo_target) 
	$(CXXLD)$(CXXLDFLAGS)/exe=$(bin)oiv.exe $(bin)oiv.obj\
,$(libWoGL)\
,$(libXo)\
,$(libhtmlw)\
,$(libWoXm)\
,$(libXm)\
,$(libWo)\
,$(libXx)\
,$(libXmu)\
,$(libXt)\
,$(libGo)\
,$(libGLX)\
,$(libGLU)\
,$(libGL)\
,$(libXext)\
,$(libX11)\
,$(libHo)\
,$(libCo)\
,$(libwww)\
,$(libm)\
$(CXXLDEND)

$(bin)oiv.obj   : $(app)oiv.cxx
	$(CXX)$(CXXFLAGS)$(HAS_CXXFLAGS)/include=($(HAS_CPPINCLUDES),$(APP_CPPINCLUDES))/define=($(APP_CXXDEFINES),$(HAS_CPPDEFINES))/obj=$(bin)oiv.obj $(app)oiv.cxx
!.............................................
$(bin)oshWo.exe : $(bin)oshWo.obj \
$(libWoGL_target) \
$(libWoXm_target) \
$(libWo_target) 
	$(CCLD)$(CCLDFLAGS)/exe=$(bin)oshWo.exe $(bin)oshWo.obj\
,$(libWoGL)\
,$(libXo)\
,$(libhtmlw)\
,$(libWoXm)\
,$(libXm)\
,$(libWo)\
,$(libXx)\
,$(libXmu)\
,$(libXt)\
,$(libGo)\
,$(libGLX)\
,$(libGLU)\
,$(libGL)\
,$(libXext)\
,$(libX11)\
,$(libHo)\
,$(libCo)\
,$(libwww)\
,$(libm)\
$(CCLDEND)

$(bin)oshWo.obj   : $(app)oshWo.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES),$(APP_CPPINCLUDES))/define=($(APP_CCDEFINES),$(HAS_CPPDEFINES))/obj=$(bin)oshWo.obj $(app)oshWo.c 
!.............................................
$(bin)WoT.exe : $(bin)WoT.obj \
$(libWoGL_target) \
$(libWoXm_target) \
$(libWo_target) 
	$(CCLD)$(CCLDFLAGS)/exe=$(bin)WoT.exe $(bin)WoT.obj\
,$(libWoGL)\
,$(libXo)\
,$(libhtmlw)\
,$(libWoXm)\
,$(libXm)\
,$(libWo)\
,$(libXx)\
,$(libXmu)\
,$(libXt)\
,$(libGo)\
,$(libGLX)\
,$(libGLU)\
,$(libGL)\
,$(libXext)\
,$(libX11)\
,$(libHo)\
,$(libCo)\
,$(libwww)\
,$(libm)\
$(CCLDEND)

$(bin)WoT.obj   : $(app)WoT.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES),$(APP_CPPINCLUDES))/define=($(APP_CCDEFINES),$(HAS_CPPDEFINES))/obj=$(bin)WoT.obj $(app)WoT.c 
!.............................................
$(bin)CiT.exe : $(bin)CiT.obj 
	$(CCLD)$(CCLDFLAGS)/exe=$(bin)CiT.exe $(bin)CiT.obj\
,$(libGo)\
,$(libCo)\
,$(libm)\
$(CCLDEND)

$(bin)CiT.obj   : $(app)CiT.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES),$(APP_CPPINCLUDES))/define=($(APP_CCDEFINES),$(HAS_CPPDEFINES))/obj=$(bin)CiT.obj $(app)CiT.c 
!.............................................
$(bin)NoHT.exe : $(bin)NoHT.obj 
	$(CCLD)$(CCLDFLAGS)/exe=$(bin)NoHT.exe $(bin)NoHT.obj\
$(CCLDEND)

$(bin)NoHT.obj   : $(app)NoHT.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES),$(APP_CPPINCLUDES))/define=($(APP_CCDEFINES),$(HAS_CPPDEFINES))/obj=$(bin)NoHT.obj $(app)NoHT.c 
!.............................................
$(bin)tclT.exe : $(bin)tclT.obj 
	$(CCLD)$(CCLDFLAGS)/exe=$(bin)tclT.exe $(bin)tclT.obj\
,$(libCo)\
,$(libm)\
$(CCLDEND)

$(bin)tclT.obj   : $(app)tclT.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES),$(APP_CPPINCLUDES))/define=($(APP_CCDEFINES),$(HAS_CPPDEFINES))/obj=$(bin)tclT.obj $(app)tclT.c 
!.............................................
$(bin)ouiToodb.exe : $(bin)ouiToodb.obj \
$(libWo_target) 
	$(CCLD)$(CCLDFLAGS)/exe=$(bin)ouiToodb.exe $(bin)ouiToodb.obj\
,$(libWo)\
,$(libXx)\
,$(libXmu)\
,$(libXt)\
,$(libXext)\
,$(libX11)\
,$(libCo)\
,$(libm)\
$(CCLDEND)

$(bin)ouiToodb.obj   : $(app)ouiToodb.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES),$(APP_CPPINCLUDES))/define=($(APP_CCDEFINES),$(HAS_CPPDEFINES))/obj=$(bin)ouiToodb.obj $(app)ouiToodb.c 
!.............................................
$(bin)ovoid.exe : $(bin)ovoid.obj \
$(libWo_target) 
	$(CCLD)$(CCLDFLAGS)/exe=$(bin)ovoid.exe $(bin)ovoid.obj\
,$(libWo)\
,$(libXx)\
,$(libXmu)\
,$(libXt)\
,$(libXext)\
,$(libX11)\
,$(libCo)\
,$(libm)\
$(CCLDEND)

$(bin)ovoid.obj   : $(app)ovoid.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES),$(APP_CPPINCLUDES))/define=($(APP_CCDEFINES),$(HAS_CPPDEFINES))/obj=$(bin)ovoid.obj $(app)ovoid.c 
!.............................................
$(bin)jo.exe : $(bin)jo.obj 
	$(CCLD)$(CCLDFLAGS)/exe=$(bin)jo.exe $(bin)jo.obj\
$(F77LDEND)
!.............................................
$(bin)jolet.exe : $(bin)jolet.obj 
	$(CCLD)$(CCLDFLAGS)/exe=$(bin)jolet.exe $(bin)jolet.obj\
$(F77LDEND)
!.............................................
$(bin)JavaT.exe : $(bin)JavaT.obj 
	$(CCLD)$(CCLDFLAGS)/exe=$(bin)JavaT.exe $(bin)JavaT.obj\
$(F77LDEND)
!.............................................
$(bin)wof77t.exe : $(bin)wof77t.obj \
$(bin)woinit.obj \
$(libWoGL_target) \
$(libWoXm_target) \
$(libWo_target) 
	$(F77LD)$(F77LDFLAGS)/exe=$(bin)wof77t.exe $(bin)wof77t.obj\
,$(bin)woinit.obj\
,$(libWoGL)\
,$(libXo)\
,$(libhtmlw)\
,$(libWoXm)\
,$(libXm)\
,$(libWo)\
,$(libXx)\
,$(libXmu)\
,$(libXt)\
,$(libGo)\
,$(libGLX)\
,$(libGLU)\
,$(libGL)\
,$(libXext)\
,$(libX11)\
,$(libHo)\
,$(libCo)\
,$(libwww)\
,$(libm)\
$(F77LDEND)

$(bin)wof77t.obj   : $(app)wof77t.f
	$(F77)$(F77FLAGS)/obj=$(bin)wof77t.obj $(app)wof77t.f
$(bin)libWo.exe : $(bin)libWo.olb 
	$(make_so) libWo Wo $(CONFIG)
	if f$search("$(bin)libWo.opt;*") .nes. "" then delete $(bin)libWo.opt;*
	open/write file $(bin)libWo.opt
	write file "WOROOT:[$(CONFIG)]libWo/share"
	close file

$(bin)libWo.olb : \
$(bin)libWo.olb(WoF77) \
$(bin)libWo.olb(WoCommands) \
$(bin)libWo.olb(WoTypes) \
$(bin)libWo.olb(Wo) \
$(bin)libWo.olb(OInterface) \
$(bin)libWo.olb(OPiece) \
$(bin)libWo.olb(OWidget) \
$(bin)libWo.olb(OClass) \
$(bin)libWo.olb(OPart) \
$(bin)libWo.olb(OInterpreter) \
$(bin)libWo.olb(OCyclic) \
$(bin)libWo.olb(XtwTools) \
$(bin)libWo.olb(OResource) \
$(bin)libWo.olb(OEvent) \
$(bin)libWo.olb(OTrace) \
$(bin)libWo.olb(WoBinds) \
$(bin)libWo.olb(X11Binds) 
	if f$search("$(bin)libWo.opt;*") .nes. "" then delete $(bin)libWo.opt;*
	open/write file $(bin)libWo.opt
	write file "WOROOT:[$(CONFIG)]libWo/lib"
	close file
$(bin)libWoXm.exe : $(bin)libWoXm.olb \
$(libWo_target) 
	$(make_so) libWoXm Wo $(CONFIG)
	if f$search("$(bin)libWoXm.opt;*") .nes. "" then delete $(bin)libWoXm.opt;*
	open/write file $(bin)libWoXm.opt
	write file "WOROOT:[$(CONFIG)]libWoXm/share"
	close file

$(bin)libWoXm.olb : \
$(bin)libWoXm.olb(XmTools) \
$(bin)libWoXm.olb(XmCommands) \
$(bin)libWoXm.olb(XmBinds) \
$(libWo_target) 
	if f$search("$(bin)libWoXm.opt;*") .nes. "" then delete $(bin)libWoXm.opt;*
	open/write file $(bin)libWoXm.opt
	write file "WOROOT:[$(CONFIG)]libWoXm/lib"
	close file
$(bin)libWoGL.exe : $(bin)libWoGL.olb 
	$(make_so) libWoGL Wo $(CONFIG)
	if f$search("$(bin)libWoGL.opt;*") .nes. "" then delete $(bin)libWoGL.opt;*
	open/write file $(bin)libWoGL.opt
	write file "WOROOT:[$(CONFIG)]libWoGL/share"
	close file

$(bin)libWoGL.olb : \
$(bin)libWoGL.olb(GL_Binds) 
	if f$search("$(bin)libWoGL.opt;*") .nes. "" then delete $(bin)libWoGL.opt;*
	open/write file $(bin)libWoGL.opt
	write file "WOROOT:[$(CONFIG)]libWoGL/lib"
	close file
!.............................................
! libWo dependencies 
!.............................................
$(bin)libWo.olb(WoF77) : $(src)WoF77.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)WoF77.obj $(src)WoF77.c
	$(inlib) $(bin)libWo.olb $(bin)WoF77.obj
	del $(bin)WoF77.obj;*

$(bin)libWo.olb(WoCommands) : $(src)WoCommands.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)WoCommands.obj $(src)WoCommands.c
	$(inlib) $(bin)libWo.olb $(bin)WoCommands.obj
	del $(bin)WoCommands.obj;*

$(bin)libWo.olb(WoTypes) : $(src)WoTypes.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)WoTypes.obj $(src)WoTypes.c
	$(inlib) $(bin)libWo.olb $(bin)WoTypes.obj
	del $(bin)WoTypes.obj;*

$(bin)libWo.olb(Wo) : $(src)Wo.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)Wo.obj $(src)Wo.c
	$(inlib) $(bin)libWo.olb $(bin)Wo.obj
	del $(bin)Wo.obj;*

$(bin)libWo.olb(OInterface) : $(src)OInterface.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)OInterface.obj $(src)OInterface.c
	$(inlib) $(bin)libWo.olb $(bin)OInterface.obj
	del $(bin)OInterface.obj;*

$(bin)libWo.olb(OPiece) : $(src)OPiece.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)OPiece.obj $(src)OPiece.c
	$(inlib) $(bin)libWo.olb $(bin)OPiece.obj
	del $(bin)OPiece.obj;*

$(bin)libWo.olb(OWidget) : $(src)OWidget.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)OWidget.obj $(src)OWidget.c
	$(inlib) $(bin)libWo.olb $(bin)OWidget.obj
	del $(bin)OWidget.obj;*

$(bin)libWo.olb(OClass) : $(src)OClass.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)OClass.obj $(src)OClass.c
	$(inlib) $(bin)libWo.olb $(bin)OClass.obj
	del $(bin)OClass.obj;*

$(bin)libWo.olb(OPart) : $(src)OPart.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)OPart.obj $(src)OPart.c
	$(inlib) $(bin)libWo.olb $(bin)OPart.obj
	del $(bin)OPart.obj;*

$(bin)libWo.olb(OInterpreter) : $(src)OInterpreter.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)OInterpreter.obj $(src)OInterpreter.c
	$(inlib) $(bin)libWo.olb $(bin)OInterpreter.obj
	del $(bin)OInterpreter.obj;*

$(bin)libWo.olb(OCyclic) : $(src)OCyclic.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)OCyclic.obj $(src)OCyclic.c
	$(inlib) $(bin)libWo.olb $(bin)OCyclic.obj
	del $(bin)OCyclic.obj;*

$(bin)libWo.olb(XtwTools) : $(src)XtwTools.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)XtwTools.obj $(src)XtwTools.c
	$(inlib) $(bin)libWo.olb $(bin)XtwTools.obj
	del $(bin)XtwTools.obj;*

$(bin)libWo.olb(OResource) : $(src)OResource.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)OResource.obj $(src)OResource.c
	$(inlib) $(bin)libWo.olb $(bin)OResource.obj
	del $(bin)OResource.obj;*

$(bin)libWo.olb(OEvent) : $(src)OEvent.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)OEvent.obj $(src)OEvent.c
	$(inlib) $(bin)libWo.olb $(bin)OEvent.obj
	del $(bin)OEvent.obj;*

$(bin)libWo.olb(OTrace) : $(src)OTrace.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)OTrace.obj $(src)OTrace.c
	$(inlib) $(bin)libWo.olb $(bin)OTrace.obj
	del $(bin)OTrace.obj;*

$(bin)libWo.olb(WoBinds) : $(src)WoBinds.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)WoBinds.obj $(src)WoBinds.c
	$(inlib) $(bin)libWo.olb $(bin)WoBinds.obj
	del $(bin)WoBinds.obj;*

$(bin)libWo.olb(X11Binds) : $(src)X11Binds.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)X11Binds.obj $(src)X11Binds.c
	$(inlib) $(bin)libWo.olb $(bin)X11Binds.obj
	del $(bin)X11Binds.obj;*

!.............................................
! libWoXm dependencies 
!.............................................
$(bin)libWoXm.olb(XmTools) : $(src)XmTools.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)XmTools.obj $(src)XmTools.c
	$(inlib) $(bin)libWoXm.olb $(bin)XmTools.obj
	del $(bin)XmTools.obj;*

$(bin)libWoXm.olb(XmCommands) : $(src)XmCommands.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)XmCommands.obj $(src)XmCommands.c
	$(inlib) $(bin)libWoXm.olb $(bin)XmCommands.obj
	del $(bin)XmCommands.obj;*

$(bin)libWoXm.olb(XmBinds) : $(src)XmBinds.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)XmBinds.obj $(src)XmBinds.c
	$(inlib) $(bin)libWoXm.olb $(bin)XmBinds.obj
	del $(bin)XmBinds.obj;*

!.............................................
! libWoGL dependencies 
!.............................................
$(bin)libWoGL.olb(GL_Binds) : $(src)GL_Binds.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)GL_Binds.obj $(src)GL_Binds.c
	$(inlib) $(bin)libWoGL.olb $(bin)GL_Binds.obj
	del $(bin)GL_Binds.obj;*

$(bin)woinit.obj : $(app)woinit.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)woinit.obj $(app)woinit.c


