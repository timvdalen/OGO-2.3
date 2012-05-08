CPP      = g++
CXXFLAGS = `wx-config --cxxflags`
LIBS     = `wx-config --libs`
OBJ      = gui.o
BIN      = ../../../bin/lobby
RM	 = rm -f

.phony: all

ifdef SYSTEMROOT
	LIBS = `wx-config --static=yes --libs` -static
endif

all: $(BIN)

clean:
	$(RM) $(OBJ) $(BIN)

gui.o: gui.cpp
	$(CPP) -c gui.cpp -o gui.o $(CXXFLAGS)

$(BIN): $(OBJ)
	$(CPP) $(OBJ) -o $(BIN) $(LIBS)
