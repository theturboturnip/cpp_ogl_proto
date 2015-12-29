
all: prog


OS := $(shell uname)

LINK      = g++
LINKFLAGS = -g -lGL -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf
CPPFLAGS  = -g -Wall -I/usr/include/SDL2

ifeq ($(OS),Darwin)
GLM = ../glm
LINK      = c++
LINKFLAGS = -iframework /Library/Frameworks -framework SDL2 -framework SDL2_image -framework SDL2_ttf -framework OpenGL
CPPFLAGS  = -Wall -I$(GLM) -iframework /Library/Frameworks -I/Library/Frameworks/SDL2.framework/Headers -I/Library/Frameworks/SDL2_image.framework/Headers -I/Library/Frameworks/SDL2_ttf.framework/Headers
endif

OBJS = main.o loader.o mesh.o
GAME_OBJS  = game_main.o loader.o game.o game_level.o hud.o

prog: $(OBJS)
	$(LINK) $(OBJS) $(LINKFLAGS) -o prog

game: $(GAME_OBJS)
	$(LINK) $(GAME_OBJS) $(LINKFLAGS) -o game

clean:
	rm *.o prog

