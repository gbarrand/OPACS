C/* +---------------------- Copyright notice -------------------------------+ */
C/* | Copyright (C) 1995, Guy Barrand, LAL Orsay. (barrand@lalcls.in2p3.fr) | */
C/* |   Permission to use, copy, modify, and distribute this software       | */
C/* |   and its documentation for any purpose and without fee is hereby     | */
C/* |   granted, provided that the above copyright notice appear in all     | */
C/* |   copies and that both that copyright notice and this permission      | */
C/* |   notice appear in supporting documentation.  This software is        | */
C/* |   provided "as is" without express or implied warranty.               | */
C/* +---------------------- Copyright notice -------------------------------+ */
C************************************************************
      SUBROUTINE OHBOOK ()
C************************************************************
C For VMS mms.
C First routine must have same name as file name so that
C descrip.mms works fine with lib dependencies.
C!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IMPLICIT NONE
C............................................................
      RETURN 
      END
C************************************************************
      SUBROUTINE OPAWI ()
C************************************************************
C  On AIX, to initialize common variables, DATA xxx/0/ involves link problems. 
C  We then use the OPAWI routine to do the job. It must be called
C before ANY CERNLIB routine that use PAWC, ZUNIT.
C  It is called in XzTypes.c/XzSetTypes.     
C!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IMPLICIT NONE
C+KEEP, PAWC.
      INTEGER     NWPAW,IXPAWC,IHDIV,IXHIGZ,IXKU
      COMMON/PAWC/NWPAW,IXPAWC,IHDIV,IXHIGZ,IXKU
C+KEEP, ZUNIT. 
      INTEGER IQREAD,IQPRNT,IQPR2,IQLOG,IQPNCH,IQTTIN,IQTYPE
      COMMON /ZUNIT/ IQREAD,IQPRNT,IQPR2,IQLOG,IQPNCH,IQTTIN,IQTYPE
C
      INTEGER FIRST
      SAVE    FIRST
      DATA    FIRST/0/
C............................................................
      IF(FIRST.EQ.0) THEN
C        PRINT *,'Initializing PAWC, ZUNIT commons.'
        NWPAW  = 0
        IXPAWC = 0
        IHDIV  = 0
        IXHIGZ = 0
        IXKU   = 0
        IQREAD = 0
C
        FIRST  = 1
      ENDIF
      RETURN 
      END
C************************************************************
C*** over HBOOK routines manipulating directories ***********
C************************************************************
C************************************************************
      SUBROUTINE OHINIT (SIZE)
C************************************************************
C!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IMPLICIT NONE
      INTEGER  SIZE
C+KEEP, PAWC.
      INTEGER     NWPAW,IXPAWC,IHDIV,IXHIGZ,IXKU
      COMMON/PAWC/NWPAW,IXPAWC,IHDIV,IXHIGZ,IXKU
C+KEEP, ZUNIT. 
      INTEGER IQREAD,IQPRNT,IQPR2,IQLOG,IQPNCH,IQTTIN,IQTYPE
      COMMON /ZUNIT/ IQREAD,IQPRNT,IQPR2,IQLOG,IQPNCH,IQTTIN,IQTYPE
C
      INTEGER NPAW,NKUIP,LENGTH
C............................................................
      CALL OPAWC(NPAW,NKUIP)
C      PRINT *,'DEBUG : OHINIT : NWPAW ',NPAW
      IF((SIZE.GT.NPAW).OR.(SIZE.LE.0)) THEN
        SIZE = NPAW        
      ENDIF
C      PRINT *,'DEBUG : OHINIT : IQREAD H1',IQREAD,NWPAW
      IF(IQREAD.EQ.0) THEN
C MZEBRA NOT DONE
        LENGTH =  SIZE
      ELSE 
C MZEBRA DONE
C IF MZPAW DONE NWPAW=NW SET BY MZPAW(NW)
        IF(NWPAW.EQ.0) THEN
C MZPAW NOT CALLED
          LENGTH = -SIZE
        ELSE
C MZEBRA AND MZPAW DONE
C HLIMIT CALLED INVOLVES IHDIV NE NULL.
C          PRINT *,'DEBUG : OHINIT : IXPAWC,IHDIV',IXPAWC,IHDIV
          LENGTH = 0
          IF(IHDIV.NE.0) RETURN
