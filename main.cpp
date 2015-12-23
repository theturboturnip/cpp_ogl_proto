#define GL_GLEXT_PROTOTYPES
#include <SDL2/SDL.h> 
#include <SDL2/SDL_opengl.h>
#include <iostream>
#include "loadShader.cpp"
#include <stdlib.h>
#include <stdio.h>
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtx/transform.hpp>
#include <cstdlib>

int SCREEN_WIDTH=640,SCREEN_HEIGHT=480;
float FOV=45;
SDL_Window* window=NULL;
SDL_GLContext glContext;
bool shouldEnd=false;
SDL_Event e;
glm::mat4 MVP;
GLuint VertexArrayID,VertexBuffer,ShaderProgramID,MatrixID;

void checkSDLError(void)
{
	const char *error = SDL_GetError();
	if (*error != '\0')
	{
		fprintf(stderr,"SDL Error: %s\n", error);
		SDL_ClearError();
	}
}

glm::mat4 FindProjectionMatrix(float zNearClip,float zFarClip){
  float FOVrads=glm::radians(FOV),aspect=(float)SCREEN_WIDTH/(float)SCREEN_HEIGHT;
  return glm::perspective(FOVrads,aspect,zNearClip,zFarClip);
}

glm::mat4 FindViewMatrix(){
  return glm::lookAt(glm::vec3(4,3,3),
		     glm::vec3(0,0,0),
		     glm::vec3(0,1,0));
}
glm::mat4 FindModelMatrix(){
  return glm::mat4(1.0f);
}

void ThrowSDLError(void){
  std::cout << "SDL Error Encountered: "<<SDL_GetError() << "\n";
}

int CreateWindow(void){
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
}

void InitScene(void){
  glClearColor(0.0f,0.0f,0.4f,0.0f);
  glGenVertexArrays(1,&VertexArrayID);
  glBindVertexArray(VertexArrayID);
  GLfloat g_vertex_buffer_data[] = {
    -1.0f,-1.0f,0.0f,
    1.0f,-1.0f,0.0f,
    0.0f,1.0f,0.0f};
  glGenBuffers(1,&VertexBuffer);
  glBindBuffer(GL_ARRAY_BUFFER,VertexBuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
  ShaderProgramID=LoadShaders("MatrixVertexShader.glsl","SimpleFragmentShader.glsl");
  //Getting MVP, giving to shader
  MatrixID=glGetUniformLocation(ShaderProgramID, "MVP");
  MVP =FindProjectionMatrix(0.1f,100.0f)*FindViewMatrix()*FindModelMatrix();
}
/*Basic object creation
 need vertex shader + fragment shader for entire object
 compile and store program
 load verticies from .obj to VBO and VAO
 To draw, use a Draw() function
*/

void EndSDL(void){
  SDL_DestroyWindow(window);
  SDL_Quit();
  return;
}

void MainLoop(void){
  while (SDL_PollEvent(&e)!=0){
    if (e.type==SDL_QUIT)
      shouldEnd=true;
  }
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glUseProgram(ShaderProgramID);
  glUniformMatrix4fv(MatrixID,1,GL_FALSE,&MVP[0][0]);

  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
  glVertexAttribPointer(0,
			3,
			GL_FLOAT,
			GL_FALSE,
			0,
			(void*)0);
  glDrawArrays(GL_TRIANGLES,0,3);
  glDisableVertexAttribArray(0);
  SDL_GL_SwapWindow(window);
}

int main(int argc,char *argv[]){
  //fprintf(stderr, "Argument Count: %d\n",argc);
  if (argc==3){
    SCREEN_WIDTH=atoi(argv[1]);
    SCREEN_HEIGHT=atoi(argv[2]);
  }
  if (CreateWindow()==1) return 1;
  InitScene();
  while (!shouldEnd){
    MainLoop();
  }
  EndSDL();
  return 0;
}
