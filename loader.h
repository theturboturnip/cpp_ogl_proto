#ifndef __INC_LOADER_H__
#define __INC_LOADER_H__

#define GL_GLEXT_PROTOTYPES
#define GLM_FORCE_RADIANS

#include <iostream>
#include <fstream>
#include <vector>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace std;

GLuint LoadShaderFromFile(string pathString,GLenum shaderType);
GLuint LoadShadersIntoProgram(string vertexPath,string fragPath,GLuint program=0);
GLuint LoadTextureFromFile(string imagePath, GLuint imageType);
int LoadModelFromFile(string modelPath, GLuint buffers[3], float scale=1.0);
int LoadModelFromFile(string modelPath, GLuint buffers[3], glm::mat3 *transform=NULL, glm::vec3 *translation=NULL);

#endif // __INC_LOADER_H__

