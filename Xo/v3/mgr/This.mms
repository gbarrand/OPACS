!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!  File produced by the Co/omake tool
! using file Make.odb.
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!  The soft linked packages has been taken
! from the property 'has' of the 
! 'Make' object of file Make.odb.
! It had the value :
!    X11 Xext Xt Xmu GL HTML Xm XoXm
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
HAS_CCFLAGS = \
$(Xo_CCFLAGS)\
$(X11_CCFLAGS)\
$(Xext_CCFLAGS)\
$(Xt_CCFLAGS)\
$(Xmu_CCFLAGS)\
$(GL_CCFLAGS)\
$(HTML_CCFLAGS)\
$(Xm_CCFLAGS)\
$(XoXm_CCFLAGS)
HAS_CXXFLAGS = \
$(Xo_CXXFLAGS)\
$(X11_CXXFLAGS)\
$(Xext_CXXFLAGS)\
$(Xt_CXXFLAGS)\
$(Xmu_CXXFLAGS)\
$(GL_CXXFLAGS)\
$(HTML_CXXFLAGS)\
$(Xm_CXXFLAGS)\
$(XoXm_CXXFLAGS)
HAS_CPPINCLUDES = \
 $(Xo_CPPINCLUDES)\
,$(X11_CPPINCLUDES)\
,$(Xext_CPPINCLUDES)\
,$(Xt_CPPINCLUDES)\
,$(Xmu_CPPINCLUDES)\
,$(GL_CPPINCLUDES)\
,$(HTML_CPPINCLUDES)\
,$(Xm_CPPINCLUDES)\
,$(XoXm_CPPINCLUDES)
HAS_CPPDEFINES = \
 $(Xo_CPPDEFINES)\
,$(X11_CPPDEFINES)\
,$(Xext_CPPDEFINES)\
,$(Xt_CPPDEFINES)\
,$(Xmu_CPPDEFINES)\
,$(GL_CPPDEFINES)\
,$(HTML_CPPDEFINES)\
,$(Xm_CPPDEFINES)\
,$(XoXm_CPPDEFINES)
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

libXo_target = $(bin)libXo.$(olb_or_exe)
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
all :: \
$(libXo_target) \
$(bin)XoCameraT.exe \
$(bin)XoPlotterT.exe \
$(bin)OPlotterT.exe \
$(bin)XoMosaicT.exe \
$(bin)XoGraphT.exe \
$(bin)XoLabelT.exe \
$(bin)XoEyesT.exe \
$(bin)XoImageT.exe \
$(bin)XoColormapT.exe \
$(bin)XoRheostatT.exe \
$(bin)XoHelpT.exe 
	@ write sys$output "Xo : all ok." 

libs :: \
$(libXo_target) 
	@ write sys$output "Xo : libs ok." 

apps :: \
$(bin)XoCameraT.exe \
$(bin)XoPlotterT.exe \
$(bin)OPlotterT.exe \
$(bin)XoMosaicT.exe \
$(bin)XoGraphT.exe \
$(bin)XoLabelT.exe \
$(bin)XoEyesT.exe \
$(bin)XoImageT.exe \
$(bin)XoColormapT.exe \
$(bin)XoRheostatT.exe \
$(bin)XoHelpT.exe \
$(bin)XoShapeT.exe \
$(bin)XawT.exe \
$(bin)XmPushBT.exe \
$(bin)NoHT.exe 
	@ write sys$output "Xo : apps ok." 
