CPP      = g++
CXXFLAGS = `wx-config --cxxflags`
LIBS     = `wx-config --libs`
LIBSS	 = `wx-config --static=yes --libs`
OBJ      = gui.o
BIN      = lobby
RM	 = rm

.phony: all

all: $(BIN)

clean:
	$(RM) $(OBJ) $(BIN)

gui.o: gui.cpp
	$(CPP) -c gui.cpp -o gui.o $(CXXFLAGS)

$(BIN): $(OBJ)
	$(CPP) $(OBJ) -o lobby $(LIBS)

static: $(OBJ)
	$(CPP) $(OBJ) -o lobby $(LIBSS) -static