C HLIMIT NOT CALLED
        ENDIF
      ENDIF
C      PRINT *,'DEBUG : OHINIT : LENGTH ',LENGTH
C IF(LENGTH>0) CALL MZEBRA AND MZPAW
C IF(LENGTH=0) NO CALL TO MZEBRA AND MZPAW
C IF(LENGTH<0) CALL MZPAW
      CALL HLIMIT(LENGTH)
      RETURN 
      END
C************************************************************
      INTEGER FUNCTION OHOPEN (IUNIT,SFNAM,LFNAM,SDNAME,LDNAME,MODE)
C************************************************************
C!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IMPLICIT NONE
C  DO NOT PUT  *(*) SFNAM DUE TO C/FORTRAN LINKAGE ON VM
      INTEGER       IUNIT
      CHARACTER*256 SFNAM
      INTEGER       LFNAM
      CHARACTER*80  SDNAME
      INTEGER       LDNAME
      INTEGER       MODE
C
      INTEGER       ISTAT
C............................................................
           IF(MODE.EQ.1) THEN
         CALL HROPEN(IUNIT,SDNAME(1:LDNAME),SFNAM(1:LFNAM),
     +   'N',1024, ISTAT )
      ELSE IF(MODE.EQ.2) THEN
         CALL HROPEN(IUNIT,SDNAME(1:LDNAME),SFNAM(1:LFNAM),
     +   'U',1024, ISTAT )
      ELSE
         CALL HROPEN(IUNIT,SDNAME(1:LDNAME),SFNAM(1:LFNAM),
     +   ' ',1024, ISTAT )
      ENDIF
      IF ( ISTAT .NE. 0 ) THEN
         PRINT *, ' FAILURE TO OPEN FILE: ',SFNAM(1:LFNAM)
         OHOPEN = 0
         RETURN
      ENDIF
      OHOPEN = 1
      RETURN 
      END
C************************************************************
      SUBROUTINE OHREND (IUNIT,SDNAME,LDNAME)
C************************************************************
C!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IMPLICIT NONE
C  DO NOT PUT  *(*) SDNAME DUE TO C/FORTRAN LINKAGE ON VM
      INTEGER       IUNIT
      CHARACTER*80  SDNAME
      INTEGER       LDNAME
C............................................................
      CALL HDELET (0)
      CALL HREND  (SDNAME(1:LDNAME))
      IF(IUNIT.NE.0) THEN
        CLOSE (IUNIT)
      ENDIF
      RETURN 
      END
C************************************************************
      SUBROUTINE OHROUT (IDENT)
C************************************************************
C!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IMPLICIT NONE
      INTEGER       IDENT
      INTEGER       ICYCL
C............................................................
      CALL HROUT (IDENT,ICYCL,' ')
      RETURN 
      END
C************************************************************
      SUBROUTINE OHLNXT (IDH,SCHTYP,LCHTYP,
     +                       SCHTIT,LCHTIT,
     +                       SCHOPT,LCHOPT)
C************************************************************
C!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IMPLICIT NONE
C  DO NOT PUT  *(*) SCHTYP ,SCHTIT ,SCHOPT DUE TO C/FORTRAN LINKAGE ON VM
      INTEGER        IDH
      CHARACTER*80   SCHTYP,SCHTIT,SCHOPT
      INTEGER        LCHTYP,LCHTIT,LCHOPT
C............................................................
      CALL HLNEXT (IDH,SCHTYP,SCHTIT,SCHOPT(1:LCHOPT))
      CALL STGLEN (SCHTYP,LCHTYP)
      CALL STGLEN (SCHTIT,LCHTIT)
      CALL STGLEN (SCHOPT,LCHOPT)
      RETURN 
      END
C************************************************************
      SUBROUTINE OHPWD (SDNAME,LDNAME)
C************************************************************
C!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IMPLICIT NONE
C  DO NOT PUT  *(*) SDNAME DUE TO C/FORTRAN LINKAGE ON VM
      CHARACTER*80  SDNAME
      INTEGER       LDNAME
