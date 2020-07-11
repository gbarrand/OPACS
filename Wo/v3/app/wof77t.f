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
      PROGRAM WOF77T
C************************************************************
C!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IMPLICIT NONE
      EXTERNAL CMDF77
C............................................................
C  To load an interface file, 
C you can also remove WOFILE and
C set the environment variable 
C WOENVIRONMENT.
      CALL WOINIT ()
      CALL WOCMD  ('Wo/cmdf77',9,CMDF77)
      CALL WOFILE ('WoTest.odb',10)
      CALL WORUN  ()
      STOP
      END
C************************************************************
      INTEGER*4 FUNCTION CMDF77 ()
C************************************************************
C!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IMPLICIT NONE
C
      INTEGER*4     ARGN
      CHARACTER*80  STRING
      INTEGER*4     ARGI,LSTR  
      INTEGER*4     CDARGN,CDARGI
C............................................................
      ARGN = CDARGN()
      DO 10 ARGI=1,ARGN
        LSTR = CDARGI(ARGI,STRING,80)
        PRINT *,'ARGUMENT:',ARGI,' ',STRING(1:LSTR)
 10   CONTINUE
      CMDF77 = 0
      RETURN
      END

