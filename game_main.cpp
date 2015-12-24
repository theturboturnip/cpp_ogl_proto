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

class c_main
{
public:
    c_main(void);
    ~c_main();
    void checkSDLError(void);
    int Init(void);
    void MainLoop(void);
    void LoopForever(void);
    void Exit(void);
    int CreateWindow(int width, int height);
    int InitScene(void);
    SDL_Window    *window;
    SDL_GLContext glContext;
    GLuint tetra_buffers[3];
    float lookat[3];
};

int SCREEN_WIDTH=640,SCREEN_HEIGHT=480;
float FOV=45;
bool shouldEnd=false;
SDL_Event e;
glm::mat4 MVP;
GLuint VertexArrayID,VertexBuffer,ShaderProgramID,MatrixID;

c_main::c_main(void)
{
    window = NULL;
    lookat[0] = 4.0;
    lookat[1] = 3.0;
    lookat[2] = 3.0;
}

void
c_main::checkSDLError(void)
{
	const char *error;
    error = SDL_GetError();
	if (*error != '\0') {
		fprintf(stderr,"SDL Error: %s\n", error);
		SDL_ClearError();
	}
}

glm::mat4 FindProjectionMatrix(float zNearClip,float zFarClip){
  float FOVrads=glm::radians(FOV),aspect=(float)SCREEN_WIDTH/(float)SCREEN_HEIGHT;
  return glm::perspective(FOVrads,aspect,zNearClip,zFarClip);
}

glm::mat4 FindViewMatrix(float lookat[3]){
    return glm::lookAt(glm::vec3(lookat[0],lookat[1],lookat[2]),
		     glm::vec3(0,0,0),
		     glm::vec3(0,1,0));
}
glm::mat4 FindModelMatrix(){
  return glm::mat4(12.0f);
}

int c_main::Init(void)
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        return 0;
    }

    SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 8 );
    SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 8 );
    SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 8 );
    SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, 8 );

    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 3 );
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    checkSDLError();

    return 1;
}

void
c_main::Exit(void)
{
    if (window) {
        SDL_DestroyWindow(window);
    }
    SDL_Quit();
}

int
c_main::CreateWindow(int width, int height)
{
    int flags; 

    flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
    window = SDL_CreateWindow("OpenGL Test",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,width, height, flags);
    checkSDLError();

    if (!window) {
        return 0;
    }

    glContext=  SDL_GL_CreateContext(window);
    if (!glContext) {
        return 0;
    }

    if (0) {
        int major, minor;
        SDL_GL_GetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, &major );
        SDL_GL_GetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, &minor );
        checkSDLError();
        fprintf(stderr, "Using OpenGL version %d.%d\n",major,minor);
    }
    return 1;
}

int 
c_main::InitScene(void)
{

  glClearColor(0.0f,0.0f,0.4f,0.0f);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glGenVertexArrays(1,&VertexArrayID);
  glBindVertexArray(VertexArrayID);
  GLfloat g_vertex_buffer_data[] = {
    -1.0f,-1.0f,0.0f,
    1.0f,-1.0f,0.0f,
    0.0f,1.0f,0.0f};
  glGenBuffers(1,&VertexBuffer);
  glBindBuffer(GL_ARRAY_BUFFER,VertexBuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
  
  if (LoadModelFromFile("tetra.obj", tetra_buffers)!=1) {
      return 0;
  }

  ShaderProgramID=LoadShadersIntoProgram("MatrixVertexShader.glsl","SimpleFragmentShader.glsl");
  if(ShaderProgramID==0){
    fprintf(stderr,"Shader link failed\n");
    return 0;
  }
  //Getting MVP, giving to shader
  MatrixID = glGetUniformLocation(ShaderProgramID, "MVP");
  return 1;
}

/*Basic object creation
 need vertex shader + fragment shader for entire object
 compile and store program
 load verticies from .obj to VBO and VAO
 To draw, use a Draw() function
*/


void
c_main::MainLoop(void)
{
  while (SDL_PollEvent(&e)!=0){
    if (e.type==SDL_QUIT)
      shouldEnd=true;
    if (e.type==SDL_KEYDOWN) {
        if (e.key.keysym.sym==SDLK_a) {
            lookat[0] -= 0.1;
        }
        if (e.key.keysym.sym==SDLK_d) {
            lookat[0] += 0.1;
        }
        if (e.key.keysym.sym==SDLK_r) {
            lookat[0] = 4.0;
        }
    }
  }
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glUseProgram(ShaderProgramID);

  MVP      = FindProjectionMatrix(0.1f,100.0f)*FindViewMatrix(lookat)*FindModelMatrix();

  glUniformMatrix4fv(MatrixID,1,GL_FALSE,&MVP[0][0]);

  glEnableVertexAttribArray(0);
//  glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, tetra_buffers[0]);
  glVertexAttribPointer(0,
			3,
			GL_FLOAT,
			GL_FALSE,
			0,
			(void*)0);
  glDrawArrays(GL_TRIANGLES,0,3*4);
  glDisableVertexAttribArray(0);
  SDL_GL_SwapWindow(window);
}

void
c_main::LoopForever(void)
{
    while (!shouldEnd){
        MainLoop();
    }
}

int main(int argc,char *argv[]){
    int screen_width, screen_height;
    c_main *m;

    screen_width = 640;
    screen_height = 512;
    if (argc==3){
        screen_width  = atoi(argv[1]);
        screen_height = atoi(argv[2]);
    }

    m = new c_main();
    m->Init();
    if (!m->CreateWindow(screen_width, screen_height))
        return 4;
    if (!m->InitScene())
        return 4;

    m->LoopForever();
    m->Exit();
    return 0;
}
