#ifndef __INC_MESH_H__
#define __INC_MESH_H__

#define GLM_FORCE_RADIANS
#define GL_GLEXT_PROTOTYPES
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtx/transform.hpp>
#include <SDL2/SDL_opengl.h>
#include "loader.h"

class Transform{
 public:
  glm::vec3 position,rotation,scale;
  glm::mat4 Evaluate(void);
  Transform(void);
};

class Mesh{
 public:
  Mesh(string modelPath,string texturePath,Transform *t=NULL);
  void Draw(GLuint shaderMatrixLocation,GLuint shaderTextureLocation);
  Transform *transform;
 protected:
  GLuint model[3],texture,VertexArrayID;
  unsigned int vertexNum;
};

#endif // __INC_MESH_H__
