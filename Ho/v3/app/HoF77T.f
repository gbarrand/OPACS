C************************************************************
      PROGRAM HOF77T
C************************************************************
C!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IMPLICIT NONE
C
      INTEGER I
      REAL X,Y
      REAL CRANDO
C............................................................
C OHistogram
      CALL OH1CRE('hist_1',6,'X',1,10,0.,1.)     
      CALL OH1CRE('hist_2',6,'X',1,20,0.,20.)     
C SET CURRENT HISTO AND FILL
      CALL OHCURR('hist_1',6)
      DO 10 I=1,1000
        X = CRANDO()
        CALL OH1FIL(X,1.)
10    CONTINUE
      CALL OHDUMP()
C
      CALL OHCURR('hist_2',6)
      DO 20 I=1,2000
        X = 20. * CRANDO()
        CALL OH1FIL(X,1.)
20    CONTINUE
      CALL OHDUMP()
C
C OScatter
      CALL OS2CRE('scat_1',6,
     +            'X',1,0.,1.,     
     +            'Y',1,0.,1.)     
C ALLOCATE POINTS
      CALL OSALLO (100)
C SET CURRENT SCAT AND FILL
      CALL OSCURR('scat_1',6)
      DO 30 I=1,100
        X = CRANDO()
        Y = CRANDO()
        CALL OS2FIL(X,Y)
30    CONTINUE
      CALL OSDUMP()
C
C
      STOP
      END
