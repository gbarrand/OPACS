C/* +---------------------- Copyright notice -------------------------------+ */
C/* | Copyright (C) 1995, Guy Barrand, LAL Orsay. (barrand@lalcls.in2p3.fr) | */
C/* |   Permission to use, copy, modify, and distribute this software       | */
C/* |   and its documentation for any purpose and without fee is hereby     | */
C/* |   granted, provided that the above copyright notice appear in all     | */
C/* |   copies and that both that copyright notice and this permission      | */
C/* |   notice appear in supporting documentation.  This software is        | */
C/* |   provided "as is" without express or implied warranty.               | */
C/* +---------------------- Copyright notice -------------------------------+ */
      IMPLICIT NONE

      INTEGER    NWPAW
      PARAMETER( NWPAW = 500000)
      REAL          H
      COMMON /PAWC/ H(NWPAW)

      INTEGER ISTAT
C............................................................
      CALL HLIMIT(NWPAW)
      
      CALL HROPEN( 1, 'PHAN','phan.rz', ' ',1024, ISTAT )
      IF ( ISTAT .NE. 0 ) THEN
         PRINT *, ' FAILURE TO OPEN FILE '
         STOP
      ENDIF
C PUT //PHAN IN //PAWC
      CALL HRIN    (0, 9999,0)
      CALL HREND   ('PHAN')

      CALL HLDIR (' ',' ')

      CALL HPLINT(1)
      CALL HPLOT(12,' ',' ',0)

      PRINT *,'TYPE A NUMBER TO EXIT.'
      READ *,ISTAT

      STOP
      END