C............................................................
      CALL HCDIR  (SDNAME,'R')
      CALL STGLEN (SDNAME,LDNAME)
      RETURN
      END
C************************************************************
      INTEGER FUNCTION OHCD (SDNAME,LDNAME)
C************************************************************
C!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IMPLICIT NONE
C  DO NOT PUT  *(*) SDNAME DUE TO C/FORTRAN LINKAGE ON VM
      CHARACTER*80  SDNAME
      INTEGER       LDNAME
C+KEEP,QUEST
      INTEGER IQUEST
      COMMON/QUEST/IQUEST(100)
C............................................................
      CALL HCDIR   (SDNAME(1:LDNAME),' ')
      IF(IQUEST(1).EQ.1) THEN
        OHCD = 0
      ELSE
        OHCD = 1
      ENDIF
      RETURN
      END
C************************************************************
      SUBROUTINE OHLSR ()
C************************************************************
C!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IMPLICIT NONE
C............................................................
      CALL HLDIR (' ','T')
      RETURN
      END
C************************************************************
C************************************************************
C****** Global sections *************************************
C************************************************************
C************************************************************
C****** VMS *************************************************
C************************************************************
C************************************************************
      INTEGER FUNCTION OHGMAP (SDNAME,LDNAME,LENGTH)
C************************************************************
C!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IMPLICIT NONE
      CHARACTER*80  SDNAME
      INTEGER       LDNAME
      INTEGER       LENGTH
C
      INTEGER       H
      COMMON /PAWC/ H(5)
C
      EXTERNAL      HMAPG
      INTEGER       HMAPG
      INTEGER       IOFFSET
C............................................................
      LENGTH  = HMAPG(SDNAME(1:LDNAME),H(1),IOFFSET)
      IF ( LENGTH .LE. 0 ) THEN
         PRINT *, ' FAILURE TO MAP SECTION: ',SDNAME(1:LDNAME)
         LENGTH = 0
         OHGMAP = 0
         RETURN
      ENDIF
      OHGMAP = IOFFSET
      RETURN 
      END
C************************************************************
      SUBROUTINE OHGR (SDNAME,LDNAME,IOFFSET)
C************************************************************
C!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IMPLICIT NONE
      CHARACTER*80  SDNAME
      INTEGER       LDNAME
      INTEGER       IOFFSET
C
      INTEGER       H
      COMMON /PAWC/ H(5)
C............................................................
      CALL HRFILE   (H(IOFFSET),SDNAME(1:LDNAME),'G')
      RETURN 
      END
C************************************************************
      SUBROUTINE OHGUNM (IOFFSET,LENGTH)
C************************************************************
C!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IMPLICIT NONE
      INTEGER       IOFFSET
      INTEGER       LENGTH
C
      INTEGER       H
      COMMON /PAWC/ H(5)
C
      EXTERNAL HFREEG
      INTEGER  HFREEG
C
      INTEGER STATUS
C............................................................
      STATUS = HFREEG (LENGTH,H(1),IOFFSET)
      IF(STATUS.LE.0) THEN
        PRINT *,'GLOBAL SECTION BADLY UNMAPPED.'
      ENDIF
      RETURN 
      END
C************************************************************
C******* UNIX ***********************************************
C************************************************************
      INTEGER FUNCTION OHMMAP (SDNAME,LDNAME)
C************************************************************
C!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IMPLICIT NONE
      CHARACTER*80  SDNAME
      INTEGER       LDNAME
C
      INTEGER       H
      COMMON /PAWC/ H(5)
C
      EXTERNAL      HMAPM
      INTEGER       HMAPM
      INTEGER       RETVAL,IOFFSET
C............................................................
      RETVAL  = HMAPM(SDNAME(1:LDNAME),H(1),IOFFSET)
      IF ( RETVAL .NE. 0 ) THEN
         PRINT *, ' FAILURE TO MAP SECTION: ',SDNAME(1:LDNAME)
         OHMMAP = 0
         RETURN
      ENDIF
      OHMMAP = IOFFSET
      RETURN 
      END
C************************************************************
      SUBROUTINE OHMR (SDNAME,LDNAME,IOFFSET)
