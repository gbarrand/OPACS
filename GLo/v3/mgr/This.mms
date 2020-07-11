!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!  File produced by the Co/omake tool
! using file Make.odb.
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!  The soft linked packages has been taken
! from the property 'has' of the 
! 'Make' object of file Make.odb.
! It had the value :
!    X11
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
HAS_CCFLAGS = \
$(GLo_CCFLAGS)\
$(X11_CCFLAGS)
HAS_CXXFLAGS = \
$(GLo_CXXFLAGS)\
$(X11_CXXFLAGS)
HAS_CPPINCLUDES = \
 $(GLo_CPPINCLUDES)\
,$(X11_CPPINCLUDES)
HAS_CPPDEFINES = \
 $(GLo_CPPDEFINES)\
,$(X11_CPPDEFINES)
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

libGLo_target = $(bin)libGLo.$(olb_or_exe)
libGLUo_target = $(bin)libGLUo.$(olb_or_exe)
libGLXo_target = $(bin)libGLXo.$(olb_or_exe)
libGLPSo_target = $(bin)libGLPSo.$(olb_or_exe)
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
all :: \
$(libGLo_target) \
$(libGLUo_target) \
$(libGLXo_target) \
$(libGLPSo_target) \
$(bin)glXT.exe \
$(bin)glPST.exe 
	@ write sys$output "GLo : all ok." 

libs :: \
$(libGLo_target) \
$(libGLUo_target) \
$(libGLXo_target) \
$(libGLPSo_target) 
	@ write sys$output "GLo : libs ok." 

apps :: \
$(bin)glXT.exe \
$(bin)glPST.exe \
$(bin)NoHT.exe 
	@ write sys$output "GLo : apps ok." 
!.............................................
rmexeo :
	if f$search("$(bin)glXT.obj;*") .nes. "" then del $(bin)glXT.obj;*
	if f$search("$(bin)glPST.obj;*") .nes. "" then del $(bin)glPST.obj;*
	if f$search("$(bin)NoHT.obj;*") .nes. "" then del $(bin)NoHT.obj;*
!.............................................
glXT : $(bin)glXT.exe
	@ write sys$output "GLo : glXT ok."
glPST : $(bin)glPST.exe
	@ write sys$output "GLo : glPST ok."
NoHT : $(bin)NoHT.exe
	@ write sys$output "GLo : NoHT ok."
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!.............................................
$(bin)glXT.exe : $(bin)glXT.obj \
$(libGLXo_target) \
$(libGLUo_target) \
$(libGLo_target) 
	$(CCLD)$(CCLDFLAGS)/exe=$(bin)glXT.exe $(bin)glXT.obj\
,$(libGLXo)\
,$(libGLUo)\
,$(libGLo)\
,$(libCo)\
,$(libX11)\
,$(libm)\
$(CCLDEND)

$(bin)glXT.obj   : $(app)glXT.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES),$(APP_CPPINCLUDES))/define=($(APP_CCDEFINES),$(HAS_CPPDEFINES))/obj=$(bin)glXT.obj $(app)glXT.c 
!.............................................
$(bin)glPST.exe : $(bin)glPST.obj \
$(libGLPSo_target) \
$(libGLUo_target) \
$(libGLo_target) 
	$(CCLD)$(CCLDFLAGS)/exe=$(bin)glPST.exe $(bin)glPST.obj\
,$(libGLPSo)\
,$(libGLUo)\
,$(libGLo)\
,$(libCo)\
,$(libm)\
$(CCLDEND)

$(bin)glPST.obj   : $(app)glPST.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES),$(APP_CPPINCLUDES))/define=($(APP_CCDEFINES),$(HAS_CPPDEFINES))/obj=$(bin)glPST.obj $(app)glPST.c 
!.............................................
$(bin)NoHT.exe : $(bin)NoHT.obj 
	$(CCLD)$(CCLDFLAGS)/exe=$(bin)NoHT.exe $(bin)NoHT.obj\
$(CCLDEND)

$(bin)NoHT.obj   : $(app)NoHT.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES),$(APP_CPPINCLUDES))/define=($(APP_CCDEFINES),$(HAS_CPPDEFINES))/obj=$(bin)NoHT.obj $(app)NoHT.c 
$(bin)libGLo.exe : $(bin)libGLo.olb 
	$(make_so) libGLo GLo $(CONFIG)
	if f$search("$(bin)libGLo.opt;*") .nes. "" then delete $(bin)libGLo.opt;*
	open/write file $(bin)libGLo.opt
	write file "GLOROOT:[$(CONFIG)]libGLo/share"
	close file

$(bin)libGLo.olb : \
$(bin)libGLo.olb(gl) \
$(bin)libGLo.olb(glList) \
$(bin)libGLo.olb(glDriverZ) \
$(bin)libGLo.olb(ZBuffer) \
$(bin)libGLo.olb(ZPolygon) 
	if f$search("$(bin)libGLo.opt;*") .nes. "" then delete $(bin)libGLo.opt;*
	open/write file $(bin)libGLo.opt
	write file "GLOROOT:[$(CONFIG)]libGLo/lib"
	close file
