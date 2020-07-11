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
      SUBROUTINE OGEANT ()
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
      SUBROUTINE OGCBANK (NGEANT)
C************************************************************
C!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IMPLICIT NONE
      INTEGER NGEANT
C+GCBANK
      INTEGER NWGEANT
C      PARAMETER (NWGEANT=500000)
      PARAMETER (NWGEANT=1000000)
      INTEGER NZEBRA
      REAL GVERSN,ZVERSN
      INTEGER Q
      COMMON/GCBANK/NZEBRA,GVERSN,ZVERSN,Q(NWGEANT)
C Init of IQREAD also done in Xz/opawc.f then could have link pbs.
C We hope that compiler init it to 0 !
C+KEEP, ZUNIT. 
C      INTEGER IQREAD,IQPRNT,IQPR2,IQLOG,IQPNCH,IQTTIN,IQTYPE
C      COMMON /ZUNIT/ IQREAD,IQPRNT,IQPR2,IQLOG,IQPNCH,IQTTIN,IQTYPE
C      DATA IQREAD/0/
C
      DATA NZEBRA/0/
      DATA GVERSN,ZVERSN/0.0,0.0/
C
C............................................................
      NGEANT = NWGEANT
      END
C************************************************************
      SUBROUTINE OGINIT ()
C************************************************************
C GZEBRA : call MZEBRA and set NZEBRA to NWGEANT
C  if MZEBRA already done (by HLIMIT) then problems.
C GZINIT : init GEANT/ZBOOK data structure. It 
C  is protected against multiple call by 
C  flag IFINIT(2) in GFLAG common.
C!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IMPLICIT NONE
C+KEEP,GCBANK
      INTEGER IQ,LQ,NZEBRA,IXSTOR,IXDIV,IXCONS,LMAIN,LR1
      INTEGER KWBANK,KWWORK,IWS
      REAL GVERSN,ZVERSN,FENDQ,WS,Q
      PARAMETER (KWBANK=69000,KWWORK=5200)
      COMMON/GCBANK/NZEBRA,GVERSN,ZVERSN,IXSTOR,IXDIV,IXCONS,FENDQ(16)
     +             ,LMAIN,LR1,WS(KWBANK)
      DIMENSION IQ(2),Q(2),LQ(8000),IWS(2)
      EQUIVALENCE (Q(1),IQ(1),LQ(9)),(LQ(1),LMAIN),(IWS(1),WS(1))
C+KEEP, ZUNIT. 
      INTEGER IQREAD,IQPRNT,IQPR2,IQLOG,IQPNCH,IQTTIN,IQTYPE
      COMMON /ZUNIT/ IQREAD,IQPRNT,IQPR2,IQLOG,IQPNCH,IQTTIN,IQTYPE
C
      INTEGER ICALL
      DATA ICALL/0/
      SAVE ICALL
      INTEGER NGEANT
C............................................................
      IF (ICALL.EQ.1) RETURN
      ICALL = 1
C
C..  IF NOT DONE; INITIALIZE ZEBRA and GCBANK
C      PRINT *,'DEBUG : OGINIT : NZEBRA,KWBANK,IQREAD ',
C     + NZEBRA,KWBANK,IQREAD
      IF(NZEBRA.LT.KWBANK) THEN
C        CALL GZEBRA (NWGEANT)
        CALL OGCBANK (NGEANT)
        NZEBRA=NGEANT
        IF(IQREAD.EQ.0) THEN
C MZEBRA NOT DONE
           CALL MZEBRA (-3)
        ENDIF
        CALL MZSTOR(IXSTOR,'/GCBANK/',' ',FENDQ,LQ,LR1,WS,LQ(KWWORK+100)
     +        ,LQ(NZEBRA-30))
        CALL MZLOGL(IXSTOR,0)
      ENDIF

C..  IF NOT DONE; INITIALISE GEANT
C      PRINT *,'DEBUG : OGINIT : NZEBRA ',GVERSN,ZVERSN
      IF(GVERSN.EQ.0.AND.ZVERSN.EQ.0) THEN
        CALL GINIT
      ENDIF

      CALL GZINIT
C
      RETURN
      END
C************************************************************
      INTEGER FUNCTION OGEVBG()
C************************************************************
C Produce one event, clearing of event is made at
C next event production, so at the end of this routine
C event is there and could be displayed.
C!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IMPLICIT NONE
C+KEEP,GTFLAG.
      INTEGER       IDEBUG,IDEMIN,IDEMAX,ITEST,IDRUN,IDEVT,IEORUN
     +        ,IEOTRI,IEVENT,ISWIT,IFINIT,NEVENT,NRNDM
C+KEEP,GCFLAG
      COMMON/GCFLAG/IDEBUG,IDEMIN,IDEMAX,ITEST,IDRUN,IDEVT,IEORUN
     +        ,IEOTRI,IEVENT,ISWIT(10),IFINIT(20),NEVENT,NRNDM(2)
C
C............................................................
      IEVENT=IEVENT+1
*
*             Initialises event partition
*
      CALL GTRIGI
*
*             Process one event (trigger)
*
      CALL GTRIG
*
*
      OGEVBG = IEVENT
      RETURN
      END
C************************************************************
      INTEGER FUNCTION OGKINN ()
C************************************************************
C return number of KINE
C!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IMPLICIT NONE
C+KEEP,GCLINK
      INTEGER       JDIGI ,JDRAW ,JHEAD ,JHITS ,JKINE ,JMATE ,JPART
     +      ,JROTM ,JRUNG ,JSET  ,JSTAK ,JGSTAT,JTMED ,JTRACK,JVERTX
     +      ,JVOLUM,JXYZ  ,JGPAR ,JGPAR2,JSKLT
      COMMON/GCLINK/JDIGI ,JDRAW ,JHEAD ,JHITS ,JKINE ,JMATE ,JPART
     +      ,JROTM ,JRUNG ,JSET  ,JSTAK ,JGSTAT,JTMED ,JTRACK,JVERTX
     +      ,JVOLUM,JXYZ  ,JGPAR ,JGPAR2,JSKLT
C+KEEP,GCNUM.
      INTEGER      NMATE ,NVOLUM,NROTM,NTMED,NTMULT,NTRACK,NPART
     +            ,NSTMAX,NVERTX,NHEAD,NBIT ,NALIVE,NTMSTO
      COMMON/GCNUM/NMATE ,NVOLUM,NROTM,NTMED,NTMULT,NTRACK,NPART
     +            ,NSTMAX,NVERTX,NHEAD,NBIT
      COMMON /GCNUMX/ NALIVE,NTMSTO
C............................................................
      OGKINN = 0
      IF (JKINE.LE.0) RETURN
      OGKINN = NTRACK
      RETURN
      END
C************************************************************
      SUBROUTINE OGKINV (IOBJ,IPROP,IVAL,RVAL,SVAL)
C************************************************************
C return KINE property value
C!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IMPLICIT NONE
      INTEGER IOBJ,IPROP,IVAL
      REAL RVAL
      CHARACTER*20 SVAL
C+KEEP,GCBANK
      INTEGER IQ,LQ,NZEBRA,IXSTOR,IXDIV,IXCONS,LMAIN,LR1,JCG
      INTEGER KWBANK,KWWORK,IWS
      REAL GVERSN,ZVERSN,FENDQ,WS,Q
      PARAMETER (KWBANK=69000,KWWORK=5200)
      COMMON/GCBANK/NZEBRA,GVERSN,ZVERSN,IXSTOR,IXDIV,IXCONS,FENDQ(16)
     +             ,LMAIN,LR1,WS(KWBANK)
      DIMENSION IQ(2),Q(2),LQ(8000),IWS(2)
      EQUIVALENCE (Q(1),IQ(1),LQ(9)),(LQ(1),LMAIN),(IWS(1),WS(1))
      EQUIVALENCE (JCG,JGSTAT)
C+KEEP,GCLINK
      INTEGER       JDIGI ,JDRAW ,JHEAD ,JHITS ,JKINE ,JMATE ,JPART
     +      ,JROTM ,JRUNG ,JSET  ,JSTAK ,JGSTAT,JTMED ,JTRACK,JVERTX
     +      ,JVOLUM,JXYZ  ,JGPAR ,JGPAR2,JSKLT
      COMMON/GCLINK/JDIGI ,JDRAW ,JHEAD ,JHITS ,JKINE ,JMATE ,JPART
     +      ,JROTM ,JRUNG ,JSET  ,JSTAK ,JGSTAT,JTMED ,JTRACK,JVERTX
     +      ,JVOLUM,JXYZ  ,JGPAR ,JGPAR2,JSKLT
C
C+KEEP,GCNUM.
      INTEGER      NMATE ,NVOLUM,NROTM,NTMED,NTMULT,NTRACK,NPART
     +            ,NSTMAX,NVERTX,NHEAD,NBIT ,NALIVE,NTMSTO
      COMMON/GCNUM/NMATE ,NVOLUM,NROTM,NTMED,NTMULT,NTRACK,NPART
     +            ,NSTMAX,NVERTX,NHEAD,NBIT
      COMMON /GCNUMX/ NALIVE,NTMSTO
C
      INTEGER JK,IPART,JPA
C............................................................
      IVAL = 0
      RVAL = 0.
      IF (JKINE.LE.0) RETURN
      JK     = LQ(JKINE-IOBJ)