!.............................................
rmexeo :
	if f$search("$(bin)XoCameraT.obj;*") .nes. "" then del $(bin)XoCameraT.obj;*
	if f$search("$(bin)XoPlotterT.obj;*") .nes. "" then del $(bin)XoPlotterT.obj;*
	if f$search("$(bin)OPlotterT.obj;*") .nes. "" then del $(bin)OPlotterT.obj;*
	if f$search("$(bin)XoMosaicT.obj;*") .nes. "" then del $(bin)XoMosaicT.obj;*
	if f$search("$(bin)XoGraphT.obj;*") .nes. "" then del $(bin)XoGraphT.obj;*
	if f$search("$(bin)XoLabelT.obj;*") .nes. "" then del $(bin)XoLabelT.obj;*
	if f$search("$(bin)XoEyesT.obj;*") .nes. "" then del $(bin)XoEyesT.obj;*
	if f$search("$(bin)XoImageT.obj;*") .nes. "" then del $(bin)XoImageT.obj;*
	if f$search("$(bin)XoColormapT.obj;*") .nes. "" then del $(bin)XoColormapT.obj;*
	if f$search("$(bin)XoRheostatT.obj;*") .nes. "" then del $(bin)XoRheostatT.obj;*
	if f$search("$(bin)XoHelpT.obj;*") .nes. "" then del $(bin)XoHelpT.obj;*
	if f$search("$(bin)XoShapeT.obj;*") .nes. "" then del $(bin)XoShapeT.obj;*
	if f$search("$(bin)XawT.obj;*") .nes. "" then del $(bin)XawT.obj;*
	if f$search("$(bin)XmPushBT.obj;*") .nes. "" then del $(bin)XmPushBT.obj;*
	if f$search("$(bin)NoHT.obj;*") .nes. "" then del $(bin)NoHT.obj;*
!.............................................
XoCameraT : $(bin)XoCameraT.exe
	@ write sys$output "Xo : XoCameraT ok."
XoPlotterT : $(bin)XoPlotterT.exe
	@ write sys$output "Xo : XoPlotterT ok."
OPlotterT : $(bin)OPlotterT.exe
	@ write sys$output "Xo : OPlotterT ok."
XoMosaicT : $(bin)XoMosaicT.exe
	@ write sys$output "Xo : XoMosaicT ok."
XoGraphT : $(bin)XoGraphT.exe
	@ write sys$output "Xo : XoGraphT ok."
XoLabelT : $(bin)XoLabelT.exe
	@ write sys$output "Xo : XoLabelT ok."
XoEyesT : $(bin)XoEyesT.exe
	@ write sys$output "Xo : XoEyesT ok."
XoImageT : $(bin)XoImageT.exe
	@ write sys$output "Xo : XoImageT ok."
XoColormapT : $(bin)XoColormapT.exe
	@ write sys$output "Xo : XoColormapT ok."
XoRheostatT : $(bin)XoRheostatT.exe
	@ write sys$output "Xo : XoRheostatT ok."
XoHelpT : $(bin)XoHelpT.exe
	@ write sys$output "Xo : XoHelpT ok."
XoShapeT : $(bin)XoShapeT.exe
	@ write sys$output "Xo : XoShapeT ok."
XawT : $(bin)XawT.exe
	@ write sys$output "Xo : XawT ok."
XmPushBT : $(bin)XmPushBT.exe
	@ write sys$output "Xo : XmPushBT ok."
NoHT : $(bin)NoHT.exe
	@ write sys$output "Xo : NoHT ok."
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!.............................................
$(bin)XoCameraT.exe : $(bin)XoCameraT.obj \
$(libXo_target) 
	$(CCLD)$(CCLDFLAGS)/exe=$(bin)XoCameraT.exe $(bin)XoCameraT.obj\
,$(libXo)\
,$(libwww)\
,$(libhtmlw)\
,$(libXm)\
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
,$(libm)\
$(CCLDEND)

$(bin)XoCameraT.obj   : $(app)XoCameraT.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES),$(APP_CPPINCLUDES))/define=($(APP_CCDEFINES),$(HAS_CPPDEFINES))/obj=$(bin)XoCameraT.obj $(app)XoCameraT.c 
!.............................................
$(bin)XoPlotterT.exe : $(bin)XoPlotterT.obj \
$(libXo_target) 
	$(CCLD)$(CCLDFLAGS)/exe=$(bin)XoPlotterT.exe $(bin)XoPlotterT.obj\
,$(libXo)\
,$(libwww)\
,$(libhtmlw)\
,$(libXm)\
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
,$(libm)\
$(CCLDEND)

