#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <cstdlib>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL.h> 
#include <SDL2/SDL_image.h>
using namespace std;

GLuint LoadShaderFromFile(string pathString,GLenum shaderType);
GLuint LoadShadersIntoProgram(string vertexPath,string fragPath,GLuint program=0);
GLuint LoadTextureFromFile(string imagePath);
