#include "game.h"

int LoadProject(){
    //Create GameWindow
    //read config.cfg in projectFolder
    char configFileLoc[256];
    sprintf(configFileLoc,"%s/config.cfg",projectFolder);
    fprintf(stderr,"\nLooking for scene config at %s\n",configFileLoc);
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
    std::string sceneName=config->IdentifyValue("StartScene");
    if(sceneName.compare("")==0){
        fprintf(stderr,"No starting scene was found so no objects will be loaded.\n");
        return 1;
    }
    char sceneFileLoc[256];
    sprintf(sceneFileLoc,"%s/Scenes/%s.scene",projectFolder,sceneName.c_str());
    fprintf(stderr,"\nLooking for scene file at %s\n",sceneFileLoc);
    scene=new PlaygroundScene(sceneFileLoc);

    //Load Objects
    scene->IdentifyObjects(projectFolder);
    scene->camera->SetAspectRatio(window->aspect);
    scene->camera->CalculateVP();

    delete config;
    //delete configFile;

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

    for(j=0;j<sLightCount;j++){
        
        l=((*scene->sLights)[j]);
        l->FindVP();
        VP=&(l->VP);
        l->InitShadowRender();
        for(i=0;i<scene->objects->size();i++){
            (*scene->objects)[i].Draw(VP,&(*scene->shadowMat));
        }
        l->SaveTexture();
    }

    glBindFramebuffer(GL_FRAMEBUFFER,0);
    window->ApplyResolution();
    window->ClearWindow();

    VP=&(scene->camera->VP);
    for(i=0;i<scene->objects->size();i++){
        Material *m=(*scene->objects)[i].mat;
        //Object o=(*scene->objects)[i];
        if(sLightCount>0&&m!=NULL){
            (*scene->sLights)[0]->SetupMaterial(m);
            //o.mat->SetTexture("SLightDepthMap",((*scene->sLights)[0]->depthMapTex));
            //o.mat->SetVector("SLightColor",&((*scene->sLights)[0]->color));
            //o.mat->SetFloat("SLightFarClip",scene->camera->farClip);
            //o.mat->SetFloat("SLightNearClip",scene->camera->nearClip);
        }
        
        (*scene->objects)[i].Draw(VP);
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
    InitGraphics();

    GameLoop();
    bool shouldEnd=false;
    SDL_Event e;
    while(!shouldEnd){
        while (SDL_PollEvent(&e)!=0){
            if (e.type==SDL_QUIT)
                shouldEnd=true;
        }
    }
    window->End();
    return 1;
}
