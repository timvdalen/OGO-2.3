CPP      = g++
CXXFLAGS = `wx-config --cxxflags`
LIBS     = `wx-config --static=yes --libs` -static
OBJ      = gui.o
BIN      = lobby

.phony: all

all: $(BIN)

gui.o: gui.cpp
	$(CPP) -c gui.cpp -o gui.o $(CXXFLAGS)

$(BIN): $(OBJ)
	$(CPP) $(OBJ) -o lobby $(LIBS)