$(bin)XoPlotterT.obj   : $(app)XoPlotterT.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES),$(APP_CPPINCLUDES))/define=($(APP_CCDEFINES),$(HAS_CPPDEFINES))/obj=$(bin)XoPlotterT.obj $(app)XoPlotterT.c 
!.............................................
$(bin)OPlotterT.exe : $(bin)OPlotterT.obj \
$(libXo_target) 
	$(CCLD)$(CCLDFLAGS)/exe=$(bin)OPlotterT.exe $(bin)OPlotterT.obj\
,$(libXo)\
,$(libwww)\
,$(libhtmlw)\
,$(libXm)\
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
,$(libm)\
$(CCLDEND)

$(bin)OPlotterT.obj   : $(app)OPlotterT.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES),$(APP_CPPINCLUDES))/define=($(APP_CCDEFINES),$(HAS_CPPDEFINES))/obj=$(bin)OPlotterT.obj $(app)OPlotterT.c 
!.............................................
$(bin)XoMosaicT.exe : $(bin)XoMosaicT.obj \
$(libXo_target) 
	$(CCLD)$(CCLDFLAGS)/exe=$(bin)XoMosaicT.exe $(bin)XoMosaicT.obj\
,$(libXo)\
,$(libwww)\
,$(libhtmlw)\
,$(libXm)\
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
,$(libm)\
$(CCLDEND)

$(bin)XoMosaicT.obj   : $(app)XoMosaicT.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES),$(APP_CPPINCLUDES))/define=($(APP_CCDEFINES),$(HAS_CPPDEFINES))/obj=$(bin)XoMosaicT.obj $(app)XoMosaicT.c 
!.............................................
$(bin)XoGraphT.exe : $(bin)XoGraphT.obj \
$(libXo_target) 
	$(CCLD)$(CCLDFLAGS)/exe=$(bin)XoGraphT.exe $(bin)XoGraphT.obj\
,$(libXo)\
,$(libwww)\
,$(libhtmlw)\
,$(libXm)\
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
,$(libm)\
$(CCLDEND)

$(bin)XoGraphT.obj   : $(app)XoGraphT.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES),$(APP_CPPINCLUDES))/define=($(APP_CCDEFINES),$(HAS_CPPDEFINES))/obj=$(bin)XoGraphT.obj $(app)XoGraphT.c 
!.............................................
$(bin)XoLabelT.exe : $(bin)XoLabelT.obj \
$(libXo_target) 
	$(CCLD)$(CCLDFLAGS)/exe=$(bin)XoLabelT.exe $(bin)XoLabelT.obj\
,$(libXo)\
,$(libwww)\
,$(libhtmlw)\
,$(libXm)\
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
,$(libm)\
$(CCLDEND)

$(bin)XoLabelT.obj   : $(app)XoLabelT.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES),$(APP_CPPINCLUDES))/define=($(APP_CCDEFINES),$(HAS_CPPDEFINES))/obj=$(bin)XoLabelT.obj $(app)XoLabelT.c 
!.............................................
$(bin)XoEyesT.exe : $(bin)XoEyesT.obj \
$(libXo_target) 
	$(CCLD)$(CCLDFLAGS)/exe=$(bin)XoEyesT.exe $(bin)XoEyesT.obj\
,$(libXo)\
,$(libwww)\
,$(libhtmlw)\
,$(libXm)\
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
,$(libm)\
$(CCLDEND)

$(bin)XoEyesT.obj   : $(app)XoEyesT.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES),$(APP_CPPINCLUDES))/define=($(APP_CCDEFINES),$(HAS_CPPDEFINES))/obj=$(bin)XoEyesT.obj $(app)XoEyesT.c 
!.............................................
$(bin)XoImageT.exe : $(bin)XoImageT.obj \
$(libXo_target) 
	$(CCLD)$(CCLDFLAGS)/exe=$(bin)XoImageT.exe $(bin)XoImageT.obj\
,$(libXo)\
,$(libwww)\
,$(libhtmlw)\
,$(libXm)\
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
,$(libm)\
$(CCLDEND)

