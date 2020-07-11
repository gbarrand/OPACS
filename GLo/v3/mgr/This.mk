#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#  File produced by the Co/omake tool
# using file Make.odb.
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#  The soft linked packages has been taken
# from the property 'has' of the 
# 'Make' object of file Make.odb.
# It had the value :
#    X11
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
HAS_CPPFLAGS = \
 $(GLo_CPPFLAGS)\
 $(X11_CPPFLAGS)
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

libGLo_target = $(bin)/libGLo.$(a_or_so)
libGLUo_target = $(bin)/libGLUo.$(a_or_so)
libGLXo_target = $(bin)/libGLXo.$(a_or_so)
libGLPSo_target = $(bin)/libGLPSo.$(a_or_so)
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
all :: mkdir \
$(libGLo_target) \
$(libGLUo_target) \
$(libGLXo_target) \
$(libGLPSo_target) \
$(bin)/glXT.exe \
$(bin)/glPST.exe 
	@echo "GLo : all ok." 

libs :: mkdir \
$(libGLo_target) \
$(libGLUo_target) \
$(libGLXo_target) \
$(libGLPSo_target) 
	@echo "GLo : libs ok." 

apps :: mkdir \
$(bin)/glXT.exe \
$(bin)/glPST.exe \
$(bin)/NoHT.exe 
	@echo "GLo : apps ok." 
#--------------------------------------------
rmexeo :
	/bin/rm -f $(bin)/glXT.o
	/bin/rm -f $(bin)/glPST.o
	/bin/rm -f $(bin)/NoHT.o
#--------------------------------------------
glXT : $(bin)/glXT.exe
	@echo "GLo : glXT ok."
glPST : $(bin)/glPST.exe
	@echo "GLo : glPST ok."
NoHT : $(bin)/NoHT.exe
	@echo "GLo : NoHT ok."
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#--------------------------------------------
$(bin)/glXT.exe : $(bin)/glXT.o \
$(libGLXo_target) \
$(libGLUo_target) \
$(libGLo_target) 
	$(CCLD) $(CCLDFLAGS) -o $(bin)/glXT.exe $(bin)/glXT.o \
$(libGLXo) \
$(libGLUo) \
$(libGLo) \
$(libCo) \
$(libX11) \
$(libm) \
$(CCLDEND) 

