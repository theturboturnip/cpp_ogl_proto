
all: prog


OS := $(shell uname)

LINK      = g++
LINKFLAGS = -lGL -lSDL2main -lSDL2 -lSDL2_image
CPPFLAGS  = -Wall -I/usr/include/SDL2

ifeq ($(OS),Darwin)
GLM = ../glm
LINK      = c++
LINKFLAGS = -iframework /Library/Frameworks -framework SDL2 -framework SDL2_image -framework OpenGL
CPPFLAGS  = -Wall -I$(GLM) -iframework /Library/Frameworks -I/Library/Frameworks/SDL2.framework/Headers -I/Library/Frameworks/SDL2_image.framework/Headers
endif

OBJS = main.o loader.o game.o game_level.o mesh.o

prog: $(OBJS)
	$(LINK) $(OBJS) $(LINKFLAGS) -o prog

clean:
	rm *.o prog

