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
      SUBROUTINE OMINUIT ()
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
      SUBROUTINE OMNFIT
     &(A_TYPE,A_NPT,A_XXS,A_VALS,A_ERRS,A_NPAR,A_PARS,A_STPS)
C************************************************************
C npts,xxs,vals,errs: input
C          pars,stps: input output
C!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IMPLICIT NONE
      INTEGER A_TYPE
      INTEGER A_NPT
      DOUBLE PRECISION A_XXS(*),A_VALS(*),A_ERRS(*)
      INTEGER A_NPAR
      DOUBLE PRECISION A_PARS(*),A_STPS(*)
C
C...Put not array at end due to VM warnings
      INTEGER   X_NPT
      PARAMETER (X_NPT=1000)
      INTEGER   X_NPAR
      PARAMETER (X_NPAR=50)
      COMMON/OMNCOM/C_XXS(X_NPT),C_VALS(X_NPT),C_ERRS(X_NPT),
     +              C_PARS(X_NPAR),C_NPAR,C_NPT,C_TYPE
      DOUBLE PRECISION C_XXS,C_VALS,C_ERRS
      DOUBLE PRECISION C_PARS
      INTEGER C_NPAR
      INTEGER C_NPT
      INTEGER C_TYPE
C
      DOUBLE PRECISION ZERO
      INTEGER IPT
      INTEGER IFLAG
      DOUBLE PRECISION ARGLIS(10)
      DOUBLE PRECISION BMIN,BMAX
C
      EXTERNAL     OFCN
      CHARACTER*10 STRG
C............................................................
C...FILL COMMON
      C_TYPE = A_TYPE
      IF(A_NPT.GT.X_NPT) THEN
        PRINT *,'OMNFIT:',A_NPT,' GREATER THAN ',X_NPT
        A_TYPE = -1
        RETURN
      ENDIF
      C_NPT  = A_NPT
      IF(A_NPAR.GT.X_NPAR) THEN
        PRINT *,'OMNFIT:',A_NPAR,' GREATER THAN ',X_NPAR
        A_TYPE = -1
        RETURN
      ENDIF      
      C_NPAR = A_NPAR
      DO 10 IPT = 1,C_NPT
        C_XXS (IPT) = A_XXS (IPT)
        C_VALS(IPT) = A_VALS(IPT)
        C_ERRS(IPT) = A_ERRS(IPT)
C        PRINT *,C_XXS(IPT),C_VALS(IPT),C_ERRS(IPT)
 10   CONTINUE   
      DO 20 IPT = 1,A_NPAR
        C_PARS(IPT) = A_PARS(IPT)
C        PRINT *,C_PARS(IPT)
 20   CONTINUE   
  
      PRINT *,
     &'==========================================================='
      PRINT *,
     &'==========================================================='
      PRINT *,
     &'==========================================================='
      CALL MNINIT(5,6,7)

      ZERO   = 0.
      DO 30 IPT = 1,A_NPAR
C...Build name of variable 
C                     1234567890
        STRG(1:3)  = 'P00       '
        IF(IPT.LE.9) THEN
          WRITE(STRG(3:3),'(I1)') IPT
        ELSE
          WRITE(STRG(2:3),'(I2)') IPT
        ENDIF
        CALL MNPARM
     &  (IPT,STRG(1:3),A_PARS(IPT),A_STPS(IPT),ZERO,ZERO,IFLAG)
 30   CONTINUE   

           IF(A_TYPE.EQ.1) THEN
        CALL MNSETI('GAUSSIAN FIT')
      ELSE IF(A_TYPE.EQ.2) THEN
        CALL MNSETI('EXPONENTIAL FIT')
      ELSE IF(A_TYPE.EQ.3) THEN
        CALL MNSETI('POLYNOMIAL FIT')
      ELSE IF(A_TYPE.EQ.4) THEN
        CALL MNSETI('BREIT WIGNER FIT')
      ELSE IF(A_TYPE.EQ.5) THEN
        CALL MNSETI('USER DEFINED FIT')
      ENDIF
      ARGLIS(1) = 0
      CALL MNEXCM(OFCN,'SET PRI',ARGLIS,1,IFLAG,0)
C
      CALL MNEXCM(OFCN,'MIGRAD' ,ARGLIS,0,IFLAG,0)
      IF(IFLAG.NE.0) THEN
        RETURN
      ENDIF
C      CALL MNEXCM(OFCN,'MINOS'  ,ARGLIS,0,IFLAG,0)
C
      DO 40 IPT = 1,A_NPAR
        CALL MNPOUT(IPT,STRG,A_PARS(IPT),A_STPS(IPT),BMIN,BMAX,IFLAG)
 40   CONTINUE   

      RETURN
      END
C************************************************************
      SUBROUTINE OFCN(NPAR,GRAD,FVAL,XVAL,IFLAG,FUTIL)
C************************************************************
C   NPAR: INPUT
C   GRAD: OUTPUT
C   FVAL: OUTPUT
C   XVAL: INPUT
C  IFLAG: INPUT
C!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IMPLICIT NONE
      INTEGER NPAR
      DOUBLE PRECISION GRAD(*)
      DOUBLE PRECISION FVAL
      DOUBLE PRECISION XVAL(*)
      INTEGER  IFLAG
      EXTERNAL FUTIL
