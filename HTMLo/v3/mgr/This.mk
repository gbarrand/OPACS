#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#  File produced by the Co/omake tool
# using file Make.odb.
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#  The soft linked packages has been taken
# from the property 'has' of the 
# 'Make' object of file Make.odb.
# It had the value :
#    X11 Xext Xt Xmu Xm HTMLoXm
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
HAS_CPPFLAGS = \
 $(HTMLo_CPPFLAGS)\
 $(X11_CPPFLAGS)\
 $(Xext_CPPFLAGS)\
 $(Xt_CPPFLAGS)\
 $(Xmu_CPPFLAGS)\
 $(Xm_CPPFLAGS)\
 $(HTMLoXm_CPPFLAGS)
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

libHTMLo_target = $(bin)/libHTMLo.$(a_or_so)
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
all :: mkdir \
$(libHTMLo_target) \
$(bin)/htmlT.exe 
	@echo "HTMLo : all ok." 

libs :: mkdir \
$(libHTMLo_target) 
	@echo "HTMLo : libs ok." 

apps :: mkdir \
$(bin)/htmlT.exe 
	@echo "HTMLo : apps ok." 
#--------------------------------------------
rmexeo :
	/bin/rm -f $(bin)/htmlT.o
#--------------------------------------------
htmlT : $(bin)/htmlT.exe
	@echo "HTMLo : htmlT ok."
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#--------------------------------------------
$(bin)/htmlT.exe : $(bin)/htmlT.o \
$(bin)/CFile.o \
$(libHTMLo_target) 
	$(CCLD) $(CCLDFLAGS) -o $(bin)/htmlT.exe $(bin)/htmlT.o \
$(bin)/CFile.o \
$(libHTMLo) \
$(libXm) \
$(libXmu) \
$(libXt) \
$(libXext) \
$(libX11) \
$(libm) \
$(CCLDEND) 

