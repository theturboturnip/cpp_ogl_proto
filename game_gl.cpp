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
#include "hud.h"
#include "game_gl.h"

c_game_gl::c_game_gl(int width, int height)
{
    aspect_ratio = (float)width / height;
}

glm::mat4 c_game_gl::FindProjectionMatrix(float zNearClip,float zFarClip){
    float FOVrads=glm::radians(FOV),aspect=aspect_ratio;
    return glm::perspective(FOVrads,aspect,zNearClip,zFarClip);
}

glm::mat4 c_game_gl::FindViewMatrix(void){
    // direction facting is (body_facing rotated by yaw around Z) rotated by pitch around X
    glm::vec3 eye_direction;
    eye_direction = glm::rotate( body_facing, glm::radians(head_yaw), glm::vec3(0,0,1) );
    eye_direction = glm::rotate( eye_direction, glm::radians(head_pitch), glm::vec3(1,0,0) );
    return glm::lookAt(body_pos,
                       body_pos + eye_direction,
                       glm::vec3(0,0,1)); // Z is up
}

glm::mat4 c_game_gl::FindModelMatrix(float x, float y,float z, float scale){
    glm::mat4 a;
    a = glm::mat4(scale);
    a[3][0] = x;
    a[3][1] = y;
    a[3][2] = z;
    a[3][3] = 1.0;//.00.1;
    return a;//glm::mat4(12.0f);
}

int 
c_game_gl::init(void)
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

void
c_game_gl::draw_start(void)
{
    glUseProgram(ShaderProgramID);
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
c_game_gl::draw_complete(void)
{
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}

void
c_game_gl::draw_cube(float x, float y, float z)
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
    glDrawArrays(GL_LINES,0,3*num_triangles);
    glDrawArrays(GL_TRIANGLES,0,3*num_triangles);
}

void
c_game_gl::draw(void)
{
    int i;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    draw_start();
    for (i=0; i<10; i++) {
        draw_cube(i,i,i);
    }
    draw_complete();
}
