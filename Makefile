CC = g++
FLAGS = -lraylib -lwinmm -lgdi32 -Wall
INC = ./raylib_5.5/include
LIB = ./raylib_5.5/lib

all: game.exe

game.exe:
	$(CC) -o game.exe main.cpp operator_overloads.cpp map_utils.cpp -I $(INC) -L $(LIB) $(FLAGS)

clean:
	rm -rf game.exe