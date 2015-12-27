#define GLM_FORCE_RADIANS
#define GL_GLEXT_PROTOTYPES
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtx/transform.hpp>
#include <SDL2/SDL_opengl.h>
#include "loader.h"

class Transform{
 public:
  glm::vec3 position,rotation,scale;
};

class Mesh{
 public:
  Mesh(string modelPath,string texturePath,Transform t);
  void Draw(glm::mat4 VP,GLuint shaderMatrixLocation,GLuint shaderTextureLocation);
 protected:
  Transform transform;
  GLuint model[3],texture;
  unsigned int vertexNum;
};