C************************************************************
C!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IMPLICIT NONE
      CHARACTER*80  SDNAME
      INTEGER       LDNAME
      INTEGER       IOFFSET
C
      INTEGER       H
      COMMON /PAWC/ H(5)
C............................................................
      CALL HRFILE   (H(IOFFSET+1),SDNAME(1:LDNAME),'M')
      RETURN 
      END
C************************************************************
      SUBROUTINE OHMUNM (IOFFSET)
C************************************************************
C!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IMPLICIT NONE
      INTEGER  IOFFSET
C
      EXTERNAL HFREEM
      INTEGER  HFREEM
C
      INTEGER STATUS
C............................................................
      STATUS = HFREEM (IOFFSET)
      IF(STATUS.NE.0) THEN
        PRINT *,'GLOBAL SECTION BADLY UNMAPPED.'
      ENDIF
      RETURN 
      END
C************************************************************
C*** over HBOOK routines working on objects in //PAWC *******
C************************************************************
C************************************************************
      SUBROUTINE OHBK1 (ID,SNAME,LNAME,XN,XMIN,XMAX,VMX)
C************************************************************
C!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IMPLICIT NONE
C  DO NOT PUT  *(*) SNAME DUE TO C/FORTRAN LINKAGE ON VM
      INTEGER       ID
      CHARACTER*80  SNAME
      INTEGER       LNAME
      INTEGER       XN
      REAL*4        XMIN,XMAX
      REAL*4        VMX
C............................................................
      CALL HBOOK1 (ID,SNAME(1:LNAME),XN,XMIN,XMAX,VMX)
      RETURN 
      END
C************************************************************
      SUBROUTINE OHBK2 (ID,SNAME,LNAME,XN,XMIN,XMAX,YN,YMIN,YMAX,VMX)
C************************************************************
C!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IMPLICIT NONE
C  DO NOT PUT  *(*) SNAME DUE TO C/FORTRAN LINKAGE ON VM
      INTEGER       ID
      CHARACTER*80  SNAME
      INTEGER       LNAME
      INTEGER       XN
      REAL*4        XMIN,XMAX
      INTEGER       YN
      REAL*4        YMIN,YMAX
      REAL*4        VMX
C............................................................
      CALL HBOOK2 (ID,SNAME(1:LNAME),XN,XMIN,XMAX,YN,YMIN,YMAX,VMX)
      RETURN 
      END
C************************************************************
      SUBROUTINE OHBKP (ID,SNAME,LNAME,XN,XMIN,XMAX,YMIN,YMAX,
     +                  SCHOPT,LCHOPT)
C************************************************************
C!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IMPLICIT NONE
C  DO NOT PUT  *(*) SNAME DUE TO C/FORTRAN LINKAGE ON VM
      INTEGER       ID
      CHARACTER*80  SNAME
      INTEGER       LNAME
      INTEGER       XN
      REAL*4        XMIN,XMAX
      REAL*4        YMIN,YMAX
      CHARACTER*80  SCHOPT
      INTEGER       LCHOPT
C............................................................
      CALL HBPROF (ID,SNAME(1:LNAME),XN,XMIN,XMAX,YMIN,YMAX,
     +             SCHOPT(1:LCHOPT))
      RETURN 
      END
C************************************************************
      SUBROUTINE OHRAZ (ID)
C************************************************************
C!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IMPLICIT NONE
      INTEGER       ID
C............................................................
      CALL HRESET (ID,'  ')
      RETURN 
      END
C************************************************************
      SUBROUTINE OHSTAT (ID,IAX,MEAN,SIGMA)
C************************************************************
C!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IMPLICIT NONE
      INTEGER  ID,IAX
      REAL     MEAN,SIGMA
      EXTERNAL HSTATI
      REAL     HSTATI
C............................................................
      IF(IAX.EQ.0) THEN
        MEAN  = -1.
        SIGMA = -1.
      ELSE IF(IAX.EQ.1) THEN
        MEAN  = -1.
        SIGMA = -1.
      ELSE
        MEAN  = HSTATI (ID,1,' ',0)
        SIGMA = HSTATI (ID,2,' ',0)
      ENDIF
      RETURN
      END
C************************************************************
      SUBROUTINE OHTIT (SOPT,LOPT)