$(bin)XoImageT.obj   : $(app)XoImageT.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES),$(APP_CPPINCLUDES))/define=($(APP_CCDEFINES),$(HAS_CPPDEFINES))/obj=$(bin)XoImageT.obj $(app)XoImageT.c 
!.............................................
$(bin)XoColormapT.exe : $(bin)XoColormapT.obj \
$(libXo_target) 
	$(CCLD)$(CCLDFLAGS)/exe=$(bin)XoColormapT.exe $(bin)XoColormapT.obj\
,$(libXo)\
,$(libwww)\
,$(libhtmlw)\
,$(libXm)\
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
,$(libm)\
$(CCLDEND)

$(bin)XoColormapT.obj   : $(app)XoColormapT.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES),$(APP_CPPINCLUDES))/define=($(APP_CCDEFINES),$(HAS_CPPDEFINES))/obj=$(bin)XoColormapT.obj $(app)XoColormapT.c 
!.............................................
$(bin)XoRheostatT.exe : $(bin)XoRheostatT.obj \
$(libXo_target) 
	$(CCLD)$(CCLDFLAGS)/exe=$(bin)XoRheostatT.exe $(bin)XoRheostatT.obj\
,$(libXo)\
,$(libwww)\
,$(libhtmlw)\
,$(libXm)\
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
,$(libm)\
$(CCLDEND)

$(bin)XoRheostatT.obj   : $(app)XoRheostatT.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES),$(APP_CPPINCLUDES))/define=($(APP_CCDEFINES),$(HAS_CPPDEFINES))/obj=$(bin)XoRheostatT.obj $(app)XoRheostatT.c 
!.............................................
$(bin)XoHelpT.exe : $(bin)XoHelpT.obj \
$(libXo_target) 
	$(CCLD)$(CCLDFLAGS)/exe=$(bin)XoHelpT.exe $(bin)XoHelpT.obj\
,$(libXo)\
,$(libwww)\
,$(libhtmlw)\
,$(libXm)\
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
,$(libm)\
$(CCLDEND)

$(bin)XoHelpT.obj   : $(app)XoHelpT.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES),$(APP_CPPINCLUDES))/define=($(APP_CCDEFINES),$(HAS_CPPDEFINES))/obj=$(bin)XoHelpT.obj $(app)XoHelpT.c 
!.............................................
$(bin)XoShapeT.exe : $(bin)XoShapeT.obj \
$(libXo_target) 
	$(CCLD)$(CCLDFLAGS)/exe=$(bin)XoShapeT.exe $(bin)XoShapeT.obj\
,$(libXo)\
,$(libwww)\
,$(libhtmlw)\
,$(libXm)\
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
,$(libm)\
$(CCLDEND)

$(bin)XoShapeT.obj   : $(app)XoShapeT.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES),$(APP_CPPINCLUDES))/define=($(APP_CCDEFINES),$(HAS_CPPDEFINES))/obj=$(bin)XoShapeT.obj $(app)XoShapeT.c 
!.............................................
$(bin)XawT.exe : $(bin)XawT.obj 
	$(CCLD)$(CCLDFLAGS)/exe=$(bin)XawT.exe $(bin)XawT.obj\
,$(libXmu)\
,$(libXt)\
,$(libXext)\
,$(libX11)\
,$(libm)\
$(CCLDEND)

$(bin)XawT.obj   : $(app)XawT.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES),$(APP_CPPINCLUDES))/define=($(APP_CCDEFINES),$(HAS_CPPDEFINES))/obj=$(bin)XawT.obj $(app)XawT.c 
!.............................................
$(bin)XmPushBT.exe : $(bin)XmPushBT.obj 
	$(CCLD)$(CCLDFLAGS)/exe=$(bin)XmPushBT.exe $(bin)XmPushBT.obj\
,$(libXm)\
,$(libXt)\
,$(libX11)\
$(CCLDEND)

