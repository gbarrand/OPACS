#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#  File produced by the Co/omake tool
# using file Make.odb.
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#  The soft linked packages has been taken
# from the property 'has' of the 
# 'Make' object of file Make.odb.
# It had the value :
#    X11 Xext Xt Xmu CERN
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
HAS_CPPFLAGS = \
 $(Xz_CPPFLAGS)\
 $(X11_CPPFLAGS)\
 $(Xext_CPPFLAGS)\
 $(Xt_CPPFLAGS)\
 $(Xmu_CPPFLAGS)\
 $(CERN_CPPFLAGS)
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

libXz_target = $(bin)/libXz.a
libXzPAWC_target = $(bin)/libXzPAWC.a
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
all :: mkdir \
$(libXz_target) \
$(libXzPAWC_target) \
$(bin)/XzPlotterT.exe \
$(bin)/oshXz.exe 
	@echo "Xz : all ok." 

libs :: mkdir \
$(libXz_target) \
$(libXzPAWC_target) 
	@echo "Xz : libs ok." 

apps :: mkdir \
$(bin)/XzPlotterT.exe \
$(bin)/oshXz.exe \
$(bin)/hbookT.exe \
$(bin)/kuipT.exe \
$(bin)/pawcT.exe \
$(bin)/NoHT.exe 
	@echo "Xz : apps ok." 
#--------------------------------------------
rmexeo :
	/bin/rm -f $(bin)/XzPlotterT.o
	/bin/rm -f $(bin)/oshXz.o
	/bin/rm -f $(bin)/hbookT.o
	/bin/rm -f $(bin)/kuipT.o
	/bin/rm -f $(bin)/pawcT.o
	/bin/rm -f $(bin)/NoHT.o
#--------------------------------------------
XzPlotterT : $(bin)/XzPlotterT.exe
	@echo "Xz : XzPlotterT ok."
oshXz : $(bin)/oshXz.exe
	@echo "Xz : oshXz ok."
hbookT : $(bin)/hbookT.exe
	@echo "Xz : hbookT ok."
kuipT : $(bin)/kuipT.exe
	@echo "Xz : kuipT ok."
pawcT : $(bin)/pawcT.exe
	@echo "Xz : pawcT ok."
NoHT : $(bin)/NoHT.exe
	@echo "Xz : NoHT ok."
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#--------------------------------------------
$(bin)/XzPlotterT.exe : $(bin)/XzPlotterT.o \
$(libXz_target) \
$(libXzPAWC_target) 
	$(CCLD) $(CCLDFLAGS) -o $(bin)/XzPlotterT.exe $(bin)/XzPlotterT.o \
$(libXz) \
$(libXzPAWC) \
$(libgraflib) \
$(libgrafX11) \
$(libpacklib) \
$(libkernlib) \
$(libf77) \
$(libXx) \
$(libXmu) \
$(libXt) \
$(libXext) \
$(libX11) \
$(libCo) \
$(libm) \
$(CCLDEND) 