C************************************************************
C!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IMPLICIT NONE
      CHARACTER*80 SOPT
      INTEGER      LOPT
C............................................................
      CALL HTITLE (SOPT(1:LOPT))
      RETURN
      END
C************************************************************
      INTEGER FUNCTION OHTYPE (ID)
C************************************************************
      IMPLICIT NONE
      INTEGER ID

      CHARACTER*80 TNAME
      INTEGER      NINOUT
      INTEGER      NVARS
      PARAMETER   (NVARS = 500)
      CHARACTER*8  TAGS(NVARS)
      REAL         XLOW(NVARS),XHIGH(NVARS)

C RESULTS OF HGIVE
      CHARACTER*80  CHTIT
      INTEGER       NX, NY, NWT, LOC
      REAL          XMI, XMA, YMI, YMA
C............................................................
      OHTYPE = 0
      CALL HGIVE ( ID, CHTIT, NX, XMI, XMA,
     +                         NY, YMI, YMA, NWT, LOC)
      IF ( NX .EQ. 0 .AND.
     +     NY .EQ. 0) THEN
        NINOUT = NVARS
        CALL HGIVEN(ID, TNAME, NINOUT, TAGS, XLOW, XHIGH )
        IF ( NINOUT .NE. 0 ) THEN
          OHTYPE = 3
        ENDIF
      ELSE IF ( NY .NE. 0) THEN
        OHTYPE = 2
      ELSE
        OHTYPE = 1
      ENDIF
      RETURN
      END
C************************************************************
      SUBROUTINE OHGIVH (ID,SCHTIT,LCHTIT,NX,XMI,XMA,NY,YMI,YMA)
C************************************************************
C!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IMPLICIT NONE
      INTEGER       ID
      CHARACTER*80  SCHTIT
      INTEGER       LCHTIT
      INTEGER       NX, NY
      REAL          XMI, XMA, YMI, YMA

      INTEGER       NWT, LOC
C............................................................
      NX  = 0
      XMI = 0
      XMA = 0
      NY  = 0
      YMI = 0
      YMA = 0
      CALL HGIVE ( ID, SCHTIT, NX, XMI, XMA,
     +                         NY, YMI, YMA, NWT, LOC)
      CALL STGLEN (SCHTIT,LCHTIT)
      RETURN
      END
C************************************************************
      SUBROUTINE OHBKN (ID,SCHTIT,LCHTIT,COLN,ROWN,STAGS,LSTAGS)
C************************************************************
C!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IMPLICIT NONE
      INTEGER        ID
      CHARACTER*80   SCHTIT
      INTEGER        LCHTIT
      INTEGER        COLN,ROWN
      CHARACTER*4000 STAGS
      INTEGER        LSTAGS
C
      INTEGER      NVARS
      PARAMETER   (NVARS = 500)
      CHARACTER*8  TAGS(NVARS)
      INTEGER      SIZE,IBEG,ICOL,IDUM
C............................................................
      IF(COLN.LE.0)   RETURN
      IF(COLN.GT.500) RETURN
      IF(ROWN.LE.0) THEN
        SIZE = 1000
      ELSE
        SIZE = COLN * ROWN
      ENDIF
      IBEG = 1
      DO 10 ICOL = 1,COLN
        TAGS(ICOL)(1:8) = STAGS(IBEG:IBEG+7)
        IBEG = IBEG + 8
 10   CONTINUE
      CALL HBOOKN (ID,SCHTIT(1:LCHTIT),COLN,' ',SIZE,TAGS)
      IDUM = LSTAGS
      RETURN 
      END
C************************************************************
      SUBROUTINE OHGIVN (ID,SCHTIT,LCHTIT,NCOL,NROW,STAGS,LSTAGS)
C************************************************************
C!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IMPLICIT NONE
      INTEGER        ID
      CHARACTER*80   SCHTIT
      INTEGER        LCHTIT
      INTEGER        NCOL,NROW
      CHARACTER*4000 STAGS
      INTEGER        LSTAGS
C N-TUPLE INFORMATION RETURNED BY HGIVEN...
      INTEGER      NINOUT
      INTEGER      NVARS
      PARAMETER   (NVARS = 500)
      CHARACTER*8  TAGS(NVARS)
      REAL         XLOW(NVARS), XHIGH(NVARS)
      INTEGER      ICOL,IBEG
