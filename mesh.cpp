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
Mesh::Draw(glm::mat4 VP,GLuint shaderMatrixLocation){
  //VP is Projection  * View
  //Scale,rotate,translate for model matrix
  //glm::mat4 rotationMatrix=;
  glm::mat4 M=glm::translate(transform.position)*glm::scale(transform.scale);
  glm::mat4 MVP=VP*M;
  //Apply MVP matrix
  glUniformMatrix4fv(shaderMatrixLocation,1,GL_FALSE,&MVP[0][0]);
  //Apply Texture 
  //textureSampler
  //Give verticies
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER,model[0]);
  glVertexAttribPointer(0,