C
      IF(IPROP.EQ.1) THEN
         IF (JPART.LE.0) RETURN
         IPART  = Q(JK+5)
         JPA    = LQ(JPART-IPART)
         CALL   UHTOC(IQ(JPA+1),4,SVAL,20)
      ELSE IF(IPROP.EQ.2) THEN
         RVAL   = Q(JK+4)
      ELSE IF(IPROP.EQ.3) THEN
         IF (JPART.LE.0) RETURN
         IPART  = Q(JK+5)
         JPA    = LQ(JPART-IPART)
         IVAL   = Q(JPA+6)
      ENDIF
C
      RETURN
      END
C************************************************************
      SUBROUTINE OGKIND(IT)
C************************************************************
C     Draws track IT (KINE) and secondaries
C   for which space points have been stored
C   in banks JXYZ by GSXYZ.
C             ITRTYP  PARTICLE  |       HIGZ         |
C             ----------------------------------------
C                 1   gammas    | dotted      blue   |
C                 2   electrons | solid       red    |
C                 3   neutral   | dot-dashed  black  |
C                 4   hadrons   | solid       red    |
C                 5   muons     | dashed      green  |
C                 6   geantino  | dot-dashed  black  |
C                 7   cerenkov  | dotted      yellow |
C                 8   ions      | solid       light b|
C!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IMPLICIT NONE
      INTEGER IT
C+KEEP,GCBANK
      INTEGER IQ,LQ,NZEBRA,IXSTOR,IXDIV,IXCONS,LMAIN,LR1,JCG
      INTEGER KWBANK,KWWORK,IWS
      REAL GVERSN,ZVERSN,FENDQ,WS,Q
      PARAMETER (KWBANK=69000,KWWORK=5200)
      COMMON/GCBANK/NZEBRA,GVERSN,ZVERSN,IXSTOR,IXDIV,IXCONS,FENDQ(16)
     +             ,LMAIN,LR1,WS(KWBANK)
      DIMENSION IQ(2),Q(2),LQ(8000),IWS(2)
      EQUIVALENCE (Q(1),IQ(1),LQ(9)),(LQ(1),LMAIN),(IWS(1),WS(1))
      EQUIVALENCE (JCG,JGSTAT)
C+KEEP,GCLINK
      INTEGER       JDIGI ,JDRAW ,JHEAD ,JHITS ,JKINE ,JMATE ,JPART
     +      ,JROTM ,JRUNG ,JSET  ,JSTAK ,JGSTAT,JTMED ,JTRACK,JVERTX
     +      ,JVOLUM,JXYZ  ,JGPAR ,JGPAR2,JSKLT
      COMMON/GCLINK/JDIGI ,JDRAW ,JHEAD ,JHITS ,JKINE ,JMATE ,JPART
     +      ,JROTM ,JRUNG ,JSET  ,JSTAK ,JGSTAT,JTMED ,JTRACK,JVERTX
     +      ,JVOLUM,JXYZ  ,JGPAR ,JGPAR2,JSKLT
C+KEEP,GCMAIL
      COMMON/GCMAIL/CHMAIL
      CHARACTER*132 CHMAIL
C
      INTEGER JX,IPOINT,NPOINT
      INTEGER NXYZ
C
      INTEGER  ISCN
C
      INTEGER MODE(8),COLO(8)
      SAVE MODE,COLO
      DATA MODE /3, 1, 4, 1, 2, 1, 3, 1/
C...INDEX IN ocmap_X
C   blue 3, red 47, black 2, green 23, grey 37 
C                 1   gammas    | dotted      blue   |
C                 2   electrons | solid       red    |
C                 3   neutral   | dot-dashed  black  |
C                 4   hadrons   | solid       red    |
C                 5   muons     | dashed      green  |
C                 6   geantino  | dot-dashed  black  |
C                 7   cerenkov  | dotted      yellow |
C                 8   ions      | solid       light b|
C      DATA COLO /3, 47 , 2, 47 , 23 , 2/
C black -> grey.
      DATA COLO /3, 47 , 37, 47 , 23 , 37, 63, 7/
      INTEGER IPART,JPA,ITRTYP,IMOD,ICOL
C............................................................
      IF (JXYZ.LE.0) THEN
        WRITE(CHMAIL,'(A)') 'Bank JXYZ empty'
        CALL GMAIL(0,0)
        RETURN
      ENDIF
C...NXYZ is not the number of track.
      NXYZ = IQ(JXYZ-2)
      IF(IT.LE.0)    RETURN
      IF(IT.GT.NXYZ) RETURN
C
      JX = LQ(JXYZ-IT)
      IF(JX.LE.0)    RETURN
C
      ISCN   = 1
C...Do primary+secondaries in same KINE object
      IPOINT = JX+3
 10   IF(IPOINT.GT.JX+IQ(JX-1)) GOTO 50
      NPOINT = Q(IPOINT)
      IF(NPOINT.LE.0) GOTO 50
C
      IPART  = Q(IPOINT+1)
      JPA    = LQ(JPART-IPART)
      ITRTYP = Q(JPA+6)
      IMOD   = MODE(ITRTYP)-1
      ICOL   = COLO(ITRTYP)
      CALL GOASET (ICOL,IMOD,1,1,1,1)
C
      IF(NPOINT.EQ.1) THEN
        CALL GOPMAR(1,Q(IPOINT+2),
     +                Q(IPOINT+3),
     +                Q(IPOINT+4))
      ELSE     
        CALL GOGLIN(NPOINT,Q(IPOINT+2))
      ENDIF
      IPOINT = IPOINT + 2 + 3 * NPOINT
C
      ISCN = ISCN + 1
      GO TO 10
C
 50   CONTINUE
C     
      RETURN
      END
C************************************************************
      INTEGER FUNCTION OGHITV (IUSET,IUDET,IHIT,
     & VOLN,VOLS,HITN,HITS,ITRA)
C************************************************************
C Get infos of a hit.
C!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IMPLICIT NONE
      CHARACTER*4 IUSET,IUDET
      INTEGER IHIT
      INTEGER VOLS(50)
      REAL    HITS(50)
      INTEGER VOLN,HITN,ITRA
C+KEEP,GCBANK
      INTEGER IQ,LQ,NZEBRA,IXSTOR,IXDIV,IXCONS,LMAIN,LR1,JCG
      INTEGER KWBANK,KWWORK,IWS
      REAL GVERSN,ZVERSN,FENDQ,WS,Q
      PARAMETER (KWBANK=69000,KWWORK=5200)
      COMMON/GCBANK/NZEBRA,GVERSN,ZVERSN,IXSTOR,IXDIV,IXCONS,FENDQ(16)
     +             ,LMAIN,LR1,WS(KWBANK)
      DIMENSION IQ(2),Q(2),LQ(8000),IWS(2)
      EQUIVALENCE (Q(1),IQ(1),LQ(9)),(LQ(1),LMAIN),(IWS(1),WS(1))
      EQUIVALENCE (JCG,JGSTAT)
C+KEEP,GCLINK
      INTEGER       JDIGI ,JDRAW ,JHEAD ,JHITS ,JKINE ,JMATE ,JPART
     +      ,JROTM ,JRUNG ,JSET  ,JSTAK ,JGSTAT,JTMED ,JTRACK,JVERTX
     +      ,JVOLUM,JXYZ  ,JGPAR ,JGPAR2,JSKLT
      COMMON/GCLINK/JDIGI ,JDRAW ,JHEAD ,JHITS ,JKINE ,JMATE ,JPART
     +      ,JROTM ,JRUNG ,JSET  ,JSTAK ,JGSTAT,JTMED ,JTRACK,JVERTX
     +      ,JVOLUM,JXYZ  ,JGPAR ,JGPAR2,JSKLT
C
      INTEGER NSET,ISET,JS,NDET,IDET,JD,JH
      INTEGER JHD,JIHIT,NW,NV,NVM,NK,K,IV,NB,JDH
      INTEGER NH,NHM,IH,KHIT
      INTEGER    NMAX
      PARAMETER (NMAX=50)
C............................................................
      OGHITV = 0
      IF (JSET.LE.0)  RETURN
      NSET = IQ(JSET-1)
      CALL GLOOK(IUSET,IQ(JSET+1),NSET,ISET)
      IF (ISET .LE.0) RETURN
      JS   = LQ(JSET -ISET)
      IF(JS.LE.0) RETURN
      NDET = IQ(JS-1)
      CALL GLOOK(IUDET,IQ(JS+1),NDET,IDET)
      IF (IDET .LE.0) RETURN
      JD = LQ(JS-IDET)
      IF(JD.LE.0) RETURN
      JDH=LQ(JD-1)
      IF(JDH.LE.0) RETURN

      NW = IQ(JD+1) + IQ(JD+3) + 1

      IF (JHITS.LE.0)  RETURN
      JH = LQ(JHITS-ISET)
      IF(JH.LE.0) RETURN
      JHD = LQ(JH-IDET)
      IF (JHD .LE.0) RETURN

      JIHIT = JHD + 1 + (IHIT-1) * NW 

      ITRA = IQ(JIHIT)