$(bin)libGLUo.exe : $(bin)libGLUo.olb \
$(libGLo_target) 
	$(make_so) libGLUo GLo $(CONFIG)
	if f$search("$(bin)libGLUo.opt;*") .nes. "" then delete $(bin)libGLUo.opt;*
	open/write file $(bin)libGLUo.opt
	write file "GLOROOT:[$(CONFIG)]libGLUo/share"
	close file

$(bin)libGLUo.olb : \
$(bin)libGLUo.olb(glu) \
$(libGLo_target) 
	if f$search("$(bin)libGLUo.opt;*") .nes. "" then delete $(bin)libGLUo.opt;*
	open/write file $(bin)libGLUo.opt
	write file "GLOROOT:[$(CONFIG)]libGLUo/lib"
	close file
$(bin)libGLXo.exe : $(bin)libGLXo.olb \
$(libGLo_target) 
	$(make_so) libGLXo GLo $(CONFIG)
	if f$search("$(bin)libGLXo.opt;*") .nes. "" then delete $(bin)libGLXo.opt;*
	open/write file $(bin)libGLXo.opt
	write file "GLOROOT:[$(CONFIG)]libGLXo/share"
	close file

$(bin)libGLXo.olb : \
$(bin)libGLXo.olb(glx) \
$(bin)libGLXo.olb(glDriverX) \
$(libGLo_target) 
	if f$search("$(bin)libGLXo.opt;*") .nes. "" then delete $(bin)libGLXo.opt;*
	open/write file $(bin)libGLXo.opt
	write file "GLOROOT:[$(CONFIG)]libGLXo/lib"
	close file
$(bin)libGLPSo.exe : $(bin)libGLPSo.olb \
$(libGLo_target) 
	$(make_so) libGLPSo GLo $(CONFIG)
	if f$search("$(bin)libGLPSo.opt;*") .nes. "" then delete $(bin)libGLPSo.opt;*
	open/write file $(bin)libGLPSo.opt
	write file "GLOROOT:[$(CONFIG)]libGLPSo/share"
	close file

$(bin)libGLPSo.olb : \
$(bin)libGLPSo.olb(glPS) \
$(bin)libGLPSo.olb(glDriverPS) \
$(libGLo_target) 
	if f$search("$(bin)libGLPSo.opt;*") .nes. "" then delete $(bin)libGLPSo.opt;*
	open/write file $(bin)libGLPSo.opt
	write file "GLOROOT:[$(CONFIG)]libGLPSo/lib"
	close file
!.............................................
! libGLo dependencies 
!.............................................
$(bin)libGLo.olb(gl) : $(src)gl.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)gl.obj $(src)gl.c
	$(inlib) $(bin)libGLo.olb $(bin)gl.obj
	del $(bin)gl.obj;*

$(bin)libGLo.olb(glList) : $(src)glList.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)glList.obj $(src)glList.c
	$(inlib) $(bin)libGLo.olb $(bin)glList.obj
	del $(bin)glList.obj;*

$(bin)libGLo.olb(glDriverZ) : $(src)glDriverZ.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)glDriverZ.obj $(src)glDriverZ.c
	$(inlib) $(bin)libGLo.olb $(bin)glDriverZ.obj
	del $(bin)glDriverZ.obj;*

$(bin)libGLo.olb(ZBuffer) : $(src)ZBuffer.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)ZBuffer.obj $(src)ZBuffer.c
	$(inlib) $(bin)libGLo.olb $(bin)ZBuffer.obj
	del $(bin)ZBuffer.obj;*

$(bin)libGLo.olb(ZPolygon) : $(src)ZPolygon.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)ZPolygon.obj $(src)ZPolygon.c
	$(inlib) $(bin)libGLo.olb $(bin)ZPolygon.obj
	del $(bin)ZPolygon.obj;*

!.............................................
! libGLUo dependencies 
!.............................................
$(bin)libGLUo.olb(glu) : $(src)glu.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)glu.obj $(src)glu.c
	$(inlib) $(bin)libGLUo.olb $(bin)glu.obj
	del $(bin)glu.obj;*

!.............................................
! libGLXo dependencies 
!.............................................
$(bin)libGLXo.olb(glx) : $(src)glx.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)glx.obj $(src)glx.c
	$(inlib) $(bin)libGLXo.olb $(bin)glx.obj
	del $(bin)glx.obj;*

$(bin)libGLXo.olb(glDriverX) : $(src)glDriverX.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)glDriverX.obj $(src)glDriverX.c
	$(inlib) $(bin)libGLXo.olb $(bin)glDriverX.obj
	del $(bin)glDriverX.obj;*

!.............................................
! libGLPSo dependencies 
!.............................................
$(bin)libGLPSo.olb(glPS) : $(src)glPS.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)glPS.obj $(src)glPS.c
	$(inlib) $(bin)libGLPSo.olb $(bin)glPS.obj
	del $(bin)glPS.obj;*

$(bin)libGLPSo.olb(glDriverPS) : $(src)glDriverPS.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)glDriverPS.obj $(src)glDriverPS.c
	$(inlib) $(bin)libGLPSo.olb $(bin)glDriverPS.obj
	del $(bin)glDriverPS.obj;*


