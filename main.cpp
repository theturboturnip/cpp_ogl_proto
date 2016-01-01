#define GL_GLEXT_PROTOTYPES
#define GLM_FORCE_RADIANS
#include <SDL.h> 
#include <SDL_opengl.h>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtx/transform.hpp>
#include <cstdlib>
#include "loader.h"
#include "mesh.h"
#include "Engine/window.h"

class GameWindow *window=NULL;
bool shouldEnd=false;
SDL_Event e;
glm::mat4 V,P;
GLuint ShaderProgramID,MatrixID,TextureID,RockTexture,Rock2Texture;
Mesh *m;



glm::mat4 FindViewMatrix(){
  return glm::lookAt(glm::vec3(4,4,3),
		     glm::vec3(0,0,0),
		     glm::vec3(0,1,0));
}
glm::mat4 FindModelMatrix(){
  return glm::mat4(1.0f);
}

/*int CreateWindow(void){
  int major, minor;

   if (SDL_Init(SDL_INIT_EVERYTHING)!= 0){
    ThrowSDLError();
    return 1;
  }

  fprintf(stderr,"Setting GL basic attributes\n");
  SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 8 );
  SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 8 );
  SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 8 );
  SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, 8 );

  fprintf(stderr,"Setting GL version and profile\n");
  SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
  SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 3 );
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  checkSDLError();

  int flags=SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
  window = SDL_CreateWindow("OpenGL Test",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,SCREEN_WIDTH,SCREEN_HEIGHT,flags);
  if (window==NULL){
    ThrowSDLError();
    return 1;
  }
  glContext=  SDL_GL_CreateContext(window);
  if (!glContext) {
    return 1;
  }

  SDL_GL_GetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, &major );
  SDL_GL_GetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, &minor );
  checkSDLError();
  fprintf(stderr, "Using OpenGL version %d.%d\n",major,minor);

  return 0;
  }*/

int InitScene(void){
  window->SetClearColor(0.0f,0.0f,0.4f,0.0f);
  
  /*GLfloat g_vertex_buffer_data[] = {
    -1.0f,-1.0f,0.0f,
    1.0f,-1.0f,0.0f,
    0.0f,1.0f,0.0f};
  glGenBuffers(1,&VertexBuffer);
  glBindBuffer(GL_ARRAY_BUFFER,VertexBuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);*/
  m=new Mesh(new Transform(),"monkey.obj","rock.png");
  ShaderProgramID=LoadShadersIntoProgram("game_vertex_shader.glsl","custom_fragment_shader.glsl");
  window->UseShaderProgram(ShaderProgramID);
  if(ShaderProgramID==0){
    fprintf(stderr,"Shader link failed\n");
    return 1;
  }
  //Getting MVP, giving to shader
  MatrixID=glGetUniformLocation(ShaderProgramID, "M");
  GLuint ViewMatID=glGetUniformLocation(ShaderProgramID, "V"),ProjectionMatID=glGetUniformLocation(ShaderProgramID, "P"),LightPosID=glGetUniformLocation(ShaderProgramID, "LightPosition_worldspace");
  //checkSDLError();
  //VP =FindProjectionMatrix(0.1f,100.0f)*FindViewMatrix();
  P=window->ProjectionMatrix;
  V=FindViewMatrix();
  glUniformMatrix4fv(ViewMatID,1,GL_FALSE,&V[0][0]);
  glUniformMatrix4fv(ProjectionMatID,1,GL_FALSE,&P[0][0]);
  glUniform3f(LightPosID,4.0f,3.0f,3.0f);

  TextureID=glGetUniformLocation(ShaderProgramID,"textureSampler");
  //LoadTextureFromFile("rock.png",RockTexture);
  //LoadTextureFromFile("rock2.png",Rock2Texture);
  return 0;
}

int InitWindow(){
    window=new GameWindow();
    if (window==NULL)
        return 1;
    return 0;
}

void BindTexture(GLuint texture){
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture);
  glUniform1i(TextureID,0);
}

/*Basic object creation
 need vertex shader + fragment shader for entire object
 compile and store program
 load verticies from .obj to VBO and VAO
 To draw, use a Draw() function
*/

void MainLoop(void){
  while (SDL_PollEvent(&e)!=0){
    if (e.type==SDL_QUIT)
      shouldEnd=true;
  }
  window->ClearWindow();
  //glUseProgram(ShaderProgramID);
  m->Draw(MatrixID);
  m->transform->rotation.x+=0.1;
  //BindTexture(Rock2Texture,TextureID);
  //m->Draw(MatrixID);
  //m->transform->position.x=0.0;
  window->Flip();
}

int main(int argc,char *argv[]){
  if (InitWindow()==1) return 1;
  if (InitScene()==1) return 1;
  while (!shouldEnd){
    MainLoop();
  }
  window->End();
  return 0;
}