C...Get unpacked volume numbers
      NV  = IQ(JD+2)
      NVM = MIN(NV,NMAX)
      NK  = 1
      IF(NV.GT.0)THEN
         K=1
         DO 50 IV=1,NVM
            NB=IQ(JD+2*IV+10)
            IF(NB.LE.0)THEN
               IF(K.GT.1)THEN
                   K=1
                   NK=NK+1
               ENDIF
               VOLS(IV)=IQ(JIHIT+NK)
               IF(IV.NE.NV)NK=NK+1
            ELSE
               IF(K+NB.GT.33)THEN
                  K=1
                  NK=NK+1
               ENDIF
               VOLS(IV)=IBITS(IQ(JIHIT+NK),K-1,NB)
               K=K+NB
            ENDIF
 50      CONTINUE
         NK=NK+1
      ENDIF
      VOLN = NVM

C...Get unpacked hits
C             Hits origin is shifted . Division by scale factor
C
      NH  = IQ(JD+4)
      NHM = MIN(NH,NMAX)
      IF(NH.GT.0)THEN
         K=1
         DO 90 IH=1,NHM
            NB=IQ(JDH+4*IH-2)
            IF(NB.LE.0)THEN
               IF(K.GT.1)THEN
                   NK=NK+1
               ENDIF
               KHIT=IQ(JIHIT+NK)
               K=1
               NK=NK+1
            ELSE
               IF(K+NB.GT.33)THEN
                  K=1
                  NK=NK+1
               ENDIF
               KHIT=IBITS(IQ(JIHIT+NK),K-1,NB)
               K=K+NB
            ENDIF
            HITS(IH)=FLOAT(KHIT)/Q(JDH+4*IH) - Q(JDH+4*IH-1)
 90      CONTINUE
      ENDIF
      HITN   = NHM
      OGHITV = 1
      RETURN
      END
C************************************************************
      INTEGER*4 FUNCTION OGVOLD (NAME,VOLN,VOLS,DVOLS)
C************************************************************
C!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IMPLICIT NONE
      CHARACTER*4 NAME
      INTEGER VOLN
      INTEGER VOLS(50)
      INTEGER DVOLS(50)
C+KEEP,GCBANK
      INTEGER IQ,LQ,NZEBRA,IXSTOR,IXDIV,IXCONS,LMAIN,LR1,JCG
      INTEGER KWBANK,KWWORK,IWS
      REAL GVERSN,ZVERSN,FENDQ,WS,Q
      PARAMETER (KWBANK=69000,KWWORK=5200)
      COMMON/GCBANK/NZEBRA,GVERSN,ZVERSN,IXSTOR,IXDIV,IXCONS,FENDQ(16)
     +             ,LMAIN,LR1,WS(KWBANK)
      DIMENSION IQ(2),Q(2),LQ(8000),IWS(2)
      EQUIVALENCE (Q(1),IQ(1),LQ(9)),(LQ(1),LMAIN),(IWS(1),WS(1))
      EQUIVALENCE (JCG,JGSTAT)
C+KEEP,GCLINK
      INTEGER       JDIGI ,JDRAW ,JHEAD ,JHITS ,JKINE ,JMATE ,JPART
     +      ,JROTM ,JRUNG ,JSET  ,JSTAK ,JGSTAT,JTMED ,JTRACK,JVERTX
     +      ,JVOLUM,JXYZ  ,JGPAR ,JGPAR2,JSKLT
      COMMON/GCLINK/JDIGI ,JDRAW ,JHEAD ,JHITS ,JKINE ,JMATE ,JPART
     +      ,JROTM ,JRUNG ,JSET  ,JSTAK ,JGSTAT,JTMED ,JTRACK,JVERTX
     +      ,JVOLUM,JXYZ  ,JGPAR ,JGPAR2,JSKLT
C+KEEP,GCNUM.
      INTEGER      NMATE ,NVOLUM,NROTM,NTMED,NTMULT,NTRACK,NPART
     +            ,NSTMAX,NVERTX,NHEAD,NBIT ,NALIVE,NTMSTO
      COMMON/GCNUM/NMATE ,NVOLUM,NROTM,NTMED,NTMULT,NTRACK,NPART
     +            ,NSTMAX,NVERTX,NHEAD,NBIT
      COMMON /GCNUMX/ NALIVE,NTMSTO
C+KEEP,GCVOLU
      INTEGER NLEVEL,NAMES,NUMBER,LVOLUM,LINDEX,INFROM,NLEVMX,
     +        NLDEV,LINMX
      REAL GTRAN,GRMAT,GONLY,GLX
      COMMON/GCVOLU/NLEVEL,NAMES(15),NUMBER(15),
     +LVOLUM(15),LINDEX(15),INFROM,NLEVMX,NLDEV(15),LINMX(15),
     +GTRAN(3,15),GRMAT(10,15),GONLY(15),GLX(3)
C+KEEP,GCMAIL
      COMMON/GCMAIL/CHMAIL
      CHARACTER*132 CHMAIL
C
      REAL    X(3),ATT(10)
      INTEGER LVOLS(15),LINDX(15),LNAMES(15)
      REAL    GPAR(50,15),WORK,SEEN
      INTEGER I,J,K,KLEV,LLL,NLMIN,NLMAX,ISHAPE
      INTEGER NIN,ICOPY,IER,NLVTOP,NPAR,NATT,IN
      INTEGER JVO,JVOF,IVO,JIN,JATT,IFCVOL,LEVSEE,NLEV
      INTEGER ISEENL
C
      INTEGER IFOUND,VOLI,VOLJ
      INTEGER FVOLS(50)
C............................................................
      OGVOLD = 0 
      IF (JVOLUM.LE.0) THEN
        WRITE(CHMAIL,'(A)') 'Bank JVOLUM empty'
        CALL GMAIL(0,0)
        RETURN
      ENDIF
C
      IF(VOLN.NE.0) IFOUND = 0
C
C             Save /GCVOLU/ if necessary
C
      IFCVOL=0
      IF (NLEVEL.NE.0) THEN
         CALL GSCVOL
         IFCVOL=1
      ENDIF
      IF (NLEVEL.LT.0) NLEVEL=IABS(NLEVEL)
C
C
C             Start of general code
C
      CALL GLOOK(NAME,IQ(JVOLUM+1),NVOLUM,IVO)
      IF(IVO.LE.0) THEN
        WRITE(CHMAIL,10000)NAME
        CALL GMAIL(0,0)
        GO TO 290
      ENDIF
C
C
      JVO=LQ(JVOLUM-IVO)
C
C             Initialize JIN to switch correctly CALL GFPARA/GFIPAR
C
      JIN=0
C
      LEVSEE=1000
C
C             Initialize for new geometry structure
C
      IF (JGPAR.EQ.0) CALL GMEDIN
      CALL GLMOTH(NAME,1,NLEV,LVOLS,LINDX)
      DO 50 J=1, NLEV
         LNAMES(J)=IQ(JVOLUM+LVOLS(J))
 50   CONTINUE
      NLEV=NLEV+1
      CALL UCTOH(NAME,LNAMES(NLEV),4,4)
      LINDX(NLEV)=1
      DO 1212 KLEV=2,NLEV
         JVOF = LQ(JVOLUM-LVOLS(KLEV-1))
         NIN  = Q(JVOF+3)
         IF(NIN.GT.0) THEN
            JIN = LQ(JVOF-LINDX(KLEV))
            ICOPY = Q(JIN+3)
         ELSE
            ICOPY = 1
         ENDIF
         LINDX(KLEV) = ICOPY
 1212 CONTINUE
      CALL GLVOLU(NLEV, LNAMES, LINDX, IER)
C
      NLVTOP=NLEVEL
      NLMIN=NLEVEL
      NLMAX=NLEVEL
C
      CALL GFPARA
     +     (NAME,1,1,NPAR,NATT,GPAR(1,NLEVEL),ATT)
C
      IF (NPAR.LE.0) THEN
         WRITE(CHMAIL,10100) NAME
         CALL GMAIL(0,0)
         GO TO 290
      ENDIF
C     
      DO 90  LLL=1,NLEVEL
         DO 80 I=1,3
            GTRAN(I,LLL)=0.0
            X(I)=0.0
            DO 70 J=1,3
               K=(I-1)*3+J
               GRMAT(K,LLL)=0.0
 70         CONTINUE
            K=I*4-3
            GRMAT(K,LLL)=1.0
 80      CONTINUE
         GRMAT(10,LLL)=0.0
 90   CONTINUE
C
C   Main loop, it loop over JVO occurences
C    - volume 'NAME' set 
C    - sons  (GOTO 210)
C
  100 CONTINUE
C
C  here we works on JVO
C
C Get attributes
      ISHAPE=Q(JVO+2)
C
      IF (NLEVEL.NE.NLVTOP) THEN
         NPAR=Q(JVO+5)
         NATT=Q(JVO+6)
         JATT=JVO+7+NPAR
         CALL UCOPY(Q(JATT),ATT,NATT)
      ENDIF
C
      WORK  =ATT(1)
      SEEN  =ATT(2)
C
C             WORK attribute enabled ?
C
      IF(WORK.LE.0.)GO TO 260
C
C             SEEN attribute processing
C
      IF (SEEN.GE.50.) THEN
         ISEENL=SEEN/10.+0.5
         SEEN=ISEENL-10
      ENDIF
      IF(NLEVEL.LE.LEVSEE)LEVSEE=1000
      IF(SEEN.EQ.-1.)GO TO 260
      IF (NLEVEL.GT.LEVSEE) GO TO 260
      IF(SEEN.EQ.0.)GO TO 210
