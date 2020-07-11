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
      SUBROUTINE G3OINI()
C************************************************************
C!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IMPLICIT NONE
C+GCBANK
      INTEGER NWGEANT
      PARAMETER (NWGEANT=500000)
      INTEGER Q
      COMMON/GCBANK/Q(NWGEANT)
C+GCFLAG
      INTEGER       IDEBUG,IDEMIN,IDEMAX,ITEST,IDRUN,IDEVT,IEORUN
     +        ,IEOTRI,IEVENT,ISWIT,IFINIT,NEVENT,NRNDM
      COMMON/GCFLAG/IDEBUG,IDEMIN,IDEMAX,ITEST,IDRUN,IDEVT,IEORUN
     +        ,IEOTRI,IEVENT,ISWIT(10),IFINIT(20),NEVENT,NRNDM(2)
C
      INTEGER IECAL,ITOWE
      COMMON/CECAL/IECAL,ITOWE
C
C...SIZE IN CM
      REAL        PMOTH(3)
      DATA        PMOTH/1000.,1000.,1000./
      REAL        PECAL(3)
      DATA        PECAL/100.,160,200./

      INTEGER     IVOLU

      INTEGER     NV
      PARAMETER  (NV=3)
      CHARACTER*4 NAMESV(NV)
      INTEGER     NBITSV(NV)
      DATA        NAMESV/'LAYE','SLIC','TOWE'/
      DATA        NBITSV/32,32,32/

      CHARACTER*4 NAMESH(1)
      INTEGER     NBITSH(1)
      REAL        ORIGS(1),FACTS(1)
      DATA        NAMESH/'E'/
      DATA        NBITSH/32/
      DATA        ORIGS/0./
      DATA        FACTS/1.E+9/
C............................................................
C..INITIALIZE ZEBRA and GCBANK
      CALL GZEBRA (NWGEANT)

C..INITIALISE GEANT
      CALL GINIT

C..INITIALIZE GEANT/ZBOOK DATA STUCTURES
      CALL GZINIT

C..DEFINE STD PARTICLE AND MATERIAL DATA
      CALL GPART
      CALL GMATE

C...BUILD A GEOMETRY
C...DEFINE MOTHER MEDIUM/VOLUME
C      CALL GSTMED (1,'AIR' ,15,0,0,0.,0.,10.,0.05,0.0010,0.0010,0,0)
C...PUT A MAGNETIC FIELD OF 10 KGAUSS ALONG Z AXIS
C     CALL GSTMED (1,'AIR' ,15,0,3,50.,2.,10.,0.05,0.0010,0.0010,0,0)
C      CALL GSTMED (1,'VAC' ,16,0,0,0.,0.,10.,0.05,0.0010,0.0010,0,0)
      CALL GSTMED (1,'VAC' ,16,0,3,50.,2.,10.,0.05,0.0010,0.0010,0,0)
      CALL GSVOLU ('MOTH','BOX ',1,PMOTH,3,IVOLU)

C...DEFINE CALORIMETER MEDIUM/VOLUME
C...CUIV IS DECLARED AS SENSITIVE SO THAT GFINDS IS CALLED BY GTRACK.
      CALL GSTMED (2,'CUIV',11,1,0,0.,0.,10.,0.05,0.0010,0.0010,0,0)
      CALL GSVOLU ('ECAL','TUBE',2,PECAL,3,IVOLU)
      CALL GSPOS  ('ECAL',1,'MOTH',0.,0.,0.,0,'ONLY')
      CALL GSDVN  ('LAYE','ECAL' , 3,1)
      CALL GSDVN  ('SLIC','LAYE' ,40,3)
      CALL GSDVN  ('TOWE','SLIC' ,36,2)

C..ECAL HIT,  GEANT PART
      CALL GSDET  ('ECAL','TOWE',NV,NAMESV,NBITSV,1,100,100,IECAL,ITOWE)
