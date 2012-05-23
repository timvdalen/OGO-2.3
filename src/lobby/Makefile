# Project: Grudge of the Oblivious: Lobby application
# Makefile for gnu compilers
#
# Windows users:
# 	 type 'set platform=win32' prior to using this makefile
#
# examples:
#   make clean run-cli build=netdebug
#   make clean all
#   make build=release
#
# have fun!
#

CPP      = g++
CC       = gcc
RM       = -rm -f
MV       = -mv -f

OBJDIR   = ../../obj/
BINDIR   = ../../bin/
INCS     =  -I"../common"
CXXINCS  =  -I"../common"
OBJ      = $(OBJDIR)games.o $(OBJDIR)lobby.o $(OBJDIR)ConfigFile.o $(OBJDIR)CrossPlatform.o $(OBJDIR)net.o $(OBJDIR)protocol.o
BIN      = $(BINDIR)Lobby
CXXFLAGS = $(CXXINCS)
LIBS     = -lpthread

CLI_OBJ = $(OBJDIR)main.o $(OBJ)
CLI_BIN := $(BIN)CLI
CLI_CXXFLAGS = $(CXXFLAGS)
CLI_LIBS = $(LIBS)

GUI_OBJ = $(OBJDIR)gui.o $(OBJ)
GUI_BIN := $(BIN)GUI
GUI_CXXFLAGS = `wx-config --cxxflags` $(CXXFLAGS)
GUI_LIBS = `wx-config --libs` $(LIBS)

ifeq ($(platform),win32)
	LIBS    += -lws2_32
	BIN     := $(BIN).exe
	CLI_BIN := $(CLI_BIN).exe
	GUI_BIN := $(GUI_BIN).exe
	GUI_LIBS = `wx-config --static=yes --libs` -static $(LIBS)
endif

ifeq ($(build),debug)
	CXXFLAGS += -g -DDEBUG
endif
ifeq ($(build),netdebug)
	CXXFLAGS += -g -DDEBUG -DNETDEBUG
endif
ifeq ($(build),release)
	CXXFLAGS += -fexpensive-optimizations -O3
endif

.PHONY: all gui cli clean run run-cli run-gui
default: gui

all: $(CLI_BIN) $(GUI_BIN)

gui: $(GUI_BIN)
	$(MV) $(GUI_BIN) $(BIN)

cli: $(CLI_BIN)
	$(MV) $(CLI_BIN) $(BIN)

clean:
	${RM} $(BIN) $(CLI_BIN) $(GUI_BIN)
	${RM} $(OBJDIR)*.o

run: default
	@echo -------------------------------------------------------------------------------
	@echo Running default...
	@echo
	@$(BIN)

run-cli: cli
	@echo -------------------------------------------------------------------------------
	@echo Running CLI...
	@echo
	@$(BIN)

run-gui: gui
	@echo -------------------------------------------------------------------------------
	@echo Running GUI...
	@echo
	@$(BIN)

$(BIN): $(OBJ)
	$(CPP) $(OBJ) -o "$(BIN)" $(LIBS)

$(CLI_BIN): $(CLI_OBJ)
	$(CPP) $(CLI_OBJ) -o "$(CLI_BIN)" $(CLI_LIBS)

$(GUI_BIN): $(GUI_OBJ)
	$(CPP) $(GUI_OBJ) -o "$(GUI_BIN)" $(GUI_LIBS)

$(OBJDIR)games.o: games.cpp
	$(CPP) -c games.cpp -o $(OBJDIR)games.o $(CXXFLAGS)

$(OBJDIR)lobby.o: lobby.cpp
	$(CPP) -c lobby.cpp -o $(OBJDIR)lobby.o $(CXXFLAGS)

$(OBJDIR)ConfigFile.o: ../common/ConfigFile.cpp
	$(CPP) -c ../common/ConfigFile.cpp -o $(OBJDIR)ConfigFile.o $(CXXFLAGS)

$(OBJDIR)CrossPlatform.o: ../common/CrossPlatform.cpp
	$(CPP) -c ../common/CrossPlatform.cpp -o $(OBJDIR)CrossPlatform.o $(CXXFLAGS)

$(OBJDIR)net.o: ../common/net.cpp
	$(CPP) -c ../common/net.cpp -o $(OBJDIR)net.o $(CXXFLAGS) -fpermissive

$(OBJDIR)protocol.o: ../common/protocol.cpp
	$(CPP) -c ../common/protocol.cpp -o $(OBJDIR)protocol.o $(CXXFLAGS)

$(OBJDIR)main.o: main.cpp
	$(CPP) -c main.cpp -o $(OBJDIR)main.o $(CLI_CXXFLAGS)

$(OBJDIR)gui.o: gui.cpp
	$(CPP) -c gui.cpp -o $(OBJDIR)gui.o $(GUI_CXXFLAGS) 