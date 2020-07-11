#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#  File produced by the Co/omake tool
# using file Make.odb.
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#  The soft linked packages has been taken
# from the property 'has' of the 
# 'Make' object of file Make.odb.
# It had the value :
#    X11 Xext Xt Xmu
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
HAS_CPPFLAGS = \
 $(Xx_CPPFLAGS)\
 $(X11_CPPFLAGS)\
 $(Xext_CPPFLAGS)\
 $(Xt_CPPFLAGS)\
 $(Xmu_CPPFLAGS)
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

libXx_target = $(bin)/libXx.$(a_or_so)
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
all :: mkdir \
$(libXx_target) 
	@echo "Xx : all ok." 

libs :: mkdir \
$(libXx_target) 
	@echo "Xx : libs ok." 

apps :: mkdir \
$(bin)/omail.exe \
$(bin)/NoHT.exe 
	@echo "Xx : apps ok." 
#--------------------------------------------
rmexeo :
	/bin/rm -f $(bin)/omail.o
	/bin/rm -f $(bin)/NoHT.o
#--------------------------------------------
omail : $(bin)/omail.exe
	@echo "Xx : omail ok."
NoHT : $(bin)/NoHT.exe
	@echo "Xx : NoHT ok."
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#--------------------------------------------
$(bin)/omail.exe : $(bin)/omail.o \
$(libXx_target) 
	$(CCLD) $(CCLDFLAGS) -o $(bin)/omail.exe $(bin)/omail.o \
$(libXx) \
$(libCo) \
$(libXmu) \
$(libXt) \
$(libXext) \
$(libX11) \
$(libm) \
$(CCLDEND) 

$(bin)/omail.o   : $(app)/omail.c
	$(CC) $(CFLAGS) $(APP_CCFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/omail.o $(app)/omail.c 
#--------------------------------------------
$(bin)/NoHT.exe : $(bin)/NoHT.o 
	$(CCLD) $(CCLDFLAGS) -o $(bin)/NoHT.exe $(bin)/NoHT.o \
$(CCLDEND) 

$(bin)/NoHT.o   : $(app)/NoHT.c
	$(CC) $(CFLAGS) $(APP_CCFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/NoHT.o $(app)/NoHT.c 
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
# libraries 
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
.PRECIOUS : $(bin)/libXx.a 
#--------------------------------------------
$(bin)/libXx.$(so) : $(bin)/libXx.a 
	$(make_so) libXx $(bin) "\
$(libXmu) \
$(libXt) \
$(libXext) \
$(libX11) \
$(libCo) \
$(libm) \
$(libc) \
"

$(bin)/libXx.a : \
$(bin)/libXx.a(XxCommands.o) \
$(bin)/libXx.a(XxTypes.o) \
$(bin)/libXx.a(XxF77.o) \
$(bin)/libXx.a(XWidget.o) \
$(bin)/libXx.a(XMailbox.o) \
$(bin)/libXx.a(XSelection.o) \
$(bin)/libXx.a(XDisplay.o) \
$(bin)/libXx.a(LocPix.o) \
$(bin)/libXx.a(XxBinds.o) 
	@cd $(bin) ; if [ -f /bin/ranlib ] ; then /bin/ranlib libXx.a ; fi ; cd $(mgr)
#--------------------------------------------
# libXx dependencies 
#--------------------------------------------
$(bin)/libXx.a(XxCommands.o) : $(src)/XxCommands.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/XxCommands.o $(src)/XxCommands.c
	ar cr $(bin)/libXx.a $(bin)/XxCommands.o
	/bin/rm -f $(bin)/XxCommands.o

$(bin)/libXx.a(XxTypes.o) : $(src)/XxTypes.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/XxTypes.o $(src)/XxTypes.c
	ar cr $(bin)/libXx.a $(bin)/XxTypes.o
	/bin/rm -f $(bin)/XxTypes.o

$(bin)/libXx.a(XxF77.o) : $(src)/XxF77.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/XxF77.o $(src)/XxF77.c
	ar cr $(bin)/libXx.a $(bin)/XxF77.o
	/bin/rm -f $(bin)/XxF77.o

$(bin)/libXx.a(XWidget.o) : $(src)/XWidget.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/XWidget.o $(src)/XWidget.c
	ar cr $(bin)/libXx.a $(bin)/XWidget.o
	/bin/rm -f $(bin)/XWidget.o

$(bin)/libXx.a(XMailbox.o) : $(src)/XMailbox.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/XMailbox.o $(src)/XMailbox.c
	ar cr $(bin)/libXx.a $(bin)/XMailbox.o
	/bin/rm -f $(bin)/XMailbox.o

$(bin)/libXx.a(XSelection.o) : $(src)/XSelection.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/XSelection.o $(src)/XSelection.c
	ar cr $(bin)/libXx.a $(bin)/XSelection.o
	/bin/rm -f $(bin)/XSelection.o

$(bin)/libXx.a(XDisplay.o) : $(src)/XDisplay.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/XDisplay.o $(src)/XDisplay.c
	ar cr $(bin)/libXx.a $(bin)/XDisplay.o
	/bin/rm -f $(bin)/XDisplay.o

$(bin)/libXx.a(LocPix.o) : $(src)/LocPix.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/LocPix.o $(src)/LocPix.c
	ar cr $(bin)/libXx.a $(bin)/LocPix.o
	/bin/rm -f $(bin)/LocPix.o

$(bin)/libXx.a(XxBinds.o) : $(src)/XxBinds.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/XxBinds.o $(src)/XxBinds.c
	ar cr $(bin)/libXx.a $(bin)/XxBinds.o
	/bin/rm -f $(bin)/XxBinds.o


