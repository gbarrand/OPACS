C************************************************************
      PROGRAM PAWCT
C************************************************************
C!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IMPLICIT NONE
C
C+KEEP, PAWC.
      INTEGER     NWPAW,IXPAWC,IHDIV,IXHIGZ,IXKU
      COMMON/PAWC/NWPAW,IXPAWC,IHDIV,IXHIGZ,IXKU
C+KEEP, ZUNIT. 
      INTEGER IQREAD,IQPRNT,IQPR2,IQLOG,IQPNCH,IQTTIN,IQTYPE
      COMMON /ZUNIT/ IQREAD,IQPRNT,IQPR2,IQLOG,IQPNCH,IQTTIN,IQTYPE
C
C............................................................
      PRINT *,'NWPAW,IXPAWC,IHDIV,IXHIGZ,IXKU',
     +        NWPAW,IXPAWC,IHDIV,IXHIGZ,IXKU
      PRINT *,'IQREAD',IQREAD
      STOP
      END