C
C SEEN=-2,1  VOL_VIS_SON_*
C
      IF (SEEN.EQ.-2.) LEVSEE=NLEVEL
C
C Represent the shape
C
      IF(VOLN.EQ.0) THEN
        CALL GOGSHP (ISHAPE,
     +  GPAR(1,NLEVEL),GRMAT(1,NLEVEL),GTRAN(1,NLEVEL))
      ELSE
        CALL VZERO (FVOLS,VOLN)
        DO 10 VOLI = VOLN,1,-1
           DO 20 VOLJ = NLEVEL,1,-1
              IF( 
     +          (VOLS(VOLI).EQ.NUMBER(VOLJ))
     +          .AND.(DVOLS(VOLI).EQ.NAMES(VOLJ))
     +          ) FVOLS(VOLI) = VOLJ
 20        CONTINUE
 10     CONTINUE
        DO 30 VOLI = 1,VOLN
           IF(FVOLS(VOLI).EQ.0) GOTO 11
 30     CONTINUE
        IFOUND = 1
        CALL GOGSHP (ISHAPE,
     +       GPAR(1,NLEVEL),GRMAT(1,NLEVEL),GTRAN(1,NLEVEL))
 11     CONTINUE
      ENDIF
C
      JVO=LQ(JVOLUM-IVO)
C
      IF(SEEN.EQ.-2.)GO TO 260
C
  210 CONTINUE
C
C  Sons
C
      NIN = Q(JVO+3)
      IF(NIN.EQ.0) GO TO 260
      IF(NIN.GT.0) THEN
C
C             Contents placed by GSPOS
C
         IN=0
         IF(NLMAX.GT.NLEVEL) IN=LINDEX(NLEVEL+1)
         IN=IN+1
         IF(IN.GT.NIN.AND.NLEVEL.EQ.NLMIN) GO TO 290
         IF(IN.GT.NIN) GO TO 250
C
C set GRMAT,GTRAN
C
         CALL GMEPOS(JVO,IN,X,0)
         NPAR=IQ(JGPAR+NLEVEL)
         DO 220 I=1,NPAR
            GPAR(I,NLEVEL)=Q(LQ(JGPAR-NLEVEL)+I)
 220     CONTINUE
*
         IVO=LVOLUM(NLEVEL)
         JVO=LQ(JVOLUM-IVO)
         NLMAX=NLEVEL
C
      ELSE
C
C             Contents by division
C
         IN=0
         IF(NLMAX.GT.NLEVEL) IN=LINDEX(NLEVEL+1)
         IN=IN+1
C
C set GRMAT,GTRAN
C
         CALL GMEDIV(JVO,IN,X,0)
         IF (IN.EQ.0) GO TO 250
         NPAR=IQ(JGPAR+NLEVEL)
         DO 240 I=1,NPAR
            GPAR(I,NLEVEL)=Q(LQ(JGPAR-NLEVEL)+I)
 240     CONTINUE
         IVO=LVOLUM(NLEVEL)
         JVO=LQ(JVOLUM-IVO)
         NLMAX=NLEVEL
C
      ENDIF
C
      GO TO 100
C
  250 CONTINUE
      NLMAX=NLEVEL
  260 CONTINUE
      NLEVEL=NLEVEL-1
      IF(NLEVEL.GE.NLMIN) THEN
        IVO=LVOLUM(NLEVEL)
        JVO=LQ(JVOLUM-IVO)
        GO TO 210
      ENDIF
C
  290 CONTINUE
C
      IF (IFCVOL.EQ.1) THEN
         CALL GFCVOL
      ELSE
         NLEVEL=0
      ENDIF
C
      IF(VOLN.NE.0.AND.IFOUND.EQ.0) THEN
         OGVOLD = 0
      ELSE
         OGVOLD = 1
      ENDIF
C
C
10000 FORMAT(' *** GDRAW *** : Volume ',A4,' does not exist')
10100 FORMAT(' *** GDRAW *** : Top of tree ',A4,' parameters defined',
     +       '  by GSPOSP - info not available to GDRAW.')
      RETURN
      END
C************************************************************
      SUBROUTINE OGRGEO (FMT,FNAME,LFNAME,LUN)
C************************************************************
C Read 'INIT' banks in file. This will give geometry.
C See GDraw.h for FMT encoding.
C!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IMPLICIT      NONE
      CHARACTER*256 FNAME
      INTEGER       LFNAME
      INTEGER       LUN
      INTEGER       FMT
C
      INTEGER       ISR,MODE
      INTEGER       LREC
      INTEGER       IERR,IDVERS
C
      INTEGER       NKEYS
      PARAMETER     (NKEYS=1)
      CHARACTER*4   KEYS(NKEYS)
      DATA          KEYS/'INIT'/
C............................................................
      ISR   = FMT/10
      MODE  = FMT - ISR * 10
      IF(ISR.EQ.1) THEN
C r* file.
        IF(MODE.EQ.0) CALL GRFILE (LUN,FNAME(1:LFNAME),'I')
C       IF(MODE.EQ.1) CALL GRFILE (LUN,FNAME(1:LFNAME),'IA')
        IF(MODE.EQ.2) CALL GRFILE (LUN,FNAME(1:LFNAME),'IX')
      ELSE IF(ISR.EQ.2) THEN
C f* file.
        LREC   = 1024
        IDVERS = 0
        IF(MODE.EQ.0) CALL GOPEN (LUN,'I' ,LREC,IERR)
        IF(MODE.EQ.1) CALL GOPEN (LUN,'IA',LREC,IERR)
        IF(MODE.EQ.2) CALL GOPEN (LUN,'IX',LREC,IERR)
        IF (IERR.NE.0) THEN 
          PRINT *,' OGRGEO: error from gopen ',
     +    IERR,' aborting geometry reading'
          RETURN
        ENDIF
        CALL GFIN   (LUN,KEYS,NKEYS,IDVERS,' ',IERR)
C Let open for futher event reading :
C        CALL GCLOSE (LUN,IERR)
C        CLOSE       (LUN)
C        IF (IERR.NE.0) THEN
C          PRINT *,' OGRGEO: error from gclose ',
C     +    IERR
C        ENDIF
      ENDIF
      RETURN
      END
C************************************************************
      SUBROUTINE OGWGEO (FMT,FNAME,LFNAME,LUN)
C************************************************************
C write 'INIT' banks in file. This will save geometry.
C See GDraw.h for FMT encoding.
C!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IMPLICIT      NONE
      CHARACTER*256 FNAME
      INTEGER       LFNAME
      INTEGER       LUN
      INTEGER       FMT
C
      INTEGER       ISR,MODE
      INTEGER       LREC
      INTEGER       IERR,IDVERS
C
      INTEGER       NKEYS
      PARAMETER     (NKEYS=1)
      CHARACTER*4   KEYS(NKEYS)
      DATA KEYS/'INIT'/
C............................................................
      ISR   = FMT/10
      MODE  = FMT - ISR * 10
      IF(ISR.EQ.1) THEN
C r* file.
        IF(MODE.EQ.0) CALL GRFILE (LUN,FNAME(1:LFNAME),'NO')
C       IF(MODE.EQ.1) CALL GRFILE (LUN,FNAME(1:LFNAME),'NOA')
        IF(MODE.EQ.2) CALL GRFILE (LUN,FNAME(1:LFNAME),'NOX')
      ELSE IF(ISR.EQ.2) THEN
C f* file.
        LREC   = 1024
        IDVERS = 0
        IF(MODE.EQ.0) CALL GOPEN (LUN,'O' ,LREC,IERR)
        IF(MODE.EQ.1) CALL GOPEN (LUN,'OA',LREC,IERR)
        IF(MODE.EQ.2) CALL GOPEN (LUN,'OX',LREC,IERR)
        IF (IERR.NE.0) THEN 
          PRINT *,' OGWGEO: error from gopen ',
     +    IERR,' aborting geometry writing'
          RETURN
        ENDIF
        CALL GFOUT  (LUN,KEYS,NKEYS,IDVERS,' ',IERR)
        CALL GCLOSE (LUN,IERR)
        IF (IERR.NE.0) THEN
          PRINT *,' OGWGEO: error from gclose ',
     +    IERR
        ENDIF
      ENDIF
      RETURN
      END
C************************************************************
      SUBROUTINE OGREVT(LUN)
C************************************************************
C Read 'KINE', 'JXYZ' banks.
C!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IMPLICIT      NONE
      INTEGER       LUN
C
      INTEGER       IDVERS
      INTEGER       IERR
C
      INTEGER       NKEYS
      PARAMETER     (NKEYS=3)
      CHARACTER*4   KEYS(NKEYS)
      DATA          KEYS/'KINE','JXYZ','HITS'/
C............................................................
      IDVERS = 0
      CALL GFIN(LUN,KEYS,NKEYS,IDVERS,' ',IERR)
      IF(IERR.NE.0) THEN
        PRINT *,' OGREVT: GFIN error ',IERR
      ENDIF
      RETURN
      END
C************************************************************
      INTEGER FUNCTION OGVOLT (NAME)
C************************************************************
C return top volume name
C!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IMPLICIT NONE
      CHARACTER*(4) NAME
