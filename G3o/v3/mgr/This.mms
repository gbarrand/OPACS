!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!  File produced by the Co/omake tool
! using file Make.odb.
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!  The soft linked packages has been taken
! from the property 'has' of the 
! 'Make' object of file Make.odb.
! It had the value :
!    GL
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
HAS_CCFLAGS = \
$(G3o_CCFLAGS)\
$(GL_CCFLAGS)
HAS_CXXFLAGS = \
$(G3o_CXXFLAGS)\
$(GL_CXXFLAGS)
HAS_CPPINCLUDES = \
 $(G3o_CPPINCLUDES)\
,$(GL_CPPINCLUDES)
HAS_CPPDEFINES = \
 $(G3o_CPPDEFINES)\
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

libG3oEXPO_target = $(bin)libG3oEXPO.olb
libG3o_target = $(bin)libG3o.olb
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
all :: \
$(libG3oEXPO_target) \
$(libG3o_target) \
$(bin)G3oT.exe 
	@ write sys$output "G3o : all ok." 

libs :: \
$(libG3oEXPO_target) \
$(libG3o_target) 
	@ write sys$output "G3o : libs ok." 

apps :: \
$(bin)G3oT.exe \
$(bin)geantT.exe \
$(bin)NoHT.exe 
	@ write sys$output "G3o : apps ok." 
!.............................................
rmexeo :
	if f$search("$(bin)G3oT.obj;*") .nes. "" then del $(bin)G3oT.obj;*
	if f$search("$(bin)geantT.obj;*") .nes. "" then del $(bin)geantT.obj;*
	if f$search("$(bin)NoHT.obj;*") .nes. "" then del $(bin)NoHT.obj;*
!.............................................
G3oT : $(bin)G3oT.exe
	@ write sys$output "G3o : G3oT ok."
geantT : $(bin)geantT.exe
	@ write sys$output "G3o : geantT ok."
NoHT : $(bin)NoHT.exe
	@ write sys$output "G3o : NoHT ok."
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!.............................................
$(bin)G3oT.exe : $(bin)G3oT.obj \
$(bin)idummy.obj \
$(libG3oEXPO_target) \
$(libG3o_target) 
	$(CCLD)$(CCLDFLAGS)/exe=$(bin)G3oT.exe $(bin)G3oT.obj\
,$(bin)idummy.obj\
,$(libG3oEXPO)\
,$(libG3o)\
,$(libgeant)\
,$(libpacklib)\
,$(libkernlib)\
,$(libf77)\
,$(libGo)\
,$(libGLU)\
,$(libGL)\
,$(libCo)\
,$(libm)\
$(CCLDEND)

$(bin)G3oT.obj   : $(app)G3oT.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES),$(APP_CPPINCLUDES))/define=($(APP_CCDEFINES),$(HAS_CPPDEFINES))/obj=$(bin)G3oT.obj $(app)G3oT.c 
!.............................................
$(bin)geantT.exe : $(bin)geantT.obj \
$(libG3oEXPO_target) 
	$(F77LD)$(F77LDFLAGS)/exe=$(bin)geantT.exe $(bin)geantT.obj\
,$(libG3oEXPO)\
,$(libgeant)\
,$(libpacklib)\
,$(libkernlib)\
$(F77LDEND)

$(bin)geantT.obj   : $(app)geantT.f
	$(F77)$(F77FLAGS)/obj=$(bin)geantT.obj $(app)geantT.f
!.............................................
$(bin)NoHT.exe : $(bin)NoHT.obj 
	$(CCLD)$(CCLDFLAGS)/exe=$(bin)NoHT.exe $(bin)NoHT.obj\
$(CCLDEND)

$(bin)NoHT.obj   : $(app)NoHT.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES),$(APP_CPPINCLUDES))/define=($(APP_CCDEFINES),$(HAS_CPPDEFINES))/obj=$(bin)NoHT.obj $(app)NoHT.c 

$(bin)libG3oEXPO.olb : \
$(bin)libG3oEXPO.olb(G3oEXPO) \
$(bin)libG3oEXPO.olb(G3oEXPO_F77) 
	if f$search("$(bin)libG3oEXPO.opt;*") .nes. "" then delete $(bin)libG3oEXPO.opt;*
	open/write file $(bin)libG3oEXPO.opt
	write file "G3OROOT:[$(CONFIG)]libG3oEXPO/lib"
	close file

$(bin)libG3o.olb : \
$(bin)libG3o.olb(G3oCommands) \
$(bin)libG3o.olb(G3oTypes) \
$(bin)libG3o.olb(GEANT) \
$(bin)libG3o.olb(ogeant) \
$(bin)libG3o.olb(G3oBinds) 
	if f$search("$(bin)libG3o.opt;*") .nes. "" then delete $(bin)libG3o.opt;*
	open/write file $(bin)libG3o.opt
	write file "G3OROOT:[$(CONFIG)]libG3o/lib"
	close file
!.............................................
! libG3oEXPO dependencies 
!.............................................
$(bin)libG3oEXPO.olb(G3oEXPO) : $(src)G3oEXPO.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)G3oEXPO.obj $(src)G3oEXPO.c
	$(inlib) $(bin)libG3oEXPO.olb $(bin)G3oEXPO.obj
	del $(bin)G3oEXPO.obj;*

$(bin)libG3oEXPO.olb(G3oEXPO_F77) : $(src)G3oEXPO_F77.f
	$(F77)$(F77FLAGS)/obj=$(bin)G3oEXPO_F77.obj $(src)G3oEXPO_F77.f
	$(inlib) $(bin)libG3oEXPO.olb $(bin)G3oEXPO_F77.obj
	del $(bin)G3oEXPO_F77.obj;*

!.............................................
! libG3o dependencies 
!.............................................
$(bin)libG3o.olb(G3oCommands) : $(src)G3oCommands.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)G3oCommands.obj $(src)G3oCommands.c
	$(inlib) $(bin)libG3o.olb $(bin)G3oCommands.obj
	del $(bin)G3oCommands.obj;*

$(bin)libG3o.olb(G3oTypes) : $(src)G3oTypes.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)G3oTypes.obj $(src)G3oTypes.c
	$(inlib) $(bin)libG3o.olb $(bin)G3oTypes.obj
	del $(bin)G3oTypes.obj;*

$(bin)libG3o.olb(GEANT) : $(src)GEANT.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)GEANT.obj $(src)GEANT.c
	$(inlib) $(bin)libG3o.olb $(bin)GEANT.obj
	del $(bin)GEANT.obj;*

$(bin)libG3o.olb(ogeant) : $(src)ogeant.f
	$(F77)$(F77FLAGS)/obj=$(bin)ogeant.obj $(src)ogeant.f
	$(inlib) $(bin)libG3o.olb $(bin)ogeant.obj
	del $(bin)ogeant.obj;*

$(bin)libG3o.olb(G3oBinds) : $(src)G3oBinds.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)G3oBinds.obj $(src)G3oBinds.c
	$(inlib) $(bin)libG3o.olb $(bin)G3oBinds.obj
	del $(bin)G3oBinds.obj;*

$(bin)idummy.obj : $(app)idummy.f
	$(F77)$(F77FLAGS)/obj=$(bin)idummy.obj $(app)idummy.f


