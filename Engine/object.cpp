#include "object.h"
#include "includes.h"

Object::Object(glm::vec3 *pos,glm::vec3 *rot,glm::vec3 *scale,Mesh *m,Material *material,const char* _type,std::map<std::string,std::string> *_data){
    if(scale==NULL)
        scale=new glm::vec3(1);
    t=new Transform(pos,rot,scale);
    mesh=m;
    mat=material;
    type=_type;
    data=_data;
}

void Object::Draw(glm::mat4 *VP){
    if (mesh==NULL||mat==NULL)
        return;
    //glm::mat4 MVP=(*VP)*(t->Evaluate());
    mat->Apply(t->Evaluate(),*VP);
    mesh->Draw();
}

void Object::Draw(glm::mat4 *VP,Material *overrideMat){
    if (mesh==NULL)
        return;
    overrideMat->Apply(t->Evaluate(),*VP);
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

ShadowLight::ShadowLight(glm::vec3 *pos,glm::vec3 *rot,const char* _type,std::map<std::string,std::string> *_data) : Object(pos,rot,NULL,NULL,NULL,_type,_data){
    uint resX=std::stoi((*data)["ResolutionX"]),resY=std::stoi((*data)["ResolutionY"]);
    FindVP(resX,resY);
    //Gen rendering resources
    glGenFramebuffers(1,&depthMapFBO);
    
    glGenTextures(1,&depthMapTex); //Create a texture
    glBindTexture(GL_TEXTURE_2D,depthMapTex); //Set texture type
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_DEPTH_COMPONENT, //Tell OpenGL we only use depth
                 resX,
                 resY,
                 0,
                 GL_DEPTH_COMPONENT,
                 GL_FLOAT,
                 NULL);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
}

void ShadowLight::InitShadowRender(){
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        fprintf(stderr,"First OpenGL error: %d\n",err);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO); //Tell OpenGL to render to the depth map from now on
    glFramebufferTexture2D(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_TEXTURE_2D, depthMapTex,0);
    glClearColor(0.3,0.4,0.5,1.0);
    glClear(GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0,0.0,0.5,1.0);
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        fprintf(stderr,"The frame buffer is not working\n");
    }
    if (0) {
        glBindTexture(GL_TEXTURE_2D,depthMapTex); //Set texture type
        float screenData[1024*3];
        for(int y = 0; y < 32; ++y)		
            for(int x = 0; x < 32; ++x)
                screenData[y*32+x] = 0;//1.0-x*0.03;
        glTexSubImage2D(GL_TEXTURE_2D, 0 ,512, 512, 32, 32, GL_DEPTH_COMPONENT, GL_FLOAT, (GLvoid*)screenData);
        glReadPixels(510,510,32,32,GL_DEPTH_COMPONENT,GL_FLOAT,screenData);
        if (0) {
            for(int y = 0; y < 4; ++y)		
                for(int x = 0; x < 4; ++x)
                    fprintf(stderr,"%f:",screenData[y*32+x]);
            fprintf(stderr,"\n");
        }
    }
    while ((err = glGetError()) != GL_NO_ERROR) {
        fprintf(stderr,"OpenGL error: %d\n",err);
    }
}

void ShadowLight::FindVP(int resX,int resY){
    VP=glm::perspective(90.0f,((float)resX)/resY,0.3f,50.0f)*(t->Evaluate());
}