C+KEEP,GCLINK
      INTEGER       JDIGI ,JDRAW ,JHEAD ,JHITS ,JKINE ,JMATE ,JPART
     +      ,JROTM ,JRUNG ,JSET  ,JSTAK ,JGSTAT,JTMED ,JTRACK,JVERTX
     +      ,JVOLUM,JXYZ  ,JGPAR ,JGPAR2,JSKLT
      COMMON/GCLINK/JDIGI ,JDRAW ,JHEAD ,JHITS ,JKINE ,JMATE ,JPART
     +      ,JROTM ,JRUNG ,JSET  ,JSTAK ,JGSTAT,JTMED ,JTRACK,JVERTX
     +      ,JVOLUM,JXYZ  ,JGPAR ,JGPAR2,JSKLT
C+KEEP,GCNUM.
      INTEGER      NMATE ,NVOLUM,NROTM,NTMED,NTMULT,NTRACK,NPART
     +            ,NSTMAX,NVERTX,NHEAD,NBIT ,NALIVE,NTMSTO
      COMMON/GCNUM/NMATE ,NVOLUM,NROTM,NTMED,NTMULT,NTRACK,NPART
     +            ,NSTMAX,NVERTX,NHEAD,NBIT
      COMMON /GCNUMX/ NALIVE,NTMSTO
C+KEEP,GCBANK
      INTEGER IQ,LQ,NZEBRA,IXSTOR,IXDIV,IXCONS,LMAIN,LR1,JCG
      INTEGER KWBANK,KWWORK,IWS
      REAL GVERSN,ZVERSN,FENDQ,WS,Q
      PARAMETER (KWBANK=69000,KWWORK=5200)
      COMMON/GCBANK/NZEBRA,GVERSN,ZVERSN,IXSTOR,IXDIV,IXCONS,FENDQ(16)
     +             ,LMAIN,LR1,WS(KWBANK)
      DIMENSION IQ(2),Q(2),LQ(8000),IWS(2)
      EQUIVALENCE (Q(1),IQ(1),LQ(9)),(LQ(1),LMAIN),(IWS(1),WS(1))
      EQUIVALENCE (JCG,JGSTAT)
C............................................................
      NAME(1:4) = 'XXXX'
      OGVOLT    = 0
      IF (JVOLUM.LE.0) RETURN
      IF (NVOLUM.LE.0) RETURN
      CALL UHTOC(IQ(JVOLUM+1),4,NAME,4)
      OGVOLT    = 1
      RETURN
      END
C************************************************************
C************************************************************
C************************************************************
C************************************************************
C************************************************************
      SUBROUTINE GMAIL(LINBEF,LINAFT)
C************************************************************
C  GMAIL rewritten to send output to oterm widget
C!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IMPLICIT      NONE
      INTEGER LINBEF,LINAFT
C
      COMMON/GCUNIT/LIN,LOUT,NUNITS,LUNITS(5)
      INTEGER LIN,LOUT,NUNITS,LUNITS
      COMMON/GCMAIL/CHMAIL
      CHARACTER*132 CHMAIL
C
      INTEGER I,NCH
C............................................................
      DO 10 I=1,LINBEF
        CALL CWRITE (' ',1)
   10 CONTINUE
      DO 20 NCH=132,1,-1
        IF (CHMAIL(NCH:NCH).NE.' ') GO TO 30
   20 CONTINUE
      NCH=1
   30 CONTINUE
      CALL CWRITE(CHMAIL(1:NCH),NCH)
      DO 40 I=1,LINAFT
        CALL CWRITE(' ',1)
   40 CONTINUE 
      RETURN
      END
C************************************************************
C************************************************************
C************************************************************
      INTEGER FUNCTION OGVOLN ()
C************************************************************
C return volume number
C!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IMPLICIT NONE
C+KEEP,GCLINK
      INTEGER       JDIGI ,JDRAW ,JHEAD ,JHITS ,JKINE ,JMATE ,JPART
     +      ,JROTM ,JRUNG ,JSET  ,JSTAK ,JGSTAT,JTMED ,JTRACK,JVERTX
     +      ,JVOLUM,JXYZ  ,JGPAR ,JGPAR2,JSKLT
      COMMON/GCLINK/JDIGI ,JDRAW ,JHEAD ,JHITS ,JKINE ,JMATE ,JPART
     +      ,JROTM ,JRUNG ,JSET  ,JSTAK ,JGSTAT,JTMED ,JTRACK,JVERTX
     +      ,JVOLUM,JXYZ  ,JGPAR ,JGPAR2,JSKLT
C+KEEP,GCNUM.
      INTEGER      NMATE ,NVOLUM,NROTM,NTMED,NTMULT,NTRACK,NPART
     +            ,NSTMAX,NVERTX,NHEAD,NBIT ,NALIVE,NTMSTO
      COMMON/GCNUM/NMATE ,NVOLUM,NROTM,NTMED,NTMULT,NTRACK,NPART
     +            ,NSTMAX,NVERTX,NHEAD,NBIT
      COMMON /GCNUMX/ NALIVE,NTMSTO
C............................................................
      OGVOLN    = 0
      IF (JVOLUM.LE.0) RETURN
      OGVOLN    = NVOLUM
      RETURN
      END
C************************************************************
      SUBROUTINE OGVONM (IVO,NAME)
C************************************************************
C From GLMOTH
C!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IMPLICIT NONE
      INTEGER*4   IVO
      CHARACTER*4 NAME
C+KEEP,GCBANK
      INTEGER IQ,LQ,NZEBRA,IXSTOR,IXDIV,IXCONS,LMAIN,LR1,JCG
      INTEGER KWBANK,KWWORK,IWS
      REAL GVERSN,ZVERSN,FENDQ,WS,Q
      PARAMETER (KWBANK=69000,KWWORK=5200)
      COMMON/GCBANK/NZEBRA,GVERSN,ZVERSN,IXSTOR,IXDIV,IXCONS,FENDQ(16)
     +             ,LMAIN,LR1,WS(KWBANK)
      DIMENSION IQ(2),Q(2),LQ(8000),IWS(2)
      EQUIVALENCE (Q(1),IQ(1),LQ(9)),(LQ(1),LMAIN),(IWS(1),WS(1))
      EQUIVALENCE (JCG,JGSTAT)
C+KEEP,GCLINK
      INTEGER       JDIGI ,JDRAW ,JHEAD ,JHITS ,JKINE ,JMATE ,JPART
     +      ,JROTM ,JRUNG ,JSET  ,JSTAK ,JGSTAT,JTMED ,JTRACK,JVERTX
     +      ,JVOLUM,JXYZ  ,JGPAR ,JGPAR2,JSKLT
      COMMON/GCLINK/JDIGI ,JDRAW ,JHEAD ,JHITS ,JKINE ,JMATE ,JPART
     +      ,JROTM ,JRUNG ,JSET  ,JSTAK ,JGSTAT,JTMED ,JTRACK,JVERTX
     +      ,JVOLUM,JXYZ  ,JGPAR ,JGPAR2,JSKLT
C............................................................
      NAME = '    '
      IF (JVOLUM.LE.0) RETURN
      CALL UHTOC(IQ(JVOLUM+IVO),4,NAME,4)
      RETURN
      END
C************************************************************
      SUBROUTINE OGMOTH (NAME,MOTHN,MOTHS,MOTHX)
C************************************************************
C From GLMOTH
C!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IMPLICIT NONE
      CHARACTER*4 NAME
      INTEGER*4   MOTHS(*),MOTHN,MOTHX
C+KEEP,GCBANK
      INTEGER IQ,LQ,NZEBRA,IXSTOR,IXDIV,IXCONS,LMAIN,LR1,JCG
      INTEGER KWBANK,KWWORK,IWS
      REAL GVERSN,ZVERSN,FENDQ,WS,Q
      PARAMETER (KWBANK=69000,KWWORK=5200)
      COMMON/GCBANK/NZEBRA,GVERSN,ZVERSN,IXSTOR,IXDIV,IXCONS,FENDQ(16)
     +             ,LMAIN,LR1,WS(KWBANK)
      DIMENSION IQ(2),Q(2),LQ(8000),IWS(2)
      EQUIVALENCE (Q(1),IQ(1),LQ(9)),(LQ(1),LMAIN),(IWS(1),WS(1))
      EQUIVALENCE (JCG,JGSTAT)
C+KEEP,GCLINK
      INTEGER       JDIGI ,JDRAW ,JHEAD ,JHITS ,JKINE ,JMATE ,JPART
     +      ,JROTM ,JRUNG ,JSET  ,JSTAK ,JGSTAT,JTMED ,JTRACK,JVERTX
     +      ,JVOLUM,JXYZ  ,JGPAR ,JGPAR2,JSKLT
      COMMON/GCLINK/JDIGI ,JDRAW ,JHEAD ,JHITS ,JKINE ,JMATE ,JPART
     +      ,JROTM ,JRUNG ,JSET  ,JSTAK ,JGSTAT,JTMED ,JTRACK,JVERTX
     +      ,JVOLUM,JXYZ  ,JGPAR ,JGPAR2,JSKLT
C+KEEP,GCNUM.
      INTEGER      NMATE ,NVOLUM,NROTM,NTMED,NTMULT,NTRACK,NPART
     +            ,NSTMAX,NVERTX,NHEAD,NBIT ,NALIVE,NTMSTO
      COMMON/GCNUM/NMATE ,NVOLUM,NROTM,NTMED,NTMULT,NTRACK,NPART
     +            ,NSTMAX,NVERTX,NHEAD,NBIT
      COMMON /GCNUMX/ NALIVE,NTMSTO
