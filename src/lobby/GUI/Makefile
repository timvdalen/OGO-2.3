CPP      = g++
CXXFLAGS = `wx-config --cxxflags`
LIBS     = `wx-config --libs` 
OBJ      = gui.o
CFOBJ	 = ../../common/ConfigFile.o
BIN      = ../../../bin/lobby
RM	 = rm -f

.phony: all

ifdef SYSTEMROOT
	LIBS = `wx-config --static=yes --libs` -static
endif

all: $(BIN)

clean:
	$(RM) $(OBJ) $(CFOBJ) $(BIN)

gui.o: gui.cpp
	$(CPP) -c gui.cpp -o gui.o $(CXXFLAGS)

ConfigFile.o: ../../common/ConfigFile.cpp
	$(CPP) -c ../../common/ConfigFile.cpp -o $(CFOBJ)

$(BIN): $(OBJ) ConfigFile.o
	$(CPP) $(OBJ) $(CFOBJ) -o $(BIN) $(LIBS)
