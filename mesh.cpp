#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtx/transform.hpp>
#include <SDL2/SDL_opengl.h>
#include "loader.h"

Mesh::Mesh(string modelPath,string texturePath,Transform t){
  model=LoadModelFromFile(modelPath);
  texture=LoadTextureFromFile(texturePath);
  transform=t;
}
