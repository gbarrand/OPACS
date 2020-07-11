#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#  File produced by the Co/omake tool
# using file Make.odb.
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#  The soft linked packages has been taken
# from the property 'has' of the 
# 'Make' object of file Make.odb.
# It had the value :
#    X11 Xext GL
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
HAS_CPPFLAGS = \
 $(Go_CPPFLAGS)\
 $(X11_CPPFLAGS)\
 $(Xext_CPPFLAGS)\
 $(GL_CPPFLAGS)
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

libGo_target = $(bin)/libGo.$(a_or_so)
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
all :: mkdir \
$(libGo_target) \
$(bin)/GoT.exe 
	@echo "Go : all ok." 

libs :: mkdir \
$(libGo_target) 
	@echo "Go : libs ok." 

apps :: mkdir \
$(bin)/GoT.exe \
$(bin)/oshGo.exe \
$(bin)/o3dToodb.exe \
$(bin)/ograd.exe \
$(bin)/NoHT.exe 
	@echo "Go : apps ok." 
#--------------------------------------------
rmexeo :
	/bin/rm -f $(bin)/GoT.o
	/bin/rm -f $(bin)/oshGo.o
	/bin/rm -f $(bin)/o3dToodb.o
	/bin/rm -f $(bin)/ograd.o
	/bin/rm -f $(bin)/NoHT.o
#--------------------------------------------
GoT : $(bin)/GoT.exe
	@echo "Go : GoT ok."
oshGo : $(bin)/oshGo.exe
	@echo "Go : oshGo ok."
o3dToodb : $(bin)/o3dToodb.exe
	@echo "Go : o3dToodb ok."
ograd : $(bin)/ograd.exe
	@echo "Go : ograd ok."
NoHT : $(bin)/NoHT.exe
	@echo "Go : NoHT ok."
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#--------------------------------------------
$(bin)/GoT.exe : $(bin)/GoT.o \
$(libGo_target) 
	$(CCLD) $(CCLDFLAGS) -o $(bin)/GoT.exe $(bin)/GoT.o \
$(libGo) \
$(libGLX) \
$(libGLU) \
$(libGL) \
$(libXext) \
$(libX11) \
$(libCo) \
$(libm) \
$(CCLDEND) 

