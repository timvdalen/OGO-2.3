# Project: Grudge of the Oblivious: Installer and setup
# Makefile for NSIS (Nullsoft Scriptable Install System)
# Will create windows installer binaries
#
# examples:
#   make
#   make clean all
#   make test
#
# The latter will obviously only work at windows systems
#
# have fun!
#

RM     = -rm -f
NSIS   = makensis

BINDIR = ../bin/
SETUP  = $(BINDIR)Setup.exe
INST   = InstallGoto.exe

all: $(SETUP) $(INST)

clean:
	$(RM) $(SETUP)
	$(RM) $(INST)

test: all
	$(INST)

$(SETUP): config.nsi config.ini
	$(NSIS) config.nsi

$(INST): installer.nsi $(SETUP)
	$(NSIS) installer.nsi