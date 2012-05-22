# Project: Grudge of the Oblivious: Game application
# Makefile for gnu compilers
#
# Windows users:
# 	 type 'set platform=win32' prior to using this makefile
#
# examples:
#   make clean run build=netdebug
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
OBJ      = $(OBJDIR)base.o $(OBJDIR)core.o $(OBJDIR)main.o $(OBJDIR)objects.o  \
           $(OBJDIR)ConfigFile.o $(OBJDIR)net.o $(OBJDIR)protocol.o            \
           $(OBJDIR)video.o $(OBJDIR)netalg.o
BIN      = $(BINDIR)Game
CXXFLAGS = $(CXXINCS) 
LIBS     = -lpthread 

ifeq ($(platform),win32)
	CXXFLAGS += -DFREEGLUT_STATIC
	LIBS     += -lws2_32 -lfreeglut_static -lopengl32 -lgdi32 -lwinmm
	BIN      := $(BIN).exe
else
	LIBS     += -lGL
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

.PHONY: all clean run 
default: all

all: $(BIN)

clean:
	${RM} $(BIN)
	${RM} $(OBJ)

run: default
	@echo -------------------------------------------------------------------------------
	@echo Running game...
	@echo 
	@$(BIN)

$(BIN): $(OBJ)
	$(CPP) $(OBJ) -o "$(BIN)" $(LIBS)

$(OBJDIR)base.o: base.cpp
	$(CPP) -c base.cpp -o $(OBJDIR)base.o $(CXXFLAGS)

$(OBJDIR)core.o: core.cpp
	$(CPP) -c core.cpp -o $(OBJDIR)core.o $(CXXFLAGS)

$(OBJDIR)main.o: main.cpp
	$(CPP) -c main.cpp -o $(OBJDIR)main.o $(CXXFLAGS)

$(OBJDIR)objects.o: objects.cpp
	$(CPP) -c objects.cpp -o $(OBJDIR)objects.o $(CXXFLAGS)

$(OBJDIR)ConfigFile.o: ../common/ConfigFile.cpp
	$(CPP) -c ../common/ConfigFile.cpp -o $(OBJDIR)ConfigFile.o $(CXXFLAGS)

$(OBJDIR)net.o: ../common/net.cpp
	$(CPP) -c ../common/net.cpp -o $(OBJDIR)net.o $(CXXFLAGS) -fpermissive

$(OBJDIR)protocol.o: ../common/protocol.cpp
	$(CPP) -c ../common/protocol.cpp -o $(OBJDIR)protocol.o $(CXXFLAGS)

$(OBJDIR)video.o: video.cpp
	$(CPP) -c video.cpp -o $(OBJDIR)video.o $(CXXFLAGS)

$(OBJDIR)netalg.o: netalg.cpp
	$(CPP) -c netalg.cpp -o $(OBJDIR)netalg.o $(CXXFLAGS)