$(bin)/htmlT.o   : $(app)/htmlT.c
	$(CC) $(CFLAGS) $(APP_CCFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/htmlT.o $(app)/htmlT.c 
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
# libraries 
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
.PRECIOUS : $(bin)/libHTMLo.a 
#--------------------------------------------
$(bin)/libHTMLo.$(so) : $(bin)/libHTMLo.a 
	$(make_so) libHTMLo $(bin) "\
$(libXaw) \
$(libXm) \
$(libXmu) \
$(libXt) \
$(libXext) \
$(libX11) \
$(libm) \
$(libc) \
"

$(bin)/libHTMLo.a : \
$(bin)/libHTMLo.a(HTMLWidget.o) \
$(bin)/libHTMLo.a(DrawingArea.o) \
$(bin)/libHTMLo.a(HTMLformat.o) \
$(bin)/libHTMLo.a(HTMLlists.o) \
$(bin)/libHTMLo.a(HTMLPSformat.o) \
$(bin)/libHTMLo.a(HTMLimages.o) \
$(bin)/libHTMLo.a(HTMLparse.o) \
$(bin)/libHTMLo.a(HTMLjot.o) \
$(bin)/libHTMLo.a(HTMLtable.o) \
$(bin)/libHTMLo.a(HTMLwidgets.o) \
$(bin)/libHTMLo.a(list.o) \
$(bin)/libHTMLo.a(medcut.o) \
$(bin)/libHTMLo.a(gifread.o) \
$(bin)/libHTMLo.a(xpmhash.o) 
	@cd $(bin) ; if [ -f /bin/ranlib ] ; then /bin/ranlib libHTMLo.a ; fi ; cd $(mgr)
#--------------------------------------------
# libHTMLo dependencies 
#--------------------------------------------
$(bin)/libHTMLo.a(HTMLWidget.o) : $(src)/HTMLWidget.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/HTMLWidget.o $(src)/HTMLWidget.c
	ar cr $(bin)/libHTMLo.a $(bin)/HTMLWidget.o
	/bin/rm -f $(bin)/HTMLWidget.o

$(bin)/libHTMLo.a(DrawingArea.o) : $(src)/DrawingArea.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/DrawingArea.o $(src)/DrawingArea.c
	ar cr $(bin)/libHTMLo.a $(bin)/DrawingArea.o
	/bin/rm -f $(bin)/DrawingArea.o

$(bin)/libHTMLo.a(HTMLformat.o) : $(src)/HTMLformat.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/HTMLformat.o $(src)/HTMLformat.c
	ar cr $(bin)/libHTMLo.a $(bin)/HTMLformat.o
	/bin/rm -f $(bin)/HTMLformat.o

$(bin)/libHTMLo.a(HTMLlists.o) : $(src)/HTMLlists.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/HTMLlists.o $(src)/HTMLlists.c
	ar cr $(bin)/libHTMLo.a $(bin)/HTMLlists.o
	/bin/rm -f $(bin)/HTMLlists.o

$(bin)/libHTMLo.a(HTMLPSformat.o) : $(src)/HTMLPSformat.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/HTMLPSformat.o $(src)/HTMLPSformat.c
	ar cr $(bin)/libHTMLo.a $(bin)/HTMLPSformat.o
	/bin/rm -f $(bin)/HTMLPSformat.o

$(bin)/libHTMLo.a(HTMLimages.o) : $(src)/HTMLimages.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/HTMLimages.o $(src)/HTMLimages.c
	ar cr $(bin)/libHTMLo.a $(bin)/HTMLimages.o
	/bin/rm -f $(bin)/HTMLimages.o

$(bin)/libHTMLo.a(HTMLparse.o) : $(src)/HTMLparse.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/HTMLparse.o $(src)/HTMLparse.c
	ar cr $(bin)/libHTMLo.a $(bin)/HTMLparse.o
	/bin/rm -f $(bin)/HTMLparse.o

$(bin)/libHTMLo.a(HTMLjot.o) : $(src)/HTMLjot.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/HTMLjot.o $(src)/HTMLjot.c
	ar cr $(bin)/libHTMLo.a $(bin)/HTMLjot.o
	/bin/rm -f $(bin)/HTMLjot.o

$(bin)/libHTMLo.a(HTMLtable.o) : $(src)/HTMLtable.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/HTMLtable.o $(src)/HTMLtable.c
	ar cr $(bin)/libHTMLo.a $(bin)/HTMLtable.o
	/bin/rm -f $(bin)/HTMLtable.o

$(bin)/libHTMLo.a(HTMLwidgets.o) : $(src)/HTMLwidgets.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/HTMLwidgets.o $(src)/HTMLwidgets.c
	ar cr $(bin)/libHTMLo.a $(bin)/HTMLwidgets.o
	/bin/rm -f $(bin)/HTMLwidgets.o

$(bin)/libHTMLo.a(list.o) : $(src)/list.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/list.o $(src)/list.c
	ar cr $(bin)/libHTMLo.a $(bin)/list.o
	/bin/rm -f $(bin)/list.o

$(bin)/libHTMLo.a(medcut.o) : $(src)/medcut.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/medcut.o $(src)/medcut.c
	ar cr $(bin)/libHTMLo.a $(bin)/medcut.o
	/bin/rm -f $(bin)/medcut.o

$(bin)/libHTMLo.a(gifread.o) : $(src)/gifread.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/gifread.o $(src)/gifread.c
	ar cr $(bin)/libHTMLo.a $(bin)/gifread.o
	/bin/rm -f $(bin)/gifread.o

$(bin)/libHTMLo.a(xpmhash.o) : $(src)/xpmhash.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/xpmhash.o $(src)/xpmhash.c
	ar cr $(bin)/libHTMLo.a $(bin)/xpmhash.o
	/bin/rm -f $(bin)/xpmhash.o

$(bin)/CFile.o : $(app)/CFile.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/CFile.o $(app)/CFile.c


