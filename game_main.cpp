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
    int num_triangles;
    glm::mat4 M;
    glm::mat4 V;
    glm::mat4 MVP;
    GLuint ShaderProgramID;
    GLuint M_MatrixID;
    GLuint V_MatrixID;
    GLuint MVP_MatrixID;
    GLuint TextureID;
    GLuint VertexArrayID;
    GLuint LightPosID;
    GLuint texture;
    int keys_down[8];
};

int SCREEN_WIDTH=640,SCREEN_HEIGHT=480;
float FOV=45;
bool shouldEnd=false;
SDL_Event e;
static int keys[] = { SDLK_a, SDLK_d, SDLK_w, SDLK_s, SDLK_z, SDLK_x, SDLK_r, SDLK_q };

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

void
display_matrix( int m, int n, float *v )
{
    int i, j;
    for (i=0; i<m; i++) {
        for (j=0; j<m; j++) {
            printf("    %08f",*v);
            v++;
        }
        printf("\n");
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
    glm::mat4 a;
a = glm::mat4(10.0f);
a[3][3] = 1.0;//.00.1;
//display_matrix(4,4,&a[0][0]);
return a;//glm::mat4(12.0f);
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

  num_triangles = LoadModelFromFile("cube.obj", tetra_buffers);
  if (num_triangles==0) {
      fprintf(stderr, "Failed to load tetra\n");
      return 0;
  }

  texture = LoadTextureFromFile("rock.png", GL_RGB);

  ShaderProgramID=LoadShadersIntoProgram("game_vertex_shader.glsl","game_fragment_shader.glsl");
  if (ShaderProgramID == 0) {
    fprintf(stderr,"Shader load failed\n");
    return 0;
  }
  //Getting MVP, giving to shader
  LightPosID   = glGetUniformLocation(ShaderProgramID, "LightPosition_worldspace");
  M_MatrixID   = glGetUniformLocation(ShaderProgramID, "M");
  V_MatrixID   = glGetUniformLocation(ShaderProgramID, "V");
  MVP_MatrixID = glGetUniformLocation(ShaderProgramID, "MVP");

  TextureID = glGetUniformLocation(ShaderProgramID,"textureSampler");
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
    int i;
  while (SDL_PollEvent(&e)!=0){
    if (e.type==SDL_QUIT)
      shouldEnd=true;
    if (e.type==SDL_KEYDOWN) {
        for (i=0; i<sizeof(keys)/sizeof(e.key.keysym.sym); i++) {
            if (e.key.keysym.sym==keys[i]) keys_down[i]=1;
        }
    }
    if (e.type==SDL_KEYUP) {
        for (i=0; i<sizeof(keys)/sizeof(e.key.keysym.sym); i++) {
            if (e.key.keysym.sym==keys[i]) keys_down[i]=0;
        }
    }
  }
  if (keys_down[0]) { lookat[0]-=0.1; }
  if (keys_down[1]) { lookat[0]+=0.1; }
  if (keys_down[2]) { lookat[1]-=0.1; }
  if (keys_down[3]) { lookat[1]+=0.1; }
  if (keys_down[4]) { lookat[2]-=0.1; }
  if (keys_down[5]) { lookat[2]+=0.1; }
  if (keys_down[6]) { lookat[0]=4.0; lookat[1]=3.0; lookat[2]=3.0; }
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glUseProgram(ShaderProgramID);

  M = FindModelMatrix();
  V = FindViewMatrix(lookat);
  MVP = FindProjectionMatrix(0.1f,100.0f)*V*M;
//MVP = FindProjectionMatrix(0.1f,100.0f)*FindViewMatrix(lookat)*FindModelMatrix();
glUniformMatrix4fv(M_MatrixID,  1,GL_FALSE,&M[0][0]);
glUniformMatrix4fv(V_MatrixID,  1,GL_FALSE,&V[0][0]);
  glUniformMatrix4fv(MVP_MatrixID,1,GL_FALSE,&MVP[0][0]);
  glUniform3f(LightPosID,3.0,2.0,2.0);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture);
  glUniform1i(TextureID,0);

  glEnableVertexAttribArray(0);
//  glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, tetra_buffers[0]);
  glVertexAttribPointer(0,
			3,
			GL_FLOAT,
			GL_FALSE,
			0,
			(void*)0);

  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, tetra_buffers[1]);
  glVertexAttribPointer(1,
			2,
			GL_FLOAT,
			GL_FALSE,
			0,
			(void*)0);

  glEnableVertexAttribArray(2);
  glBindBuffer(GL_ARRAY_BUFFER, tetra_buffers[2]);
  glVertexAttribPointer(2,
			3,
			GL_FLOAT,
			GL_FALSE,
			0,
			(void*)0);

  glDrawArrays(GL_TRIANGLES,0,3*num_triangles);
  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
  glDisableVertexAttribArray(2);
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
    if (!m->CreateWindow(screen_width, screen_height)) {
        fprintf(stderr,"Create window failes\n");
        return 4;
    }
    if (!m->InitScene()) {
        fprintf(stderr,"Init scene failed\n");
        return 4;
    }

    fprintf(stderr,"Looping forever\n");
    m->LoopForever();
    fprintf(stderr,"Done looping forever...\n");
    m->Exit();
    return 0;
}
