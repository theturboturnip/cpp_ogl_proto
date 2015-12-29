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

class c_mesh
{
public:
    c_mesh(void);
    ~c_mesh();
    int load_obj(const char *obj_file, float scale);
    int num_triangles;
    GLuint attrib_buffers[3];
};

c_mesh::c_mesh(void)
{
}

c_mesh::~c_mesh()
{
}

class c_texture
{
public:
    c_texture(void);
    ~c_texture();
    int load_png(const char *png_filename);
    GLuint texture;
};

c_texture::c_texture(void)
{
}

c_texture::~c_texture()
{
}

int
c_texture::load_png(const char *png_filename)
{
    texture = LoadTextureFromFile(png_filename, GL_RGB);
    if (texture == 0) {
        return 0;
    }
    return 1;
}

struct game_gl_mesh {
    const char *obj_filename;
    int        mesh_id;
    float      scale;
};

struct game_gl_texture {
    const char *png_filename;
    int        texture_id;
};

static struct game_gl_mesh meshes_to_load[] =
{
    {"cube.obj",   1, 5.0},
    {"monkey.obj", 2, 1.0},
    {NULL, -1},
};

static struct game_gl_texture textures_to_load[] =
{
    {"rock.png", 1},
    {NULL, -1},
};

int
c_mesh::load_obj(const char *obj_file, float scale)
{
    num_triangles = LoadModelFromFile(obj_file, attrib_buffers, scale);
    if (num_triangles==0) {
        fprintf(stderr, "Failed to load tetra\n");
        return 0;
    }
    return 1;
}

c_game_gl::c_game_gl(int width, int height)
{
    aspect_ratio = (float)width / height;
}

glm::mat4 c_game_gl::FindProjectionMatrix(float zNearClip,float zFarClip)
{
    float FOVrads=glm::radians(FOV),aspect=aspect_ratio;
    return glm::perspective(FOVrads,aspect,zNearClip,zFarClip);
}

glm::mat4 c_game_gl::FindViewMatrix(void)
{
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
    int i;

    glClearColor(0.0f,0.0f,0.4f,0.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glGenVertexArrays(1,&VertexArrayID);
    glBindVertexArray(VertexArrayID);

    for (i=0; meshes_to_load[i].mesh_id>=0; i++) {
        int j;
        j = meshes_to_load[i].mesh_id;
        if (j < GAME_GL_MAX_MESH) {
            meshes[j] = new c_mesh();
            if (meshes[j]->load_obj(meshes_to_load[i].obj_filename, meshes_to_load[i].scale) == 0) {
                meshes[j] = NULL;
            }
        }
    }

    for (i=0; textures_to_load[i].texture_id>=0; i++) {
        int j;
        j = textures_to_load[i].texture_id;
        if (j < GAME_GL_MAX_TEXTURES) {
            textures[j] = new c_texture();
            if (textures[j]->load_png(textures_to_load[i].png_filename) == 0) {
                textures[j] = NULL;
            }
        }
    }

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
c_game_gl::draw_cube(float x, float y, float z, int mesh, int texture)
{
    if ((mesh<0) || (mesh>=GAME_GL_MAX_MESH)) return;
    if ((texture<0) || (texture>=GAME_GL_MAX_TEXTURES)) return;

    if (meshes[mesh] == NULL) return;
    if (textures[texture] == NULL) return;

    M = FindModelMatrix(x,y,z,1);
    glUniformMatrix4fv(M_MatrixID,  1,GL_FALSE,&M[0][0]);

    glBindTexture(GL_TEXTURE_2D, textures[texture]->texture);

    glBindBuffer(GL_ARRAY_BUFFER, meshes[mesh]->attrib_buffers[0]);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,NULL);
    glBindBuffer(GL_ARRAY_BUFFER, meshes[mesh]->attrib_buffers[1]);
    glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,0,NULL);
    glBindBuffer(GL_ARRAY_BUFFER, meshes[mesh]->attrib_buffers[2]);
    glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE,0,NULL);
    //glDrawArrays(GL_LINES,   0,3*num_triangles);
    glDrawArrays(GL_TRIANGLES,0,3*meshes[mesh]->num_triangles);
}

void
c_game_gl::draw(void)
{
    int i;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    draw_start();
    for (i=0; i<10; i++) {
        draw_cube(i,0,i,1+(i&1),1);
    }
    draw_complete();
}