C............................................................
      NINOUT = NVARS
      CALL HGIVEN (ID, SCHTIT, NINOUT, TAGS, XLOW, XHIGH )
      CALL STGLEN (SCHTIT,LCHTIT)
      NCOL = NINOUT
      NROW = 0
      IF(NINOUT.GT.0) THEN
         CALL OHNROW (ID,NROW)
         IBEG = 1
         DO 10 ICOL = 1,NCOL
            STAGS(IBEG:IBEG+7) = TAGS(ICOL)(1:8)
            IBEG = IBEG + 8
 10      CONTINUE
         LSTAGS = IBEG - 1
      ELSE
        LSTAGS = 0
      ENDIF
      RETURN
      END
C************************************************************
      SUBROUTINE OHIDOP (ID,SOPT,LOPT)
C************************************************************
C!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IMPLICIT     NONE
      INTEGER      ID
      CHARACTER*80 SOPT
      INTEGER      LOPT
C............................................................
      CALL HIDOPT (ID,SOPT(1:LOPT))
      RETURN
      END
C************************************************************
C************************************************************
C************************************************************
C************************************************************
      SUBROUTINE OHNROW (IDN,NROW)
C************************************************************
C** 20/8/93. From HGN code taken on cernvn:
C          gime cernpams
C          x hbook car z
C!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IMPLICIT NONE
      INTEGER  IDN,NROW
C+KEEP,HCBOOK
C*CMZ :  4.19/01 30/04/93  17.22.15  by  Rene Brun
      INTEGER     NWPAW,IXPAWC,IHDIV,IXHIGZ,IXKU,        LMAIN
      REAL                                       FENC   ,      HCV
      COMMON/PAWC/NWPAW,IXPAWC,IHDIV,IXHIGZ,IXKU,FENC(5),LMAIN,HCV(9989)
      INTEGER   IQ        ,LQ
      REAL            Q
      DIMENSION IQ(2),Q(2),LQ(8000)
      EQUIVALENCE (LQ(1),LMAIN),(IQ(1),LQ(9)),(Q(1),IQ(1))
      INTEGER       HVERSN,IHWORK,LHBOOK,LHPLOT,LGTIT,LHWORK,
     +LCDIR,LSDIR,LIDS,LTAB,LCID,LCONT,LSCAT,LPROX,LPROY,LSLIX,
     +LSLIY,LBANX,LBANY,LPRX,LPRY,LFIX,LLID,LR1,LR2,LNAME,LCHAR,LINT,
     +LREAL,LBLOK,LLBLK,LBUFM,LBUF,LTMPM,LTMP,LTMP1,LHPLIP,LHDUM,
     +LHFIT,LFUNC,LHFCO,LHFNA,LCIDN
      COMMON/HCBOOK/HVERSN,IHWORK,LHBOOK,LHPLOT,LGTIT,LHWORK,
     +LCDIR,LSDIR,LIDS,LTAB,LCID,LCONT,LSCAT,LPROX,LPROY,LSLIX,
     +LSLIY,LBANX,LBANY,LPRX,LPRY,LFIX,LLID,LR1,LR2,LNAME,LCHAR,LINT,
     +LREAL,LBLOK,LLBLK,LBUFM,LBUF,LTMPM,LTMP,LTMP1,LHPLIP,LHDUM(9),
     +LHFIT,LFUNC,LHFCO,LHFNA,LCIDN
C.......................................................................
      NROW = 0
      CALL HGNPAR(IDN,'HGN   ')
      IF(LCIDN.LE.0) RETURN
      NROW   = IQ(LCIDN+3)
      RETURN
      END
C************************************************************
      INTEGER*4 FUNCTION OHISP (IDN)
C************************************************************
C  Inquire if histo is a profile.
C  From HFILL code.
C!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IMPLICIT NONE
      INTEGER  IDN
