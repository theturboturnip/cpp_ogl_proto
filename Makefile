
all: prog

prog: main.o
	g++ -o prog main.o -lGL -lSDL2main -lSDL2

main.o: main.cpp
	g++ -Wall -c main.cpp -o main.o