C+KEEP,GCMAIL
      COMMON/GCMAIL/CHMAIL
      CHARACTER*132 CHMAIL
C
      INTEGER*4 JVO,JDIV,JIN
      INTEGER*4 IVOT,IVOS,IVO,NIN,IN
C............................................................
      MOTHN = 0

      IF (JVOLUM.LE.0) THEN
        WRITE(CHMAIL,'(A)') 'Bank JVOLUM empty'
        CALL GMAIL(0,0)
        RETURN
      ENDIF

      CALL GLOOK(NAME,IQ(JVOLUM+1),NVOLUM,IVOS)
      IF(IVOS.LE.0) THEN
        WRITE(CHMAIL,10000) NAME
        CALL GMAIL(0,0)
        RETURN
      ENDIF

      DO 25 IVO = 1, NVOLUM

            IF (IVO.EQ.IVOS)            GO TO 25

            JVO = LQ(JVOLUM-IVO)
            NIN = Q(JVO+3)
            IF (NIN.EQ.0)               GO TO 25
C Division
            IF (NIN.LT.0) THEN
               JDIV = LQ(JVO-1)
               IVOT = Q(JDIV+2)
               IF (IVOT.NE.IVOS)        GO TO 25
C Sons
            ELSE
               DO 15 IN = 1, NIN
                  JIN  = LQ(JVO-IN)
                  IVOT = Q(JIN+2)
                  IF (IVOT.NE.IVOS)     GO TO 15
                  GO TO 20
   15          CONTINUE
               GO TO 25
            ENDIF
   20       CONTINUE
            IF(MOTHN.GE.MOTHX) THEN
               WRITE(CHMAIL,'(A)') 
     +         'OGMOTH: Buffer overflow'
               CALL GMAIL(0,0)
               RETURN
            ENDIF
            MOTHN = MOTHN + 1
            MOTHS(MOTHN) = IVO
 25      CONTINUE

      RETURN
10000 FORMAT(' *** OGMOTH *** : Volume ',A4,' does not exist')
      END             
C************************************************************
      SUBROUTINE OGVOLV (IVO,IPROP,IVAL,RVAL,SVAL,RARR)
C************************************************************
C VOLU property value
C From GLMOTH
C!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IMPLICIT NONE
      INTEGER*4   IVO,IPROP
      INTEGER*4   IVAL
      REAL        RVAL
      CHARACTER*4 SVAL
      REAL        RARR(*)
C+KEEP,GCBANK
      INTEGER IQ,LQ,NZEBRA,IXSTOR,IXDIV,IXCONS,LMAIN,LR1,JCG
      INTEGER KWBANK,KWWORK,IWS
      REAL GVERSN,ZVERSN,FENDQ,WS,Q
      PARAMETER (KWBANK=69000,KWWORK=5200)
      COMMON/GCBANK/NZEBRA,GVERSN,ZVERSN,IXSTOR,IXDIV,IXCONS,FENDQ(16)
     +             ,LMAIN,LR1,WS(KWBANK)
      DIMENSION IQ(2),Q(2),LQ(8000),IWS(2)
      EQUIVALENCE (Q(1),IQ(1),LQ(9)),(LQ(1),LMAIN),(IWS(1),WS(1))
      EQUIVALENCE (JCG,JGSTAT)
C+KEEP,GCLINK
      INTEGER       JDIGI ,JDRAW ,JHEAD ,JHITS ,JKINE ,JMATE ,JPART
     +      ,JROTM ,JRUNG ,JSET  ,JSTAK ,JGSTAT,JTMED ,JTRACK,JVERTX
     +      ,JVOLUM,JXYZ  ,JGPAR ,JGPAR2,JSKLT
      COMMON/GCLINK/JDIGI ,JDRAW ,JHEAD ,JHITS ,JKINE ,JMATE ,JPART
     +      ,JROTM ,JRUNG ,JSET  ,JSTAK ,JGSTAT,JTMED ,JTRACK,JVERTX
     +      ,JVOLUM,JXYZ  ,JGPAR ,JGPAR2,JSKLT
C
      CHARACTER*4 ISHAP(30),NAME
      REAL PAR(50),ATT(20)
      SAVE ISHAP
      DATA ISHAP/'BOX ','TRD1','TRD2','TRAP','TUBE','TUBS','CONE',
     +           'CONS','SPHE','PARA','PGON','PCON','ELTU','HYPE',
     +13*'    ', 'GTRA','CTUB','    '/
C
      INTEGER JVO,NPAR,NATT,IS
C............................................................
      IVAL = 0
      RVAL = 0.
      SVAL = '    '
      IF (JVOLUM.LE.0) RETURN
      JVO = LQ(JVOLUM-IVO)
      IF (JVO.LE.0)    RETURN
C
      IF(IPROP.EQ.1) THEN
        CALL UHTOC(IQ(JVOLUM+IVO),4,SVAL,4)
        RETURN
      ELSE IF(IPROP.EQ.2) THEN
        IVAL = Q(JVO+4)
        RETURN
      ELSE IF(IPROP.EQ.3) THEN
        IS   = Q(JVO+2)
        SVAL = ISHAP(IS)
        RETURN
      ELSE IF(IPROP.EQ.4) THEN
        CALL UHTOC  (IQ(JVOLUM+IVO),4,NAME,4)
        CALL GFPARA (NAME,1,1,NPAR,NATT,PAR,ATT)
        IVAL = NPAR
        RETURN
      ELSE IF(IPROP.EQ.5) THEN
        CALL UHTOC  (IQ(JVOLUM+IVO),4,NAME,4)
        CALL GFPARA (NAME,1,1,NPAR,NATT,RARR,ATT)
        IVAL = NPAR
        RETURN
      ENDIF
      RETURN
      END
C************************************************************
      INTEGER FUNCTION OGDETN ()
C************************************************************
C!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IMPLICIT NONE
C+KEEP,GCBANK
      INTEGER IQ,LQ,NZEBRA,IXSTOR,IXDIV,IXCONS,LMAIN,LR1,JCG
      INTEGER KWBANK,KWWORK,IWS
      REAL GVERSN,ZVERSN,FENDQ,WS,Q
      PARAMETER (KWBANK=69000,KWWORK=5200)
      COMMON/GCBANK/NZEBRA,GVERSN,ZVERSN,IXSTOR,IXDIV,IXCONS,FENDQ(16)
     +             ,LMAIN,LR1,WS(KWBANK)
      DIMENSION IQ(2),Q(2),LQ(8000),IWS(2)
      EQUIVALENCE (Q(1),IQ(1),LQ(9)),(LQ(1),LMAIN),(IWS(1),WS(1))
      EQUIVALENCE (JCG,JGSTAT)
C+KEEP,GCLINK
      INTEGER       JDIGI ,JDRAW ,JHEAD ,JHITS ,JKINE ,JMATE ,JPART
     +      ,JROTM ,JRUNG ,JSET  ,JSTAK ,JGSTAT,JTMED ,JTRACK,JVERTX
     +      ,JVOLUM,JXYZ  ,JGPAR ,JGPAR2,JSKLT
      COMMON/GCLINK/JDIGI ,JDRAW ,JHEAD ,JHITS ,JKINE ,JMATE ,JPART
     +      ,JROTM ,JRUNG ,JSET  ,JSTAK ,JGSTAT,JTMED ,JTRACK,JVERTX
     +      ,JVOLUM,JXYZ  ,JGPAR ,JGPAR2,JSKLT
C+KEEP,GCMAIL
      COMMON/GCMAIL/CHMAIL
      CHARACTER*132 CHMAIL
C
      INTEGER NSET,ISET,JS,NDET,IDET,JD
C............................................................
      OGDETN = 0
      IF (JSET.LE.0)  RETURN
      NSET = IQ(JSET-1)
      DO 10 ISET=1,NSET
        JS = LQ(JSET-ISET)
        IF(JS.LE.0) GOTO 10
          NDET = IQ(JS-1)
          DO 20 IDET=1,NDET
            JD = LQ(JS-IDET)
            IF(JD.LE.0) GOTO 20
            OGDETN = OGDETN + 1
 20       CONTINUE
 10   CONTINUE
      RETURN
      END
C************************************************************
      SUBROUTINE OGDETV (IOBJ,IPROP,IVAL,RVAL,SVAL,IARR)
C************************************************************
C UDET property value
C!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IMPLICIT NONE
      INTEGER*4   IOBJ,IPROP
      INTEGER*4   IVAL
      REAL        RVAL
      CHARACTER*4 SVAL
      INTEGER     IARR(*)
C+KEEP,GCBANK
      INTEGER IQ,LQ,NZEBRA,IXSTOR,IXDIV,IXCONS,LMAIN,LR1,JCG
      INTEGER KWBANK,KWWORK,IWS
      REAL GVERSN,ZVERSN,FENDQ,WS,Q
      PARAMETER (KWBANK=69000,KWWORK=5200)
      COMMON/GCBANK/NZEBRA,GVERSN,ZVERSN,IXSTOR,IXDIV,IXCONS,FENDQ(16)
     +             ,LMAIN,LR1,WS(KWBANK)
      DIMENSION IQ(2),Q(2),LQ(8000),IWS(2)
      EQUIVALENCE (Q(1),IQ(1),LQ(9)),(LQ(1),LMAIN),(IWS(1),WS(1))
      EQUIVALENCE (JCG,JGSTAT)
