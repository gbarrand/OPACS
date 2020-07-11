C************************************************************
      PROGRAM HBOOKT
C************************************************************
C!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IMPLICIT NONE

      INTEGER    NWPAW
      PARAMETER( NWPAW = 500000)
      REAL          H
      COMMON /PAWC/ H(NWPAW)

      INTEGER ISTAT

      INTEGER        IDH
      CHARACTER*80   CHTYPE ,CHTITL

C      INTEGER        LIST(10),N
      INTEGER I
C............................................................
      CALL HLIMIT(NWPAW)

C      CALL HBOOK1 (15,'hello 1',50,1.,20,)
C      CALL HBOOK1 (12,'hello 2',50,1.,20,)
C      CALL HIDALL (LIST,N)
C      DO I=1,N
C        PRINT *,LIST(I)
C      ENDDO
      
      I = 10
      IF(I.EQ.10) GOTO 10

      CALL HROPEN( 1, 'T','t.rz', ' ',1024, ISTAT )
      IF ( ISTAT .NE. 0 ) THEN
         PRINT *, ' FAILURE TO OPEN FILE '
         STOP
      ENDIF
      CALL HLDIR   (' ','T')

      IDH = 0
 20   CONTINUE
      CALL HLNEXT (IDH,CHTYPE,CHTITL,' ')
      PRINT *,IDH,' ',CHTYPE(1:20),CHTITL(1:20)
      IF(IDH.NE.0) GOTO 20

      STOP

 10   CALL HROPEN( 1, 'PHAN','phan.rz', ' ',1024, ISTAT )
      IF ( ISTAT .NE. 0 ) THEN
         PRINT *, ' FAILURE TO OPEN FILE '
         STOP
      ENDIF
C PUT //PHAN IN //PAWC
      CALL HRIN    (0, 9999,0)
      CALL HLDIR   (' ','T')

C CREATE SUBDIRECTORIES IN //PAWC
      CALL HCDIR   ('//PAWC',' ')
      CALL HMDIR   ('SUBD1',' ')
      CALL HMDIR   ('SUBD2',' ')

      CALL HCDIR   ('//PAWC/SUBD1',' ')
      CALL HMDIR   ('SUBD3',' ')

C FILL SUBDIRECTORIES
      CALL HCDIR   ('//PAWC/SUBD1',' ')
      CALL HCDIR   ('//PHAN',' ')
      CALL HRIN    (0, 9999,0)

      CALL HCDIR   ('//PAWC/SUBD2',' ')
      CALL HCDIR   ('//PHAN',' ')
      CALL HRIN    (0, 9999,0)

      CALL HCDIR   ('//PAWC',' ')
      CALL HLDIR   (' ','T')

      CALL HREND   ('PHAN')

C Save //PAWC/... in t.rz
      CALL HRPUT   (0,'t.rz','T')

      STOP
      END