C
C...PUT NOT ARRAY AT END DUE TO VM WARNINGS
      INTEGER   X_NPT
      PARAMETER (X_NPT=1000)
      INTEGER   X_NPAR
      PARAMETER (X_NPAR=50)
      COMMON/OMNCOM/C_XXS(X_NPT),C_VALS(X_NPT),C_ERRS(X_NPT),
     +              C_PARS(X_NPAR),C_NPAR,C_NPT,C_TYPE
      DOUBLE PRECISION C_XXS,C_VALS,C_ERRS
      DOUBLE PRECISION C_PARS
      INTEGER C_NPAR
      INTEGER C_NPT
      INTEGER C_TYPE
C
      INTEGER IPT,J
      DOUBLE PRECISION VALUE,CHINEW,CHISQR
      DOUBLE PRECISION OMTGAU,OMTEXP,OMTPOL,OMTBW
C
C............................................................
      J = NPAR
      J = IFLAG
      VALUE = GRAD(1)
C
      CHISQR = 0.
      DO 10 IPT = 1,C_NPT

             IF(C_TYPE.EQ.1) THEN
          VALUE = OMTGAU(C_XXS(IPT),XVAL(1),XVAL(2),XVAL(3)) 
        ELSE IF(C_TYPE.EQ.2) THEN
          VALUE = OMTEXP(C_XXS(IPT),XVAL(1),XVAL(2))
        ELSE IF(C_TYPE.EQ.3) THEN
          VALUE = OMTPOL(C_XXS(IPT),C_NPAR,XVAL)
        ELSE IF(C_TYPE.EQ.4) THEN
          VALUE = OMTBW(C_XXS(IPT),XVAL(1),XVAL(2),XVAL(3))
        ELSE IF(C_TYPE.EQ.5) THEN
          CALL OXFVAL (C_XXS(IPT),C_NPAR,XVAL,VALUE)
        ENDIF

        CHINEW = (C_VALS(IPT) - VALUE)/C_ERRS(IPT)
        CHISQR = CHISQR + CHINEW * CHINEW
 10   CONTINUE

C...PUT FOUND PARAMS IN COMMON 
      DO 30 J = 1,C_NPAR
        C_PARS(J) = XVAL(J)
 30   CONTINUE

      FVAL = CHISQR
      RETURN
      END
C************************************************************
      DOUBLE PRECISION FUNCTION OMTGAU (A_X,A_SCALE,A_MEAN,A_SIGMA)
C************************************************************
C!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IMPLICIT NONE
      DOUBLE PRECISION A_X
      DOUBLE PRECISION A_SCALE,A_MEAN,A_SIGMA

      DOUBLE PRECISION VALUE,ZERO,SIGMA
C............................................................
      ZERO   = 0.
      IF(A_SIGMA.EQ.ZERO) THEN
        PRINT *,'OMTGAU:NULL SIGMA'
        SIGMA = 1.
      ELSE
        SIGMA = A_SIGMA
      ENDIF
      VALUE  = (A_X - A_MEAN)/SIGMA
      VALUE  = A_SCALE * EXP (-0.5 * VALUE * VALUE)
      OMTGAU = VALUE
      RETURN
      END
C************************************************************
      DOUBLE PRECISION FUNCTION OMTBW (A_X,A_HEIGHT,A_CENTER,A_WIDTH)
C************************************************************
C!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IMPLICIT NONE
      DOUBLE PRECISION A_X
      DOUBLE PRECISION A_CENTER,A_WIDTH,A_HEIGHT

      DOUBLE PRECISION WIDTH,VALUE,ZERO
C............................................................
      ZERO   = 0.
      IF(A_WIDTH.EQ.ZERO) THEN
        PRINT *,'OMTBW:NULL WIDTH'
        WIDTH = 1.
      ELSE
        WIDTH = A_WIDTH
      ENDIF
      VALUE  = 2.*(A_X - A_CENTER)/WIDTH
      OMTBW  = A_HEIGHT/(1.+VALUE*VALUE)
      RETURN
      END
C************************************************************
      DOUBLE PRECISION FUNCTION OMTEXP(A_X,A_A,A_B)
C************************************************************
C!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IMPLICIT NONE
      DOUBLE PRECISION A_X
      DOUBLE PRECISION A_A,A_B
C............................................................
      OMTEXP = EXP (A_A + A_B * A_X)
      RETURN
      END
C************************************************************
      DOUBLE PRECISION FUNCTION OMTPOL(A_X,A_NPAR,A_PARS)
C************************************************************
C!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IMPLICIT NONE
      DOUBLE PRECISION A_X
      INTEGER A_NPAR
      DOUBLE PRECISION A_PARS(*)

      DOUBLE PRECISION VALUE,XXX
      INTEGER I
C............................................................
      VALUE = 0.
      XXX   = 1.
      DO 10 I = 1,A_NPAR
          VALUE = VALUE + A_PARS(I) * XXX
          XXX   = XXX * A_X
 10   CONTINUE
      OMTPOL = VALUE
      RETURN
      END
C************************************************************
      DOUBLE PRECISION FUNCTION OMTUSR(A_X,A_NPAR,A_PARS)
C************************************************************
C!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IMPLICIT NONE
      DOUBLE PRECISION A_X
      INTEGER A_NPAR
      DOUBLE PRECISION A_PARS(*)

      DOUBLE PRECISION VALUE,XXX
      INTEGER I
C............................................................
      VALUE = 0.
      XXX   = 1.
      DO 10 I = 1,A_NPAR
          VALUE = VALUE + A_PARS(I) * XXX
          XXX   = XXX * A_X
 10   CONTINUE
      OMTUSR = VALUE
      RETURN
      END
