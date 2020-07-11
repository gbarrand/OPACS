#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#  File produced by the Co/omake tool
# using file Make.odb.
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
HAS_CPPFLAGS = \
 $(W3o_CPPFLAGS)
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

libW3o_target = $(bin)/libW3o.$(a_or_so)
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
all :: mkdir \
$(libW3o_target) 
	@echo "W3o : all ok." 

libs :: mkdir \
$(libW3o_target) 
	@echo "W3o : libs ok." 

apps :: mkdir \
$(bin)/W3oT.exe 
	@echo "W3o : apps ok." 
#--------------------------------------------
rmexeo :
	/bin/rm -f $(bin)/W3oT.o
#--------------------------------------------
W3oT : $(bin)/W3oT.exe
	@echo "W3o : W3oT ok."
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#--------------------------------------------
$(bin)/W3oT.exe : $(bin)/W3oT.o \
$(libW3o_target) 
	$(CCLD) $(CCLDFLAGS) -o $(bin)/W3oT.exe $(bin)/W3oT.o \
$(libW3o) \
$(CCLDEND) 

$(bin)/W3oT.o   : $(app)/W3oT.c
	$(CC) $(CFLAGS) $(APP_CCFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/W3oT.o $(app)/W3oT.c 
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
# libraries 
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
.PRECIOUS : $(bin)/libW3o.a 
#--------------------------------------------
$(bin)/libW3o.$(so) : $(bin)/libW3o.a 
	$(make_so) libW3o $(bin) "\
$(libm) \
$(libc) \
"

$(bin)/libW3o.a : \
$(bin)/libW3o.a(HTAABrow.o) \
$(bin)/libW3o.a(HTAAUtil.o) \
$(bin)/libW3o.a(HTAccess.o) \
$(bin)/libW3o.a(HTAlert.o) \
$(bin)/libW3o.a(HTAnchor.o) \
$(bin)/libW3o.a(HTAssoc.o) \
$(bin)/libW3o.a(HTAtom.o) \
$(bin)/libW3o.a(HTChunk.o) \
$(bin)/libW3o.a(HTCompressed.o) \
$(bin)/libW3o.a(HTFTP.o) \
$(bin)/libW3o.a(HTFWriter.o) \
$(bin)/libW3o.a(HTFile.o) \
$(bin)/libW3o.a(HTFormat.o) \
$(bin)/libW3o.a(HTGopher.o) \
$(bin)/libW3o.a(HTIcon.o) \
$(bin)/libW3o.a(HTInit.o) \
$(bin)/libW3o.a(HTList.o) \
$(bin)/libW3o.a(HTMIME.o) \
$(bin)/libW3o.a(HTMLNew.o) \
$(bin)/libW3o.a(HTMLDTD.o) \
$(bin)/libW3o.a(HTMLGen.o) \
$(bin)/libW3o.a(HTMosaicHTML.o) \
$(bin)/libW3o.a(HTNews.o) \
$(bin)/libW3o.a(HTParse.o) \
$(bin)/libW3o.a(HTPlain.o) \
$(bin)/libW3o.a(HTSort.o) \
$(bin)/libW3o.a(HTString.o) \
$(bin)/libW3o.a(HTTCP.o) \
$(bin)/libW3o.a(HTTP.o) \
$(bin)/libW3o.a(HTTelnet.o) \
$(bin)/libW3o.a(HTUU.o) \
$(bin)/libW3o.a(HTWAIS.o) \
$(bin)/libW3o.a(HTWSRC.o) \
$(bin)/libW3o.a(HTWriter.o) \
$(bin)/libW3o.a(SGML.o) \
$(bin)/libW3o.a(HTMailto.o) \
$(bin)/libW3o.a(kcms.o) \
$(bin)/libW3o.a(moHText.o) \
$(bin)/libW3o.a(momo.o) \
$(bin)/libW3o.a(ioctl.o) 
	@cd $(bin) ; if [ -f /bin/ranlib ] ; then /bin/ranlib libW3o.a ; fi ; cd $(mgr)
#--------------------------------------------
# libW3o dependencies 
#--------------------------------------------
$(bin)/libW3o.a(HTAABrow.o) : $(src)/HTAABrow.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/HTAABrow.o $(src)/HTAABrow.c
	ar cr $(bin)/libW3o.a $(bin)/HTAABrow.o
	/bin/rm -f $(bin)/HTAABrow.o

$(bin)/libW3o.a(HTAAUtil.o) : $(src)/HTAAUtil.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/HTAAUtil.o $(src)/HTAAUtil.c
	ar cr $(bin)/libW3o.a $(bin)/HTAAUtil.o
	/bin/rm -f $(bin)/HTAAUtil.o

$(bin)/libW3o.a(HTAccess.o) : $(src)/HTAccess.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/HTAccess.o $(src)/HTAccess.c
	ar cr $(bin)/libW3o.a $(bin)/HTAccess.o
	/bin/rm -f $(bin)/HTAccess.o

$(bin)/libW3o.a(HTAlert.o) : $(src)/HTAlert.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/HTAlert.o $(src)/HTAlert.c
	ar cr $(bin)/libW3o.a $(bin)/HTAlert.o
	/bin/rm -f $(bin)/HTAlert.o

$(bin)/libW3o.a(HTAnchor.o) : $(src)/HTAnchor.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/HTAnchor.o $(src)/HTAnchor.c
	ar cr $(bin)/libW3o.a $(bin)/HTAnchor.o
	/bin/rm -f $(bin)/HTAnchor.o

$(bin)/libW3o.a(HTAssoc.o) : $(src)/HTAssoc.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/HTAssoc.o $(src)/HTAssoc.c
	ar cr $(bin)/libW3o.a $(bin)/HTAssoc.o
	/bin/rm -f $(bin)/HTAssoc.o

$(bin)/libW3o.a(HTAtom.o) : $(src)/HTAtom.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/HTAtom.o $(src)/HTAtom.c
	ar cr $(bin)/libW3o.a $(bin)/HTAtom.o
	/bin/rm -f $(bin)/HTAtom.o

$(bin)/libW3o.a(HTChunk.o) : $(src)/HTChunk.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/HTChunk.o $(src)/HTChunk.c
	ar cr $(bin)/libW3o.a $(bin)/HTChunk.o
	/bin/rm -f $(bin)/HTChunk.o

$(bin)/libW3o.a(HTCompressed.o) : $(src)/HTCompressed.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/HTCompressed.o $(src)/HTCompressed.c
	ar cr $(bin)/libW3o.a $(bin)/HTCompressed.o
	/bin/rm -f $(bin)/HTCompressed.o

$(bin)/libW3o.a(HTFTP.o) : $(src)/HTFTP.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/HTFTP.o $(src)/HTFTP.c
	ar cr $(bin)/libW3o.a $(bin)/HTFTP.o
	/bin/rm -f $(bin)/HTFTP.o

$(bin)/libW3o.a(HTFWriter.o) : $(src)/HTFWriter.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/HTFWriter.o $(src)/HTFWriter.c
	ar cr $(bin)/libW3o.a $(bin)/HTFWriter.o
	/bin/rm -f $(bin)/HTFWriter.o

$(bin)/libW3o.a(HTFile.o) : $(src)/HTFile.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/HTFile.o $(src)/HTFile.c
	ar cr $(bin)/libW3o.a $(bin)/HTFile.o
	/bin/rm -f $(bin)/HTFile.o

$(bin)/libW3o.a(HTFormat.o) : $(src)/HTFormat.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/HTFormat.o $(src)/HTFormat.c
	ar cr $(bin)/libW3o.a $(bin)/HTFormat.o
	/bin/rm -f $(bin)/HTFormat.o

$(bin)/libW3o.a(HTGopher.o) : $(src)/HTGopher.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/HTGopher.o $(src)/HTGopher.c
	ar cr $(bin)/libW3o.a $(bin)/HTGopher.o
	/bin/rm -f $(bin)/HTGopher.o

$(bin)/libW3o.a(HTIcon.o) : $(src)/HTIcon.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/HTIcon.o $(src)/HTIcon.c
	ar cr $(bin)/libW3o.a $(bin)/HTIcon.o
	/bin/rm -f $(bin)/HTIcon.o

$(bin)/libW3o.a(HTInit.o) : $(src)/HTInit.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/HTInit.o $(src)/HTInit.c
	ar cr $(bin)/libW3o.a $(bin)/HTInit.o
	/bin/rm -f $(bin)/HTInit.o

$(bin)/libW3o.a(HTList.o) : $(src)/HTList.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/HTList.o $(src)/HTList.c
	ar cr $(bin)/libW3o.a $(bin)/HTList.o
	/bin/rm -f $(bin)/HTList.o

$(bin)/libW3o.a(HTMIME.o) : $(src)/HTMIME.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/HTMIME.o $(src)/HTMIME.c
	ar cr $(bin)/libW3o.a $(bin)/HTMIME.o
	/bin/rm -f $(bin)/HTMIME.o

$(bin)/libW3o.a(HTMLNew.o) : $(src)/HTMLNew.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/HTMLNew.o $(src)/HTMLNew.c
	ar cr $(bin)/libW3o.a $(bin)/HTMLNew.o
	/bin/rm -f $(bin)/HTMLNew.o

$(bin)/libW3o.a(HTMLDTD.o) : $(src)/HTMLDTD.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/HTMLDTD.o $(src)/HTMLDTD.c
	ar cr $(bin)/libW3o.a $(bin)/HTMLDTD.o
	/bin/rm -f $(bin)/HTMLDTD.o

$(bin)/libW3o.a(HTMLGen.o) : $(src)/HTMLGen.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/HTMLGen.o $(src)/HTMLGen.c
	ar cr $(bin)/libW3o.a $(bin)/HTMLGen.o
	/bin/rm -f $(bin)/HTMLGen.o

$(bin)/libW3o.a(HTMosaicHTML.o) : $(src)/HTMosaicHTML.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/HTMosaicHTML.o $(src)/HTMosaicHTML.c
	ar cr $(bin)/libW3o.a $(bin)/HTMosaicHTML.o
	/bin/rm -f $(bin)/HTMosaicHTML.o

$(bin)/libW3o.a(HTNews.o) : $(src)/HTNews.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/HTNews.o $(src)/HTNews.c
	ar cr $(bin)/libW3o.a $(bin)/HTNews.o
	/bin/rm -f $(bin)/HTNews.o

$(bin)/libW3o.a(HTParse.o) : $(src)/HTParse.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/HTParse.o $(src)/HTParse.c
	ar cr $(bin)/libW3o.a $(bin)/HTParse.o
	/bin/rm -f $(bin)/HTParse.o

$(bin)/libW3o.a(HTPlain.o) : $(src)/HTPlain.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/HTPlain.o $(src)/HTPlain.c
	ar cr $(bin)/libW3o.a $(bin)/HTPlain.o
	/bin/rm -f $(bin)/HTPlain.o

$(bin)/libW3o.a(HTSort.o) : $(src)/HTSort.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/HTSort.o $(src)/HTSort.c
	ar cr $(bin)/libW3o.a $(bin)/HTSort.o
	/bin/rm -f $(bin)/HTSort.o

$(bin)/libW3o.a(HTString.o) : $(src)/HTString.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/HTString.o $(src)/HTString.c
	ar cr $(bin)/libW3o.a $(bin)/HTString.o
	/bin/rm -f $(bin)/HTString.o

$(bin)/libW3o.a(HTTCP.o) : $(src)/HTTCP.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/HTTCP.o $(src)/HTTCP.c
	ar cr $(bin)/libW3o.a $(bin)/HTTCP.o
	/bin/rm -f $(bin)/HTTCP.o

$(bin)/libW3o.a(HTTP.o) : $(src)/HTTP.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/HTTP.o $(src)/HTTP.c
	ar cr $(bin)/libW3o.a $(bin)/HTTP.o
	/bin/rm -f $(bin)/HTTP.o

$(bin)/libW3o.a(HTTelnet.o) : $(src)/HTTelnet.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/HTTelnet.o $(src)/HTTelnet.c
	ar cr $(bin)/libW3o.a $(bin)/HTTelnet.o
	/bin/rm -f $(bin)/HTTelnet.o

$(bin)/libW3o.a(HTUU.o) : $(src)/HTUU.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/HTUU.o $(src)/HTUU.c
	ar cr $(bin)/libW3o.a $(bin)/HTUU.o
	/bin/rm -f $(bin)/HTUU.o

$(bin)/libW3o.a(HTWAIS.o) : $(src)/HTWAIS.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/HTWAIS.o $(src)/HTWAIS.c
	ar cr $(bin)/libW3o.a $(bin)/HTWAIS.o
	/bin/rm -f $(bin)/HTWAIS.o

$(bin)/libW3o.a(HTWSRC.o) : $(src)/HTWSRC.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/HTWSRC.o $(src)/HTWSRC.c
	ar cr $(bin)/libW3o.a $(bin)/HTWSRC.o
	/bin/rm -f $(bin)/HTWSRC.o

$(bin)/libW3o.a(HTWriter.o) : $(src)/HTWriter.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/HTWriter.o $(src)/HTWriter.c
	ar cr $(bin)/libW3o.a $(bin)/HTWriter.o
	/bin/rm -f $(bin)/HTWriter.o

$(bin)/libW3o.a(SGML.o) : $(src)/SGML.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/SGML.o $(src)/SGML.c
	ar cr $(bin)/libW3o.a $(bin)/SGML.o
	/bin/rm -f $(bin)/SGML.o

$(bin)/libW3o.a(HTMailto.o) : $(src)/HTMailto.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/HTMailto.o $(src)/HTMailto.c
	ar cr $(bin)/libW3o.a $(bin)/HTMailto.o
	/bin/rm -f $(bin)/HTMailto.o

$(bin)/libW3o.a(kcms.o) : $(src)/kcms.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/kcms.o $(src)/kcms.c
	ar cr $(bin)/libW3o.a $(bin)/kcms.o
	/bin/rm -f $(bin)/kcms.o

$(bin)/libW3o.a(moHText.o) : $(src)/moHText.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/moHText.o $(src)/moHText.c
	ar cr $(bin)/libW3o.a $(bin)/moHText.o
	/bin/rm -f $(bin)/moHText.o

$(bin)/libW3o.a(momo.o) : $(src)/momo.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/momo.o $(src)/momo.c
	ar cr $(bin)/libW3o.a $(bin)/momo.o
	/bin/rm -f $(bin)/momo.o

$(bin)/libW3o.a(ioctl.o) : $(src)/ioctl.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/ioctl.o $(src)/ioctl.c
	ar cr $(bin)/libW3o.a $(bin)/ioctl.o
	/bin/rm -f $(bin)/ioctl.o


