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
C*** over HPLOT, HIGZ routines ******************************
C************************************************************
C************************************************************
      SUBROUTINE OHPLOT (ID,SOPT,LOPT)
C************************************************************
C!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IMPLICIT NONE
      INTEGER ID
      CHARACTER*80 SOPT
      INTEGER      LOPT
C............................................................
      CALL HPLOT  (ID,SOPT(1:LOPT),' ',0)
      RETURN
      END
C************************************************************
      SUBROUTINE OHPSET (SOPT,LOPT,VAL)
C************************************************************
C!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IMPLICIT     NONE
      CHARACTER*80 SOPT
      INTEGER      LOPT
      REAL         VAL 
C............................................................
      CALL HPLSET (SOPT(1:LOPT),VAL)
      RETURN
      END
C************************************************************
      SUBROUTINE OHPOPT (SOPT,LOPT)
C************************************************************
C!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IMPLICIT     NONE
      CHARACTER*80 SOPT
      INTEGER      LOPT
C............................................................
      CALL HPLOPT (SOPT(1:LOPT),1)
      RETURN
      END
C************************************************************
      SUBROUTINE OHPAX (SX,LX,SY,LY)
C************************************************************
C!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IMPLICIT     NONE
      CHARACTER*80 SX
      INTEGER      LX
      CHARACTER*80 SY
      INTEGER      LY
C............................................................
      CALL HPLAX (SX(1:LX),SY(1:LY))
      RETURN
      END
C************************************************************
      SUBROUTINE OHPTIT (SOPT,LOPT)
C************************************************************
C!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IMPLICIT     NONE
      CHARACTER*80 SOPT
      INTEGER      LOPT
C............................................................
      CALL HPLTIT (SOPT(1:LOPT))
      RETURN
      END
C************************************************************
      SUBROUTINE OHPSIZ (XSIZ,YSIZ)
C************************************************************
C!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IMPLICIT     NONE
      REAL         XSIZ,YSIZ
C............................................................
      CALL  HPLSIZ (XSIZ,YSIZ,'R')
      RETURN
      END
C************************************************************
      SUBROUTINE OHPZON (NXZON,NYZON,IFIRST,SOPT,LOPT)
C************************************************************
C!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IMPLICIT     NONE
      INTEGER      NXZON,NYZON,IFIRST
      CHARACTER*80 SOPT
      INTEGER      LOPT
C............................................................
      CALL HPLZON (NXZON,NYZON,IFIRST,SOPT(1:LOPT))
      RETURN
      END
C************************************************************
C************************************************************
C************************************************************
      SUBROUTINE OISRAT (IWIDTH,IHEIGH)
C************************************************************
C!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IMPLICIT NONE
      INTEGER IWIDTH,IHEIGH

      REAL RVAL(2),XSIZ,YSIZ
C............................................................
C      CALL IGQWK  (IWKID,'MXDS',RVAL)
      RVAL(1) = FLOAT(IWIDTH-1)
      RVAL(2) = FLOAT(IHEIGH-1)
      CALL HPLSIZ (XSIZ,YSIZ,'R')
      IF (RVAL(1).GT.RVAL(2)) THEN
        XSIZ = XSIZ * (RVAL(1)/RVAL(2))
        CALL HPLSET ('XSIZ',XSIZ)
      ELSE
        YSIZ = YSIZ * (RVAL(2)/RVAL(1))
        CALL HPLSET ('YSIZ',YSIZ)
      ENDIF
      Return
      End
C************************************************************
      SUBROUTINE OISWK (IWKID,IWIDTH,IHEIGH)
C************************************************************
C** 20/8/93. From code of OPWK taken on cernvn:
C          gime cernpams
C          x higz car z
C!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IMPLICIT NONE
      INTEGER IWKID,IWIDTH,IHEIGH

      REAL RVAL(2),TRANS,SCALE
