#
#  The "setup" files define the "ROOT" environment 
# variables that specify the packages locations. 
# You have to edit these files to specify these location.
#
#  Usage :
#      UNIX> make                               # build the group in the `uname` config.
#      UNIX> make CONFIG=Linux-gxx WoXo         # build the "WoXo" group in a more specific config.
#      UNIX> make TARGET=libs                   # build the group libraries in the `uname` config 
#      UNIX> make CONFIG=Linux-gxx TARGET=libs 
#
#  The "config" are defined in : Co, W3o, HTMLo mgr/Config.mk.
# 

SHELL  = /bin/sh

TARGET = all
CONFIG = `uname`

all :: WoXo

Wo ::
	@cd $$COROOT/mgr;   $(MAKE) $(CONFIG) t=$(TARGET)
	@cd $$XXROOT/mgr;   $(MAKE) $(CONFIG) t=$(TARGET)
	@cd $$WOROOT/mgr;   $(MAKE) $(CONFIG) t=$(TARGET)

WoXo ::
	@cd $$COROOT/mgr;   $(MAKE) $(CONFIG) t=$(TARGET)
	@cd $$HOROOT/mgr;   $(MAKE) $(CONFIG) t=$(TARGET)
	@cd $$W3OROOT/mgr;  $(MAKE) $(CONFIG) t=$(TARGET)
	@cd $$HTMLOROOT/mgr;$(MAKE) $(CONFIG) t=$(TARGET)
	@cd $$GLOROOT/mgr;  $(MAKE) $(CONFIG) t=$(TARGET)
	@cd $$GOROOT/mgr;   $(MAKE) $(CONFIG) t=$(TARGET)
	@cd $$XXROOT/mgr;   $(MAKE) $(CONFIG) t=$(TARGET)
	@cd $$XOROOT/mgr;   $(MAKE) $(CONFIG) t=$(TARGET)
	@cd $$WOROOT/mgr;   $(MAKE) $(CONFIG) t=$(TARGET)

WoXoCERN ::
	@cd $$COROOT/mgr;   $(MAKE) $(CONFIG) t=$(TARGET)
	@cd $$HOROOT/mgr;   $(MAKE) $(CONFIG) t=$(TARGET)
	@cd $$W3OROOT/mgr;  $(MAKE) $(CONFIG) t=$(TARGET)
	@cd $$HTMLOROOT/mgr;$(MAKE) $(CONFIG) t=$(TARGET)
	@cd $$GLOROOT/mgr;  $(MAKE) $(CONFIG) t=$(TARGET)
	@cd $$GOROOT/mgr;   $(MAKE) $(CONFIG) t=$(TARGET)
	@cd $$XXROOT/mgr;   $(MAKE) $(CONFIG) t=$(TARGET)
	@cd $$XOROOT/mgr;   $(MAKE) $(CONFIG) t=$(TARGET)
	@cd $$XZROOT/mgr;   $(MAKE) $(CONFIG) t=$(TARGET)
	@cd $$G3OROOT/mgr;  $(MAKE) $(CONFIG) t=$(TARGET)
	@cd $$WOROOT/mgr;   $(MAKE) $(CONFIG) t=$(TARGET)

WoXoG4o ::
	@cd $$COROOT/mgr;   $(MAKE) $(CONFIG) t=$(TARGET)
	@cd $$HOROOT/mgr;   $(MAKE) $(CONFIG) t=$(TARGET)
	@cd $$W3OROOT/mgr;  $(MAKE) $(CONFIG) t=$(TARGET)
	@cd $$HTMLOROOT/mgr;$(MAKE) $(CONFIG) t=$(TARGET)
	@cd $$GLOROOT/mgr;  $(MAKE) $(CONFIG) t=$(TARGET)
	@cd $$GOROOT/mgr;   $(MAKE) $(CONFIG) t=$(TARGET)
	@cd $$XXROOT/mgr;   $(MAKE) $(CONFIG) t=$(TARGET)
	@cd $$XOROOT/mgr;   $(MAKE) $(CONFIG) t=$(TARGET)
	@cd $$G4OROOT/mgr;  $(MAKE) $(CONFIG) t=$(TARGET)
	@cd $$WOROOT/mgr;   $(MAKE) $(CONFIG) t=$(TARGET)