C..ECAL HITS, USER  PART
      CALL GSDETH ('ECAL','TOWE',1,NAMESH,NBITSH,ORIGS,FACTS)

C...END GEOMETRY DEFINITION
      CALL GGCLOS

C      CALL GSTPAR (2,'CUTELE',0.1)
C      CALL GSTPAR (2,'CUTGAM',0.1)
C      CALL GSTPAR (2,'CUTHAD',0.1)

C      CALL GPPART(0)
C      CALL GPTMED(0)
C      CALL GPVOLU(0)

C...COMPUTE CROSS SECTION AND ENERGY LOSS TABLE FOR MATERIALS
      CALL GPHYSI

      RETURN
      END
C************************************************************
      SUBROUTINE GUKINE()
C************************************************************
C  - Electron of 1GEV.
C  - Lund.
C!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IMPLICIT NONE

      INTEGER    NPELEC,NPPIP
      PARAMETER (NPELEC=3)
      PARAMETER (NPPIP =8)

      REAL V(3),P(3)
      DATA V/0.,0.,0./
      DATA P/1.,0.,0./
      INTEGER NVTX,NT
C............................................................
C      CALL GRNDM (P,3)
      CALL GSVERT(V,0,0,0,0,NVTX)
      P(1) = 1.
      P(2) = 0.
      CALL GSKINE(P,NPELEC,NVTX,0,0,NT)
      P(1) = 0.
      P(2) = 1.
      CALL GSKINE(P,NPPIP,NVTX,0,0,NT)
C      CALL GLUND()
      RETURN
      END
C************************************************************
      SUBROUTINE GUTREV()
C************************************************************
C!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IMPLICIT NONE
C............................................................
      CALL GTREVE              !Geant simulation here
C      CALL GPVERT(0)
C      CALL GPKINE(0)
C      CALL GPJXYZ(0)
C      CALL GPHITS('ECAL','TOWE')
      RETURN
      END
C************************************************************
      SUBROUTINE GUSTEP()
C************************************************************
C   MEDIUM CUIV MUST BE DECLARED AS SENSITIVE 
C  SO THAT GFINDS (THAT FILL GCSETS) IS CALLED BY GTRACK.
C!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IMPLICIT NONE
C+GCSETS.
      INTEGER       IHSET,IHDET,ISET,IDET,IDTYPE,NVNAME,NUMBV
      COMMON/GCSETS/IHSET,IHDET,ISET,IDET,IDTYPE,NVNAME,NUMBV(20)
C+GCKINE
      INTEGER       IKINE,ITRA,ISTAK,IVERT,IPART,ITRTYP,NAPART,IPAOLD
      REAL          PKINE,AMASS,CHARGE,TLIFE,VERT,PVERT
      COMMON/GCKINE/IKINE,PKINE(10),ITRA,ISTAK,IVERT,IPART,ITRTYP
     +      ,NAPART(5),AMASS,CHARGE,TLIFE,VERT(3),PVERT(4),IPAOLD
C+GCTRAK
      INTEGER NMEC,LMEC,NAMEC,NSTEP ,MAXNST,IGNEXT,INWVOL,ISTOP,MAXMEC
     + ,IGAUTO,IEKBIN,ILOSL, IMULL,INGOTO,NLDOWN,NLEVIN,NLVSAV,ISTORY
      REAL  VECT,GETOT,GEKIN,VOUT,DESTEP,DESTEL,SAFETY,SLENG ,STEP
     + ,SNEXT,SFIELD,TOFG  ,GEKRAT,UPWGHT
      PARAMETER (MAXMEC=30)
      COMMON/GCTRAK/VECT(7),GETOT,GEKIN,VOUT(7),NMEC,LMEC(MAXMEC)
     + ,NAMEC(MAXMEC),NSTEP ,MAXNST,DESTEP,DESTEL,SAFETY,SLENG
     + ,STEP  ,SNEXT ,SFIELD,TOFG  ,GEKRAT,UPWGHT,IGNEXT,INWVOL
     + ,ISTOP ,IGAUTO,IEKBIN, ILOSL, IMULL,INGOTO,NLDOWN,NLEVIN
     + ,NLVSAV,ISTORY
