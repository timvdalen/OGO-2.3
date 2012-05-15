# Project: lobby
# Makefile created by Dev-C++ 4.9.9.2

CPP  = g++ -D__DEBUG__
CC   = gcc -D__DEBUG__
RES  = 
OBJ  = ../../obj/games.o ../../obj/main.o ../../obj/net.o ../../obj/protocol.o ../../obj/lobby.o $(RES)
LINKOBJ  = ../../obj/games.o ../../obj/main.o ../../obj/net.o ../../obj/protocol.o ../../obj/lobby.o $(RES)
LIBS =  -lpthread  -g3 
INCS =  -I"../common" 
CXXINCS =  -I"../common" 
BIN  = ../../bin/lobby
CXXFLAGS = $(CXXINCS) -DNETDEBUG   -g3
CFLAGS = $(INCS)   -g3
RM = rm -f

.PHONY: all all-before all-after clean clean-custom

all: all-before ../../bin/lobby all-after


clean: clean-custom
	${RM} $(OBJ) $(BIN)

$(BIN): $(OBJ)
	$(CPP) $(LINKOBJ) -o "../../bin/lobby" $(LIBS)

../../obj/games.o: games.cpp
	$(CPP) -c games.cpp -o ../../obj/games.o $(CXXFLAGS)

../../obj/main.o: main.cpp
	$(CPP) -c main.cpp -o ../../obj/main.o $(CXXFLAGS)

../../obj/net.o: ../common/net.cpp
	$(CPP) -c ../common/net.cpp -o ../../obj/net.o $(CXXFLAGS)

../../obj/protocol.o: ../common/protocol.cpp
	$(CPP) -c ../common/protocol.cpp -o ../../obj/protocol.o $(CXXFLAGS)

../../obj/lobby.o: lobby.cpp
	$(CPP) -c lobby.cpp -o ../../obj/lobby.o $(CXXFLAGS)