$(bin)/GoT.o   : $(app)/GoT.c
	$(CC) $(CFLAGS) $(APP_CCFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/GoT.o $(app)/GoT.c 
#--------------------------------------------
$(bin)/oshGo.exe : $(bin)/oshGo.o \
$(libGo_target) 
	$(CCLD) $(CCLDFLAGS) -o $(bin)/oshGo.exe $(bin)/oshGo.o \
$(libGo) \
$(libGLU) \
$(libGL) \
$(libXext) \
$(libX11) \
$(libCo) \
$(libm) \
$(CCLDEND) 

$(bin)/oshGo.o   : $(app)/oshGo.c
	$(CC) $(CFLAGS) $(APP_CCFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/oshGo.o $(app)/oshGo.c 
#--------------------------------------------
$(bin)/o3dToodb.exe : $(bin)/o3dToodb.o \
$(libGo_target) 
	$(CCLD) $(CCLDFLAGS) -o $(bin)/o3dToodb.exe $(bin)/o3dToodb.o \
$(libGo) \
$(libGLU) \
$(libGL) \
$(libXext) \
$(libX11) \
$(libCo) \
$(libm) \
$(CCLDEND) 

$(bin)/o3dToodb.o   : $(app)/o3dToodb.c
	$(CC) $(CFLAGS) $(APP_CCFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/o3dToodb.o $(app)/o3dToodb.c 
#--------------------------------------------
$(bin)/ograd.exe : $(bin)/ograd.o 
	$(CCLD) $(CCLDFLAGS) -o $(bin)/ograd.exe $(bin)/ograd.o \
$(libCo) \
$(libm) \
$(CCLDEND) 

$(bin)/ograd.o   : $(app)/ograd.c
	$(CC) $(CFLAGS) $(APP_CCFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/ograd.o $(app)/ograd.c 
#--------------------------------------------
$(bin)/NoHT.exe : $(bin)/NoHT.o 
	$(CCLD) $(CCLDFLAGS) -o $(bin)/NoHT.exe $(bin)/NoHT.o \
$(CCLDEND) 

$(bin)/NoHT.o   : $(app)/NoHT.c
	$(CC) $(CFLAGS) $(APP_CCFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/NoHT.o $(app)/NoHT.c 
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
# libraries 
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
.PRECIOUS : $(bin)/libGo.a 
#--------------------------------------------
$(bin)/libGo.$(so) : $(bin)/libGo.a 
	$(make_so) libGo $(bin) "\
$(libGLU) \
$(libGL) \
$(libCo) \
$(libm) \
$(libc) \
"

$(bin)/libGo.a : \
$(bin)/libGo.a(GoCommands.o) \
$(bin)/libGo.a(GoTypes.o) \
$(bin)/libGo.a(GoF77.o) \
$(bin)/libGo.a(OColormap.o) \
$(bin)/libGo.a(OCamera.o) \
$(bin)/libGo.a(OCamVRML.o) \
$(bin)/libGo.a(OCamDAWN.o) \
$(bin)/libGo.a(OCamPOV.o) \
$(bin)/libGo.a(OPick.o) \
$(bin)/libGo.a(OPointList.o) \
$(bin)/libGo.a(Go.o) \
$(bin)/libGo.a(OShape.o) \
$(bin)/libGo.a(ONode.o) \
$(bin)/libGo.a(OPrimitive.o) \
$(bin)/libGo.a(OContextmap.o) \
$(bin)/libGo.a(OContext.o) \
$(bin)/libGo.a(GoBinds.o) 
	@cd $(bin) ; if [ -f /bin/ranlib ] ; then /bin/ranlib libGo.a ; fi ; cd $(mgr)
#--------------------------------------------
# libGo dependencies 
#--------------------------------------------
$(bin)/libGo.a(GoCommands.o) : $(src)/GoCommands.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/GoCommands.o $(src)/GoCommands.c
	ar cr $(bin)/libGo.a $(bin)/GoCommands.o
	/bin/rm -f $(bin)/GoCommands.o

$(bin)/libGo.a(GoTypes.o) : $(src)/GoTypes.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/GoTypes.o $(src)/GoTypes.c
	ar cr $(bin)/libGo.a $(bin)/GoTypes.o
	/bin/rm -f $(bin)/GoTypes.o

$(bin)/libGo.a(GoF77.o) : $(src)/GoF77.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/GoF77.o $(src)/GoF77.c
	ar cr $(bin)/libGo.a $(bin)/GoF77.o
	/bin/rm -f $(bin)/GoF77.o

$(bin)/libGo.a(OColormap.o) : $(src)/OColormap.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/OColormap.o $(src)/OColormap.c
	ar cr $(bin)/libGo.a $(bin)/OColormap.o
	/bin/rm -f $(bin)/OColormap.o

$(bin)/libGo.a(OCamera.o) : $(src)/OCamera.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/OCamera.o $(src)/OCamera.c
	ar cr $(bin)/libGo.a $(bin)/OCamera.o
	/bin/rm -f $(bin)/OCamera.o

$(bin)/libGo.a(OCamVRML.o) : $(src)/OCamVRML.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/OCamVRML.o $(src)/OCamVRML.c
	ar cr $(bin)/libGo.a $(bin)/OCamVRML.o
	/bin/rm -f $(bin)/OCamVRML.o

$(bin)/libGo.a(OCamDAWN.o) : $(src)/OCamDAWN.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/OCamDAWN.o $(src)/OCamDAWN.c
	ar cr $(bin)/libGo.a $(bin)/OCamDAWN.o
	/bin/rm -f $(bin)/OCamDAWN.o

$(bin)/libGo.a(OCamPOV.o) : $(src)/OCamPOV.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/OCamPOV.o $(src)/OCamPOV.c
	ar cr $(bin)/libGo.a $(bin)/OCamPOV.o
	/bin/rm -f $(bin)/OCamPOV.o

$(bin)/libGo.a(OPick.o) : $(src)/OPick.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/OPick.o $(src)/OPick.c
	ar cr $(bin)/libGo.a $(bin)/OPick.o
	/bin/rm -f $(bin)/OPick.o

$(bin)/libGo.a(OPointList.o) : $(src)/OPointList.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/OPointList.o $(src)/OPointList.c
	ar cr $(bin)/libGo.a $(bin)/OPointList.o
	/bin/rm -f $(bin)/OPointList.o

$(bin)/libGo.a(Go.o) : $(src)/Go.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/Go.o $(src)/Go.c
	ar cr $(bin)/libGo.a $(bin)/Go.o
	/bin/rm -f $(bin)/Go.o

$(bin)/libGo.a(OShape.o) : $(src)/OShape.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/OShape.o $(src)/OShape.c
	ar cr $(bin)/libGo.a $(bin)/OShape.o
	/bin/rm -f $(bin)/OShape.o

$(bin)/libGo.a(ONode.o) : $(src)/ONode.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/ONode.o $(src)/ONode.c
	ar cr $(bin)/libGo.a $(bin)/ONode.o
	/bin/rm -f $(bin)/ONode.o

$(bin)/libGo.a(OPrimitive.o) : $(src)/OPrimitive.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/OPrimitive.o $(src)/OPrimitive.c
	ar cr $(bin)/libGo.a $(bin)/OPrimitive.o
	/bin/rm -f $(bin)/OPrimitive.o

$(bin)/libGo.a(OContextmap.o) : $(src)/OContextmap.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/OContextmap.o $(src)/OContextmap.c
	ar cr $(bin)/libGo.a $(bin)/OContextmap.o
	/bin/rm -f $(bin)/OContextmap.o

$(bin)/libGo.a(OContext.o) : $(src)/OContext.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/OContext.o $(src)/OContext.c
	ar cr $(bin)/libGo.a $(bin)/OContext.o
	/bin/rm -f $(bin)/OContext.o

$(bin)/libGo.a(GoBinds.o) : $(src)/GoBinds.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/GoBinds.o $(src)/GoBinds.c
	ar cr $(bin)/libGo.a $(bin)/GoBinds.o
	/bin/rm -f $(bin)/GoBinds.o


