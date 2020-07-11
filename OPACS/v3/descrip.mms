!
! Usage :
!     VMS$ mms
!     VMS$ mms WoXo
!     VMS$ mms/macros=(CONFIG=VAX) WoXo
!     VMS$ mms/macros=(CONFIG=VAX,TARGET=libs) WoXo
!
!

!TARGET = clean
TARGET = all
!CONFIG = VAX
CONFIG = ALPHA

all :: WoXo

Wo ::
	set def COROOT:[mgr]
	mms/macro=t=$(TARGET) $(CONFIG)
	set def XXROOT:[mgr]
	mms/macro=t=$(TARGET) $(CONFIG)
	set def WOROOT:[mgr]
	mms/macro=t=$(TARGET) $(CONFIG)

WoXo ::
	set def COROOT:[mgr]
	mms/macro=t=$(TARGET) $(CONFIG)
	set def HOROOT:[mgr]
	mms/macro=t=$(TARGET) $(CONFIG)
	set def W3OROOT:[mgr]
	mms/macro=t=$(TARGET) $(CONFIG)
	set def HTMLOROOT:[mgr]
	mms/macro=t=$(TARGET) $(CONFIG)
	set def GLOROOT:[mgr]
	mms/macro=t=$(TARGET) $(CONFIG)
	set def GOROOT:[mgr]
	mms/macro=t=$(TARGET) $(CONFIG)
	set def XXROOT:[mgr]
	mms/macro=t=$(TARGET) $(CONFIG)
	set def XOROOT:[mgr]
	mms/macro=t=$(TARGET) $(CONFIG)
	set def WOROOT:[mgr]
	mms/macro=t=$(TARGET) $(CONFIG)

WoXoCERN ::
	set def COROOT:[mgr]
	mms/macro=t=$(TARGET) $(CONFIG)
	set def HOROOT:[mgr]
	mms/macro=t=$(TARGET) $(CONFIG)
	set def W3OROOT:[mgr]
	mms/macro=t=$(TARGET) $(CONFIG)
	set def HTMLOROOT:[mgr]
	mms/macro=t=$(TARGET) $(CONFIG)
	set def GLOROOT:[mgr]
	mms/macro=t=$(TARGET) $(CONFIG)
	set def GOROOT:[mgr]
	mms/macro=t=$(TARGET) $(CONFIG)
	set def XXROOT:[mgr]
	mms/macro=t=$(TARGET) $(CONFIG)
	set def XOROOT:[mgr]
	mms/macro=t=$(TARGET) $(CONFIG)
	set def XZROOT:[mgr]
	mms/macro=t=$(TARGET) $(CONFIG)
	set def G3OROOT:[mgr]
	mms/macro=t=$(TARGET) $(CONFIG)
	set def WOROOT:[mgr]
	mms/macro=t=$(TARGET) $(CONFIG)


