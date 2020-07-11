!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!  File produced by the Co/omake tool
! using file Make.odb.
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!  The soft linked packages has been taken
! from the property 'has' of the 
! 'Make' object of file Make.odb.
! It had the value :
!    X11 Xext Xt Xmu CERN
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
HAS_CCFLAGS = \
$(Xz_CCFLAGS)\
$(X11_CCFLAGS)\
$(Xext_CCFLAGS)\
$(Xt_CCFLAGS)\
$(Xmu_CCFLAGS)\
$(CERN_CCFLAGS)
HAS_CXXFLAGS = \
$(Xz_CXXFLAGS)\
$(X11_CXXFLAGS)\
$(Xext_CXXFLAGS)\
$(Xt_CXXFLAGS)\
$(Xmu_CXXFLAGS)\
$(CERN_CXXFLAGS)
HAS_CPPINCLUDES = \
 $(Xz_CPPINCLUDES)\
,$(X11_CPPINCLUDES)\
,$(Xext_CPPINCLUDES)\
,$(Xt_CPPINCLUDES)\
,$(Xmu_CPPINCLUDES)\
,$(CERN_CPPINCLUDES)
HAS_CPPDEFINES = \
 $(Xz_CPPDEFINES)\
,$(X11_CPPDEFINES)\
,$(Xext_CPPDEFINES)\
,$(Xt_CPPDEFINES)\
,$(Xmu_CPPDEFINES)\
,$(CERN_CPPDEFINES)
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

libXz_target = $(bin)libXz.olb
libXzPAWC_target = $(bin)libXzPAWC.olb
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
all :: \
$(libXz_target) \
$(libXzPAWC_target) \
$(bin)XzPlotterT.exe \
$(bin)oshXz.exe 
	@ write sys$output "Xz : all ok." 

libs :: \
$(libXz_target) \
$(libXzPAWC_target) 
	@ write sys$output "Xz : libs ok." 

apps :: \
$(bin)XzPlotterT.exe \
$(bin)oshXz.exe \
$(bin)hbookT.exe \
$(bin)kuipT.exe \
$(bin)pawcT.exe \
$(bin)NoHT.exe 
	@ write sys$output "Xz : apps ok." 
!.............................................
rmexeo :
	if f$search("$(bin)XzPlotterT.obj;*") .nes. "" then del $(bin)XzPlotterT.obj;*
	if f$search("$(bin)oshXz.obj;*") .nes. "" then del $(bin)oshXz.obj;*
	if f$search("$(bin)hbookT.obj;*") .nes. "" then del $(bin)hbookT.obj;*
	if f$search("$(bin)kuipT.obj;*") .nes. "" then del $(bin)kuipT.obj;*
	if f$search("$(bin)pawcT.obj;*") .nes. "" then del $(bin)pawcT.obj;*
	if f$search("$(bin)NoHT.obj;*") .nes. "" then del $(bin)NoHT.obj;*
!.............................................
XzPlotterT : $(bin)XzPlotterT.exe
	@ write sys$output "Xz : XzPlotterT ok."
oshXz : $(bin)oshXz.exe
	@ write sys$output "Xz : oshXz ok."
hbookT : $(bin)hbookT.exe
	@ write sys$output "Xz : hbookT ok."
kuipT : $(bin)kuipT.exe
	@ write sys$output "Xz : kuipT ok."
pawcT : $(bin)pawcT.exe
	@ write sys$output "Xz : pawcT ok."
NoHT : $(bin)NoHT.exe
	@ write sys$output "Xz : NoHT ok."
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!.............................................
$(bin)XzPlotterT.exe : $(bin)XzPlotterT.obj \
$(libXz_target) \
$(libXzPAWC_target) 
	$(CCLD)$(CCLDFLAGS)/exe=$(bin)XzPlotterT.exe $(bin)XzPlotterT.obj\
,$(libXz)\
,$(libXzPAWC)\
,$(libgraflib)\
,$(libgrafX11)\
,$(libpacklib)\
,$(libkernlib)\
,$(libf77)\
,$(libXx)\
,$(libXmu)\
,$(libXt)\
,$(libXext)\
,$(libX11)\
,$(libCo)\
,$(libm)\
$(CCLDEND)

