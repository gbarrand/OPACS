#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#  File produced by the Co/omake tool
# using file Make.odb.
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
HAS_CPPFLAGS = \
 $(Co_CPPFLAGS)
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

libCo_target = $(bin)/libCo.$(a_or_so)
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
all :: mkdir \
$(bin)/mapana.exe \
$(libCo_target) \
$(bin)/omake.exe \
$(bin)/CoT.exe \
$(bin)/oshCo.exe \
$(bin)/opatch.exe 
	@echo "Co : all ok." 

libs :: mkdir \
$(libCo_target) 
	@echo "Co : libs ok." 

apps :: mkdir \
$(bin)/omake.exe \
$(bin)/CoT.exe \
$(bin)/oshCo.exe \
$(bin)/mapana.exe \
$(bin)/opatch.exe \
$(bin)/oupper.exe \
$(bin)/ohtxTohtml.exe \
$(bin)/odoc.exe \
$(bin)/stdargT.exe \
$(bin)/CDefineT.exe \
$(bin)/OBatchT.exe \
$(bin)/CFITST.exe \
$(bin)/GIF_To_PS.exe \
$(bin)/CGIFT.exe \
$(bin)/CLibraryT.exe \
$(bin)/COptionT.exe \
$(bin)/NoHT.exe 
	@echo "Co : apps ok." 
#--------------------------------------------
rmexeo :
	/bin/rm -f $(bin)/omake.o
	/bin/rm -f $(bin)/CoT.o
	/bin/rm -f $(bin)/oshCo.o
	/bin/rm -f $(bin)/mapana.o
	/bin/rm -f $(bin)/opatch.o
	/bin/rm -f $(bin)/oupper.o
	/bin/rm -f $(bin)/ohtxTohtml.o
	/bin/rm -f $(bin)/odoc.o
	/bin/rm -f $(bin)/stdargT.o
	/bin/rm -f $(bin)/CDefineT.o
	/bin/rm -f $(bin)/OBatchT.o
	/bin/rm -f $(bin)/CFITST.o
	/bin/rm -f $(bin)/GIF_To_PS.o
	/bin/rm -f $(bin)/CGIFT.o
	/bin/rm -f $(bin)/CLibraryT.o
	/bin/rm -f $(bin)/COptionT.o
	/bin/rm -f $(bin)/NoHT.o
#--------------------------------------------
omake : $(bin)/omake.exe
	@echo "Co : omake ok."
CoT : $(bin)/CoT.exe
	@echo "Co : CoT ok."
oshCo : $(bin)/oshCo.exe
	@echo "Co : oshCo ok."
mapana : $(bin)/mapana.exe
	@echo "Co : mapana ok."
opatch : $(bin)/opatch.exe
	@echo "Co : opatch ok."
oupper : $(bin)/oupper.exe
	@echo "Co : oupper ok."
ohtxTohtml : $(bin)/ohtxTohtml.exe
	@echo "Co : ohtxTohtml ok."
odoc : $(bin)/odoc.exe
	@echo "Co : odoc ok."
stdargT : $(bin)/stdargT.exe
	@echo "Co : stdargT ok."
CDefineT : $(bin)/CDefineT.exe
	@echo "Co : CDefineT ok."
OBatchT : $(bin)/OBatchT.exe
	@echo "Co : OBatchT ok."
CFITST : $(bin)/CFITST.exe
	@echo "Co : CFITST ok."
GIF_To_PS : $(bin)/GIF_To_PS.exe
	@echo "Co : GIF_To_PS ok."
CGIFT : $(bin)/CGIFT.exe
	@echo "Co : CGIFT ok."
CLibraryT : $(bin)/CLibraryT.exe
	@echo "Co : CLibraryT ok."
COptionT : $(bin)/COptionT.exe
	@echo "Co : COptionT ok."
NoHT : $(bin)/NoHT.exe
	@echo "Co : NoHT ok."
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#--------------------------------------------
$(bin)/omake.exe : $(bin)/omake.o \
$(libCo_target) 
	$(CCLD) $(CCLDFLAGS) -o $(bin)/omake.exe $(bin)/omake.o \
$(libCo) \
$(libm) \
$(CCLDEND) 

