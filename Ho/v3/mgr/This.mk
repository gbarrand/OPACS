#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#  File produced by the Co/omake tool
# using file Make.odb.
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
HAS_CPPFLAGS = \
 $(Ho_CPPFLAGS)
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

libHo_target = $(bin)/libHo.$(a_or_so)
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
all :: mkdir \
$(libHo_target) \
$(bin)/oshHo.exe \
$(bin)/HoT.exe 
	@echo "Ho : all ok." 

libs :: mkdir \
$(libHo_target) 
	@echo "Ho : libs ok." 

apps :: mkdir \
$(bin)/oshHo.exe \
$(bin)/HoT.exe \
$(bin)/gauss.exe \
$(bin)/HoF77T.exe \
$(bin)/NoHT.exe 
	@echo "Ho : apps ok." 
#--------------------------------------------
rmexeo :
	/bin/rm -f $(bin)/oshHo.o
	/bin/rm -f $(bin)/HoT.o
	/bin/rm -f $(bin)/gauss.o
	/bin/rm -f $(bin)/HoF77T.o
	/bin/rm -f $(bin)/NoHT.o
#--------------------------------------------
oshHo : $(bin)/oshHo.exe
	@echo "Ho : oshHo ok."
HoT : $(bin)/HoT.exe
	@echo "Ho : HoT ok."
gauss : $(bin)/gauss.exe
	@echo "Ho : gauss ok."
HoF77T : $(bin)/HoF77T.exe
	@echo "Ho : HoF77T ok."
NoHT : $(bin)/NoHT.exe
	@echo "Ho : NoHT ok."
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#--------------------------------------------
$(bin)/oshHo.exe : $(bin)/oshHo.o \
$(libHo_target) 
	$(CCLD) $(CCLDFLAGS) -o $(bin)/oshHo.exe $(bin)/oshHo.o \
$(libHo) \
$(libCo) \
$(libm) \
$(CCLDEND) 

