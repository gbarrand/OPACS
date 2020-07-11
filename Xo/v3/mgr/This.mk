#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#  File produced by the Co/omake tool
# using file Make.odb.
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#  The soft linked packages has been taken
# from the property 'has' of the 
# 'Make' object of file Make.odb.
# It had the value :
#    X11 Xext Xt Xmu GL HTML Xm XoXm
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
HAS_CPPFLAGS = \
 $(Xo_CPPFLAGS)\
 $(X11_CPPFLAGS)\
 $(Xext_CPPFLAGS)\
 $(Xt_CPPFLAGS)\
 $(Xmu_CPPFLAGS)\
 $(GL_CPPFLAGS)\
 $(HTML_CPPFLAGS)\
 $(Xm_CPPFLAGS)\
 $(XoXm_CPPFLAGS)
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
all   :: mkdir

clean ::
	/bin/rm -f $(bin)/*.o;/bin/rm -f $(bin)/*.exe;/bin/rm -f $(bin)/*.class;/bin/rm -f $(bin)/*.a;/bin/rm -f $(bin)/*.so;/bin/rm -f $(bin)/*.sl
rmlib ::
	/bin/rm -f $(bin)/*.a
rmo   ::
	/bin/rm -f $(bin)/*.o
rmexe ::
	/bin/rm -f $(bin)/*.exe;/bin/rm -f $(bin)/*.class

mkdir :
	@if test -d $(bin) ; then exit ; else mkdir $(bin) ; echo "$(bin) created." ; fi

libXo_target = $(bin)/libXo.$(a_or_so)
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
all :: mkdir \
$(libXo_target) \
$(bin)/XoCameraT.exe \
$(bin)/XoPlotterT.exe \
$(bin)/OPlotterT.exe \
$(bin)/XoMosaicT.exe \
$(bin)/XoGraphT.exe \
$(bin)/XoLabelT.exe \
$(bin)/XoEyesT.exe \
$(bin)/XoImageT.exe \
$(bin)/XoColormapT.exe \
$(bin)/XoRheostatT.exe \
$(bin)/XoHelpT.exe 
	@echo "Xo : all ok." 

libs :: mkdir \
$(libXo_target) 
	@echo "Xo : libs ok." 

apps :: mkdir \
$(bin)/XoCameraT.exe \
$(bin)/XoPlotterT.exe \
$(bin)/OPlotterT.exe \
$(bin)/XoMosaicT.exe \
$(bin)/XoGraphT.exe \
$(bin)/XoLabelT.exe \
$(bin)/XoEyesT.exe \
$(bin)/XoImageT.exe \
$(bin)/XoColormapT.exe \
$(bin)/XoRheostatT.exe \
$(bin)/XoHelpT.exe \
$(bin)/XoShapeT.exe \
$(bin)/XawT.exe \
$(bin)/XmPushBT.exe \
$(bin)/NoHT.exe 
	@echo "Xo : apps ok." 
#--------------------------------------------
rmexeo :
	/bin/rm -f $(bin)/XoCameraT.o
	/bin/rm -f $(bin)/XoPlotterT.o
	/bin/rm -f $(bin)/OPlotterT.o
	/bin/rm -f $(bin)/XoMosaicT.o
	/bin/rm -f $(bin)/XoGraphT.o
	/bin/rm -f $(bin)/XoLabelT.o
	/bin/rm -f $(bin)/XoEyesT.o
	/bin/rm -f $(bin)/XoImageT.o
	/bin/rm -f $(bin)/XoColormapT.o
	/bin/rm -f $(bin)/XoRheostatT.o
	/bin/rm -f $(bin)/XoHelpT.o
	/bin/rm -f $(bin)/XoShapeT.o
	/bin/rm -f $(bin)/XawT.o
	/bin/rm -f $(bin)/XmPushBT.o
	/bin/rm -f $(bin)/NoHT.o
#--------------------------------------------
XoCameraT : $(bin)/XoCameraT.exe
	@echo "Xo : XoCameraT ok."
XoPlotterT : $(bin)/XoPlotterT.exe
	@echo "Xo : XoPlotterT ok."
OPlotterT : $(bin)/OPlotterT.exe
	@echo "Xo : OPlotterT ok."
XoMosaicT : $(bin)/XoMosaicT.exe
	@echo "Xo : XoMosaicT ok."
XoGraphT : $(bin)/XoGraphT.exe
	@echo "Xo : XoGraphT ok."
XoLabelT : $(bin)/XoLabelT.exe
	@echo "Xo : XoLabelT ok."
XoEyesT : $(bin)/XoEyesT.exe
	@echo "Xo : XoEyesT ok."
XoImageT : $(bin)/XoImageT.exe
	@echo "Xo : XoImageT ok."
XoColormapT : $(bin)/XoColormapT.exe
	@echo "Xo : XoColormapT ok."
XoRheostatT : $(bin)/XoRheostatT.exe
	@echo "Xo : XoRheostatT ok."
XoHelpT : $(bin)/XoHelpT.exe
	@echo "Xo : XoHelpT ok."
XoShapeT : $(bin)/XoShapeT.exe
	@echo "Xo : XoShapeT ok."
XawT : $(bin)/XawT.exe
	@echo "Xo : XawT ok."
XmPushBT : $(bin)/XmPushBT.exe
	@echo "Xo : XmPushBT ok."
NoHT : $(bin)/NoHT.exe
	@echo "Xo : NoHT ok."
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#--------------------------------------------
$(bin)/XoCameraT.exe : $(bin)/XoCameraT.o \
$(libXo_target) 
	$(CCLD) $(CCLDFLAGS) -o $(bin)/XoCameraT.exe $(bin)/XoCameraT.o \
$(libXo) \
$(libwww) \
$(libhtmlw) \
$(libXm) \
$(libXx) \
$(libXmu) \
$(libXt) \
$(libGo) \
$(libGLX) \
$(libGLU) \
$(libGL) \
$(libXext) \
$(libX11) \
$(libHo) \
$(libCo) \
$(libm) \
$(CCLDEND) 

$(bin)/XoCameraT.o   : $(app)/XoCameraT.c
	$(CC) $(CFLAGS) $(APP_CCFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/XoCameraT.o $(app)/XoCameraT.c 
#--------------------------------------------
$(bin)/XoPlotterT.exe : $(bin)/XoPlotterT.o \
$(libXo_target) 
	$(CCLD) $(CCLDFLAGS) -o $(bin)/XoPlotterT.exe $(bin)/XoPlotterT.o \
$(libXo) \
$(libwww) \
$(libhtmlw) \
$(libXm) \
$(libXx) \
$(libXmu) \
$(libXt) \
$(libGo) \
$(libGLX) \
$(libGLU) \
$(libGL) \
$(libXext) \
$(libX11) \
$(libHo) \
$(libCo) \
$(libm) \
$(CCLDEND) 

$(bin)/XoPlotterT.o   : $(app)/XoPlotterT.c
	$(CC) $(CFLAGS) $(APP_CCFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/XoPlotterT.o $(app)/XoPlotterT.c 
#--------------------------------------------
$(bin)/OPlotterT.exe : $(bin)/OPlotterT.o \
$(libXo_target) 
	$(CCLD) $(CCLDFLAGS) -o $(bin)/OPlotterT.exe $(bin)/OPlotterT.o \
$(libXo) \
$(libwww) \
$(libhtmlw) \
$(libXm) \
$(libXx) \
$(libXmu) \
$(libXt) \
$(libGo) \
$(libGLX) \
$(libGLU) \
$(libGL) \
$(libXext) \
$(libX11) \
$(libHo) \
$(libCo) \
$(libm) \
$(CCLDEND) 

$(bin)/OPlotterT.o   : $(app)/OPlotterT.c
	$(CC) $(CFLAGS) $(APP_CCFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/OPlotterT.o $(app)/OPlotterT.c 
#--------------------------------------------
$(bin)/XoMosaicT.exe : $(bin)/XoMosaicT.o \
$(libXo_target) 
	$(CCLD) $(CCLDFLAGS) -o $(bin)/XoMosaicT.exe $(bin)/XoMosaicT.o \
$(libXo) \
$(libwww) \
$(libhtmlw) \
$(libXm) \
$(libXx) \
$(libXmu) \
$(libXt) \
$(libGo) \
$(libGLX) \
$(libGLU) \
$(libGL) \
$(libXext) \
$(libX11) \
$(libHo) \
$(libCo) \
$(libm) \
$(CCLDEND) 

$(bin)/XoMosaicT.o   : $(app)/XoMosaicT.c
	$(CC) $(CFLAGS) $(APP_CCFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/XoMosaicT.o $(app)/XoMosaicT.c 
#--------------------------------------------
$(bin)/XoGraphT.exe : $(bin)/XoGraphT.o \
$(libXo_target) 
	$(CCLD) $(CCLDFLAGS) -o $(bin)/XoGraphT.exe $(bin)/XoGraphT.o \
$(libXo) \
$(libwww) \
$(libhtmlw) \
$(libXm) \
$(libXx) \
$(libXmu) \
$(libXt) \
$(libGo) \
$(libGLX) \
$(libGLU) \
$(libGL) \
$(libXext) \
$(libX11) \
$(libHo) \
$(libCo) \
$(libm) \
$(CCLDEND) 

$(bin)/XoGraphT.o   : $(app)/XoGraphT.c
	$(CC) $(CFLAGS) $(APP_CCFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/XoGraphT.o $(app)/XoGraphT.c 
#--------------------------------------------
$(bin)/XoLabelT.exe : $(bin)/XoLabelT.o \
$(libXo_target) 
	$(CCLD) $(CCLDFLAGS) -o $(bin)/XoLabelT.exe $(bin)/XoLabelT.o \
$(libXo) \
$(libwww) \
$(libhtmlw) \
$(libXm) \
$(libXx) \
$(libXmu) \
$(libXt) \
$(libGo) \
$(libGLX) \
$(libGLU) \
$(libGL) \
$(libXext) \
$(libX11) \
$(libHo) \
$(libCo) \
$(libm) \
$(CCLDEND) 

$(bin)/XoLabelT.o   : $(app)/XoLabelT.c
	$(CC) $(CFLAGS) $(APP_CCFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/XoLabelT.o $(app)/XoLabelT.c 
#--------------------------------------------
$(bin)/XoEyesT.exe : $(bin)/XoEyesT.o \
$(libXo_target) 
	$(CCLD) $(CCLDFLAGS) -o $(bin)/XoEyesT.exe $(bin)/XoEyesT.o \
$(libXo) \
$(libwww) \
$(libhtmlw) \
$(libXm) \
$(libXx) \
$(libXmu) \
$(libXt) \
$(libGo) \
$(libGLX) \
$(libGLU) \
$(libGL) \
$(libXext) \
$(libX11) \
$(libHo) \
$(libCo) \
$(libm) \
$(CCLDEND) 

$(bin)/XoEyesT.o   : $(app)/XoEyesT.c
	$(CC) $(CFLAGS) $(APP_CCFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/XoEyesT.o $(app)/XoEyesT.c 
#--------------------------------------------
$(bin)/XoImageT.exe : $(bin)/XoImageT.o \
$(libXo_target) 
	$(CCLD) $(CCLDFLAGS) -o $(bin)/XoImageT.exe $(bin)/XoImageT.o \
$(libXo) \
$(libwww) \
$(libhtmlw) \
$(libXm) \
$(libXx) \
$(libXmu) \
$(libXt) \
$(libGo) \
$(libGLX) \
$(libGLU) \
$(libGL) \
$(libXext) \
$(libX11) \
$(libHo) \
$(libCo) \
$(libm) \
$(CCLDEND) 

$(bin)/XoImageT.o   : $(app)/XoImageT.c
	$(CC) $(CFLAGS) $(APP_CCFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/XoImageT.o $(app)/XoImageT.c 
#--------------------------------------------
$(bin)/XoColormapT.exe : $(bin)/XoColormapT.o \
$(libXo_target) 
	$(CCLD) $(CCLDFLAGS) -o $(bin)/XoColormapT.exe $(bin)/XoColormapT.o \
$(libXo) \
$(libwww) \
$(libhtmlw) \
$(libXm) \
$(libXx) \
$(libXmu) \
$(libXt) \
$(libGo) \
$(libGLX) \
$(libGLU) \
$(libGL) \
$(libXext) \
$(libX11) \
$(libHo) \
$(libCo) \
$(libm) \
$(CCLDEND) 

$(bin)/XoColormapT.o   : $(app)/XoColormapT.c
	$(CC) $(CFLAGS) $(APP_CCFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/XoColormapT.o $(app)/XoColormapT.c 
#--------------------------------------------
$(bin)/XoRheostatT.exe : $(bin)/XoRheostatT.o \
$(libXo_target) 
	$(CCLD) $(CCLDFLAGS) -o $(bin)/XoRheostatT.exe $(bin)/XoRheostatT.o \
$(libXo) \
$(libwww) \
$(libhtmlw) \
$(libXm) \
$(libXx) \
$(libXmu) \
$(libXt) \
$(libGo) \
$(libGLX) \
$(libGLU) \
$(libGL) \
$(libXext) \
$(libX11) \
$(libHo) \
$(libCo) \
$(libm) \
$(CCLDEND) 

$(bin)/XoRheostatT.o   : $(app)/XoRheostatT.c
	$(CC) $(CFLAGS) $(APP_CCFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/XoRheostatT.o $(app)/XoRheostatT.c 
#--------------------------------------------
$(bin)/XoHelpT.exe : $(bin)/XoHelpT.o \
$(libXo_target) 
	$(CCLD) $(CCLDFLAGS) -o $(bin)/XoHelpT.exe $(bin)/XoHelpT.o \
$(libXo) \
$(libwww) \
$(libhtmlw) \
$(libXm) \
$(libXx) \
$(libXmu) \
$(libXt) \
$(libGo) \
$(libGLX) \
$(libGLU) \
$(libGL) \
$(libXext) \
$(libX11) \
$(libHo) \
$(libCo) \
$(libm) \
$(CCLDEND) 

$(bin)/XoHelpT.o   : $(app)/XoHelpT.c
	$(CC) $(CFLAGS) $(APP_CCFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/XoHelpT.o $(app)/XoHelpT.c 
#--------------------------------------------
$(bin)/XoShapeT.exe : $(bin)/XoShapeT.o \
$(libXo_target) 
	$(CCLD) $(CCLDFLAGS) -o $(bin)/XoShapeT.exe $(bin)/XoShapeT.o \
$(libXo) \
$(libwww) \
$(libhtmlw) \
$(libXm) \
$(libXx) \
$(libXmu) \
$(libXt) \
$(libGo) \
$(libGLX) \
$(libGLU) \
$(libGL) \
$(libXext) \
$(libX11) \
$(libHo) \
$(libCo) \
$(libm) \
$(CCLDEND) 

$(bin)/XoShapeT.o   : $(app)/XoShapeT.c
	$(CC) $(CFLAGS) $(APP_CCFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/XoShapeT.o $(app)/XoShapeT.c 
#--------------------------------------------
$(bin)/XawT.exe : $(bin)/XawT.o 
	$(CCLD) $(CCLDFLAGS) -o $(bin)/XawT.exe $(bin)/XawT.o \
$(libXmu) \
$(libXt) \
$(libXext) \
$(libX11) \
$(libm) \
$(CCLDEND) 

$(bin)/XawT.o   : $(app)/XawT.c
	$(CC) $(CFLAGS) $(APP_CCFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/XawT.o $(app)/XawT.c 
#--------------------------------------------
$(bin)/XmPushBT.exe : $(bin)/XmPushBT.o 
	$(CCLD) $(CCLDFLAGS) -o $(bin)/XmPushBT.exe $(bin)/XmPushBT.o \
$(libXm) \
$(libXt) \
$(libX11) \
$(CCLDEND) 

$(bin)/XmPushBT.o   : $(app)/XmPushBT.c
	$(CC) $(CFLAGS) $(APP_CCFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/XmPushBT.o $(app)/XmPushBT.c 
#--------------------------------------------
$(bin)/NoHT.exe : $(bin)/NoHT.o 
	$(CCLD) $(CCLDFLAGS) -o $(bin)/NoHT.exe $(bin)/NoHT.o \
$(CCLDEND) 

$(bin)/NoHT.o   : $(app)/NoHT.c
	$(CC) $(CFLAGS) $(APP_CCFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/NoHT.o $(app)/NoHT.c 
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
# libraries 
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
.PRECIOUS : $(bin)/libXo.a 
#--------------------------------------------
$(bin)/libXo.$(so) : $(bin)/libXo.a 
	$(make_so) libXo $(bin) "\
$(libwww) \
$(libhtmlw) \
$(libXm) \
$(libXaw) \
$(libXx) \
$(libXmu) \
$(libXt) \
$(libGo) \
$(libGLX) \
$(libGLU) \
$(libGL) \
$(libHo) \
$(libXext) \
$(libX11) \
$(libCo) \
$(libm) \
$(libc) \
"

$(bin)/libXo.a : \
$(bin)/libXo.a(OImage.o) \
$(bin)/libXo.a(OPlotter.o) \
$(bin)/libXo.a(mowww.o) \
$(bin)/libXo.a(img.o) \
$(bin)/libXo.a(picread.o) \
$(bin)/libXo.a(xpmread.o) \
$(bin)/libXo.a(XoCommands.o) \
$(bin)/libXo.a(XoTypes.o) \
$(bin)/libXo.a(OColormapX.o) \
$(bin)/libXo.a(XoF77.o) \
$(bin)/libXo.a(XoShape.o) \
$(bin)/libXo.a(XoGraph.o) \
$(bin)/libXo.a(XoEyes.o) \
$(bin)/libXo.a(XoLabel.o) \
$(bin)/libXo.a(XoColormap.o) \
$(bin)/libXo.a(XoBinds.o) \
$(bin)/libXo.a(Xo.o) \
$(bin)/libXo.a(XoRheostat.o) \
$(bin)/libXo.a(XoMosaic.o) \
$(bin)/libXo.a(XoHelp.o) \
$(bin)/libXo.a(XoPlotter.o) \
$(bin)/libXo.a(XoCamera.o) \
$(bin)/libXo.a(XoImage.o) 
	@cd $(bin) ; if [ -f /bin/ranlib ] ; then /bin/ranlib libXo.a ; fi ; cd $(mgr)
#--------------------------------------------
# libXo dependencies 
#--------------------------------------------
$(bin)/libXo.a(OImage.o) : $(src)/OImage.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/OImage.o $(src)/OImage.c
	ar cr $(bin)/libXo.a $(bin)/OImage.o
	/bin/rm -f $(bin)/OImage.o

$(bin)/libXo.a(OPlotter.o) : $(src)/OPlotter.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/OPlotter.o $(src)/OPlotter.c
	ar cr $(bin)/libXo.a $(bin)/OPlotter.o
	/bin/rm -f $(bin)/OPlotter.o

$(bin)/libXo.a(mowww.o) : $(src)/mowww.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/mowww.o $(src)/mowww.c
	ar cr $(bin)/libXo.a $(bin)/mowww.o
	/bin/rm -f $(bin)/mowww.o

$(bin)/libXo.a(img.o) : $(src)/img.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/img.o $(src)/img.c
	ar cr $(bin)/libXo.a $(bin)/img.o
	/bin/rm -f $(bin)/img.o

$(bin)/libXo.a(picread.o) : $(src)/picread.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/picread.o $(src)/picread.c
	ar cr $(bin)/libXo.a $(bin)/picread.o
	/bin/rm -f $(bin)/picread.o

$(bin)/libXo.a(xpmread.o) : $(src)/xpmread.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/xpmread.o $(src)/xpmread.c
	ar cr $(bin)/libXo.a $(bin)/xpmread.o
	/bin/rm -f $(bin)/xpmread.o

$(bin)/libXo.a(XoCommands.o) : $(src)/XoCommands.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/XoCommands.o $(src)/XoCommands.c
	ar cr $(bin)/libXo.a $(bin)/XoCommands.o
	/bin/rm -f $(bin)/XoCommands.o

$(bin)/libXo.a(XoTypes.o) : $(src)/XoTypes.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/XoTypes.o $(src)/XoTypes.c
	ar cr $(bin)/libXo.a $(bin)/XoTypes.o
	/bin/rm -f $(bin)/XoTypes.o

$(bin)/libXo.a(OColormapX.o) : $(src)/OColormapX.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/OColormapX.o $(src)/OColormapX.c
	ar cr $(bin)/libXo.a $(bin)/OColormapX.o
	/bin/rm -f $(bin)/OColormapX.o

$(bin)/libXo.a(XoF77.o) : $(src)/XoF77.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/XoF77.o $(src)/XoF77.c
	ar cr $(bin)/libXo.a $(bin)/XoF77.o
	/bin/rm -f $(bin)/XoF77.o

$(bin)/libXo.a(XoShape.o) : $(src)/XoShape.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/XoShape.o $(src)/XoShape.c
	ar cr $(bin)/libXo.a $(bin)/XoShape.o
	/bin/rm -f $(bin)/XoShape.o

$(bin)/libXo.a(XoGraph.o) : $(src)/XoGraph.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/XoGraph.o $(src)/XoGraph.c
	ar cr $(bin)/libXo.a $(bin)/XoGraph.o
	/bin/rm -f $(bin)/XoGraph.o

$(bin)/libXo.a(XoEyes.o) : $(src)/XoEyes.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/XoEyes.o $(src)/XoEyes.c
	ar cr $(bin)/libXo.a $(bin)/XoEyes.o
	/bin/rm -f $(bin)/XoEyes.o

$(bin)/libXo.a(XoLabel.o) : $(src)/XoLabel.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/XoLabel.o $(src)/XoLabel.c
	ar cr $(bin)/libXo.a $(bin)/XoLabel.o
	/bin/rm -f $(bin)/XoLabel.o

$(bin)/libXo.a(XoColormap.o) : $(src)/XoColormap.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/XoColormap.o $(src)/XoColormap.c
	ar cr $(bin)/libXo.a $(bin)/XoColormap.o
	/bin/rm -f $(bin)/XoColormap.o

$(bin)/libXo.a(XoBinds.o) : $(src)/XoBinds.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/XoBinds.o $(src)/XoBinds.c
	ar cr $(bin)/libXo.a $(bin)/XoBinds.o
	/bin/rm -f $(bin)/XoBinds.o

$(bin)/libXo.a(Xo.o) : $(src)/Xo.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/Xo.o $(src)/Xo.c
	ar cr $(bin)/libXo.a $(bin)/Xo.o
	/bin/rm -f $(bin)/Xo.o

$(bin)/libXo.a(XoRheostat.o) : $(src)/XoRheostat.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/XoRheostat.o $(src)/XoRheostat.c
	ar cr $(bin)/libXo.a $(bin)/XoRheostat.o
	/bin/rm -f $(bin)/XoRheostat.o

$(bin)/libXo.a(XoMosaic.o) : $(src)/XoMosaic.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/XoMosaic.o $(src)/XoMosaic.c
	ar cr $(bin)/libXo.a $(bin)/XoMosaic.o
	/bin/rm -f $(bin)/XoMosaic.o

$(bin)/libXo.a(XoHelp.o) : $(src)/XoHelp.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/XoHelp.o $(src)/XoHelp.c
	ar cr $(bin)/libXo.a $(bin)/XoHelp.o
	/bin/rm -f $(bin)/XoHelp.o

$(bin)/libXo.a(XoPlotter.o) : $(src)/XoPlotter.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/XoPlotter.o $(src)/XoPlotter.c
	ar cr $(bin)/libXo.a $(bin)/XoPlotter.o
	/bin/rm -f $(bin)/XoPlotter.o

$(bin)/libXo.a(XoCamera.o) : $(src)/XoCamera.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/XoCamera.o $(src)/XoCamera.c
	ar cr $(bin)/libXo.a $(bin)/XoCamera.o
	/bin/rm -f $(bin)/XoCamera.o

$(bin)/libXo.a(XoImage.o) : $(src)/XoImage.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/XoImage.o $(src)/XoImage.c
	ar cr $(bin)/libXo.a $(bin)/XoImage.o
	/bin/rm -f $(bin)/XoImage.o