$(bin)XmPushBT.obj   : $(app)XmPushBT.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES),$(APP_CPPINCLUDES))/define=($(APP_CCDEFINES),$(HAS_CPPDEFINES))/obj=$(bin)XmPushBT.obj $(app)XmPushBT.c 
!.............................................
$(bin)NoHT.exe : $(bin)NoHT.obj 
	$(CCLD)$(CCLDFLAGS)/exe=$(bin)NoHT.exe $(bin)NoHT.obj\
$(CCLDEND)

$(bin)NoHT.obj   : $(app)NoHT.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES),$(APP_CPPINCLUDES))/define=($(APP_CCDEFINES),$(HAS_CPPDEFINES))/obj=$(bin)NoHT.obj $(app)NoHT.c 
$(bin)libXo.exe : $(bin)libXo.olb 
	$(make_so) libXo Xo $(CONFIG)
	if f$search("$(bin)libXo.opt;*") .nes. "" then delete $(bin)libXo.opt;*
	open/write file $(bin)libXo.opt
	write file "XOROOT:[$(CONFIG)]libXo/share"
	close file

$(bin)libXo.olb : \
$(bin)libXo.olb(OImage) \
$(bin)libXo.olb(OPlotter) \
$(bin)libXo.olb(mowww) \
$(bin)libXo.olb(img) \
$(bin)libXo.olb(picread) \
$(bin)libXo.olb(xpmread) \
$(bin)libXo.olb(XoCommands) \
$(bin)libXo.olb(XoTypes) \
$(bin)libXo.olb(OColormapX) \
$(bin)libXo.olb(XoF77) \
$(bin)libXo.olb(XoShape) \
$(bin)libXo.olb(XoGraph) \
$(bin)libXo.olb(XoEyes) \
$(bin)libXo.olb(XoLabel) \
$(bin)libXo.olb(XoColormap) \
$(bin)libXo.olb(XoBinds) \
$(bin)libXo.olb(Xo) \
$(bin)libXo.olb(XoRheostat) \
$(bin)libXo.olb(XoMosaic) \
$(bin)libXo.olb(XoHelp) \
$(bin)libXo.olb(XoPlotter) \
$(bin)libXo.olb(XoCamera) \
$(bin)libXo.olb(XoImage) 
	if f$search("$(bin)libXo.opt;*") .nes. "" then delete $(bin)libXo.opt;*
	open/write file $(bin)libXo.opt
	write file "XOROOT:[$(CONFIG)]libXo/lib"
	close file
!.............................................
! libXo dependencies 
!.............................................
$(bin)libXo.olb(OImage) : $(src)OImage.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)OImage.obj $(src)OImage.c
	$(inlib) $(bin)libXo.olb $(bin)OImage.obj
	del $(bin)OImage.obj;*

$(bin)libXo.olb(OPlotter) : $(src)OPlotter.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)OPlotter.obj $(src)OPlotter.c
	$(inlib) $(bin)libXo.olb $(bin)OPlotter.obj
	del $(bin)OPlotter.obj;*

$(bin)libXo.olb(mowww) : $(src)mowww.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)mowww.obj $(src)mowww.c
	$(inlib) $(bin)libXo.olb $(bin)mowww.obj
	del $(bin)mowww.obj;*

$(bin)libXo.olb(img) : $(src)img.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)img.obj $(src)img.c
	$(inlib) $(bin)libXo.olb $(bin)img.obj
	del $(bin)img.obj;*

$(bin)libXo.olb(picread) : $(src)picread.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)picread.obj $(src)picread.c
	$(inlib) $(bin)libXo.olb $(bin)picread.obj
	del $(bin)picread.obj;*

$(bin)libXo.olb(xpmread) : $(src)xpmread.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)xpmread.obj $(src)xpmread.c
	$(inlib) $(bin)libXo.olb $(bin)xpmread.obj
	del $(bin)xpmread.obj;*

$(bin)libXo.olb(XoCommands) : $(src)XoCommands.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)XoCommands.obj $(src)XoCommands.c
	$(inlib) $(bin)libXo.olb $(bin)XoCommands.obj
	del $(bin)XoCommands.obj;*