$(bin)/glXT.o   : $(app)/glXT.c
	$(CC) $(CFLAGS) $(APP_CCFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/glXT.o $(app)/glXT.c 
#--------------------------------------------
$(bin)/glPST.exe : $(bin)/glPST.o \
$(libGLPSo_target) \
$(libGLUo_target) \
$(libGLo_target) 
	$(CCLD) $(CCLDFLAGS) -o $(bin)/glPST.exe $(bin)/glPST.o \
$(libGLPSo) \
$(libGLUo) \
$(libGLo) \
$(libCo) \
$(libm) \
$(CCLDEND) 

$(bin)/glPST.o   : $(app)/glPST.c
	$(CC) $(CFLAGS) $(APP_CCFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/glPST.o $(app)/glPST.c 
#--------------------------------------------
$(bin)/NoHT.exe : $(bin)/NoHT.o 
	$(CCLD) $(CCLDFLAGS) -o $(bin)/NoHT.exe $(bin)/NoHT.o \
$(CCLDEND) 

$(bin)/NoHT.o   : $(app)/NoHT.c
	$(CC) $(CFLAGS) $(APP_CCFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/NoHT.o $(app)/NoHT.c 
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
# libraries 
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
.PRECIOUS : $(bin)/libGLo.a $(bin)/libGLUo.a $(bin)/libGLXo.a $(bin)/libGLPSo.a 
#--------------------------------------------
$(bin)/libGLo.$(so) : $(bin)/libGLo.a 
	$(make_so) libGLo $(bin) "\
$(libCo) \
$(libm) \
$(libc) \
"

$(bin)/libGLo.a : \
$(bin)/libGLo.a(gl.o) \
$(bin)/libGLo.a(glList.o) \
$(bin)/libGLo.a(glDriverZ.o) \
$(bin)/libGLo.a(ZBuffer.o) \
$(bin)/libGLo.a(ZPolygon.o) 
	@cd $(bin) ; if [ -f /bin/ranlib ] ; then /bin/ranlib libGLo.a ; fi ; cd $(mgr)
$(bin)/libGLUo.$(so) : $(bin)/libGLUo.a \
$(libGLo_target) 
	$(make_so) libGLUo $(bin) "\
$(libGLo) \
$(libCo) \
$(libm) \
$(libc) \
"

$(bin)/libGLUo.a : \
$(bin)/libGLUo.a(glu.o) \
$(libGLo_target) 
	@cd $(bin) ; if [ -f /bin/ranlib ] ; then /bin/ranlib libGLUo.a ; fi ; cd $(mgr)
$(bin)/libGLXo.$(so) : $(bin)/libGLXo.a \
$(libGLo_target) 
	$(make_so) libGLXo $(bin) "\
$(libGLo) \
$(libCo) \
$(libX11) \
$(libm) \
$(libc) \
"

$(bin)/libGLXo.a : \
$(bin)/libGLXo.a(glx.o) \
$(bin)/libGLXo.a(glDriverX.o) \
$(libGLo_target) 
	@cd $(bin) ; if [ -f /bin/ranlib ] ; then /bin/ranlib libGLXo.a ; fi ; cd $(mgr)
$(bin)/libGLPSo.$(so) : $(bin)/libGLPSo.a \
$(libGLo_target) 
	$(make_so) libGLPSo $(bin) "\
$(libGLo) \
$(libCo) \
$(libm) \
$(libc) \
"

$(bin)/libGLPSo.a : \
$(bin)/libGLPSo.a(glPS.o) \
$(bin)/libGLPSo.a(glDriverPS.o) \
$(libGLo_target) 
	@cd $(bin) ; if [ -f /bin/ranlib ] ; then /bin/ranlib libGLPSo.a ; fi ; cd $(mgr)
#--------------------------------------------
# libGLo dependencies 
#--------------------------------------------
$(bin)/libGLo.a(gl.o) : $(src)/gl.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/gl.o $(src)/gl.c
	ar cr $(bin)/libGLo.a $(bin)/gl.o
	/bin/rm -f $(bin)/gl.o

$(bin)/libGLo.a(glList.o) : $(src)/glList.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/glList.o $(src)/glList.c
	ar cr $(bin)/libGLo.a $(bin)/glList.o
	/bin/rm -f $(bin)/glList.o

$(bin)/libGLo.a(glDriverZ.o) : $(src)/glDriverZ.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/glDriverZ.o $(src)/glDriverZ.c
	ar cr $(bin)/libGLo.a $(bin)/glDriverZ.o
	/bin/rm -f $(bin)/glDriverZ.o

$(bin)/libGLo.a(ZBuffer.o) : $(src)/ZBuffer.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/ZBuffer.o $(src)/ZBuffer.c
	ar cr $(bin)/libGLo.a $(bin)/ZBuffer.o
	/bin/rm -f $(bin)/ZBuffer.o

$(bin)/libGLo.a(ZPolygon.o) : $(src)/ZPolygon.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/ZPolygon.o $(src)/ZPolygon.c
	ar cr $(bin)/libGLo.a $(bin)/ZPolygon.o
	/bin/rm -f $(bin)/ZPolygon.o

#--------------------------------------------
# libGLUo dependencies 
#--------------------------------------------
$(bin)/libGLUo.a(glu.o) : $(src)/glu.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/glu.o $(src)/glu.c
	ar cr $(bin)/libGLUo.a $(bin)/glu.o
	/bin/rm -f $(bin)/glu.o

#--------------------------------------------
# libGLXo dependencies 
#--------------------------------------------
$(bin)/libGLXo.a(glx.o) : $(src)/glx.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/glx.o $(src)/glx.c
	ar cr $(bin)/libGLXo.a $(bin)/glx.o
	/bin/rm -f $(bin)/glx.o

$(bin)/libGLXo.a(glDriverX.o) : $(src)/glDriverX.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/glDriverX.o $(src)/glDriverX.c
	ar cr $(bin)/libGLXo.a $(bin)/glDriverX.o
	/bin/rm -f $(bin)/glDriverX.o

#--------------------------------------------
# libGLPSo dependencies 
#--------------------------------------------
$(bin)/libGLPSo.a(glPS.o) : $(src)/glPS.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/glPS.o $(src)/glPS.c
	ar cr $(bin)/libGLPSo.a $(bin)/glPS.o
	/bin/rm -f $(bin)/glPS.o

$(bin)/libGLPSo.a(glDriverPS.o) : $(src)/glDriverPS.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/glDriverPS.o $(src)/glDriverPS.c
	ar cr $(bin)/libGLPSo.a $(bin)/glDriverPS.o
	/bin/rm -f $(bin)/glDriverPS.o