C
      INTEGER IECAL,ITOWE
      COMMON/CECAL/IECAL,ITOWE
C
      REAL    HITV(1)
      INTEGER IHIT
C............................................................
      CALL GSXYZ
      CALL GSKING (0)
      IF(ISET.EQ.IECAL) THEN
C...ADD TRACK ENERGY LOST TO ECAL TOWE ENERGY.
C         PRINT *,'debug:',ISET,ITRA,ISTOP,NGKINE,DESTEP
         HITV(1) = DESTEP
         CALL GSCHIT (ISET,IDET,ITRA,NUMBV,HITV,1,IHIT)
      ELSE
C...STORE XYZ OF POINTS NOT IN THE ECAL.
C        CALL GSXYZ
      ENDIF
      RETURN
      END
C************************************************************
      SUBROUTINE G3OEVT(IEV)
C************************************************************
C!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IMPLICIT NONE
      INTEGER IEV
C+GCFLAG
      INTEGER       IDEBUG,IDEMIN,IDEMAX,ITEST,IDRUN,IDEVT,IEORUN
     +        ,IEOTRI,IEVENT,ISWIT,IFINIT,NEVENT,NRNDM
      COMMON/GCFLAG/IDEBUG,IDEMIN,IDEMAX,ITEST,IDRUN,IDEVT,IEORUN
     +        ,IEOTRI,IEVENT,ISWIT(10),IFINIT(20),NEVENT,NRNDM(2)
C
C............................................................
      IF(IEV.LE.0) THEN
        CALL NXTEVT()
      ELSE IF(IEVENT.LT.IEV) THEN
 10     CONTINUE
        CALL NXTEVT()
        IF (IEVENT.EQ.IEV) RETURN
        GOTO 10
      ENDIF
      RETURN
      END 
C************************************************************
      SUBROUTINE NXTEVT()
C************************************************************
C!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IMPLICIT NONE
C+GCFLAG
      INTEGER       IDEBUG,IDEMIN,IDEMAX,ITEST,IDRUN,IDEVT,IEORUN
     +        ,IEOTRI,IEVENT,ISWIT,IFINIT,NEVENT,NRNDM
      COMMON/GCFLAG/IDEBUG,IDEMIN,IDEMAX,ITEST,IDRUN,IDEVT,IEORUN
     +        ,IEOTRI,IEVENT,ISWIT(10),IFINIT(20),NEVENT,NRNDM(2)
C
      INTEGER IFIRST
      SAVE IFIRST
      DATA IFIRST/0/
C............................................................
      IF(IFIRST.EQ.0)THEN
        IFIRST=1
      ELSE
        CALL GTRIGC
      ENDIF
      IEVENT=IEVENT+1
      CALL GTRIGI
      CALL GTRIG
      RETURN
      END 
C************************************************************
      INTEGER FUNCTION IBKGET(IOBJ,IPROP)
C************************************************************
C!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IMPLICIT NONE
      INTEGER IOBJ,IPROP
C+GCFLAG
      INTEGER       IDEBUG,IDEMIN,IDEMAX,ITEST,IDRUN,IDEVT,IEORUN
     +        ,IEOTRI,IEVENT,ISWIT,IFINIT,NEVENT,NRNDM
      COMMON/GCFLAG/IDEBUG,IDEMIN,IDEMAX,ITEST,IDRUN,IDEVT,IEORUN
     +        ,IEOTRI,IEVENT,ISWIT(10),IFINIT(20),NEVENT,NRNDM(2)
C
C............................................................
      IBKGET = 0
      IF(IPROP.EQ.1) THEN
         IBKGET = IEVENT
      ELSE IF(IPROP.EQ.2) THEN
         IBKGET = 666
      ENDIF
C To avoid compiler warning
      IOBJ = 0
      RETURN
      END 

