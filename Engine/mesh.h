#ifndef __INC_MESH_H__
#define __INC_MESH_H__

#include "includes.h"
#include "loader.h"

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
    bool SetTex(const char* texKey,GLuint tex);
    bool SetVector(const char* vecKey,glm::vec3 vec);
    void Apply(glm::mat4 MVP);
private:
    std::vector<GLuint> *floatKeys,*texKeys,*vecKeys;
    std::vector<float> *floats;
    std::vector<GLuint> *textures;
    std::vector<glm::vec3> *vectors;
    //std::vector<> colors;
    GLuint shaderProgram;
    GLuint MVPloc;
};
    

class Mesh{
public:
    Mesh(string modelPath="");
    void Draw();
protected:
    GLuint model[3],VertexArrayID;
    unsigned int vertexNum;
};

#endif // __INC_MESH_H__
