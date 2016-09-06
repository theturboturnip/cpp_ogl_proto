#define GLM_FORCE_RADIANS
#define GL_GLEXT_PROTOTYPES
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtx/transform.hpp>
#include <SDL2/SDL_opengl.h>
#include "loader.h"
#include "mesh.h"
#include <map>

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
    /*floatKeys=new std::vector<GLuint>();
    texKeys=new std::vector<GLuint>();
    vecKeys=new std::vector<GLuint>();
    matKeys=new std::vector<GLuint>();
    floats=new std::vector<float>();
    textures=new std::vector<GLuint>();
    vectors=new std::vector<glm::vec3>();
    matrices=new std::vector<glm::mat4>();
    matModelR=new std::vector<bool>();*/
    //uniforms=new std::map<const char*,GLuint>();
    floats=new std::map<GLuint,float>();
    vectors=new std::map<GLuint,glm::vec3>();
    textures=new std::map<GLuint,GLuint>();
    matrices=new std::map<GLuint,glm::mat4>();
    matModelR=new std::map<GLuint,bool>();
    types=new std::map<GLuint,GLenum>();
    shaderProgram=sID;
    MVPloc=glGetUniformLocation(shaderProgram,"MVP");

    GLint numActiveUniforms = 0;
    GLint size; // size of the variable
    GLenum type; // type of the variable (float, vec3 or mat4, etc)

    const GLsizei bufSize = 16; // maximum name length
    char name[bufSize]; // variable name in GLSL
    GLsizei length; // name length
    glGetProgramiv(shaderProgram, GL_ACTIVE_UNIFORMS, &numActiveUniforms);
    for(GLint i=0;i<numActiveUniforms;i++){
        glGetActiveUniform(shaderProgram, i, bufSize, &length, &size, &type, name);
        //fprintf(stderr,"Found uniform %d named %s\n",i,name); 
        //uniforms->emplace(name,i);
        types->emplace(i,type);
    }
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

    GLuint i;
    //std::pair <GLuint,float> fl_pair;
    //std::pair <GLuint,glm::vec3> vec_pair;
    //std::pair <GLuint,GLuint> tex_pair;
    //std::pair <GLuint,glm::mat4> mat_pair;
    for( auto const &fl_pair : *floats ){
        glUniform1f(fl_pair.first,fl_pair.second);
    }
    
    i=0;
    for (auto const &tex_pair : *textures){
        glActiveTexture(GL_TEXTURE0+i);
        glBindTexture(GL_TEXTURE_2D,tex_pair.second);
        glUniform1i(tex_pair.first,i);
        //fprintf(stderr,"Applied OpenGL texture %d and shader uniform %d to slot %d\n",tex_pair.second,tex_pair.first,i);
        i++;
    }
    //fprintf(stderr,"Applied %d textures, texmap size is %d\n",i,textures->size());
    glm::vec3 v;
    for(auto const &vec_pair : *vectors){
        //v=(*vectors)[i];
        glUniform3f(vec_pair.first,vec_pair.second[0],vec_pair.second[1],vec_pair.second[2]);
    }
    //gl_check_error("Material::apply:mid");

    glm::mat4 mat;
    for(auto const &mat_pair : *matrices){
        mat=mat_pair.second;
        if ((*matModelR)[mat_pair.first])
            mat=mat*M;
        glUniformMatrix4fv(mat_pair.first,1,GL_FALSE,&mat[0][0]);
    }
    //glBindTexture(GL_TEXTURE_2D,1);
    //glUniform1i(1,0);
    //gl_check_error("Material::apply:done");
    
}

bool Material::SetFloat(const char* key,float toSet){
    GLint loc=glGetUniformLocation(shaderProgram, key);
    /*if (loc<0)
        return false;
    floatKeys->push_back(loc);
    floats->push_back(toSet);*/

    if (loc<0){
        return false;
    }
    if ((*types)[loc]!=GL_FLOAT) return false;
    (*floats)[loc]=toSet;
    return true;
}

bool Material::SetTexture(const char* key,GLuint toSet){

    GLint loc=glGetUniformLocation(shaderProgram,key);
    //fprintf(stderr,"Attempting texture %s with . Loc is %d\n",key,loc);
    if(loc<0){
        return false;
    }
    if ((*types)[loc]!=GL_SAMPLER_2D) return false;
    (*textures)[loc]=toSet;
    //fprintf(stderr,"Set texture %s (OGL ID %d) to uniform %d, new size %d\n",key,toSet,loc,textures->size());
    return true;
}

bool Material::SetVector(const char* key,glm::vec3 *toSet){
    //if (!uniforms->count(key)) return false;
    GLint loc=glGetUniformLocation(shaderProgram,key);
    if ((*types)[loc]!=GL_FLOAT_VEC3) return false;
    (*vectors)[loc]=*toSet;
    return true;
}

bool Material::SetMatrix(const char* key,glm::mat4 *toSet,bool modelRelative){
    GLint loc=glGetUniformLocation(shaderProgram,key);
    if(loc<0) return false;
    if ((*types)[loc]!=GL_FLOAT_MAT4) return false;
    (*matrices)[loc]=*toSet;
    (*matModelR)[loc]=modelRelative;
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
