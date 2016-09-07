#include "material.h"

Material::Material(GLuint sID){
    //Initialize class variables
    floats=new std::map<GLuint,float>();
    vectors=new std::map<GLuint,glm::vec3>();
    textures=new std::map<GLuint,GLuint>();
    matrices=new std::map<GLuint,glm::mat4>();
    matModelR=new std::map<GLuint,bool>();
    types=new std::map<GLuint,GLenum>();
    shaderProgram=sID;
    MVPloc=glGetUniformLocation(shaderProgram,"MVP");

    //Find all uniform types
    GLint numActiveUniforms = 0;
    GLint size; // size of the variable
    GLenum type; // type of the variable (float, vec3 or mat4, etc)

    const GLsizei bufSize = 16; // maximum name length
    char name[bufSize]; // variable name in GLSL
    GLsizei length; // name length
    glGetProgramiv(shaderProgram, GL_ACTIVE_UNIFORMS, &numActiveUniforms);
    for(GLint i=0;i<numActiveUniforms;i++){
        glGetActiveUniform(shaderProgram, i, bufSize, &length, &size, &type, name);
        types->emplace(i,type);
    }
}

void Material::Apply(glm::mat4 M,glm::mat4 VP){
    //Apply the material settings to OpenGL in preperation for rendering
    glUseProgram(shaderProgram);
    glm::mat4 MVP=VP*M;
    glUniformMatrix4fv(MVPloc, 1,GL_FALSE,&MVP[0][0]);

    GLuint i;
    for( auto const &fl_pair : *floats ){
        glUniform1f(fl_pair.first,fl_pair.second);
    }
    
    i=0;
    for (auto const &tex_pair : *textures){
        glActiveTexture(GL_TEXTURE0+i);
        glBindTexture(GL_TEXTURE_2D,tex_pair.second);
        glUniform1i(tex_pair.first,i);
        i++;
    }
    glm::vec3 v;
    for(auto const &vec_pair : *vectors){
        glUniform3f(vec_pair.first,vec_pair.second[0],vec_pair.second[1],vec_pair.second[2]);
    }

    glm::mat4 mat;
    for(auto const &mat_pair : *matrices){
        mat=mat_pair.second;
        if ((*matModelR)[mat_pair.first])
            mat=mat*M;
        glUniformMatrix4fv(mat_pair.first,1,GL_FALSE,&mat[0][0]);
    }
}

bool Material::SetFloat(const char* key,float toSet){
    GLint loc=glGetUniformLocation(shaderProgram, key);

    if (loc<0) return false;
    if ((*types)[loc]!=GL_FLOAT) return false;
    (*floats)[loc]=toSet;
    return true;
}

bool Material::SetTexture(const char* key,GLuint toSet){
    GLint loc=glGetUniformLocation(shaderProgram,key);

    if (loc<0) return false;
    if ((*types)[loc]!=GL_SAMPLER_2D) return false;
    (*textures)[loc]=toSet;
    return true;
}

bool Material::SetVector(const char* key,glm::vec3 *toSet){
    GLint loc=glGetUniformLocation(shaderProgram,key);
    
    if (loc<0) return false;
    if ((*types)[loc]!=GL_FLOAT_VEC3) return false;
    (*vectors)[loc]=*toSet;
    return true;
}

bool Material::SetMatrix(const char* key,glm::mat4 *toSet,bool modelRelative){
    GLint loc=glGetUniformLocation(shaderProgram,key);

    if (loc<0) return false;
    if ((*types)[loc]!=GL_FLOAT_MAT4) return false;
    (*matrices)[loc]=*toSet;
    (*matModelR)[loc]=modelRelative;
    return true;
}
