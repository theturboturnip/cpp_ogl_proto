#define GL_GLEXT_PROTOTYPES
#define GLM_FORCE_RADIANS
#include <SDL.h> 
#include <SDL_opengl.h>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtx/transform.hpp>
#include <cstdlib>
#include "loader.h"

#define NUM_KEYS 14
static int keys[NUM_KEYS] = {
SDLK_d, SDLK_a,
SDLK_x, SDLK_z,
SDLK_s, SDLK_w,
SDLK_r, SDLK_q,
SDLK_o, SDLK_p,
SDLK_m, SDLK_n,
SDLK_i, SDLK_j,
};

class c_main
{
public:
    c_main(int screen_width, int screen_height);
    ~c_main();
    void checkSDLError(void);
    int Init(void);
    void MainLoop(void);
    void LoopForever(void);
    void Exit(void);
    int CreateWindow(int width, int height);
    int InitScene(void);
    glm::mat4 FindProjectionMatrix(float zNearClip,float zFarClip);
    glm::mat4 FindViewMatrix(void);
    glm::mat4 FindModelMatrix(float x, float y, float z, float scale);
    void draw_start(void);
    void draw_cube(float x, float y, float z);
    void draw_complete(void);

    SDL_Window    *window;
    SDL_GLContext glContext;
    GLuint tetra_buffers[3];
    int num_triangles;
    glm::mat4 M;
    glm::mat4 V;
    glm::mat4 P;
    glm::mat4 MVP;
    GLuint ShaderProgramID;
    GLuint M_MatrixID;
    GLuint V_MatrixID;
    GLuint P_MatrixID;
    GLuint MVP_MatrixID;
    GLuint TextureID;
    GLuint VertexArrayID;
    GLuint LightPosID;
    GLuint texture;
    int keys_down[NUM_KEYS];
    bool shouldEnd;
    int screen_width, screen_height;
    float FOV;
    float head_yaw, head_pitch;
    glm::vec3 body_pos;
    glm::vec3 body_facing; // z is probably 0
};

c_main::c_main(int screen_width, int screen_height)
{
    int i;
    this->screen_width = screen_width;
    this->screen_height = screen_height;
    window = NULL;
    body_pos    = glm::vec3(4.0,3.0,3.0);
    body_facing = glm::vec3(-1.0,-1.0,0.0);
    body_facing = glm::normalize(body_facing);
    head_yaw = 0;
    head_pitch = 0;
    shouldEnd = false;
    FOV = 45;
    for (i=0; i<NUM_KEYS; i++) {
        keys_down[i] = 0;
    }
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

glm::mat4 c_main::FindProjectionMatrix(float zNearClip,float zFarClip){
    float FOVrads=glm::radians(FOV),aspect=(float)screen_width/screen_height;
  return glm::perspective(FOVrads,aspect,zNearClip,zFarClip);
}

glm::mat4 c_main::FindViewMatrix(void){
    // direction facting is (body_facing rotated by yaw around Z) rotated by pitch around X
    glm::vec3 eye_direction;
    eye_direction = glm::rotate( body_facing, glm::radians(head_yaw), glm::vec3(0,0,1) );
    eye_direction = glm::rotate( eye_direction, glm::radians(head_pitch), glm::vec3(1,0,0) );
    return glm::lookAt(body_pos,
                       body_pos + eye_direction,
                       glm::vec3(0,0,1)); // Z is up
}

glm::mat4 c_main::FindModelMatrix(float x, float y,float z, float scale){
    glm::mat4 a;
a = glm::mat4(scale);
a[3][0] = x;
a[3][1] = y;
a[3][2] = z;
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

  ShaderProgramID=LoadShadersIntoProgram("game_vertex_shader.glsl","custom_fragment_shader.glsl");
  if (ShaderProgramID == 0) {
    fprintf(stderr,"Shader load failed\n");
    return 0;
  }
  //Getting MVP, giving to shader
  LightPosID   = glGetUniformLocation(ShaderProgramID, "LightPosition_worldspace");
  M_MatrixID   = glGetUniformLocation(ShaderProgramID, "M");
  V_MatrixID   = glGetUniformLocation(ShaderProgramID, "V");
  P_MatrixID   = glGetUniformLocation(ShaderProgramID, "P");

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
c_main::draw_start(void)
{
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    //glCullFace(GL_FRONT); // You can try this to cull front faces for fun :-)
    glActiveTexture(GL_TEXTURE0); // Uset texture unit 0 throughout
    V = FindViewMatrix();
    P = FindProjectionMatrix(0.1f,100.0f);
    glUniformMatrix4fv(V_MatrixID,  1,GL_FALSE,&V[0][0]);
    glUniformMatrix4fv(P_MatrixID,  1,GL_FALSE,&P[0][0]);
    glUniform1i(TextureID,0);
    glUniform3f(LightPosID,4.0,3.0,3.0);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
}

void
c_main::draw_complete(void)
{
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}

void
c_main::draw_cube(float x, float y, float z)
{

    M = FindModelMatrix(x,y,z,5);
    glUniformMatrix4fv(M_MatrixID,  1,GL_FALSE,&M[0][0]);

    glBindTexture(GL_TEXTURE_2D, texture);

    glBindBuffer(GL_ARRAY_BUFFER, tetra_buffers[0]);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,NULL);
    glBindBuffer(GL_ARRAY_BUFFER, tetra_buffers[1]);
    glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,0,NULL);
    glBindBuffer(GL_ARRAY_BUFFER, tetra_buffers[2]);
    glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE,0,NULL);
    glDrawArrays(GL_TRIANGLES,0,3*num_triangles);
    glDrawArrays(GL_LINES,0,3*num_triangles);
}

void
c_main::MainLoop(void)
{
    int i;
SDL_Event e;
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
  if (keys_down[0]) { body_pos[0]-=0.1; }
  if (keys_down[1]) { body_pos[0]+=0.1; }
  if (keys_down[2]) { body_pos[1]-=0.1; }
  if (keys_down[3]) { body_pos[1]+=0.1; }
  if (keys_down[4]) { body_pos[2]-=0.1; }
  if (keys_down[5]) { body_pos[2]+=0.1; }
  if (keys_down[6]) { body_pos[0]=4.0; body_pos[1]=3.0; body_pos[2]=3.0; }
  if (keys_down[7]) { shouldEnd = true; }
  if (keys_down[8]) { FOV -= 1; }
  if (keys_down[9]) { FOV += 1; }
  if (keys_down[10]) { head_yaw -= 1; }
  if (keys_down[11]) { head_yaw += 1; }
  if (keys_down[12]) { head_pitch -= 1; }
  if (keys_down[13]) { head_pitch += 1; }
  head_yaw = 0.97*head_yaw;
  head_pitch = 0.97*head_pitch;
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glUseProgram(ShaderProgramID);

  draw_start();
  for (i=0; i<10; i++) {
      draw_cube(i,i,i);
  }
  draw_complete();

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

    m = new c_main(screen_width, screen_height);
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