C+KEEP,GCLINK
      INTEGER       JDIGI ,JDRAW ,JHEAD ,JHITS ,JKINE ,JMATE ,JPART
     +      ,JROTM ,JRUNG ,JSET  ,JSTAK ,JGSTAT,JTMED ,JTRACK,JVERTX
     +      ,JVOLUM,JXYZ  ,JGPAR ,JGPAR2,JSKLT
      COMMON/GCLINK/JDIGI ,JDRAW ,JHEAD ,JHITS ,JKINE ,JMATE ,JPART
     +      ,JROTM ,JRUNG ,JSET  ,JSTAK ,JGSTAT,JTMED ,JTRACK,JVERTX
     +      ,JVOLUM,JXYZ  ,JGPAR ,JGPAR2,JSKLT
C
      INTEGER NSET,ISET,JS,NDET,IDET,JD,ICOUNT,NV,I
C............................................................
      IVAL = 0
      RVAL = 0.
      SVAL = '    '
C
      ICOUNT = 0      
      IF (JSET.LE.0)  RETURN
      NSET = IQ(JSET-1)
      DO 10 ISET=1,NSET
        JS = LQ(JSET-ISET)
        IF(JS.LE.0) GOTO 10
          NDET = IQ(JS-1)
          DO 20 IDET=1,NDET
            JD = LQ(JS-IDET)
            IF(JD.LE.0) GOTO 20
            ICOUNT = ICOUNT + 1
            IF(ICOUNT.EQ.IOBJ) GOTO 30
 20       CONTINUE
 10   CONTINUE
      RETURN
C
 30   IF(IPROP.EQ.1) THEN
        CALL UHTOC(IQ(JS+IDET),4,SVAL,4)
        RETURN
      ELSE IF(IPROP.EQ.2) THEN
        CALL UHTOC(IQ(JSET+ISET),4,SVAL,4)
        RETURN
      ELSE IF(IPROP.EQ.3) THEN
        IVAL = IQ(JD+2)
        RETURN
      ELSE IF(IPROP.EQ.4) THEN
         NV = IQ(JD+2)
         DO 40 I=1,NV
            IARR(I) = IQ(JD+2*I+ 9)
  40     CONTINUE
         IVAL = NV
         RETURN
      ENDIF
      RETURN
      END
C************************************************************
      INTEGER FUNCTION OGDIGV (IUSET,IUDET,IDIG,VOLN,VOLS,DIGN,DIGS)
C************************************************************
C Get volumes and data of a digi.
C From GFDIGI.
C!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IMPLICIT NONE
      CHARACTER*4 IUSET,IUDET
      INTEGER IDIG
      INTEGER VOLS(50)
      INTEGER DIGS(50)
      INTEGER VOLN,DIGN
C+KEEP,GCBANK
      INTEGER IQ,LQ,NZEBRA,IXSTOR,IXDIV,IXCONS,LMAIN,LR1,JCG
      INTEGER KWBANK,KWWORK,IWS
      REAL GVERSN,ZVERSN,FENDQ,WS,Q
      PARAMETER (KWBANK=69000,KWWORK=5200)
      COMMON/GCBANK/NZEBRA,GVERSN,ZVERSN,IXSTOR,IXDIV,IXCONS,FENDQ(16)
     +             ,LMAIN,LR1,WS(KWBANK)
      DIMENSION IQ(2),Q(2),LQ(8000),IWS(2)
      EQUIVALENCE (Q(1),IQ(1),LQ(9)),(LQ(1),LMAIN),(IWS(1),WS(1))
      EQUIVALENCE (JCG,JGSTAT)
C+KEEP,GCLINK
      INTEGER       JDIGI ,JDRAW ,JHEAD ,JHITS ,JKINE ,JMATE ,JPART
     +      ,JROTM ,JRUNG ,JSET  ,JSTAK ,JGSTAT,JTMED ,JTRACK,JVERTX
     +      ,JVOLUM,JXYZ  ,JGPAR ,JGPAR2,JSKLT
      COMMON/GCLINK/JDIGI ,JDRAW ,JHEAD ,JHITS ,JKINE ,JMATE ,JPART
     +      ,JROTM ,JRUNG ,JSET  ,JSTAK ,JGSTAT,JTMED ,JTRACK,JVERTX
     +      ,JVOLUM,JXYZ  ,JGPAR ,JGPAR2,JSKLT
C
      INTEGER NSET,ISET,JS,JDI,NDET,IDET,JD,JDID,JDDI
      INTEGER NV,ND,ILAST,I,NWDI,K,NTRM1,NTRT,NWTR,NK
      INTEGER ICOUNT,IV,NB,ID
      INTEGER NVMAX
      PARAMETER (NVMAX=50)
C............................................................
      OGDIGV = 0
      IF(JDIGI.LE.0) RETURN
      NSET=IQ(JSET-1)
      CALL GLOOK(IUSET,IQ(JSET+1),NSET,ISET)
      IF(ISET.LE.0) RETURN
C
      JS=LQ(JSET-ISET)
      JDI=LQ(JDIGI-ISET)
      IF(JS.LE.0)  RETURN
      IF(JDI.LE.0) RETURN
      NDET=IQ(JS-1)
      CALL GLOOK(IUDET,IQ(JS+1),NDET,IDET)
      IF(IDET.EQ.0) RETURN
C
      JD=LQ(JS-IDET)
      JDID=LQ(JDI-IDET)
      IF(JDID.LE.0) RETURN
      JDDI=LQ(JD-2)
C
      ILAST=IQ(JDI+IDET)
      IF(ILAST.EQ.0) RETURN
C
      ICOUNT = 0
      I      = 0
      NWDI   = 0
 10   CONTINUE
      I    = I + NWDI
      IF(I.GE.ILAST) RETURN
      NWDI = IQ(JDID+I+1)
      ICOUNT = ICOUNT + 1
      IF(IDIG.EQ.ICOUNT) GOTO 20
      GOTO 10

 20   NK   = 2
      NTRM1= IBITS(IQ(JDID+I+NK),0,16)
      NTRT = NTRM1+1
      NWTR = NTRT/2+1
      NK   = NK+NWTR
C
C             Find the selected volume
C             (if NO volumes exist take ALL digits)
C
      NV=IQ(JD+2)
      IF(NV.GT.0)THEN
         K=1
         DO 40 IV=1,NV
            NB=IQ(JD+2*IV+10)
            IF(NB.LE.0)THEN
               IF(K.GT.1)THEN
                   K=1
                   NK=NK+1
               ENDIF
               VOLS(IV)=IQ(JDID+I+NK)
               IF(IV.NE.NV)    NK = NK + 1
            ELSE
               IF(K+NB.GT.33)THEN
                  K=1
                  NK=NK+1
               ENDIF
               VOLS(IV)=IBITS(IQ(JDID+I+NK),K-1,NB)
               K=K+NB
            ENDIF
   40    CONTINUE
         NK=NK+1
      ENDIF
      VOLN = NV
C
C             Get unpacked digits
C
      ND=IQ(JD+6)
      IF(ND.GT.0) THEN
         K=1
         DO 90 ID=1,ND
            NB=IQ(JDDI+2*ID)
            IF(NB.LE.0)THEN
               IF(K.GT.1)THEN
                  K=1
                  NK=NK+1
               ENDIF
               DIGS(ID) = IQ(JDID+I+NK)
               IF(ID.NE.ND)NK=NK+1
            ELSE
               IF(K+NB.GT.33)THEN
                  K=1
                  NK=NK+1
               ENDIF
               DIGS(ID) = IBITS(IQ(JDID+I+NK),K-1,NB)
               K=K+NB
            ENDIF
 90      CONTINUE
      ENDIF
      DIGN = ND
      OGDIGV = 1
      RETURN
      END
C************************************************************
      SUBROUTINE OGDETO (IUSET,IUDET,VOLN,VOLS)
C************************************************************
C!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IMPLICIT NONE
      CHARACTER*4 IUSET,IUDET
      INTEGER VOLN
      INTEGER VOLS(50)
C+KEEP,GCBANK
      INTEGER IQ,LQ,NZEBRA,IXSTOR,IXDIV,IXCONS,LMAIN,LR1,JCG
      INTEGER KWBANK,KWWORK,IWS
      REAL GVERSN,ZVERSN,FENDQ,WS,Q
      PARAMETER (KWBANK=69000,KWWORK=5200)
      COMMON/GCBANK/NZEBRA,GVERSN,ZVERSN,IXSTOR,IXDIV,IXCONS,FENDQ(16)
     +             ,LMAIN,LR1,WS(KWBANK)
      DIMENSION IQ(2),Q(2),LQ(8000),IWS(2)
      EQUIVALENCE (Q(1),IQ(1),LQ(9)),(LQ(1),LMAIN),(IWS(1),WS(1))
      EQUIVALENCE (JCG,JGSTAT)
