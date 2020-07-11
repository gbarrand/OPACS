#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#  File produced by the Co/omake tool
# using file Make.odb.
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
include macros.mk
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#  The soft linked packages has been taken
# from the command line option '-has'.
# It had the value :
#    X11 Xext Xt Xmu Xm WoXm WoGL WoXo WoHTML
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
HAS_CPPFLAGS = \
 $(Wo_CPPFLAGS)\
 $(X11_CPPFLAGS)\
 $(Xext_CPPFLAGS)\
 $(Xt_CPPFLAGS)\
 $(Xmu_CPPFLAGS)\
 $(Xm_CPPFLAGS)\
 $(WoXm_CPPFLAGS)\
 $(WoGL_CPPFLAGS)\
 $(WoXo_CPPFLAGS)\
 $(WoHTML_CPPFLAGS)
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

libWo_target = $(bin)/libWo.$(a_or_so)
libWoXm_target = $(bin)/libWoXm.$(a_or_so)
libWoGL_target = $(bin)/libWoGL.$(a_or_so)
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
all :: mkdir \
$(libWo_target) \
$(libWoXm_target) \
$(libWoGL_target) \
$(bin)/oshow.exe \
$(bin)/onew.exe 
	@echo "Wo : all ok." 

libs :: mkdir \
$(libWo_target) \
$(libWoXm_target) \
$(libWoGL_target) 
	@echo "Wo : libs ok." 

apps :: mkdir \
$(bin)/oshow.exe \
$(bin)/onew.exe \
$(bin)/oiv.exe \
$(bin)/oshWo.exe \
$(bin)/WoT.exe \
$(bin)/CiT.exe \
$(bin)/NoHT.exe \
$(bin)/tclT.exe \
$(bin)/ouiToodb.exe \
$(bin)/ovoid.exe \
$(bin)/jo.class \
$(bin)/jolet.class \
$(bin)/JavaT.class \
$(bin)/wof77t.exe 
	@echo "Wo : apps ok." 
#--------------------------------------------
rmexeo :
	/bin/rm -f $(bin)/oshow.o
	/bin/rm -f $(bin)/onew.o
	/bin/rm -f $(bin)/oiv.o
	/bin/rm -f $(bin)/oshWo.o
	/bin/rm -f $(bin)/WoT.o
	/bin/rm -f $(bin)/CiT.o
	/bin/rm -f $(bin)/NoHT.o
	/bin/rm -f $(bin)/tclT.o
	/bin/rm -f $(bin)/ouiToodb.o
	/bin/rm -f $(bin)/ovoid.o
	/bin/rm -f $(bin)/wof77t.o
#--------------------------------------------
oshow : $(bin)/oshow.exe
	@echo "Wo : oshow ok."
onew : $(bin)/onew.exe
	@echo "Wo : onew ok."
oiv : $(bin)/oiv.exe
	@echo "Wo : oiv ok."
oshWo : $(bin)/oshWo.exe
	@echo "Wo : oshWo ok."
WoT : $(bin)/WoT.exe
	@echo "Wo : WoT ok."
CiT : $(bin)/CiT.exe
	@echo "Wo : CiT ok."
NoHT : $(bin)/NoHT.exe
	@echo "Wo : NoHT ok."
tclT : $(bin)/tclT.exe
	@echo "Wo : tclT ok."
ouiToodb : $(bin)/ouiToodb.exe
	@echo "Wo : ouiToodb ok."
ovoid : $(bin)/ovoid.exe
	@echo "Wo : ovoid ok."
jo : $(bin)/jo.class
	@echo "Wo : jo ok."
jolet : $(bin)/jolet.class
	@echo "Wo : jolet ok."
JavaT : $(bin)/JavaT.class
	@echo "Wo : JavaT ok."
wof77t : $(bin)/wof77t.exe
	@echo "Wo : wof77t ok."
libWo : $(libWo_target)
	@echo "Wo : libWo ok."
libWoXm : $(libWoXm_target)
	@echo "Wo : libWoXm ok."
