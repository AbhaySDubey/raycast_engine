CC := g++
INC = ./raylib_5.5/include
LIB = ./raylib_5.5/lib

CFLAGS := -Wall -I $(INC)
LFLAGS := -L $(LIB) -lraylib -lwinmm -lgdi32 -Wall

SRC := main.cpp operator_overloads.cpp map_utils.cpp
OBJ := $(SRC:%.cpp=build/%.o)


TARGET := build/game.exe

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(LFLAGS)

build/%.o: %.cpp
	@mkdir -p build
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -rf build/