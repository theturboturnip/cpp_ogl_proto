#ifndef __INC_MESH_H__
#define __INC_MESH_H__

#include "includes.h"
#include <map>
#include <utility>

class Transform{
public:
    glm::vec3 position,rotation,scale;
    glm::mat4 Evaluate(void);
    Transform(glm::vec3 *pos,glm::vec3 *rot,glm::vec3 *_scale);
};

class Material{
public:
    Material(GLuint sID);
    bool SetFloat(const char* floatKey,float toSet);
    bool SetTexture(const char* texKey,GLuint tex);
    bool SetVector(const char* vecKey,glm::vec3 *vec);
    bool SetMatrix(const char* matKey,glm::mat4 *mat,bool modelRelative=true);
    void Apply(glm::mat4 M,glm::mat4 VP);
private:
    /*std::vector<GLuint> *floatKeys,*texKeys,*vecKeys,*matKeys;
    std::vector<float> *floats;
    std::vector<GLuint> *textures;
    std::vector<glm::vec3> *vectors;
    std::vector<glm::mat4> *matrices;
    std::vector<bool> *matModelR;*/
    //std::vector<> colors;
    //std::map<const char*,GLuint> *uniforms;
    std::map<GLuint,float> *floats;
    std::map<GLuint,glm::vec3> *vectors;
    std::map<GLuint,GLuint> *textures;
    std::map<GLuint,glm::mat4> *matrices;
    std::map<GLuint,bool> *matModelR;
    std::map<GLuint,GLenum> *types;
    GLuint shaderProgram;
    GLuint MVPloc;
};
    

class Mesh{
public:
    Mesh(std::string modelPath="");
    void Draw();
protected:
    GLuint model[3],VertexArrayID;
    unsigned int vertexNum;
};

#endif // __INC_MESH_H__