libWoGL : $(libWoGL_target)
	@echo "Wo : libWoGL ok."
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#--------------------------------------------
$(bin)/oshow.exe : $(bin)/oshow.o \
$(libWoGL_target) \
$(libWoXm_target) \
$(libWo_target) 
	$(CCLD) $(CCLDFLAGS) -o $(bin)/oshow.exe $(bin)/oshow.o \
$(libWoGL) \
$(libXo) \
$(libhtmlw) \
$(libWoXm) \
$(libXm) \
$(libWo) \
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
$(libwww) \
$(libm) \
$(CCLDEND) 

$(bin)/oshow.o   : $(app)/oshow.c
	$(CC) $(CFLAGS) $(APP_CCFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/oshow.o $(app)/oshow.c 
#--------------------------------------------
$(bin)/onew.exe : $(bin)/onew.o \
$(libWoGL_target) \
$(libWoXm_target) \
$(libWo_target) 
	$(CCLD) $(CCLDFLAGS) -o $(bin)/onew.exe $(bin)/onew.o \
$(libWoGL) \
$(libXo) \
$(libhtmlw) \
$(libWoXm) \
$(libXm) \
$(libWo) \
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
$(libwww) \
$(libm) \
$(CCLDEND) 

$(bin)/onew.o   : $(app)/onew.c
	$(CC) $(CFLAGS) $(APP_CCFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/onew.o $(app)/onew.c 
#--------------------------------------------
$(bin)/oiv.exe : $(bin)/oiv.o \
$(libWoGL_target) \
$(libWoXm_target) \
$(libWo_target) 
	$(CXXLD) $(CXXLDFLAGS) $(HAS_CPPFLAGS) -o $(bin)/oiv.exe $(bin)/oiv.o \
$(libWoGL) \
$(libXo) \
$(libhtmlw) \
$(libWoXm) \
$(libXm) \
$(libWo) \
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
$(libwww) \
$(libm) \
$(CXXLDEND) 

$(bin)/oiv.o   : $(app)/oiv.cxx
	$(CXX) $(CXXFLAGS) $(APP_CXXFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/oiv.o $(app)/oiv.cxx
#--------------------------------------------
$(bin)/oshWo.exe : $(bin)/oshWo.o \
$(libWoGL_target) \
$(libWoXm_target) \
$(libWo_target) 
	$(CCLD) $(CCLDFLAGS) -o $(bin)/oshWo.exe $(bin)/oshWo.o \
$(libWoGL) \
$(libXo) \
$(libhtmlw) \
$(libWoXm) \
$(libXm) \
$(libWo) \
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
$(libwww) \
$(libm) \
$(CCLDEND) 

$(bin)/oshWo.o   : $(app)/oshWo.c
	$(CC) $(CFLAGS) $(APP_CCFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/oshWo.o $(app)/oshWo.c 
#--------------------------------------------
$(bin)/WoT.exe : $(bin)/WoT.o \
$(libWoGL_target) \
$(libWoXm_target) \
$(libWo_target) 
	$(CCLD) $(CCLDFLAGS) -o $(bin)/WoT.exe $(bin)/WoT.o \
$(libWoGL) \
$(libXo) \
$(libhtmlw) \
$(libWoXm) \
$(libXm) \
$(libWo) \
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
$(libwww) \
$(libm) \
$(CCLDEND) 

$(bin)/WoT.o   : $(app)/WoT.c
	$(CC) $(CFLAGS) $(APP_CCFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/WoT.o $(app)/WoT.c 
#--------------------------------------------
$(bin)/CiT.exe : $(bin)/CiT.o 
	$(CCLD) $(CCLDFLAGS) -o $(bin)/CiT.exe $(bin)/CiT.o \
$(libGo) \
$(libCo) \
$(libm) \
$(CCLDEND) 

$(bin)/CiT.o   : $(app)/CiT.c
	$(CC) $(CFLAGS) $(APP_CCFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/CiT.o $(app)/CiT.c 
#--------------------------------------------
$(bin)/NoHT.exe : $(bin)/NoHT.o 
	$(CCLD) $(CCLDFLAGS) -o $(bin)/NoHT.exe $(bin)/NoHT.o \
$(CCLDEND) 

$(bin)/NoHT.o   : $(app)/NoHT.c
	$(CC) $(CFLAGS) $(APP_CCFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/NoHT.o $(app)/NoHT.c 
#--------------------------------------------
$(bin)/tclT.exe : $(bin)/tclT.o 
	$(CCLD) $(CCLDFLAGS) -o $(bin)/tclT.exe $(bin)/tclT.o \
$(libCo) \
$(libm) \
$(CCLDEND) 

$(bin)/tclT.o   : $(app)/tclT.c
	$(CC) $(CFLAGS) $(APP_CCFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/tclT.o $(app)/tclT.c 
#--------------------------------------------
$(bin)/ouiToodb.exe : $(bin)/ouiToodb.o \
$(libWo_target) 
	$(CCLD) $(CCLDFLAGS) -o $(bin)/ouiToodb.exe $(bin)/ouiToodb.o \
$(libWo) \
$(libXx) \
$(libXmu) \
$(libXt) \
$(libXext) \
$(libX11) \
$(libCo) \
$(libm) \
$(CCLDEND) 

$(bin)/ouiToodb.o   : $(app)/ouiToodb.c
	$(CC) $(CFLAGS) $(APP_CCFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/ouiToodb.o $(app)/ouiToodb.c 
#--------------------------------------------
$(bin)/ovoid.exe : $(bin)/ovoid.o \
$(libWo_target) 
	$(CCLD) $(CCLDFLAGS) -o $(bin)/ovoid.exe $(bin)/ovoid.o \
$(libWo) \
$(libXx) \
$(libXmu) \
$(libXt) \
$(libXext) \
$(libX11) \
$(libCo) \
$(libm) \
$(CCLDEND) 

$(bin)/ovoid.o   : $(app)/ovoid.c
	$(CC) $(CFLAGS) $(APP_CCFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/ovoid.o $(app)/ovoid.c 
#--------------------------------------------
$(bin)/jo.class : $(app)/jo.java
	javac $(app)/jo.java
	/bin/mv $(app)/jo.class $(bin)/.
#--------------------------------------------
$(bin)/jolet.class : $(app)/jolet.java
	javac $(app)/jolet.java
	/bin/mv $(app)/jolet.class $(bin)/.
#--------------------------------------------
$(bin)/JavaT.class : $(app)/JavaT.java
	javac $(app)/JavaT.java
	/bin/mv $(app)/JavaT.class $(bin)/.
#--------------------------------------------
$(bin)/wof77t.exe : $(bin)/wof77t.o \
$(bin)/woinit.o \
$(libWoGL_target) \
$(libWoXm_target) \
$(libWo_target) 
	$(F77LD) $(F77LDFLAGS) -o $(bin)/wof77t.exe $(bin)/wof77t.o \
$(bin)/woinit.o \
$(libWoGL) \
$(libXo) \
$(libhtmlw) \
$(libWoXm) \
$(libXm) \
$(libWo) \
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
$(libwww) \
$(libm) \
$(F77LDEND) 

$(bin)/wof77t.o   : $(app)/wof77t.f
	$(F77) $(F77FLAGS) -c -o $(bin)/wof77t.o $(app)/wof77t.f
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
# libraries 
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
.PRECIOUS : $(bin)/libWo.a $(bin)/libWoXm.a $(bin)/libWoGL.a 
#--------------------------------------------
$(bin)/libWo.$(so) : $(bin)/libWo.a 
	$(make_so) libWo $(bin) "\
$(libXx) \
$(libXmu) \
$(libXt) \
$(libXext) \
$(libX11) \
$(libCo) \
$(libm) \
$(libc) \
"

$(bin)/libWo.a : \
$(bin)/libWo.a(WoF77.o) \
$(bin)/libWo.a(WoCommands.o) \
$(bin)/libWo.a(WoTypes.o) \
$(bin)/libWo.a(Wo.o) \
$(bin)/libWo.a(OInterface.o) \
$(bin)/libWo.a(OPiece.o) \
$(bin)/libWo.a(OWidget.o) \
$(bin)/libWo.a(OClass.o) \
$(bin)/libWo.a(OPart.o) \
$(bin)/libWo.a(OInterpreter.o) \
$(bin)/libWo.a(OCyclic.o) \
$(bin)/libWo.a(XtwTools.o) \
$(bin)/libWo.a(OResource.o) \
$(bin)/libWo.a(OEvent.o) \
$(bin)/libWo.a(OTrace.o) \
$(bin)/libWo.a(WoBinds.o) \
$(bin)/libWo.a(X11Binds.o) 
	if [ -f /bin/ranlib ] ; then /bin/ranlib $(bin)/libWo.a ; fi

$(bin)/libWoXm.$(so) : $(bin)/libWoXm.a \
$(libWo_target) 
	$(make_so) libWoXm $(bin) "\
$(libXm) \
$(libXmu) \
$(libXt) \
$(libXext) \
$(libX11) \
$(libWo) \
$(libXx) \
$(libCo) \
$(libm) \
$(libc) \
"

$(bin)/libWoXm.a : \
$(bin)/libWoXm.a(XmTools.o) \
$(bin)/libWoXm.a(XmCommands.o) \
$(bin)/libWoXm.a(XmBinds.o) \
$(libWo_target) 
	if [ -f /bin/ranlib ] ; then /bin/ranlib $(bin)/libWoXm.a ; fi

$(bin)/libWoGL.$(so) : $(bin)/libWoGL.a 
	$(make_so) libWoGL $(bin) "\
$(libGL) \
$(libGLU) \
$(libGLX) \
$(libX11) \
$(libCo) \
$(libc) \
"

$(bin)/libWoGL.a : \
$(bin)/libWoGL.a(GL_Binds.o) 
	if [ -f /bin/ranlib ] ; then /bin/ranlib $(bin)/libWoGL.a ; fi

#--------------------------------------------
# libWo dependencies 
#--------------------------------------------
$(bin)/libWo.a(WoF77.o) : $(src)/WoF77.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/WoF77.o $(src)/WoF77.c
	ar cr $(bin)/libWo.a $(bin)/WoF77.o
	/bin/rm -f $(bin)/WoF77.o

$(bin)/libWo.a(WoCommands.o) : $(src)/WoCommands.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/WoCommands.o $(src)/WoCommands.c
	ar cr $(bin)/libWo.a $(bin)/WoCommands.o
	/bin/rm -f $(bin)/WoCommands.o

$(bin)/libWo.a(WoTypes.o) : $(src)/WoTypes.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/WoTypes.o $(src)/WoTypes.c
	ar cr $(bin)/libWo.a $(bin)/WoTypes.o
	/bin/rm -f $(bin)/WoTypes.o

$(bin)/libWo.a(Wo.o) : $(src)/Wo.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/Wo.o $(src)/Wo.c
	ar cr $(bin)/libWo.a $(bin)/Wo.o
	/bin/rm -f $(bin)/Wo.o

$(bin)/libWo.a(OInterface.o) : $(src)/OInterface.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/OInterface.o $(src)/OInterface.c
	ar cr $(bin)/libWo.a $(bin)/OInterface.o
	/bin/rm -f $(bin)/OInterface.o

$(bin)/libWo.a(OPiece.o) : $(src)/OPiece.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/OPiece.o $(src)/OPiece.c
	ar cr $(bin)/libWo.a $(bin)/OPiece.o
	/bin/rm -f $(bin)/OPiece.o

$(bin)/libWo.a(OWidget.o) : $(src)/OWidget.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/OWidget.o $(src)/OWidget.c
	ar cr $(bin)/libWo.a $(bin)/OWidget.o
	/bin/rm -f $(bin)/OWidget.o

$(bin)/libWo.a(OClass.o) : $(src)/OClass.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/OClass.o $(src)/OClass.c
	ar cr $(bin)/libWo.a $(bin)/OClass.o
	/bin/rm -f $(bin)/OClass.o

$(bin)/libWo.a(OPart.o) : $(src)/OPart.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/OPart.o $(src)/OPart.c
	ar cr $(bin)/libWo.a $(bin)/OPart.o
	/bin/rm -f $(bin)/OPart.o

$(bin)/libWo.a(OInterpreter.o) : $(src)/OInterpreter.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/OInterpreter.o $(src)/OInterpreter.c
	ar cr $(bin)/libWo.a $(bin)/OInterpreter.o
	/bin/rm -f $(bin)/OInterpreter.o

$(bin)/libWo.a(OCyclic.o) : $(src)/OCyclic.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/OCyclic.o $(src)/OCyclic.c
	ar cr $(bin)/libWo.a $(bin)/OCyclic.o
	/bin/rm -f $(bin)/OCyclic.o

$(bin)/libWo.a(XtwTools.o) : $(src)/XtwTools.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/XtwTools.o $(src)/XtwTools.c
	ar cr $(bin)/libWo.a $(bin)/XtwTools.o
	/bin/rm -f $(bin)/XtwTools.o

$(bin)/libWo.a(OResource.o) : $(src)/OResource.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/OResource.o $(src)/OResource.c
	ar cr $(bin)/libWo.a $(bin)/OResource.o
	/bin/rm -f $(bin)/OResource.o

$(bin)/libWo.a(OEvent.o) : $(src)/OEvent.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/OEvent.o $(src)/OEvent.c
	ar cr $(bin)/libWo.a $(bin)/OEvent.o
	/bin/rm -f $(bin)/OEvent.o

$(bin)/libWo.a(OTrace.o) : $(src)/OTrace.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/OTrace.o $(src)/OTrace.c
	ar cr $(bin)/libWo.a $(bin)/OTrace.o
	/bin/rm -f $(bin)/OTrace.o

$(bin)/libWo.a(WoBinds.o) : $(src)/WoBinds.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/WoBinds.o $(src)/WoBinds.c
	ar cr $(bin)/libWo.a $(bin)/WoBinds.o
	/bin/rm -f $(bin)/WoBinds.o

$(bin)/libWo.a(X11Binds.o) : $(src)/X11Binds.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/X11Binds.o $(src)/X11Binds.c
	ar cr $(bin)/libWo.a $(bin)/X11Binds.o
	/bin/rm -f $(bin)/X11Binds.o

#--------------------------------------------
# libWoXm dependencies 
#--------------------------------------------
$(bin)/libWoXm.a(XmTools.o) : $(src)/XmTools.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/XmTools.o $(src)/XmTools.c
	ar cr $(bin)/libWoXm.a $(bin)/XmTools.o
	/bin/rm -f $(bin)/XmTools.o

$(bin)/libWoXm.a(XmCommands.o) : $(src)/XmCommands.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/XmCommands.o $(src)/XmCommands.c
	ar cr $(bin)/libWoXm.a $(bin)/XmCommands.o
	/bin/rm -f $(bin)/XmCommands.o

$(bin)/libWoXm.a(XmBinds.o) : $(src)/XmBinds.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/XmBinds.o $(src)/XmBinds.c
	ar cr $(bin)/libWoXm.a $(bin)/XmBinds.o
	/bin/rm -f $(bin)/XmBinds.o

#--------------------------------------------
# libWoGL dependencies 
#--------------------------------------------
$(bin)/libWoGL.a(GL_Binds.o) : $(src)/GL_Binds.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/GL_Binds.o $(src)/GL_Binds.c
	ar cr $(bin)/libWoGL.a $(bin)/GL_Binds.o
	/bin/rm -f $(bin)/GL_Binds.o

$(bin)/woinit.o : $(app)/woinit.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/woinit.o $(app)/woinit.c


