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
      SUBROUTINE OKUIP ()
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
C************************************************************
C************************************************************
      SUBROUTINE OKINIT (SIZE)
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
      INTEGER NPAW,NKUIP
C............................................................
      CALL OPAWC(NPAW,NKUIP)
C      PRINT *,'DEBUG : OKINIT : NWPAW',NPAW
      IF((SIZE.GT.NPAW).OR.(SIZE.LE.0)) THEN
        SIZE = NKUIP
      ENDIF
C
C      PRINT *,'DEBUG : OKINIT : IQREAD,NWPAW,IKUIP',IQREAD,NWPAW,IXKU
      IF(IQREAD.EQ.0) THEN
C MZEBRA NOT DONE
        CALL MZEBRA (-3)
      ENDIF
      IF(NWPAW.EQ.0) THEN
C MZPAW NOT DONE
        CALL MZPAW  (NPAW,' ')
      ENDIF
C
C     IXKU not null if KUIP inited
C
      IF(IXKU.EQ.0) THEN    
        CALL KUINIT (SIZE)
      ENDIF
      RETURN 
      END
C************************************************************
      INTEGER*4 FUNCTION OKITED ()
C************************************************************
C Return 1 if KUIP already inited, 0 if not.
C!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IMPLICIT NONE
C+KEEP, PAWC.
      INTEGER     NWPAW,IXPAWC,IHDIV,IXHIGZ,IXKU
      COMMON/PAWC/NWPAW,IXPAWC,IHDIV,IXHIGZ,IXKU
C............................................................
      IF(IXKU.EQ.0) THEN    
        OKITED = 0
      ELSE
        OKITED = 1
      ENDIF
      RETURN 
      END