C+KEEP,GCLINK
      INTEGER       JDIGI ,JDRAW ,JHEAD ,JHITS ,JKINE ,JMATE ,JPART
     +      ,JROTM ,JRUNG ,JSET  ,JSTAK ,JGSTAT,JTMED ,JTRACK,JVERTX
     +      ,JVOLUM,JXYZ  ,JGPAR ,JGPAR2,JSKLT
      COMMON/GCLINK/JDIGI ,JDRAW ,JHEAD ,JHITS ,JKINE ,JMATE ,JPART
     +      ,JROTM ,JRUNG ,JSET  ,JSTAK ,JGSTAT,JTMED ,JTRACK,JVERTX
     +      ,JVOLUM,JXYZ  ,JGPAR ,JGPAR2,JSKLT
C
      INTEGER NSET,ISET,JS,NDET,IDET,JD,NV,I
C............................................................
      VOLN = 0
      IF(JSET.LE.0) RETURN
      NSET=IQ(JSET-1)
      CALL GLOOK(IUSET,IQ(JSET+1),NSET,ISET)
      IF(ISET.LE.0) RETURN
C
      JS=LQ(JSET-ISET)
      IF(JS.LE.0)  RETURN
      NDET=IQ(JS-1)
      CALL GLOOK(IUDET,IQ(JS+1),NDET,IDET)
      IF(IDET.EQ.0) RETURN
C
      JD=LQ(JS-IDET)
      IF(JD.LE.0) RETURN
C
      NV = IQ(JD+2)
      IF(NV.LE.0) RETURN

      DO 40 I=1,NV
         VOLS(I) = IQ(JD+2*I+ 9)
 40   CONTINUE
      VOLN = NV
      RETURN
      END
C************************************************************
      INTEGER FUNCTION OGVERN ()
C************************************************************
C return number of VERTEX
C!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IMPLICIT NONE
C+KEEP,GCLINK
      INTEGER       JDIGI ,JDRAW ,JHEAD ,JHITS ,JKINE ,JMATE ,JPART
     +      ,JROTM ,JRUNG ,JSET  ,JSTAK ,JGSTAT,JTMED ,JTRACK,JVERTX
     +      ,JVOLUM,JXYZ  ,JGPAR ,JGPAR2,JSKLT
      COMMON/GCLINK/JDIGI ,JDRAW ,JHEAD ,JHITS ,JKINE ,JMATE ,JPART
     +      ,JROTM ,JRUNG ,JSET  ,JSTAK ,JGSTAT,JTMED ,JTRACK,JVERTX
     +      ,JVOLUM,JXYZ  ,JGPAR ,JGPAR2,JSKLT
C+KEEP,GCNUM.
      INTEGER      NMATE ,NVOLUM,NROTM,NTMED,NTMULT,NTRACK,NPART
     +            ,NSTMAX,NVERTX,NHEAD,NBIT ,NALIVE,NTMSTO
      COMMON/GCNUM/NMATE ,NVOLUM,NROTM,NTMED,NTMULT,NTRACK,NPART
     +            ,NSTMAX,NVERTX,NHEAD,NBIT
      COMMON /GCNUMX/ NALIVE,NTMSTO
C............................................................
      OGVERN = 0
      IF (JVERTX.LE.0) RETURN
      OGVERN = NVERTX
      RETURN
      END
C************************************************************
      SUBROUTINE OGVERD(IV)
C************************************************************
C     Draws vertex IV (VERTEX).
C!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IMPLICIT NONE
      INTEGER IV
C+KEEP,GCBANK
      INTEGER IQ,LQ,NZEBRA,IXSTOR,IXDIV,IXCONS,LMAIN,LR1,JCG
      INTEGER KWBANK,KWWORK,IWS
      REAL GVERSN,ZVERSN,FENDQ,WS,Q
      PARAMETER (KWBANK=69000,KWWORK=5200)
      COMMON/GCBANK/NZEBRA,GVERSN,ZVERSN,IXSTOR,IXDIV,IXCONS,FENDQ(16)
     +             ,LMAIN,LR1,WS(KWBANK)
      DIMENSION IQ(2),Q(2),LQ(8000),IWS(2)
      EQUIVALENCE (Q(1),IQ(1),LQ(9)),(LQ(1),LMAIN),(IWS(1),WS(1))
      EQUIVALENCE (JCG,JGSTAT)
C+KEEP,GCLINK
      INTEGER       JDIGI ,JDRAW ,JHEAD ,JHITS ,JKINE ,JMATE ,JPART
     +      ,JROTM ,JRUNG ,JSET  ,JSTAK ,JGSTAT,JTMED ,JTRACK,JVERTX
     +      ,JVOLUM,JXYZ  ,JGPAR ,JGPAR2,JSKLT
      COMMON/GCLINK/JDIGI ,JDRAW ,JHEAD ,JHITS ,JKINE ,JMATE ,JPART
     +      ,JROTM ,JRUNG ,JSET  ,JSTAK ,JGSTAT,JTMED ,JTRACK,JVERTX
     +      ,JVOLUM,JXYZ  ,JGPAR ,JGPAR2,JSKLT
C+KEEP,GCNUM.
      INTEGER      NMATE ,NVOLUM,NROTM,NTMED,NTMULT,NTRACK,NPART
     +            ,NSTMAX,NVERTX,NHEAD,NBIT ,NALIVE,NTMSTO
      COMMON/GCNUM/NMATE ,NVOLUM,NROTM,NTMED,NTMULT,NTRACK,NPART
     +            ,NSTMAX,NVERTX,NHEAD,NBIT
      COMMON /GCNUMX/ NALIVE,NTMSTO
C+KEEP,GCMAIL
      COMMON/GCMAIL/CHMAIL
      CHARACTER*132 CHMAIL
C
      INTEGER JV,NPOINT
      REAL X,Y,Z
C............................................................
      IF (JVERTX.LE.0) THEN
        WRITE(CHMAIL,'(A)') 'Bank JVERTX empty'
        CALL GMAIL(0,0)
        RETURN
      ENDIF
      IF(IV.LT.1)      RETURN
      IF(IV.GT.NVERTX) RETURN
      JV = LQ(JVERTX-IV)
      IF(JV.EQ.0)      RETURN
      NPOINT = 1 
      X      = Q(JV+1)
      Y      = Q(JV+2)
      Z      = Q(JV+3)
      CALL GOPMAR (NPOINT,X,Y,Z)
      RETURN
      END
C************************************************************
      SUBROUTINE OGVERV (IOBJ,IPROP,IVAL,RVAL,SVAL)
C************************************************************
C return VERT property value
C!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IMPLICIT NONE
      INTEGER IOBJ,IPROP,IVAL
      REAL RVAL
      CHARACTER*20 SVAL
C+KEEP,GCBANK
      INTEGER IQ,LQ,NZEBRA,IXSTOR,IXDIV,IXCONS,LMAIN,LR1,JCG
      INTEGER KWBANK,KWWORK,IWS
      REAL GVERSN,ZVERSN,FENDQ,WS,Q
      PARAMETER (KWBANK=69000,KWWORK=5200)
      COMMON/GCBANK/NZEBRA,GVERSN,ZVERSN,IXSTOR,IXDIV,IXCONS,FENDQ(16)
     +             ,LMAIN,LR1,WS(KWBANK)
      DIMENSION IQ(2),Q(2),LQ(8000),IWS(2)
      EQUIVALENCE (Q(1),IQ(1),LQ(9)),(LQ(1),LMAIN),(IWS(1),WS(1))
      EQUIVALENCE (JCG,JGSTAT)
C+KEEP,GCLINK
      INTEGER       JDIGI ,JDRAW ,JHEAD ,JHITS ,JKINE ,JMATE ,JPART
     +      ,JROTM ,JRUNG ,JSET  ,JSTAK ,JGSTAT,JTMED ,JTRACK,JVERTX
     +      ,JVOLUM,JXYZ  ,JGPAR ,JGPAR2,JSKLT
      COMMON/GCLINK/JDIGI ,JDRAW ,JHEAD ,JHITS ,JKINE ,JMATE ,JPART
     +      ,JROTM ,JRUNG ,JSET  ,JSTAK ,JGSTAT,JTMED ,JTRACK,JVERTX
     +      ,JVOLUM,JXYZ  ,JGPAR ,JGPAR2,JSKLT
C
C+KEEP,GCNUM.
      INTEGER      NMATE ,NVOLUM,NROTM,NTMED,NTMULT,NTRACK,NPART
     +            ,NSTMAX,NVERTX,NHEAD,NBIT ,NALIVE,NTMSTO
      COMMON/GCNUM/NMATE ,NVOLUM,NROTM,NTMED,NTMULT,NTRACK,NPART
     +            ,NSTMAX,NVERTX,NHEAD,NBIT
      COMMON /GCNUMX/ NALIVE,NTMSTO
C
      INTEGER JV
C............................................................
      IVAL = 0
      RVAL = 0.
      SVAL = '    '
      IF(JVERTX.LE.0)    RETURN
      IF(IOBJ.LT.1)      RETURN
      IF(IOBJ.GT.NVERTX) RETURN
      JV = LQ(JVERTX-IOBJ)
      IF(JV.EQ.0)        RETURN
      IF(IPROP.EQ.1) THEN
         RVAL   = Q(JV+1)
      ELSE IF(IPROP.EQ.2) THEN
         RVAL   = Q(JV+2)
      ELSE IF(IPROP.EQ.3) THEN
         RVAL   = Q(JV+3)
      ENDIF
      RETURN
      END