$(bin)XzPlotterT.obj   : $(app)XzPlotterT.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES),$(APP_CPPINCLUDES))/define=($(APP_CCDEFINES),$(HAS_CPPDEFINES))/obj=$(bin)XzPlotterT.obj $(app)XzPlotterT.c 
!.............................................
$(bin)oshXz.exe : $(bin)oshXz.obj \
$(libXz_target) \
$(libXzPAWC_target) 
	$(CCLD)$(CCLDFLAGS)/exe=$(bin)oshXz.exe $(bin)oshXz.obj\
,$(libXz)\
,$(libXzPAWC)\
,$(libgraflib)\
,$(libgrafX11)\
,$(libpacklib)\
,$(libkernlib)\
,$(libf77)\
,$(libXx)\
,$(libXmu)\
,$(libXt)\
,$(libXext)\
,$(libX11)\
,$(libCo)\
,$(libm)\
$(CCLDEND)

$(bin)oshXz.obj   : $(app)oshXz.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES),$(APP_CPPINCLUDES))/define=($(APP_CCDEFINES),$(HAS_CPPDEFINES))/obj=$(bin)oshXz.obj $(app)oshXz.c 
!.............................................
$(bin)hbookT.exe : $(bin)hbookT.obj 
	$(F77LD)$(F77LDFLAGS)/exe=$(bin)hbookT.exe $(bin)hbookT.obj\
,$(libpacklib)\
,$(libkernlib)\
$(F77LDEND)

$(bin)hbookT.obj   : $(app)hbookT.f
	$(F77)$(F77FLAGS)/obj=$(bin)hbookT.obj $(app)hbookT.f
!.............................................
$(bin)kuipT.exe : $(bin)kuipT.obj \
$(bin)ktcdf.obj \
$(bin)ktyyy.obj 
	$(F77LD)$(F77LDFLAGS)/exe=$(bin)kuipT.exe $(bin)kuipT.obj\
,$(bin)ktcdf.obj\
,$(bin)ktyyy.obj\
,$(libpacklib)\
$(F77LDEND)

$(bin)kuipT.obj   : $(app)kuipT.f
	$(F77)$(F77FLAGS)/obj=$(bin)kuipT.obj $(app)kuipT.f
!.............................................
$(bin)pawcT.exe : $(bin)pawcT.obj 
	$(F77LD)$(F77LDFLAGS)/exe=$(bin)pawcT.exe $(bin)pawcT.obj\
$(F77LDEND)

$(bin)pawcT.obj   : $(app)pawcT.f
	$(F77)$(F77FLAGS)/obj=$(bin)pawcT.obj $(app)pawcT.f
!.............................................
$(bin)NoHT.exe : $(bin)NoHT.obj 
	$(CCLD)$(CCLDFLAGS)/exe=$(bin)NoHT.exe $(bin)NoHT.obj\
$(CCLDEND)

$(bin)NoHT.obj   : $(app)NoHT.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES),$(APP_CPPINCLUDES))/define=($(APP_CCDEFINES),$(HAS_CPPDEFINES))/obj=$(bin)NoHT.obj $(app)NoHT.c 

$(bin)libXz.olb : \
$(bin)libXz.olb(XzCommands) \
$(bin)libXz.olb(XzTypes) \
$(bin)libXz.olb(XzPlotter) \
$(bin)libXz.olb(HFile) \
$(bin)libXz.olb(HBook) \
$(bin)libXz.olb(HPlot) \
$(bin)libXz.olb(Kuip) \
$(bin)libXz.olb(ohplot) \
$(bin)libXz.olb(ohbook) \
$(bin)libXz.olb(okuip) \
$(bin)libXz.olb(hg) \
$(bin)libXz.olb(XzBinds) 
	if f$search("$(bin)libXz.opt;*") .nes. "" then delete $(bin)libXz.opt;*
	open/write file $(bin)libXz.opt
	write file "XZROOT:[$(CONFIG)]libXz/lib"
	close file