$(bin)/omake.o   : $(app)/omake.c
	$(CC) $(CFLAGS) $(APP_CCFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/omake.o $(app)/omake.c 
#--------------------------------------------
$(bin)/CoT.exe : $(bin)/CoT.o \
$(libCo_target) 
	$(CCLD) $(CCLDFLAGS) -o $(bin)/CoT.exe $(bin)/CoT.o \
$(libCo) \
$(libm) \
$(CCLDEND) 

$(bin)/CoT.o   : $(app)/CoT.c
	$(CC) $(CFLAGS) $(APP_CCFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/CoT.o $(app)/CoT.c 
#--------------------------------------------
$(bin)/oshCo.exe : $(bin)/oshCo.o \
$(libCo_target) 
	$(CCLD) $(CCLDFLAGS) -o $(bin)/oshCo.exe $(bin)/oshCo.o \
$(libCo) \
$(libm) \
$(CCLDEND) 

$(bin)/oshCo.o   : $(app)/oshCo.c
	$(CC) $(CFLAGS) $(APP_CCFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/oshCo.o $(app)/oshCo.c 
#--------------------------------------------
$(bin)/mapana.exe : $(bin)/mapana.o 
	$(CCLD) $(CCLDFLAGS) -o $(bin)/mapana.exe $(bin)/mapana.o \
$(CCLDEND) 

$(bin)/mapana.o   : $(app)/mapana.c
	$(CC) $(CFLAGS) $(APP_CCFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/mapana.o $(app)/mapana.c 
#--------------------------------------------
$(bin)/opatch.exe : $(bin)/opatch.o \
$(libCo_target) 
	$(CCLD) $(CCLDFLAGS) -o $(bin)/opatch.exe $(bin)/opatch.o \
$(libCo) \
$(libm) \
$(CCLDEND) 

$(bin)/opatch.o   : $(app)/opatch.c
	$(CC) $(CFLAGS) $(APP_CCFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/opatch.o $(app)/opatch.c 
#--------------------------------------------
$(bin)/oupper.exe : $(bin)/oupper.o 
	$(CCLD) $(CCLDFLAGS) -o $(bin)/oupper.exe $(bin)/oupper.o \
$(CCLDEND) 

$(bin)/oupper.o   : $(app)/oupper.c
	$(CC) $(CFLAGS) $(APP_CCFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/oupper.o $(app)/oupper.c 
#--------------------------------------------
$(bin)/ohtxTohtml.exe : $(bin)/ohtxTohtml.o \
$(libCo_target) 
	$(CCLD) $(CCLDFLAGS) -o $(bin)/ohtxTohtml.exe $(bin)/ohtxTohtml.o \
$(libCo) \
$(libm) \
$(CCLDEND) 

$(bin)/ohtxTohtml.o   : $(app)/ohtxTohtml.c
	$(CC) $(CFLAGS) $(APP_CCFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/ohtxTohtml.o $(app)/ohtxTohtml.c 
#--------------------------------------------
$(bin)/odoc.exe : $(bin)/odoc.o \
$(libCo_target) 
	$(CCLD) $(CCLDFLAGS) -o $(bin)/odoc.exe $(bin)/odoc.o \
$(libCo) \
$(libm) \
$(CCLDEND) 

$(bin)/odoc.o   : $(app)/odoc.c
	$(CC) $(CFLAGS) $(APP_CCFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/odoc.o $(app)/odoc.c 
#--------------------------------------------
$(bin)/stdargT.exe : $(bin)/stdargT.o 
	$(CCLD) $(CCLDFLAGS) -o $(bin)/stdargT.exe $(bin)/stdargT.o \
$(CCLDEND) 

$(bin)/stdargT.o   : $(app)/stdargT.c
	$(CC) $(CFLAGS) $(APP_CCFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/stdargT.o $(app)/stdargT.c 
#--------------------------------------------
$(bin)/CDefineT.exe : $(bin)/CDefineT.o 
	$(CCLD) $(CCLDFLAGS) -o $(bin)/CDefineT.exe $(bin)/CDefineT.o \
$(CCLDEND) 

$(bin)/CDefineT.o   : $(app)/CDefineT.c
	$(CC) $(CFLAGS) $(APP_CCFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/CDefineT.o $(app)/CDefineT.c 
#--------------------------------------------
$(bin)/OBatchT.exe : $(bin)/OBatchT.o \
$(libCo_target) 
	$(CCLD) $(CCLDFLAGS) -o $(bin)/OBatchT.exe $(bin)/OBatchT.o \
$(libCo) \
$(libm) \
$(CCLDEND) 

$(bin)/OBatchT.o   : $(app)/OBatchT.c
	$(CC) $(CFLAGS) $(APP_CCFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/OBatchT.o $(app)/OBatchT.c 
#--------------------------------------------
$(bin)/CFITST.exe : $(bin)/CFITST.o \
$(libCo_target) 
	$(CCLD) $(CCLDFLAGS) -o $(bin)/CFITST.exe $(bin)/CFITST.o \
$(libCo) \
$(libm) \
$(CCLDEND) 

$(bin)/CFITST.o   : $(app)/CFITST.c
	$(CC) $(CFLAGS) $(APP_CCFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/CFITST.o $(app)/CFITST.c 
#--------------------------------------------
$(bin)/GIF_To_PS.exe : $(bin)/GIF_To_PS.o \
$(libCo_target) 
	$(CCLD) $(CCLDFLAGS) -o $(bin)/GIF_To_PS.exe $(bin)/GIF_To_PS.o \
$(libCo) \
$(libm) \
$(CCLDEND) 

$(bin)/GIF_To_PS.o   : $(app)/GIF_To_PS.c
	$(CC) $(CFLAGS) $(APP_CCFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/GIF_To_PS.o $(app)/GIF_To_PS.c 
#--------------------------------------------
$(bin)/CGIFT.exe : $(bin)/CGIFT.o \
$(libCo_target) 
	$(CCLD) $(CCLDFLAGS) -o $(bin)/CGIFT.exe $(bin)/CGIFT.o \
$(libCo) \
$(libm) \
$(CCLDEND) 

$(bin)/CGIFT.o   : $(app)/CGIFT.c
	$(CC) $(CFLAGS) $(APP_CCFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/CGIFT.o $(app)/CGIFT.c 
#--------------------------------------------
$(bin)/CLibraryT.exe : $(bin)/CLibraryT.o \
$(libCo_target) 
	$(CCLD) $(CCLDFLAGS) -o $(bin)/CLibraryT.exe $(bin)/CLibraryT.o \
$(libCo) \
$(libm) \
$(CCLDEND) 

$(bin)/CLibraryT.o   : $(app)/CLibraryT.c
	$(CC) $(CFLAGS) $(APP_CCFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/CLibraryT.o $(app)/CLibraryT.c 
#--------------------------------------------
$(bin)/COptionT.exe : $(bin)/COptionT.o \
$(libCo_target) 
	$(CCLD) $(CCLDFLAGS) -o $(bin)/COptionT.exe $(bin)/COptionT.o \
$(libCo) \
$(libm) \
$(CCLDEND) 

$(bin)/COptionT.o   : $(app)/COptionT.c
	$(CC) $(CFLAGS) $(APP_CCFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/COptionT.o $(app)/COptionT.c 
#--------------------------------------------
$(bin)/NoHT.exe : $(bin)/NoHT.o 
	$(CCLD) $(CCLDFLAGS) -o $(bin)/NoHT.exe $(bin)/NoHT.o \
$(CCLDEND) 

$(bin)/NoHT.o   : $(app)/NoHT.c
	$(CC) $(CFLAGS) $(APP_CCFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/NoHT.o $(app)/NoHT.c 
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
# libraries 
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
.PRECIOUS : $(bin)/libCo.a 
#--------------------------------------------
$(bin)/libCo.$(so) : $(bin)/libCo.a 
	$(make_so) libCo $(bin) "\
$(libm) \
$(libc) \
"

$(bin)/libCo.a : \
$(bin)/libCo.a(CoCommands.o) \
$(bin)/libCo.a(CManager.o) \
$(bin)/libCo.a(OModule.o) \
$(bin)/libCo.a(OTouch.o) \
$(bin)/libCo.a(CoTypes.o) \
$(bin)/libCo.a(OType.o) \
$(bin)/libCo.a(OProperty.o) \
$(bin)/libCo.a(OProcedureL.o) \
$(bin)/libCo.a(OProcedure.o) \
$(bin)/libCo.a(OProcess.o) \
$(bin)/libCo.a(OHandle.o) \
$(bin)/libCo.a(OShell.o) \
$(bin)/libCo.a(OCommand.o) \
$(bin)/libCo.a(OMatrix.o) \
$(bin)/libCo.a(OMatrix33.o) \
$(bin)/libCo.a(OLUT.o) \
$(bin)/libCo.a(CoF77.o) \
$(bin)/libCo.a(OBatch.o) \
$(bin)/libCo.a(FString.o) \
$(bin)/libCo.a(OHTML.o) \
$(bin)/libCo.a(OColor.o) \
$(bin)/libCo.a(CFITS.o) \
$(bin)/libCo.a(CPS.o) \
$(bin)/libCo.a(CGIF.o) \
$(bin)/libCo.a(CTIFF.o) \
$(bin)/libCo.a(CXPM.o) \
$(bin)/libCo.a(CFile.o) \
$(bin)/libCo.a(CStream.o) \
$(bin)/libCo.a(CText.o) \
$(bin)/libCo.a(CString.o) \
$(bin)/libCo.a(CCharacter.o) \
$(bin)/libCo.a(CColor.o) \
$(bin)/libCo.a(CMath.o) \
$(bin)/libCo.a(CVector.o) \
$(bin)/libCo.a(CList.o) \
$(bin)/libCo.a(CLibrary.o) \
$(bin)/libCo.a(CMemory.o) \
$(bin)/libCo.a(COption.o) \
$(bin)/libCo.a(CError.o) \
$(bin)/libCo.a(CPrinter.o) \
$(bin)/libCo.a(CTime.o) \
$(bin)/libCo.a(CJPEG.o) \
$(bin)/libCo.a(CImageshort.o) \
$(bin)/libCo.a(CSystem.o) \
$(bin)/libCo.a(CBatch.o) \
$(bin)/libCo.a(CoBinds.o) 
	@cd $(bin) ; if [ -f /bin/ranlib ] ; then /bin/ranlib libCo.a ; fi ; cd $(mgr)
#--------------------------------------------
# libCo dependencies 
#--------------------------------------------
$(bin)/libCo.a(CoCommands.o) : $(src)/CoCommands.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/CoCommands.o $(src)/CoCommands.c
	ar cr $(bin)/libCo.a $(bin)/CoCommands.o
	/bin/rm -f $(bin)/CoCommands.o

$(bin)/libCo.a(CManager.o) : $(src)/CManager.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/CManager.o $(src)/CManager.c
	ar cr $(bin)/libCo.a $(bin)/CManager.o
	/bin/rm -f $(bin)/CManager.o

$(bin)/libCo.a(OModule.o) : $(src)/OModule.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/OModule.o $(src)/OModule.c
	ar cr $(bin)/libCo.a $(bin)/OModule.o
	/bin/rm -f $(bin)/OModule.o

$(bin)/libCo.a(OTouch.o) : $(src)/OTouch.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/OTouch.o $(src)/OTouch.c
	ar cr $(bin)/libCo.a $(bin)/OTouch.o
	/bin/rm -f $(bin)/OTouch.o

$(bin)/libCo.a(CoTypes.o) : $(src)/CoTypes.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/CoTypes.o $(src)/CoTypes.c
	ar cr $(bin)/libCo.a $(bin)/CoTypes.o
	/bin/rm -f $(bin)/CoTypes.o

$(bin)/libCo.a(OType.o) : $(src)/OType.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/OType.o $(src)/OType.c
	ar cr $(bin)/libCo.a $(bin)/OType.o
	/bin/rm -f $(bin)/OType.o

$(bin)/libCo.a(OProperty.o) : $(src)/OProperty.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/OProperty.o $(src)/OProperty.c
	ar cr $(bin)/libCo.a $(bin)/OProperty.o
	/bin/rm -f $(bin)/OProperty.o

$(bin)/libCo.a(OProcedureL.o) : $(src)/OProcedureL.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/OProcedureL.o $(src)/OProcedureL.c
	ar cr $(bin)/libCo.a $(bin)/OProcedureL.o
	/bin/rm -f $(bin)/OProcedureL.o

$(bin)/libCo.a(OProcedure.o) : $(src)/OProcedure.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/OProcedure.o $(src)/OProcedure.c
	ar cr $(bin)/libCo.a $(bin)/OProcedure.o
	/bin/rm -f $(bin)/OProcedure.o

$(bin)/libCo.a(OProcess.o) : $(src)/OProcess.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/OProcess.o $(src)/OProcess.c
	ar cr $(bin)/libCo.a $(bin)/OProcess.o
	/bin/rm -f $(bin)/OProcess.o

$(bin)/libCo.a(OHandle.o) : $(src)/OHandle.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/OHandle.o $(src)/OHandle.c
	ar cr $(bin)/libCo.a $(bin)/OHandle.o
	/bin/rm -f $(bin)/OHandle.o

$(bin)/libCo.a(OShell.o) : $(src)/OShell.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/OShell.o $(src)/OShell.c
	ar cr $(bin)/libCo.a $(bin)/OShell.o
	/bin/rm -f $(bin)/OShell.o

$(bin)/libCo.a(OCommand.o) : $(src)/OCommand.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/OCommand.o $(src)/OCommand.c
	ar cr $(bin)/libCo.a $(bin)/OCommand.o
	/bin/rm -f $(bin)/OCommand.o

$(bin)/libCo.a(OMatrix.o) : $(src)/OMatrix.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/OMatrix.o $(src)/OMatrix.c
	ar cr $(bin)/libCo.a $(bin)/OMatrix.o
	/bin/rm -f $(bin)/OMatrix.o

$(bin)/libCo.a(OMatrix33.o) : $(src)/OMatrix33.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/OMatrix33.o $(src)/OMatrix33.c
	ar cr $(bin)/libCo.a $(bin)/OMatrix33.o
	/bin/rm -f $(bin)/OMatrix33.o

$(bin)/libCo.a(OLUT.o) : $(src)/OLUT.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/OLUT.o $(src)/OLUT.c
	ar cr $(bin)/libCo.a $(bin)/OLUT.o
	/bin/rm -f $(bin)/OLUT.o

$(bin)/libCo.a(CoF77.o) : $(src)/CoF77.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/CoF77.o $(src)/CoF77.c
	ar cr $(bin)/libCo.a $(bin)/CoF77.o
	/bin/rm -f $(bin)/CoF77.o

$(bin)/libCo.a(OBatch.o) : $(src)/OBatch.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/OBatch.o $(src)/OBatch.c
	ar cr $(bin)/libCo.a $(bin)/OBatch.o
	/bin/rm -f $(bin)/OBatch.o

$(bin)/libCo.a(FString.o) : $(src)/FString.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/FString.o $(src)/FString.c
	ar cr $(bin)/libCo.a $(bin)/FString.o
	/bin/rm -f $(bin)/FString.o

$(bin)/libCo.a(OHTML.o) : $(src)/OHTML.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/OHTML.o $(src)/OHTML.c
	ar cr $(bin)/libCo.a $(bin)/OHTML.o
	/bin/rm -f $(bin)/OHTML.o

$(bin)/libCo.a(OColor.o) : $(src)/OColor.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/OColor.o $(src)/OColor.c
	ar cr $(bin)/libCo.a $(bin)/OColor.o
	/bin/rm -f $(bin)/OColor.o

$(bin)/libCo.a(CFITS.o) : $(src)/CFITS.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/CFITS.o $(src)/CFITS.c
	ar cr $(bin)/libCo.a $(bin)/CFITS.o
	/bin/rm -f $(bin)/CFITS.o

$(bin)/libCo.a(CPS.o) : $(src)/CPS.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/CPS.o $(src)/CPS.c
	ar cr $(bin)/libCo.a $(bin)/CPS.o
	/bin/rm -f $(bin)/CPS.o

$(bin)/libCo.a(CGIF.o) : $(src)/CGIF.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/CGIF.o $(src)/CGIF.c
	ar cr $(bin)/libCo.a $(bin)/CGIF.o
	/bin/rm -f $(bin)/CGIF.o

$(bin)/libCo.a(CTIFF.o) : $(src)/CTIFF.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/CTIFF.o $(src)/CTIFF.c
	ar cr $(bin)/libCo.a $(bin)/CTIFF.o
	/bin/rm -f $(bin)/CTIFF.o

$(bin)/libCo.a(CXPM.o) : $(src)/CXPM.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/CXPM.o $(src)/CXPM.c
	ar cr $(bin)/libCo.a $(bin)/CXPM.o
	/bin/rm -f $(bin)/CXPM.o

$(bin)/libCo.a(CFile.o) : $(src)/CFile.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/CFile.o $(src)/CFile.c
	ar cr $(bin)/libCo.a $(bin)/CFile.o
	/bin/rm -f $(bin)/CFile.o

$(bin)/libCo.a(CStream.o) : $(src)/CStream.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/CStream.o $(src)/CStream.c
	ar cr $(bin)/libCo.a $(bin)/CStream.o
	/bin/rm -f $(bin)/CStream.o

$(bin)/libCo.a(CText.o) : $(src)/CText.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/CText.o $(src)/CText.c
	ar cr $(bin)/libCo.a $(bin)/CText.o
	/bin/rm -f $(bin)/CText.o

$(bin)/libCo.a(CString.o) : $(src)/CString.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/CString.o $(src)/CString.c
	ar cr $(bin)/libCo.a $(bin)/CString.o
	/bin/rm -f $(bin)/CString.o

$(bin)/libCo.a(CCharacter.o) : $(src)/CCharacter.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/CCharacter.o $(src)/CCharacter.c
	ar cr $(bin)/libCo.a $(bin)/CCharacter.o
	/bin/rm -f $(bin)/CCharacter.o

$(bin)/libCo.a(CColor.o) : $(src)/CColor.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/CColor.o $(src)/CColor.c
	ar cr $(bin)/libCo.a $(bin)/CColor.o
	/bin/rm -f $(bin)/CColor.o

$(bin)/libCo.a(CMath.o) : $(src)/CMath.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/CMath.o $(src)/CMath.c
	ar cr $(bin)/libCo.a $(bin)/CMath.o
	/bin/rm -f $(bin)/CMath.o

$(bin)/libCo.a(CVector.o) : $(src)/CVector.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/CVector.o $(src)/CVector.c
	ar cr $(bin)/libCo.a $(bin)/CVector.o
	/bin/rm -f $(bin)/CVector.o

$(bin)/libCo.a(CList.o) : $(src)/CList.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/CList.o $(src)/CList.c
	ar cr $(bin)/libCo.a $(bin)/CList.o
	/bin/rm -f $(bin)/CList.o

$(bin)/libCo.a(CLibrary.o) : $(src)/CLibrary.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/CLibrary.o $(src)/CLibrary.c
	ar cr $(bin)/libCo.a $(bin)/CLibrary.o
	/bin/rm -f $(bin)/CLibrary.o

$(bin)/libCo.a(CMemory.o) : $(src)/CMemory.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/CMemory.o $(src)/CMemory.c
	ar cr $(bin)/libCo.a $(bin)/CMemory.o
	/bin/rm -f $(bin)/CMemory.o

$(bin)/libCo.a(COption.o) : $(src)/COption.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/COption.o $(src)/COption.c
	ar cr $(bin)/libCo.a $(bin)/COption.o
	/bin/rm -f $(bin)/COption.o

$(bin)/libCo.a(CError.o) : $(src)/CError.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/CError.o $(src)/CError.c
	ar cr $(bin)/libCo.a $(bin)/CError.o
	/bin/rm -f $(bin)/CError.o

$(bin)/libCo.a(CPrinter.o) : $(src)/CPrinter.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/CPrinter.o $(src)/CPrinter.c
	ar cr $(bin)/libCo.a $(bin)/CPrinter.o
	/bin/rm -f $(bin)/CPrinter.o

$(bin)/libCo.a(CTime.o) : $(src)/CTime.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/CTime.o $(src)/CTime.c
	ar cr $(bin)/libCo.a $(bin)/CTime.o
	/bin/rm -f $(bin)/CTime.o

$(bin)/libCo.a(CJPEG.o) : $(src)/CJPEG.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/CJPEG.o $(src)/CJPEG.c
	ar cr $(bin)/libCo.a $(bin)/CJPEG.o
	/bin/rm -f $(bin)/CJPEG.o

$(bin)/libCo.a(CImageshort.o) : $(src)/CImageshort.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/CImageshort.o $(src)/CImageshort.c
	ar cr $(bin)/libCo.a $(bin)/CImageshort.o
	/bin/rm -f $(bin)/CImageshort.o

$(bin)/libCo.a(CSystem.o) : $(src)/CSystem.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/CSystem.o $(src)/CSystem.c
	ar cr $(bin)/libCo.a $(bin)/CSystem.o
	/bin/rm -f $(bin)/CSystem.o

$(bin)/libCo.a(CBatch.o) : $(src)/CBatch.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/CBatch.o $(src)/CBatch.c
	ar cr $(bin)/libCo.a $(bin)/CBatch.o
	/bin/rm -f $(bin)/CBatch.o

$(bin)/libCo.a(CoBinds.o) : $(src)/CoBinds.c
	$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/CoBinds.o $(src)/CoBinds.c
	ar cr $(bin)/libCo.a $(bin)/CoBinds.o
	/bin/rm -f $(bin)/CoBinds.o