C............................................................
C Native code
C      REAL RVAL(2),XWKWN,YWKWN
C      CALL  IGQWK(IWKID,'MXDS',RVAL)
C      XWKWN = MIN(1.,RVAL(1)/RVAL(2))
C      YWKWN = MIN(1.,RVAL(2)/RVAL(1))
C      CALL ISWKWN (IWKID,0.,XWKWN,0.,YWKWN)
C      CALL ISWKVP (IWKID,0.,RVAL(1),0.,RVAL(2))
C      CALL IKUWK  (IWKID)
      RVAL(1) = FLOAT(IWIDTH-1)
      RVAL(2) = FLOAT(IHEIGH-1)
      IF (RVAL(1).GT.RVAL(2)) THEN
        SCALE       = RVAL(2)/RVAL(1)
        CALL ISWKWN (IWKID,0.,SCALE,0.,SCALE)
        TRANS       = -(RVAL(1)-RVAL(2))/2. 
        CALL ISWKVP (IWKID,0.,RVAL(1),TRANS,RVAL(2)+TRANS)
      ELSE
        SCALE       = RVAL(1)/RVAL(2)
        CALL ISWKWN (IWKID,0.,SCALE,0.,SCALE)
        TRANS       = -(RVAL(2)-RVAL(1))/2. 
        CALL ISWKVP (IWKID,TRANS,RVAL(1)+TRANS,0.,RVAL(2))
      ENDIF
      CALL IKUWK  (IWKID)
      Return
      End
C************************************************************
      SUBROUTINE OIGWID (IWKID,IWID)
C************************************************************
C** 20/8/93. From code taken on cernvn:
C          gime cernpams
C          x higz car z
C!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IMPLICIT NONE
      INTEGER IWKID,IWID

C+KEEP,HIATT. Attributes list
C*CMZ :  1.18/00 23/04/93  17.27.56  by  O.Couet
      INTEGER INTR
     +,IWTYPE,IDID  ,IDTY
     +,ILOPWK,IWTYL,INOPWK
     +,IWINID
     +,IPICT,ICLIP,NT0PRI
     +,IPLCI ,ILN   ,IPMCI ,IMK   ,IFACI ,IFAIS ,IFASI ,ITXCI
     +,ITXALG,IFTPR ,INPASS,IBORD ,INLINE,INTATT
     +,IFONT,IPREC,ITXALH,ITXALV
     +,IDIM,INBCOL
      REAL XRATIO,YRATIO
     +,RWXMIN,RWXMAX,RWYMIN,RWYMAX,RVXMIN,RVXMAX,RVYMIN,RVYMAX
     +,RDWXMI,RDWXMA,RDWYMI,RDWYMA,RDVXMI,RDVXMA,RDVYMI,RDVYMA
     +,RMDSX ,RMDSY
     +,XWKSIZ,YWKSIZ,WKMAX,XWKR,YWKR
     +,XWKW1,YWKW1,XWKV1,YWKV1
     +,XWKW2,YWKW2,XWKV2,YWKV2
     +,REDIT
     +,RMKSC ,RLWSC ,RBSL  ,RANGLE,RCHH  ,RCSHIF,RBOF  ,RBWD
     +,RAWL  ,RTMS  ,RALH  ,RALD  ,REAATT
     +,RCHUX,RCHUY
      LOGICAL ACWKFL,NODRFL

      COMMON /HIATT/ INTR,XRATIO,YRATIO
     +,RWXMIN,RWXMAX,RWYMIN,RWYMAX,RVXMIN,RVXMAX,RVYMIN,RVYMAX
     +,RDWXMI,RDWXMA,RDWYMI,RDWYMA,RDVXMI,RDVXMA,RDVYMI,RDVYMA
     +,RMDSX ,RMDSY ,IWTYPE,IDID  ,IDTY
     +,ILOPWK(10),ACWKFL(10),NODRFL(10),IWTYL(10) ,INOPWK
     +,XWKSIZ(10),YWKSIZ(10),WKMAX(10) ,IWINID(10),XWKR(10),YWKR(10)
     +,XWKW1(10),YWKW1(10),XWKV1(10),YWKV1(10)
     +,XWKW2(10),YWKW2(10),XWKV2(10),YWKV2(10)
     +,IPICT,ICLIP,REDIT,NT0PRI
     +,RMKSC ,RLWSC ,RBSL  ,RANGLE,RCHH  ,RCSHIF,RBOF  ,RBWD
     +,RAWL  ,RTMS  ,RALH  ,RALD  ,REAATT(18)
     +,IPLCI ,ILN   ,IPMCI ,IMK   ,IFACI ,IFAIS ,IFASI ,ITXCI
     +,ITXALG,IFTPR ,INPASS,IBORD ,INLINE,INTATT(17)
     +,IFONT,IPREC,ITXALH,ITXALV,RCHUX,RCHUY
     +,IDIM,INBCOL

      EXTERNAL IGIWIN
      INTEGER  IGIWIN
C............................................................
      IWID = IWINID(IGIWIN(IWKID))
      RETURN
      END
