#define GLM_FORCE_RADIANS
#define GL_GLEXT_PROTOTYPES
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtx/transform.hpp>
#include <SDL2/SDL_opengl.h>
#include "loader.h"
#include "mesh.h"

Transform::Transform(glm::vec3 *pos,glm::vec3 *rot,glm::vec3 *_scale){
    position=(*pos);
    rotation=(*rot);
    scale=(*_scale);
}

glm::mat4 Transform::Evaluate(){
    //Scale,Rotate,Translate
    glm::mat4 toReturn=glm::scale(scale);
    glm::mat4 rotationMat=glm::rotate(glm::radians(rotation.x),glm::vec3(1,0,0));
    rotationMat=glm::rotate(rotationMat,glm::radians(rotation.y),glm::vec3(0,1,0));
    rotationMat=glm::rotate(rotationMat,glm::radians(rotation.z),glm::vec3(0,0,1));
    glm::mat4 translateMat=glm::translate(toReturn,-position);
    //toReturn*=rotationMat;
    //scale,then rotate, then translate
    toReturn=translateMat*rotationMat*toReturn;
    return toReturn;
}

Material::Material(GLuint sID){
    floatKeys=new std::vector<GLuint>();
    texKeys=new std::vector<GLuint>();
    vecKeys=new std::vector<GLuint>();
    matKeys=new std::vector<GLuint>();
    floats=new std::vector<float>();
    textures=new std::vector<GLuint>();
    vectors=new std::vector<glm::vec3>();
    matrices=new std::vector<glm::mat4>();
    matModelR=new std::vector<bool>();
    shaderProgram=sID;
    MVPloc=glGetUniformLocation(shaderProgram,"MVP");
}

static void gl_check_error(const char *msg)
{
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        fprintf(stderr,"OpenGL error: %s : %d\n",msg,err);
    }
}

void Material::Apply(glm::mat4 M,glm::mat4 VP){
    glUseProgram(shaderProgram);
    glm::mat4 MVP=VP*M;
    glUniformMatrix4fv(MVPloc, 1,GL_FALSE,&MVP[0][0]);

    uint i;
    for(i=0;i<floatKeys->size();i++)
        glUniform1f((*floatKeys)[i],(*floats)[i]);
    for(i=0;i<texKeys->size();i++){
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,(*textures)[i]);
        glUniform1i((*texKeys)[i],0);
    }
    glm::vec3 v;
    for(i=0;i<vecKeys->size();i++){
        v=(*vectors)[i];
        glUniform3f((*vecKeys)[i],v[0],v[1],v[2]);
    }
    //gl_check_error("Material::apply:mid");

    glm::mat4 mat;
    for(i=0;i<matKeys->size();i++){
        mat=((*matrices)[i]);
        if ((*matModelR)[i])
            mat=mat*M;
        glUniformMatrix4fv((*matKeys)[i],1,GL_FALSE,&mat[0][0]);
    }
    //glBindTexture(GL_TEXTURE_2D,1);
    //glUniform1i(1,0);
    //gl_check_error("Material::apply:done");
}

bool Material::SetFloat(const char* key,float toSet){
    GLint loc=glGetUniformLocation(shaderProgram, key);
    if (loc<0)
        return false;
    floatKeys->push_back(loc);
    floats->push_back(toSet);
    return true;
}

bool Material::SetTexture(const char* key,GLuint toSet){
    GLint loc=glGetUniformLocation(shaderProgram, key);
    //fprintf(stderr,"Found tex key %s at %d\n",key,toSet);
    if (loc<0)
        return false;
    texKeys->push_back(loc);
    textures->push_back(toSet);
    return true;
}

bool Material::SetVector(const char* key,glm::vec3 *toSet){
    GLint loc=glGetUniformLocation(shaderProgram, key);
    //fprintf(stderr,"location of %s is %d\n",key,loc);
    if (loc<0)
        return false;
    vecKeys->push_back(loc);
    vectors->push_back(*toSet);
    return true;
}

bool Material::SetMatrix(const char* key,glm::mat4 *toSet,bool modelRelative){
    GLint loc=glGetUniformLocation(shaderProgram,key);
    if(loc<0)
        return false;
    matKeys->push_back(loc);
    matrices->push_back(*toSet);
    matModelR->push_back(modelRelative);
    return true;
}

Mesh::Mesh(std::string modelPath){
    //GLuint VertexArrayID;
    glGenVertexArrays(1,&VertexArrayID);
    glBindVertexArray(VertexArrayID);
    vertexNum=LoadModelFromFile(modelPath, model, 1.0f); // Returns 0 on ERROR
}
void Mesh::Draw(){
    //VP is Projection  * View
    //Scale,rotate,translate for model matrix
    //Calculate MVP Matrix
    //glm::mat4 M= glm::mat4(1.0f);
    //M[3][3] = 1.0;
    //Apply MVP Matrix
    //glUniformMatrix4fv(shaderMatrixLocation,1,GL_FALSE,&M[0][0]);
    //Enable Arrays
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    //Assign Verticies
    glBindBuffer(GL_ARRAY_BUFFER,model[0]);
    glVertexAttribPointer(0,
			3,//Vertex has 3 points: x,y,z
			GL_FLOAT,
			GL_FALSE,
			0,
			(void*)0);
    //Assign UV
    glBindBuffer(GL_ARRAY_BUFFER,model[1]);
    glVertexAttribPointer(1,
			2,//UV has 2 points: x,y
			GL_FLOAT,
			GL_FALSE,
			0,
			(void*)0);
    //Assign Normals
    glBindBuffer(GL_ARRAY_BUFFER,model[2]);
    glVertexAttribPointer(2,
			3,//Normal has 3 points: x,y,z
			GL_FLOAT,
			GL_FALSE,
			0,
			(void*)0);
    //Actually Draw
    glDrawArrays(GL_TRIANGLES,0,vertexNum*3);
    //Disable Arrays
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}
