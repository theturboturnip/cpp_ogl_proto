
all: prog


OS := $(shell uname)

LINK      = g++
LINKFLAGS = -lGL -lSDL2main -lSDL2 -lSDL2_image
CPPFLAGS  = -Wall

ifeq ($(OS),Darwin)
GLM = ../glm
LINK      = c++
LINKFLAGS = -iframework /Library/Frameworks -framework SDL2 -framework SDL2_image -framework OpenGL
CPPFLAGS  = -Wall -I$(GLM) -iframework /Library/Frameworks
endif

OBJS = main.o loader.o game.o game_level.o

prog: $(OBJS)
	$(LINK) $(OBJS) $(LINKFLAGS) -o prog

clean:
	rm *.o prog

