#ifndef __INC_GAME_GL_H__
#define __INC_GAME_GL_H__

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

class c_game_gl
{
public:
    c_game_gl(int width, int height);
    ~c_game_gl();
    int init(void);
    glm::mat4 FindProjectionMatrix(float zNearClip,float zFarClip);
    glm::mat4 FindViewMatrix(void);
    glm::mat4 FindModelMatrix(float x, float y, float z, float scale);
    void draw(void);
    void draw_start(void);
    void draw_cube(float x, float y, float z);
    void draw_complete(void);

    GLuint tetra_buffers[3];
    int num_triangles;
    float aspect_ratio;
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

    float FOV;
    float head_yaw, head_pitch;
    glm::vec3 body_pos;
    glm::vec3 body_facing; // z is probably 0
};

#endif // __INC_GAME_GL_H__
