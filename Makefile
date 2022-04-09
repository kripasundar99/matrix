# Adapted from https://dev.to/talhabalaj/setup-visual-studio-code-for-multi-file-c-projects-1jpi

CPP       := g++
CPP_FLAGS := -std=c++17 -ggdb

BIN     := bin
SRC     := src
INCLUDE := include

LIBRARIES   :=
EXECUTABLE  := matrix


all: $(BIN)/$(EXECUTABLE)

run: clean all
	clear
	./$(BIN)/$(EXECUTABLE)

$(BIN)/$(EXECUTABLE): $(SRC)/*.cpp
	$(CPP) $(CPP_FLAGS) -I$(INCLUDE) $^ -o $@ $(LIBRARIES)
	$(BIN)/$(EXECUTABLE)

clean:
	-rm $(BIN)/*

