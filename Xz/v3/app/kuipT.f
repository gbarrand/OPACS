C************************************************************
      PROGRAM KUIPT 
C************************************************************
C  KTDEF in file ktcdf.c
C produced with : 
C    UNIX> /cern/pro/bin/kuipc ktcdf.txt ktcdf.c 
C!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IMPLICIT NONE
C
      INTEGER    NWPAW
      PARAMETER( NWPAW = 500000)
      REAL          H
      COMMON /PAWC/ H(NWPAW)
C
      INTEGER NWKUIP
C............................................................
      CALL MZEBRA(-3)
      CALL MZPAW(500000,' ')
C
      NWKUIP = 200000
      CALL KUINIT (NWKUIP)
C
      CALL KUEXEC('help kuip')
      CALL KUEXEC('help macro')
C
      CALL KTDEF()
C
      CALL KUWHAT()
C
      CALL KUEXEC('xxx tst.c 50')
C
      STOP
      END
C************************************************************
      SUBROUTINE KTXXX
C************************************************************
C   User routine referenced in ktcdf.txt with
C    >Action KTACT
C  KUGET routines must be called in same order
C  than '>Parameters' defined in ktcdf.txt.
C!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IMPLICIT NONE
C
      CHARACTER*32 CHPATH,CFILE
      INTEGER      NPAR,LFILE,ID,NUMBER
C............................................................
      CALL KUPATL (CHPATH,NPAR)
C
C mandatory params
      CALL KUGETS (CFILE,LFILE)
      CALL KUGETI (ID)
C
C optional params
      CALL KUGETI (NUMBER)
C
      PRINT *,'XXX     : ',CHPATH,NPAR
      PRINT *,' File   : ',CFILE(1:LFILE)
      PRINT *,' Id     : ',ID
      PRINT *,' Number : ',NUMBER
      RETURN
      END
