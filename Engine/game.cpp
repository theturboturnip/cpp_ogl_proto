#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
//#include <sstream>
#include <vector>
#include <typeinfo>
#include "playground_parser.h"


//using namespace std;



/*bool LoadObject(const char *name){
    //Find the object file
    char objectPath[256];
    sprintf(objectPath,"%s/Objects/%s.object",projectFolder,name);
    PlaygroundFile *objectFile=new PlaygroundFile(objectPath);
    
    //Get the material and mesh
    Material *material=LoadMaterial(objectFile->IdentifyValue("MaterialName").c_str(),projectFolder);
    Mesh *mesh=LoadMesh(objectFile->IdentifyValue("MeshName").c_str(),projectFolder);

    //Create the object and send to the object vector
    //Currently assuming pos rot and scale, it should be loaded from the object
    glm::vec3 *pos=new glm::vec3(1,0,-10),*rot=new glm::vec3(0),*scale=new glm::vec3(1,1,1);
    Object *o=new Object(pos,rot,scale,mesh,material);
    objects->push_back(*o);
    return true;
}*/

int LoadProject(){
    //Create GameWindow
    //read config.cfg in projectFolder
    char configFileLoc[256];
    sprintf(configFileLoc,"%s/config.cfg",projectFolder);
    fprintf(stderr,"Looking for scene config at %s\n",configFileLoc);
    std::ifstream configFile(configFileLoc);
    if (configFile==NULL){
        fprintf(stderr,"No config file found in project.\n");
        return 0;
    }

    int width=640,height=480;
    bool resizeable=true;
    const char* name="Window";
    PlaygroundFile *config=new PlaygroundFile(&configFile);

    std::string newName=config->IdentifyValue("WindowName");
    if(newName.compare("")!=0)
        name=newName.c_str();
    std::string newWidth=config->IdentifyValue("WindowWidth");
    if (newWidth.compare("")!=0)
        width=std::stoi(newWidth);
    std::string newHeight=config->IdentifyValue("WindowHeight");
    if (newHeight.compare("")!=0)
        height=std::stoi(newHeight);
    
    window=new GameWindow(width,height,name,resizeable);

    //Find scene
    //objects=new std::vector<Object>();
    std::string sceneName=config->IdentifyValue("StartScene");
    if(sceneName.compare("")==0){
        fprintf(stderr,"No starting scene was found so no objects will be loaded.\n");
        return 1;
    }
    char sceneFileLoc[256];
    sprintf(sceneFileLoc,"%s/Scenes/%s.scene",projectFolder,sceneName.c_str());
    fprintf(stderr,"Looking for scene file at %s\n",sceneFileLoc);
    scene=new PlaygroundScene(sceneFileLoc);

    //Load Objects
    
    scene->IdentifyObjects(projectFolder);
    scene->camera->SetAspectRatio(window->aspect);
    scene->camera->CalculateVP();

    return 1;
}


void InitGraphics(){
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
}


int GameLoop(){
    if (1) {
        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR) {
            fprintf(stderr,"Game a OpenGL error: %d\n",err);
        }
    }
    window->ClearWindow();
    glm::mat4 *VP;
    uint i,j,sLightCount;
    ShadowLight *l;
    
    //Basic Light Rendering Steps:
    //If the light does shadowing:
    //    Query for VP
    //    Switch current renderbuffer to that of the light
    //    Render all objects
    //    Switch renderbuffer back
    //    Render all objects again
    
    sLightCount=scene->sLights->size();
    //fprintf(stderr,"%d",sLightCount);
    //glClearDepth(0.5f);
    if (1) {
        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR) {
            fprintf(stderr,"Game b OpenGL error: %d\n",err);
        }
    }
    for(j=0;j<sLightCount;j++){
        
        l=&((*scene->sLights)[j]);
        VP=&(l->VP);
        l->InitShadowRender();
        for(i=0;i<scene->objects->size();i++){
            (*scene->objects)[i].Draw(VP,&(*scene->shadowMat));
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER,0);
    window->ClearWindow();
    if (1) {
        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR) {
            fprintf(stderr,"Game c OpenGL error: %d\n",err);
        }
    }

    //glClearDepth(1);
    VP=&(scene->camera->VP);
    for(i=0;i<scene->objects->size();i++){
        Material *m=(*scene->objects)[i].mat;
        if(sLightCount>0&&m!=NULL){
            (*scene->objects)[i].mat->SetMatrix("SLightMVP",&((*scene->sLights)[0].VP));
            (*scene->objects)[i].mat->SetTexture("SLightShadowMap",((*scene->sLights)[0].depthMapTex));
        }
        (*scene->objects)[i].Draw(VP);
    }

    if (1) {
        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR) {
            fprintf(stderr,"Game d OpenGL error: %d\n",err);
        }
    }
	const char *error;
    error = SDL_GetError();
	if (*error != '\0') {
		fprintf(stderr,"SDL Error: %s\n", error);
		SDL_ClearError();
	}
    if (1) {
        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR) {
            fprintf(stderr,"Game loop end OpenGL error: %d\n",err);
        }
    }
    window->Flip();
    return 1;
}

int main(int argc,char* argv[]){
    if (argc<2){
        fprintf(stderr, "No project folder found.\n");
        return 0;
    }
    projectFolder=argv[1];
    if (LoadProject()==0)
        return 0;
    if (1) {
        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR) {
            fprintf(stderr,"Game init a OpenGL error: %d\n",err);
        }
    }
    InitGraphics();
    if (1) {
        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR) {
            fprintf(stderr,"Game init b OpenGL error: %d\n",err);
        }
    }
    bool shouldEnd=false;
    SDL_Event e;
    while(!shouldEnd){
        while (SDL_PollEvent(&e)!=0){
            if (e.type==SDL_QUIT)
                shouldEnd=true;
        }
        GameLoop();
    }
    window->End();
    return 1;
}
