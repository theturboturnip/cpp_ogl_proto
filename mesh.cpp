#define GLM_FORCE_RADIANS
#define GL_GLEXT_PROTOTYPES
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtx/transform.hpp>
#include <SDL2/SDL_opengl.h>
#include "loader.h"
#include "mesh.h"

Transform::Transform(){
  position=glm::vec3(0,0,0);
  rotation=glm::vec3(0,0,0);
  scale=glm::vec3(1,1,1);
}

glm::mat4 Transform::Evaluate(){
  //Scale,Rotate,Translate
  glm::mat4 toReturn=glm::scale(scale);
  glm::mat4 rotationMat=glm::rotate(rotation.x,glm::vec3(1,0,0));
  rotationMat=glm::rotate(rotationMat,rotation.y,glm::vec3(0,1,0));
  rotationMat=glm::rotate(rotationMat,rotation.z,glm::vec3(0,0,1));
  toReturn*=rotationMat;
  toReturn=glm::translate(toReturn,position);
  return toReturn;
}

Mesh::Mesh(std::string modelPath,std::string texturePath,Transform *t){
  vertexNum=LoadModelFromFile(modelPath, model); // Returns 0 on ERROR
  texture=LoadTextureFromFile(texturePath, GL_RGB);
  transform=t;
}
void Mesh::Draw(GLuint shaderMatrixLocation,GLuint shaderTextureLocation){
  //VP is Projection  * View
  //Scale,rotate,translate for model matrix
  //Calculate MVP Matrix
  glm::mat4 M;
  if (transform!=NULL&&false){
    M=transform->Evaluate();//glm::translate(transform->position)*glm::scale(transform->scale);
    //MVP=VP*M;
  }else {
      glm::mat4 a;
      a = glm::mat4(1.0f);
      a[3][3] = 1.0;//.00.1;
      M=a; //glm::mat4(12.0f);
  }
  //Apply MVP Matrix
  glUniformMatrix4fv(shaderMatrixLocation,1,GL_FALSE,&M[0][0]);
  //Apply Texture 
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture);
  glUniform1i(shaderTextureLocation,0);
  //Enable Arrays
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);
  //Assign Verticies
  glBindBuffer(GL_ARRAY_BUFFER,model[0]);
  glVertexAttribPointer(0,
			3,//Vertex has 3 points: x,y,z
			GL_FLOAT,
			GL_FALSE,
			0,
			(void*)0);
  //Assign UV
  glBindBuffer(GL_ARRAY_BUFFER,model[1]);
  glVertexAttribPointer(1,
			2,//UV has 2 points: x,y
			GL_FLOAT,
			GL_FALSE,
			0,
			(void*)0);
  //Assign Normals
  glBindBuffer(GL_ARRAY_BUFFER,model[2]);
  glVertexAttribPointer(2,
			3,//Normal has 3 points: x,y,z
			GL_FLOAT,
			GL_FALSE,
			0,
			(void*)0);
  //Actually Draw
  glDrawArrays(GL_TRIANGLES,0,vertexNum*3);
  //Disable Arrays
  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
  glDisableVertexAttribArray(2);
}