$(bin)/XzPlotterT.o   : $(app)/XzPlotterT.c
	$(CC) $(CFLAGS) $(APP_CCFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/XzPlotterT.o $(app)/XzPlotterT.c 
#--------------------------------------------
$(bin)/oshXz.exe : $(bin)/oshXz.o \
$(libXz_target) \
$(libXzPAWC_target) 
	$(CCLD) $(CCLDFLAGS) -o $(bin)/oshXz.exe $(bin)/oshXz.o \
$(libXz) \
$(libXzPAWC) \
$(libgraflib) \
$(libgrafX11) \
$(libpacklib) \
$(libkernlib) \
$(libf77) \
$(libXx) \
$(libXmu) \
$(libXt) \
$(libXext) \
$(libX11) \
$(libCo) \
$(libm) \
$(CCLDEND) 

$(bin)/oshXz.o   : $(app)/oshXz.c
	$(CC) $(CFLAGS) $(APP_CCFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/oshXz.o $(app)/oshXz.c 
#--------------------------------------------
$(bin)/hbookT.exe : $(bin)/hbookT.o 
	$(F77LD) $(F77LDFLAGS) -o $(bin)/hbookT.exe $(bin)/hbookT.o \
$(libpacklib) \
$(libkernlib) \
$(F77LDEND) 

$(bin)/hbookT.o   : $(app)/hbookT.f
	$(F77) $(F77FLAGS) -c -o $(bin)/hbookT.o $(app)/hbookT.f
#--------------------------------------------
$(bin)/kuipT.exe : $(bin)/kuipT.o \
$(bin)/ktcdf.o \
$(bin)/ktyyy.o 
	$(F77LD) $(F77LDFLAGS) -o $(bin)/kuipT.exe $(bin)/kuipT.o \
$(bin)/ktcdf.o \
$(bin)/ktyyy.o \
$(libpacklib) \
$(F77LDEND) 

$(bin)/kuipT.o   : $(app)/kuipT.f
	$(F77) $(F77FLAGS) -c -o $(bin)/kuipT.o $(app)/kuipT.f
#--------------------------------------------
$(bin)/pawcT.exe : $(bin)/pawcT.o 
	$(F77LD) $(F77LDFLAGS) -o $(bin)/pawcT.exe $(bin)/pawcT.o \
$(F77LDEND) 

$(bin)/pawcT.o   : $(app)/pawcT.f
	$(F77) $(F77FLAGS) -c -o $(bin)/pawcT.o $(app)/pawcT.f
#--------------------------------------------
$(bin)/NoHT.exe : $(bin)/NoHT.o 
	$(CCLD) $(CCLDFLAGS) -o $(bin)/NoHT.exe $(bin)/NoHT.o \
$(CCLDEND) 

$(bin)/NoHT.o   : $(app)/NoHT.c
	$(CC) $(CFLAGS) $(APP_CCFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/NoHT.o $(app)/NoHT.c 
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
# libraries 
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
.PRECIOUS : $(bin)/libXz.a $(bin)/libXzPAWC.a 
#--------------------------------------------

$(bin)/libXz.a : \
$(bin)/libXz.a(XzCommands.o) \
$(bin)/libXz.a(XzTypes.o) \
$(bin)/libXz.a(XzPlotter.o) \
$(bin)/libXz.a(HFile.o) \
$(bin)/libXz.a(HBook.o) \
$(bin)/libXz.a(HPlot.o) \
$(bin)/libXz.a(Kuip.o) \
$(bin)/libXz.a(ohplot.o) \
$(bin)/libXz.a(ohbook.o) \
$(bin)/libXz.a(okuip.o) \
$(bin)/libXz.a(hg.o) \
$(bin)/libXz.a(XzBinds.o) 
	@cd $(bin) ; if [ -f /bin/ranlib ] ; then /bin/ranlib libXz.a ; fi ; cd $(mgr)

$(bin)/libXzPAWC.a : \
$(bin)/libXzPAWC.a(opawc.o) 
	@cd $(bin) ; if [ -f /bin/ranlib ] ; then /bin/ranlib libXzPAWC.a ; fi ; cd $(mgr)
#--------------------------------------------
# libXz dependencies 
#--------------------------------------------
$(bin)/libXz.a(XzCommands.o) : $(src)/XzCommands.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/XzCommands.o $(src)/XzCommands.c
	ar cr $(bin)/libXz.a $(bin)/XzCommands.o
	/bin/rm -f $(bin)/XzCommands.o

$(bin)/libXz.a(XzTypes.o) : $(src)/XzTypes.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/XzTypes.o $(src)/XzTypes.c
	ar cr $(bin)/libXz.a $(bin)/XzTypes.o
	/bin/rm -f $(bin)/XzTypes.o

$(bin)/libXz.a(XzPlotter.o) : $(src)/XzPlotter.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/XzPlotter.o $(src)/XzPlotter.c
	ar cr $(bin)/libXz.a $(bin)/XzPlotter.o
	/bin/rm -f $(bin)/XzPlotter.o

$(bin)/libXz.a(HFile.o) : $(src)/HFile.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/HFile.o $(src)/HFile.c
	ar cr $(bin)/libXz.a $(bin)/HFile.o
	/bin/rm -f $(bin)/HFile.o

$(bin)/libXz.a(HBook.o) : $(src)/HBook.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/HBook.o $(src)/HBook.c
	ar cr $(bin)/libXz.a $(bin)/HBook.o
	/bin/rm -f $(bin)/HBook.o

$(bin)/libXz.a(HPlot.o) : $(src)/HPlot.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/HPlot.o $(src)/HPlot.c
	ar cr $(bin)/libXz.a $(bin)/HPlot.o
	/bin/rm -f $(bin)/HPlot.o

$(bin)/libXz.a(Kuip.o) : $(src)/Kuip.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/Kuip.o $(src)/Kuip.c
	ar cr $(bin)/libXz.a $(bin)/Kuip.o
	/bin/rm -f $(bin)/Kuip.o

$(bin)/libXz.a(ohplot.o) : $(src)/ohplot.f 
	$(F77) $(F77FLAGS) -c -o $(bin)/ohplot.o $(src)/ohplot.f
	ar cr $(bin)/libXz.a $(bin)/ohplot.o
	/bin/rm -f $(bin)/ohplot.o

$(bin)/libXz.a(ohbook.o) : $(src)/ohbook.f 
	$(F77) $(F77FLAGS) -c -o $(bin)/ohbook.o $(src)/ohbook.f
	ar cr $(bin)/libXz.a $(bin)/ohbook.o
	/bin/rm -f $(bin)/ohbook.o

$(bin)/libXz.a(okuip.o) : $(src)/okuip.f 
	$(F77) $(F77FLAGS) -c -o $(bin)/okuip.o $(src)/okuip.f
	ar cr $(bin)/libXz.a $(bin)/okuip.o
	/bin/rm -f $(bin)/okuip.o

$(bin)/libXz.a(hg.o) : $(src)/hg.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/hg.o $(src)/hg.c
	ar cr $(bin)/libXz.a $(bin)/hg.o
	/bin/rm -f $(bin)/hg.o

$(bin)/libXz.a(XzBinds.o) : $(src)/XzBinds.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/XzBinds.o $(src)/XzBinds.c
	ar cr $(bin)/libXz.a $(bin)/XzBinds.o
	/bin/rm -f $(bin)/XzBinds.o

#--------------------------------------------
# libXzPAWC dependencies 
#--------------------------------------------
$(bin)/libXzPAWC.a(opawc.o) : $(src)/opawc.f 
	$(F77) $(F77FLAGS) -c -o $(bin)/opawc.o $(src)/opawc.f
	ar cr $(bin)/libXzPAWC.a $(bin)/opawc.o
	/bin/rm -f $(bin)/opawc.o

$(bin)/ktcdf.o : $(app)/ktcdf.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/ktcdf.o $(app)/ktcdf.c

$(bin)/ktyyy.o : $(app)/ktyyy.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/ktyyy.o $(app)/ktyyy.c


