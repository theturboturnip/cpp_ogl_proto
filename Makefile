
all: prog


OS := $(shell uname)

LINK      = g++
LINKFLAGS = -lGL -lSDL2main -lSDL2
CPPFLAGS  = -Wall

ifeq ($(OS),Darwin)
GLM = ../glm
LINK      = c++
LINKFLAGS = -iframework /Library/Frameworks -framework SDL2 -framework OpenGL
CPPFLAGS  = -Wall -I$(GLM) -iframework /Library/Frameworks
endif

OBJS = main.o loader.o

prog: $(OBJS)
	$(LINK) $(OBJS) $(LINKFLAGS) -o prog

clean:
	rm *.o prog

