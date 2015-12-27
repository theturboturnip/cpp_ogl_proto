#define GLM_FORCE_RADIANS
#define GL_GLEXT_PROTOTYPES
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtx/transform.hpp>
#include <SDL2/SDL_opengl.h>
#include "loader.h"
#include "mesh.h"

Mesh::Mesh(string modelPath,string texturePath,Transform t){
  LoadModelFromFile(modelPath, model, &vertexNum); // Returns 0 on ERROR
  texture=LoadTextureFromFile(texturePath, GL_RGB);
  transform=t;
}
void Mesh::Draw(glm::mat4 VP,GLuint shaderMatrixLocation,GLuint shaderTextureLocation){
  //VP is Projection  * View
  //Scale,rotate,translate for model matrix
  //glm::mat4 rotationMatrix=;
  glm::mat4 M=glm::translate(transform.position)*glm::scale(transform.scale);
  glm::mat4 MVP=VP*M;
  //Apply MVP matrix
  glUniformMatrix4fv(shaderMatrixLocation,1,GL_FALSE,&MVP[0][0]);
  //Apply Texture 
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture);
  glUniform1i(shaderTextureLocation,0);
  //Give verticies
  glEnableVertexAttribArray(0);
  //Assign Verticies
  glBindBuffer(GL_ARRAY_BUFFER,model[0]);
  glVertexAttribPointer(0,
			3,//Vertex has 3 points: x,y,z
			GL_FLOAT,
			GL_FALSE,
			0,
			(void*)0);
  glEnableVertexAttribArray(1);
  //Assign UV
  glBindBuffer(GL_ARRAY_BUFFER,model[1]);
  glVertexAttribPointer(1,
			2,//UV has 2 points: x,y
			GL_FLOAT,
			GL_FALSE,
			0,
			(void*)0);
  glDrawArrays(GL_TRIANGLES,0,vertexNum*3);
}
