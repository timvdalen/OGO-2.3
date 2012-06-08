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
UNZIP    = unzip -uo 
WGET     = wget --no-check-certificate

OBJDIR   = ../../obj/
BINDIR   = ../../bin/
INCS     =  -I"../common"
CXXINCS  =  -I"../common"
OBJ      = $(OBJDIR)base.o $(OBJDIR)core.o $(OBJDIR)main.o $(OBJDIR)video.o    \
           $(OBJDIR)ConfigFile.o $(OBJDIR)net.o $(OBJDIR)protocol.o            \
           $(OBJDIR)objects.o  $(OBJDIR)clique.o $(OBJDIR)tokenring.o          \
           $(OBJDIR)CrossPlatform.o $(OBJDIR)materials.o $(OBJDIR)movement.o   \
           $(OBJDIR)input.o $(OBJDIR)image.o $(OBJDIR)assets.o                 \
           $(OBJDIR)netcode.o $(OBJDIR)game.o  $(OBJDIR)hud.o                  \
           $(OBJDIR)loader3ds.o $(OBJDIR)structures.o $(OBJDIR)world.o
BIN      = $(BINDIR)Game
CXXFLAGS = $(CXXINCS)
LIBS     = -lpthread -lpng -lz -l3ds

DLDIR    = https://github.com/downloads/timvdalen/OGO-2.3/
ASSETS   = assets.zip

ifeq ($(platform),win32)
	CXXFLAGS += -DFREEGLUT_STATIC
	LIBS     += -lws2_32 -lfreeglut_static -lglu32 -lopengl32 -lgdi32 -lwinmm
	BIN      := $(BIN).exe
else
	LIBS     += -lGL -lGLU
ifneq ($(platform),macos)
    LIBS     += -lrt -lglut
else
    LIBS 	+= -lfreeglut
endif
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

.PHONY: all clean run assets
default: all

all: $(BIN)

clean:
	${RM} $(BIN)
	${RM} $(OBJ)

run: default
	@echo -------------------------------------------------------------------------------
	@echo Running game...
	@echo
	@$(BIN) -p $(BINDIR) &

assets:
	$(WGET) $(DLDIR)$(ASSETS) -O $(ASSETS)
	$(UNZIP) $(ASSETS) -d $(BINDIR)
	$(RM) $(ASSETS)

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

$(OBJDIR)structures.o: structures.cpp
	$(CPP) -c structures.cpp -o $(OBJDIR)structures.o $(CXXFLAGS)

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

$(OBJDIR)clique.o: clique.cpp
	$(CPP) -c clique.cpp -o $(OBJDIR)clique.o $(CXXFLAGS) 

$(OBJDIR)tokenring.o: tokenring.cpp
	$(CPP) -c tokenring.cpp -o $(OBJDIR)tokenring.o $(CXXFLAGS) 

$(OBJDIR)materials.o: materials.cpp
	$(CPP) -c materials.cpp -o $(OBJDIR)materials.o $(CXXFLAGS) 

$(OBJDIR)image.o: image.cpp
	$(CPP) -c image.cpp -o $(OBJDIR)image.o $(CXXFLAGS) 

$(OBJDIR)assets.o: assets.cpp
	$(CPP) -c assets.cpp -o $(OBJDIR)assets.o $(CXXFLAGS) 

$(OBJDIR)netcode.o: netcode.cpp
	$(CPP) -c netcode.cpp -o $(OBJDIR)netcode.o $(CXXFLAGS) 

$(OBJDIR)game.o: game.cpp
	$(CPP) -c game.cpp -o $(OBJDIR)game.o $(CXXFLAGS) 

$(OBJDIR)hud.o: hud.cpp
	$(CPP) -c hud.cpp -o $(OBJDIR)hud.o $(CXXFLAGS) 

$(OBJDIR)loader3ds.o: loader3ds.cpp
	$(CPP) -c loader3ds.cpp -o $(OBJDIR)loader3ds.o $(CXXFLAGS) 
	
$(OBJDIR)world.o: world.cpp
	$(CPP) -c world.cpp -o $(OBJDIR)world.o $(CXXFLAGS) 