$(bin)libXo.olb(XoTypes) : $(src)XoTypes.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)XoTypes.obj $(src)XoTypes.c
	$(inlib) $(bin)libXo.olb $(bin)XoTypes.obj
	del $(bin)XoTypes.obj;*

$(bin)libXo.olb(OColormapX) : $(src)OColormapX.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)OColormapX.obj $(src)OColormapX.c
	$(inlib) $(bin)libXo.olb $(bin)OColormapX.obj
	del $(bin)OColormapX.obj;*

$(bin)libXo.olb(XoF77) : $(src)XoF77.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)XoF77.obj $(src)XoF77.c
	$(inlib) $(bin)libXo.olb $(bin)XoF77.obj
	del $(bin)XoF77.obj;*

$(bin)libXo.olb(XoShape) : $(src)XoShape.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)XoShape.obj $(src)XoShape.c
	$(inlib) $(bin)libXo.olb $(bin)XoShape.obj
	del $(bin)XoShape.obj;*

$(bin)libXo.olb(XoGraph) : $(src)XoGraph.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)XoGraph.obj $(src)XoGraph.c
	$(inlib) $(bin)libXo.olb $(bin)XoGraph.obj
	del $(bin)XoGraph.obj;*

$(bin)libXo.olb(XoEyes) : $(src)XoEyes.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)XoEyes.obj $(src)XoEyes.c
	$(inlib) $(bin)libXo.olb $(bin)XoEyes.obj
	del $(bin)XoEyes.obj;*

$(bin)libXo.olb(XoLabel) : $(src)XoLabel.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)XoLabel.obj $(src)XoLabel.c
	$(inlib) $(bin)libXo.olb $(bin)XoLabel.obj
	del $(bin)XoLabel.obj;*

$(bin)libXo.olb(XoColormap) : $(src)XoColormap.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)XoColormap.obj $(src)XoColormap.c
	$(inlib) $(bin)libXo.olb $(bin)XoColormap.obj
	del $(bin)XoColormap.obj;*

$(bin)libXo.olb(XoBinds) : $(src)XoBinds.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)XoBinds.obj $(src)XoBinds.c
	$(inlib) $(bin)libXo.olb $(bin)XoBinds.obj
	del $(bin)XoBinds.obj;*

$(bin)libXo.olb(Xo) : $(src)Xo.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)Xo.obj $(src)Xo.c
	$(inlib) $(bin)libXo.olb $(bin)Xo.obj
	del $(bin)Xo.obj;*

$(bin)libXo.olb(XoRheostat) : $(src)XoRheostat.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)XoRheostat.obj $(src)XoRheostat.c
	$(inlib) $(bin)libXo.olb $(bin)XoRheostat.obj
	del $(bin)XoRheostat.obj;*

$(bin)libXo.olb(XoMosaic) : $(src)XoMosaic.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)XoMosaic.obj $(src)XoMosaic.c
	$(inlib) $(bin)libXo.olb $(bin)XoMosaic.obj
	del $(bin)XoMosaic.obj;*

$(bin)libXo.olb(XoHelp) : $(src)XoHelp.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)XoHelp.obj $(src)XoHelp.c
	$(inlib) $(bin)libXo.olb $(bin)XoHelp.obj
	del $(bin)XoHelp.obj;*

$(bin)libXo.olb(XoPlotter) : $(src)XoPlotter.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)XoPlotter.obj $(src)XoPlotter.c
	$(inlib) $(bin)libXo.olb $(bin)XoPlotter.obj
	del $(bin)XoPlotter.obj;*

$(bin)libXo.olb(XoCamera) : $(src)XoCamera.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)XoCamera.obj $(src)XoCamera.c
	$(inlib) $(bin)libXo.olb $(bin)XoCamera.obj
	del $(bin)XoCamera.obj;*

$(bin)libXo.olb(XoImage) : $(src)XoImage.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)XoImage.obj $(src)XoImage.c
	$(inlib) $(bin)libXo.olb $(bin)XoImage.obj
	del $(bin)XoImage.obj;*


