#ifndef __INC_MESH_H__
#define __INC_MESH_H__

#include "includes.h"
#include "loader.h"

class Transform{
 public:
  glm::vec3 position,rotation,scale;
  glm::mat4 Evaluate(void);
  Transform(void);
};

class Mesh{
 public:
  Mesh(Transform *t=NULL,string modelPath="",string texturePath="");
  void Draw(GLuint shaderMatrixLocation,GLuint shaderTextureLocation=0);
  Transform *transform;
 protected:
  GLuint model[3],texture,VertexArrayID;
  bool textureEnabled;
  unsigned int vertexNum;
};

#endif // __INC_MESH_H__