$(bin)libXzPAWC.olb : \
$(bin)libXzPAWC.olb(opawc) 
	if f$search("$(bin)libXzPAWC.opt;*") .nes. "" then delete $(bin)libXzPAWC.opt;*
	open/write file $(bin)libXzPAWC.opt
	write file "XZROOT:[$(CONFIG)]libXzPAWC/lib"
	close file
!.............................................
! libXz dependencies 
!.............................................
$(bin)libXz.olb(XzCommands) : $(src)XzCommands.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)XzCommands.obj $(src)XzCommands.c
	$(inlib) $(bin)libXz.olb $(bin)XzCommands.obj
	del $(bin)XzCommands.obj;*

$(bin)libXz.olb(XzTypes) : $(src)XzTypes.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)XzTypes.obj $(src)XzTypes.c
	$(inlib) $(bin)libXz.olb $(bin)XzTypes.obj
	del $(bin)XzTypes.obj;*

$(bin)libXz.olb(XzPlotter) : $(src)XzPlotter.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)XzPlotter.obj $(src)XzPlotter.c
	$(inlib) $(bin)libXz.olb $(bin)XzPlotter.obj
	del $(bin)XzPlotter.obj;*

$(bin)libXz.olb(HFile) : $(src)HFile.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)HFile.obj $(src)HFile.c
	$(inlib) $(bin)libXz.olb $(bin)HFile.obj
	del $(bin)HFile.obj;*

$(bin)libXz.olb(HBook) : $(src)HBook.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)HBook.obj $(src)HBook.c
	$(inlib) $(bin)libXz.olb $(bin)HBook.obj
	del $(bin)HBook.obj;*

$(bin)libXz.olb(HPlot) : $(src)HPlot.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)HPlot.obj $(src)HPlot.c
	$(inlib) $(bin)libXz.olb $(bin)HPlot.obj
	del $(bin)HPlot.obj;*

$(bin)libXz.olb(Kuip) : $(src)Kuip.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)Kuip.obj $(src)Kuip.c
	$(inlib) $(bin)libXz.olb $(bin)Kuip.obj
	del $(bin)Kuip.obj;*

$(bin)libXz.olb(ohplot) : $(src)ohplot.f
	$(F77)$(F77FLAGS)/obj=$(bin)ohplot.obj $(src)ohplot.f
	$(inlib) $(bin)libXz.olb $(bin)ohplot.obj
	del $(bin)ohplot.obj;*

$(bin)libXz.olb(ohbook) : $(src)ohbook.f
	$(F77)$(F77FLAGS)/obj=$(bin)ohbook.obj $(src)ohbook.f
	$(inlib) $(bin)libXz.olb $(bin)ohbook.obj
	del $(bin)ohbook.obj;*

$(bin)libXz.olb(okuip) : $(src)okuip.f
	$(F77)$(F77FLAGS)/obj=$(bin)okuip.obj $(src)okuip.f
	$(inlib) $(bin)libXz.olb $(bin)okuip.obj
	del $(bin)okuip.obj;*

$(bin)libXz.olb(hg) : $(src)hg.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)hg.obj $(src)hg.c
	$(inlib) $(bin)libXz.olb $(bin)hg.obj
	del $(bin)hg.obj;*

$(bin)libXz.olb(XzBinds) : $(src)XzBinds.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)XzBinds.obj $(src)XzBinds.c
	$(inlib) $(bin)libXz.olb $(bin)XzBinds.obj
	del $(bin)XzBinds.obj;*

!.............................................
! libXzPAWC dependencies 
!.............................................
$(bin)libXzPAWC.olb(opawc) : $(src)opawc.f
	$(F77)$(F77FLAGS)/obj=$(bin)opawc.obj $(src)opawc.f
	$(inlib) $(bin)libXzPAWC.olb $(bin)opawc.obj
	del $(bin)opawc.obj;*

$(bin)ktcdf.obj : $(app)ktcdf.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)ktcdf.obj $(app)ktcdf.c

$(bin)ktyyy.obj : $(app)ktyyy.c
	$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)ktyyy.obj $(app)ktyyy.c


