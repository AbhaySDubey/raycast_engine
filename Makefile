CC = g++
FLAGS = -lraylib -lwinmm -lgdi32 -Wall
INC = ./raylib-5.5_win64_mingw-w64/include
LIB = ./raylib-5.5_win64_mingw-w64/lib

all: game.exe

game.exe:
	$(CC) -o game.exe main.cpp -I $(INC) -L $(LIB) $(FLAGS)

clean:
	rm -rf game.exe