C+KEEP,HCBOOK
C*CMZ :  4.19/01 30/04/93  17.22.15  by  Rene Brun
      INTEGER     NWPAW,IXPAWC,IHDIV,IXHIGZ,IXKU,        LMAIN
      REAL                                       FENC   ,      HCV
      COMMON/PAWC/NWPAW,IXPAWC,IHDIV,IXHIGZ,IXKU,FENC(5),LMAIN,HCV(9989)
      INTEGER   IQ        ,LQ
      REAL            Q
      DIMENSION IQ(2),Q(2),LQ(8000)
      EQUIVALENCE (LQ(1),LMAIN),(IQ(1),LQ(9)),(Q(1),IQ(1))
      INTEGER       HVERSN,IHWORK,LHBOOK,LHPLOT,LGTIT,LHWORK,
     +LCDIR,LSDIR,LIDS,LTAB,LCID,LCONT,LSCAT,LPROX,LPROY,LSLIX,
     +LSLIY,LBANX,LBANY,LPRX,LPRY,LFIX,LLID,LR1,LR2,LNAME,LCHAR,LINT,
     +LREAL,LBLOK,LLBLK,LBUFM,LBUF,LTMPM,LTMP,LTMP1,LHPLIP,LHDUM,
     +LHFIT,LFUNC,LHFCO,LHFNA,LCIDN
      COMMON/HCBOOK/HVERSN,IHWORK,LHBOOK,LHPLOT,LGTIT,LHWORK,
     +LCDIR,LSDIR,LIDS,LTAB,LCID,LCONT,LSCAT,LPROX,LPROY,LSLIX,
     +LSLIY,LBANX,LBANY,LPRX,LPRY,LFIX,LLID,LR1,LR2,LNAME,LCHAR,LINT,
     +LREAL,LBLOK,LLBLK,LBUFM,LBUF,LTMPM,LTMP,LTMP1,LHPLIP,LHDUM(9),
     +LHFIT,LFUNC,LHFCO,LHFNA,LCIDN
C+KEEP,HCK
C*CMZ :  4.19/00 26/04/93  12.34.29  by  Rene Brun
      INTEGER   KNCX   ,KXMIN  ,KXMAX  ,KMIN1  ,KMAX1 ,KNORM  , KTIT1,
     +          KNCY   ,KYMIN  ,KYMAX  ,KMIN2  ,KMAX2 ,KSCAL2 , KTIT2,
     +          KNBIT  ,KNOENT ,KSTAT1 ,KNSDIR  ,KNRH ,
     +          KCON1  ,KCON2  ,KBITS  ,KNTOT
      PARAMETER(KNCX=3,KXMIN=4,KXMAX=5,KMIN1=7,KMAX1=8,KNORM=9,KTIT1=10,
     +          KNCY=7,KYMIN=8,KYMAX=9,KMIN2=6,KMAX2=10,KSCAL2=11,
     +          KTIT2=12,KNBIT=1,KNOENT=2,KSTAT1=3,KNSDIR=5,KNRH=6,
     +          KCON1=9,KCON2=3,KBITS=1,KNTOT=2)
C
      INTEGER*4 IDPOS
      INTEGER*4 LOCATI,JBIT
C.......................................................................
      OHISP = 0
      IDPOS = LOCATI(IQ(LTAB+1),IQ(LCDIR+KNRH),IDN)
      IF(IDPOS.LE.0)RETURN
      OHISP = JBIT(IQ(LQ(LTAB-IDPOS)+KBITS),8)
      RETURN
      END
C***********************************************************************
C******* Utilities *****************************************************
C***********************************************************************
C***********************************************************************
      SUBROUTINE STGLEN (CHAR,LN)
C***********************************************************************
C!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IMPLICIT NONE
      CHARACTER*(*) CHAR
      INTEGER IASCII
      CHARACTER CH
      INTEGER LN,I
      INTEGER ICHAR
C.......................................................................
      IF(ICHAR(CHAR(1:1)) .EQ. 0) THEN
        LN = 0
        RETURN
      ENDIF
      LN = LEN(CHAR)
      DO 10 I=1,LN
        CH     = CHAR((LN+1-I):(LN+1-I))
        IASCII = ICHAR(CH)
        IF(IASCII.EQ.0)  GOTO 10
        IF(CH.NE.' ') GOTO 20
   10 CONTINUE
   20 LN = LN-I+1
      RETURN
      END
