#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtx/transform.hpp>
#include <SDL2/SDL_opengl.h>
#include "loader.h"
#include "mesh.h"

Mesh::Mesh(string modelPath,string texturePath,Transform t){
  LoadModelFromFile(modelPath, model); // Returns 0 on ERROR
  texture=LoadTextureFromFile(texturePath, GL_RGB);
  transform=t;
}
