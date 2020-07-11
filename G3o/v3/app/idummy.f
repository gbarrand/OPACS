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
C With these dummy routine, we could link only
C  with libgeant and libpacklib (not to libgraflib)
C************************************************************
      SUBROUTINE IPM ()
C************************************************************
C!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IMPLICIT      NONE
C............................................................
      CALL CWRITE('G3o/idummy.f/IPM: dummy',10)
      RETURN
      END
C************************************************************
      SUBROUTINE ISPMCI ()
C************************************************************
C!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IMPLICIT      NONE
C............................................................
      CALL CWRITE('G3o/idummy.f/ISPMCI: dummy',13)
      RETURN
      END
