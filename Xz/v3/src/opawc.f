C/* +---------------------- Copyright notice -------------------------------+ */
C/* | Copyright (C) 1995, Guy Barrand, LAL Orsay. (barrand@lalcls.in2p3.fr) | */
C/* |   Permission to use, copy, modify, and distribute this software       | */
C/* |   and its documentation for any purpose and without fee is hereby     | */
C/* |   granted, provided that the above copyright notice appear in all     | */
C/* |   copies and that both that copyright notice and this permission      | */
C/* |   notice appear in supporting documentation.  This software is        | */
C/* |   provided "as is" without express or implied warranty.               | */
C/* +---------------------- Copyright notice -------------------------------+ */
C VMS : First routine must have same name as file name so that
C       descrip.mms works fine with lib dependencies.
C AXP : NWPAW doit etre un multiple de 16 : 409600 = 3200 * 128
C AIX : To initialize common variables, DATA xxx/0/ involves link problems. 
C       We then use the ohbook.f/OPAWI routine to do the job. It must be called
C       before ANY CERNLIB routine that use PAWC, ZUNIT.
C       It is called in XzTypes.c/XzSetTypes.     
C
C If MZEBRA called then IQREAD ne 0
C If MZPAW  called then H(1)=NWPAW
C************************************************************
      SUBROUTINE OPAWC (NPAW,NKUIP)
C************************************************************
C!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IMPLICIT NONE
      INTEGER NPAW,NKUIP
C+KEEP,HCBOOK
      INTEGER    NWPAW
      PARAMETER (NWPAW = 3200 * 128)     
      INTEGER       H
      COMMON /PAWC/ H(NWPAW)
C+KEEP, ZUNIT. 
      INTEGER IQREAD,IQPRNT,IQPR2,IQLOG,IQPNCH,IQTTIN,IQTYPE
      COMMON /ZUNIT/ IQREAD,IQPRNT,IQPR2,IQLOG,IQPNCH,IQTTIN,IQTYPE
C
      INTEGER    NWKUIP 
      PARAMETER (NWKUIP = 200000)
C............................................................
      NPAW  = NWPAW
      NKUIP = NWKUIP
      RETURN 
      END

