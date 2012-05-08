CPP      = g++
CXXFLAGS = `wx-config --cxxflags`
LIBS     = `wx-config --libs`
OBJ      = gui.o
BIN      = lobby
RM	 = rm

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
	$(CPP) $(OBJ) -o lobby $(LIBS)