$(bin)/oshHo.o   : $(app)/oshHo.c
	$(CC) $(CFLAGS) $(APP_CCFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/oshHo.o $(app)/oshHo.c 
#--------------------------------------------
$(bin)/HoT.exe : $(bin)/HoT.o \
$(libHo_target) 
	$(CCLD) $(CCLDFLAGS) -o $(bin)/HoT.exe $(bin)/HoT.o \
$(libHo) \
$(libCo) \
$(libm) \
$(CCLDEND) 

$(bin)/HoT.o   : $(app)/HoT.c
	$(CC) $(CFLAGS) $(APP_CCFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/HoT.o $(app)/HoT.c 
#--------------------------------------------
$(bin)/gauss.exe : $(bin)/gauss.o 
	$(CCLD) $(CCLDFLAGS) -o $(bin)/gauss.exe $(bin)/gauss.o \
$(libCo) \
$(libm) \
$(CCLDEND) 

$(bin)/gauss.o   : $(app)/gauss.c
	$(CC) $(CFLAGS) $(APP_CCFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/gauss.o $(app)/gauss.c 
#--------------------------------------------
$(bin)/HoF77T.exe : $(bin)/HoF77T.o \
$(libHo_target) 
	$(F77LD) $(F77LDFLAGS) -o $(bin)/HoF77T.exe $(bin)/HoF77T.o \
$(libHo) \
$(libCo) \
$(libm) \
$(F77LDEND) 

$(bin)/HoF77T.o   : $(app)/HoF77T.f
	$(F77) $(F77FLAGS) -c -o $(bin)/HoF77T.o $(app)/HoF77T.f
#--------------------------------------------
$(bin)/NoHT.exe : $(bin)/NoHT.o 
	$(CCLD) $(CCLDFLAGS) -o $(bin)/NoHT.exe $(bin)/NoHT.o \
$(CCLDEND) 

$(bin)/NoHT.o   : $(app)/NoHT.c
	$(CC) $(CFLAGS) $(APP_CCFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/NoHT.o $(app)/NoHT.c 
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
# libraries 
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
.PRECIOUS : $(bin)/libHo.a 
#--------------------------------------------
$(bin)/libHo.$(so) : $(bin)/libHo.a 
	$(make_so) libHo $(bin) "\
$(libCo) \
$(libm) \
$(libc) \
"

$(bin)/libHo.a : \
$(bin)/libHo.a(HoCommands.o) \
$(bin)/libHo.a(HoTypes.o) \
$(bin)/libHo.a(Ho.o) \
$(bin)/libHo.a(HoF77.o) \
$(bin)/libHo.a(OTuple.o) \
$(bin)/libHo.a(OCut.o) \
$(bin)/libHo.a(OFunction.o) \
$(bin)/libHo.a(OFit.o) \
$(bin)/libHo.a(OHistogram.o) \
$(bin)/libHo.a(OScatter.o) \
$(bin)/libHo.a(OAxis.o) \
$(bin)/libHo.a(OStatement.o) \
$(bin)/libHo.a(HoBinds.o) 
	@cd $(bin) ; if [ -f /bin/ranlib ] ; then /bin/ranlib libHo.a ; fi ; cd $(mgr)
#--------------------------------------------
# libHo dependencies 
#--------------------------------------------
$(bin)/libHo.a(HoCommands.o) : $(src)/HoCommands.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/HoCommands.o $(src)/HoCommands.c
	ar cr $(bin)/libHo.a $(bin)/HoCommands.o
	/bin/rm -f $(bin)/HoCommands.o

$(bin)/libHo.a(HoTypes.o) : $(src)/HoTypes.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/HoTypes.o $(src)/HoTypes.c
	ar cr $(bin)/libHo.a $(bin)/HoTypes.o
	/bin/rm -f $(bin)/HoTypes.o

$(bin)/libHo.a(Ho.o) : $(src)/Ho.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/Ho.o $(src)/Ho.c
	ar cr $(bin)/libHo.a $(bin)/Ho.o
	/bin/rm -f $(bin)/Ho.o

$(bin)/libHo.a(HoF77.o) : $(src)/HoF77.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/HoF77.o $(src)/HoF77.c
	ar cr $(bin)/libHo.a $(bin)/HoF77.o
	/bin/rm -f $(bin)/HoF77.o

$(bin)/libHo.a(OTuple.o) : $(src)/OTuple.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/OTuple.o $(src)/OTuple.c
	ar cr $(bin)/libHo.a $(bin)/OTuple.o
	/bin/rm -f $(bin)/OTuple.o

$(bin)/libHo.a(OCut.o) : $(src)/OCut.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/OCut.o $(src)/OCut.c
	ar cr $(bin)/libHo.a $(bin)/OCut.o
	/bin/rm -f $(bin)/OCut.o

$(bin)/libHo.a(OFunction.o) : $(src)/OFunction.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/OFunction.o $(src)/OFunction.c
	ar cr $(bin)/libHo.a $(bin)/OFunction.o
	/bin/rm -f $(bin)/OFunction.o

$(bin)/libHo.a(OFit.o) : $(src)/OFit.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/OFit.o $(src)/OFit.c
	ar cr $(bin)/libHo.a $(bin)/OFit.o
	/bin/rm -f $(bin)/OFit.o

$(bin)/libHo.a(OHistogram.o) : $(src)/OHistogram.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/OHistogram.o $(src)/OHistogram.c
	ar cr $(bin)/libHo.a $(bin)/OHistogram.o
	/bin/rm -f $(bin)/OHistogram.o

$(bin)/libHo.a(OScatter.o) : $(src)/OScatter.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/OScatter.o $(src)/OScatter.c
	ar cr $(bin)/libHo.a $(bin)/OScatter.o
	/bin/rm -f $(bin)/OScatter.o

$(bin)/libHo.a(OAxis.o) : $(src)/OAxis.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/OAxis.o $(src)/OAxis.c
	ar cr $(bin)/libHo.a $(bin)/OAxis.o
	/bin/rm -f $(bin)/OAxis.o

$(bin)/libHo.a(OStatement.o) : $(src)/OStatement.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/OStatement.o $(src)/OStatement.c
	ar cr $(bin)/libHo.a $(bin)/OStatement.o
	/bin/rm -f $(bin)/OStatement.o

$(bin)/libHo.a(HoBinds.o) : $(src)/HoBinds.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/HoBinds.o $(src)/HoBinds.c
	ar cr $(bin)/libHo.a $(bin)/HoBinds.o
	/bin/rm -f $(bin)/HoBinds.o


