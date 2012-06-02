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
OBJ      = $(OBJDIR)base.o $(OBJDIR)core.o $(OBJDIR)main.o $(OBJDIR)video.o    \
           $(OBJDIR)ConfigFile.o $(OBJDIR)net.o $(OBJDIR)protocol.o            \
           $(OBJDIR)objects.o  $(OBJDIR)netalg.o $(OBJDIR)CrossPlatform.o      \
           $(OBJDIR)materials.o $(OBJDIR)movement.o $(OBJDIR)input.o           \
           $(OBJDIR)image.o
BIN      = $(BINDIR)Game
CXXFLAGS = $(CXXINCS)
LIBS     = -lpthread -lpng -lz 

ifeq ($(platform),win32)
	CXXFLAGS += -DFREEGLUT_STATIC
	LIBS     += -lws2_32 -lfreeglut_static -lglu32 -lopengl32 -lgdi32 -lwinmm
	BIN      := $(BIN).exe
else
	LIBS     += -lGL -lGLU -lglut
endif

ifneq ($(platform),macos)
    LIBS     += -lrt
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

$(OBJDIR)CrossPlatform.o: ../common/CrossPlatform.cpp
	$(CPP) -c ../common/CrossPlatform.cpp -o $(OBJDIR)CrossPlatform.o $(CXXFLAGS)

$(OBJDIR)net.o: ../common/net.cpp
	$(CPP) -c ../common/net.cpp -o $(OBJDIR)net.o $(CXXFLAGS) -fpermissive

$(OBJDIR)protocol.o: ../common/protocol.cpp
	$(CPP) -c ../common/protocol.cpp -o $(OBJDIR)protocol.o $(CXXFLAGS)

$(OBJDIR)video.o: video_glut.cpp
	$(CPP) -c video_glut.cpp -o $(OBJDIR)video.o $(CXXFLAGS)

$(OBJDIR)input.o: input_glut.cpp
	$(CPP) -c input_glut.cpp -o $(OBJDIR)input.o $(CXXFLAGS)

$(OBJDIR)movement.o: movement.cpp
	$(CPP) -c movement.cpp -o $(OBJDIR)movement.o $(CXXFLAGS)

$(OBJDIR)netalg.o: netalg.cpp
	$(CPP) -c netalg.cpp -o $(OBJDIR)netalg.o $(CXXFLAGS) 

$(OBJDIR)materials.o: materials.cpp
	$(CPP) -c materials.cpp -o $(OBJDIR)materials.o $(CXXFLAGS) 

$(OBJDIR)image.o: image.cpp
	$(CPP) -c image.cpp -o $(OBJDIR)image.o $(CXXFLAGS) 