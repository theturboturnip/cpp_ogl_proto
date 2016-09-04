#include "object.h"
#include "includes.h"

Object::Object(glm::vec3 *pos,glm::vec3 *rot,glm::vec3 *scale,Mesh *m,Material *material,const char* _type,std::map<std::string,std::string> *_data){
    t=new Transform(pos,rot,scale);
    mesh=m;
    mat=material;
    type=_type;
    data=_data;
}

void Object::Draw(glm::mat4 *VP){
    if (mesh==NULL||mat==NULL)
        return;
    glm::mat4 MVP=(*VP)*(t->Evaluate());
    mat->Apply(MVP);
    mesh->Draw();
}

void Object::Draw(glm::mat4 *VP,Material *overrideMat){
    if (mesh==NULL)
        return;
    glm::mat4 MVP=(*VP)*(t->Evaluate());
    overrideMat->Apply(MVP);
    mesh->Draw();
}

Camera::Camera(glm::vec3 *pos,glm::vec3 *rot,glm::vec3 *scale,const char* _type,std::map<std::string,std::string> *_data) : Object(pos,rot,scale,NULL,NULL,_type,_data){
    FOV=std::stof((*data)["FOV"]);
    nearClip=std::stof((*data)["NearClip"]);
    farClip=std::stof((*data)["FarClip"]);
}

void Camera::SetAspectRatio(float aspectRatio){
    projectionMatrix=glm::perspective(FOV,aspectRatio,nearClip,farClip);
    //projectionMatrix=&();
}

glm::mat4 Camera::CalculateVP(){
    VP=(projectionMatrix)*(t->Evaluate());
    return VP;
}

/*void Light::Light(glm::vec3 *pos,glm::vec3 *rot,glm::vec3 *scale,const char* _type,std::map<std::string,std::string> *_data) : Camera(pos,rot,scale,_type,_data){
    Set
}*/
