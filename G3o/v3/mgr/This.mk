#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#  File produced by the Co/omake tool
# using file Make.odb.
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#  The soft linked packages has been taken
# from the property 'has' of the 
# 'Make' object of file Make.odb.
# It had the value :
#    GL
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
HAS_CPPFLAGS = \
 $(G3o_CPPFLAGS)\
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

libG3oEXPO_target = $(bin)/libG3oEXPO.a
libG3o_target = $(bin)/libG3o.a
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
all :: mkdir \
$(libG3oEXPO_target) \
$(libG3o_target) \
$(bin)/G3oT.exe 
	@echo "G3o : all ok." 

libs :: mkdir \
$(libG3oEXPO_target) \
$(libG3o_target) 
	@echo "G3o : libs ok." 

apps :: mkdir \
$(bin)/G3oT.exe \
$(bin)/geantT.exe \
$(bin)/NoHT.exe 
	@echo "G3o : apps ok." 
#--------------------------------------------
rmexeo :
	/bin/rm -f $(bin)/G3oT.o
	/bin/rm -f $(bin)/geantT.o
	/bin/rm -f $(bin)/NoHT.o
#--------------------------------------------
G3oT : $(bin)/G3oT.exe
	@echo "G3o : G3oT ok."
geantT : $(bin)/geantT.exe
	@echo "G3o : geantT ok."
NoHT : $(bin)/NoHT.exe
	@echo "G3o : NoHT ok."
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#--------------------------------------------
$(bin)/G3oT.exe : $(bin)/G3oT.o \
$(bin)/idummy.o \
$(libG3oEXPO_target) \
$(libG3o_target) 
	$(CCLD) $(CCLDFLAGS) -o $(bin)/G3oT.exe $(bin)/G3oT.o \
$(bin)/idummy.o \
$(libG3oEXPO) \
$(libG3o) \
$(libgeant) \
$(libpacklib) \
$(libkernlib) \
$(libf77) \
$(libGo) \
$(libGLU) \
$(libGL) \
$(libCo) \
$(libm) \
$(CCLDEND) 

$(bin)/G3oT.o   : $(app)/G3oT.c
	$(CC) $(CFLAGS) $(APP_CCFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/G3oT.o $(app)/G3oT.c 
#--------------------------------------------
$(bin)/geantT.exe : $(bin)/geantT.o \
$(libG3oEXPO_target) 
	$(F77LD) $(F77LDFLAGS) -o $(bin)/geantT.exe $(bin)/geantT.o \
$(libG3oEXPO) \
$(libgeant) \
$(libpacklib) \
$(libkernlib) \
$(F77LDEND) 

$(bin)/geantT.o   : $(app)/geantT.f
	$(F77) $(F77FLAGS) -c -o $(bin)/geantT.o $(app)/geantT.f
#--------------------------------------------
$(bin)/NoHT.exe : $(bin)/NoHT.o 
	$(CCLD) $(CCLDFLAGS) -o $(bin)/NoHT.exe $(bin)/NoHT.o \
$(CCLDEND) 

$(bin)/NoHT.o   : $(app)/NoHT.c
	$(CC) $(CFLAGS) $(APP_CCFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/NoHT.o $(app)/NoHT.c 
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
# libraries 
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
.PRECIOUS : $(bin)/libG3oEXPO.a $(bin)/libG3o.a 
#--------------------------------------------

$(bin)/libG3oEXPO.a : \
$(bin)/libG3oEXPO.a(G3oEXPO.o) \
$(bin)/libG3oEXPO.a(G3oEXPO_F77.o) 
	@cd $(bin) ; if [ -f /bin/ranlib ] ; then /bin/ranlib libG3oEXPO.a ; fi ; cd $(mgr)

$(bin)/libG3o.a : \
$(bin)/libG3o.a(G3oCommands.o) \
$(bin)/libG3o.a(G3oTypes.o) \
$(bin)/libG3o.a(GEANT.o) \
$(bin)/libG3o.a(ogeant.o) \
$(bin)/libG3o.a(G3oBinds.o) 
	@cd $(bin) ; if [ -f /bin/ranlib ] ; then /bin/ranlib libG3o.a ; fi ; cd $(mgr)
#--------------------------------------------
# libG3oEXPO dependencies 
#--------------------------------------------
$(bin)/libG3oEXPO.a(G3oEXPO.o) : $(src)/G3oEXPO.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/G3oEXPO.o $(src)/G3oEXPO.c
	ar cr $(bin)/libG3oEXPO.a $(bin)/G3oEXPO.o
	/bin/rm -f $(bin)/G3oEXPO.o

$(bin)/libG3oEXPO.a(G3oEXPO_F77.o) : $(src)/G3oEXPO_F77.f 
	$(F77) $(F77FLAGS) -c -o $(bin)/G3oEXPO_F77.o $(src)/G3oEXPO_F77.f
	ar cr $(bin)/libG3oEXPO.a $(bin)/G3oEXPO_F77.o
	/bin/rm -f $(bin)/G3oEXPO_F77.o

#--------------------------------------------
# libG3o dependencies 
#--------------------------------------------
$(bin)/libG3o.a(G3oCommands.o) : $(src)/G3oCommands.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/G3oCommands.o $(src)/G3oCommands.c
	ar cr $(bin)/libG3o.a $(bin)/G3oCommands.o
	/bin/rm -f $(bin)/G3oCommands.o

$(bin)/libG3o.a(G3oTypes.o) : $(src)/G3oTypes.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/G3oTypes.o $(src)/G3oTypes.c
	ar cr $(bin)/libG3o.a $(bin)/G3oTypes.o
	/bin/rm -f $(bin)/G3oTypes.o

$(bin)/libG3o.a(GEANT.o) : $(src)/GEANT.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/GEANT.o $(src)/GEANT.c
	ar cr $(bin)/libG3o.a $(bin)/GEANT.o
	/bin/rm -f $(bin)/GEANT.o

$(bin)/libG3o.a(ogeant.o) : $(src)/ogeant.f 
	$(F77) $(F77FLAGS) -c -o $(bin)/ogeant.o $(src)/ogeant.f
	ar cr $(bin)/libG3o.a $(bin)/ogeant.o
	/bin/rm -f $(bin)/ogeant.o

$(bin)/libG3o.a(G3oBinds.o) : $(src)/G3oBinds.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/G3oBinds.o $(src)/G3oBinds.c
	ar cr $(bin)/libG3o.a $(bin)/G3oBinds.o
	/bin/rm -f $(bin)/G3oBinds.o

$(bin)/idummy.o : $(app)/idummy.f 
	$(F77) $(F77FLAGS) -c -o $(bin)/idummy.o $(app)/idummy.f


