#include "game.h"

#define NUM_KEYS 11
static int keys[NUM_KEYS] = {
SDLK_w, SDLK_s,
SDLK_a, SDLK_d,
SDLK_SPACE, SDLK_LSHIFT,
SDLK_UP, SDLK_DOWN,
SDLK_LEFT, SDLK_RIGHT,
SDLK_TAB
};

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
    glEnable(GL_BLEND);
}

void RenderingPass(ShadowLight *l,glm::mat4 *VP,bool additive=false){
    //glm::mat4 *VP=&(scene->camera->VP);
     glClear(GL_DEPTH_BUFFER_BIT);
    if (additive){
        glBlendFunc(GL_ONE,GL_ONE);
    }else
        glBlendFunc(GL_ONE,GL_ZERO);
    for(uint i=0;i<scene->objects->size();i++){
        Material *m=(*scene->objects)[i].mat;
        if(l!=NULL)
            l->SetupMaterial(m);
        (*scene->objects)[i].Draw(VP);
    }
}

int RenderScene(glm::mat4 *VP){
    window->ClearWindow();
    uint i,j,sLightCount;
    ShadowLight *l;
    
    sLightCount=scene->sLights->size();

    for(j=0;j<sLightCount;j++){
        
        l=((*scene->sLights)[j]);
        l->FindVP();
        VP=&(l->VP);
        l->InitShadowRender();
        for(i=0;i<scene->objects->size();i++){
            (*scene->objects)[i].Draw(VP,&(*scene->shadowMat));
        }
        //l->SaveTexture();
    }

    glBindFramebuffer(GL_FRAMEBUFFER,0);
    window->ApplyResolution();
    window->ClearWindow();

    VP=&(scene->camera->VP);

    //Simple multi-light rendering. Do one base pass, then 1x additive pass per light.
    bool additive=false;
    //fprintf(stderr,"%d\n",sLightCount);
    for(i=0;i<sLightCount;i++){
        RenderingPass((*scene->sLights)[i],VP,additive);
        //fprintf(stderr,"%d\n",additive);
        additive=true;
    }
    glBlendFunc(GL_ONE,GL_ZERO);//Make sure it works for the shadow pass

    window->Flip();
    return 1;
}

void HandleInput(int *keys_down,float deltaTime){
    //Camera translation
    float cameraHorizMoveSpeed=2.0f,cameraVertMoveSpeed=1.0f;
    glm::vec3 *moveDir=new glm::vec3(0);
    if (keys_down[0]) moveDir->z=1;
    if (keys_down[1]) moveDir->z=-1;
    if (keys_down[2]) moveDir->x=1;
    if (keys_down[3]) moveDir->x=-1;
    *moveDir=(*moveDir*cameraHorizMoveSpeed);
    if (keys_down[4]) moveDir->y=-cameraVertMoveSpeed;
    if (keys_down[5]) moveDir->y=cameraVertMoveSpeed;
    glm::rotateY(*moveDir,scene->camera->t->rotation.y);
    *moveDir=(*moveDir*deltaTime);
    (scene->camera->t->position)=(scene->camera->t->position)+*moveDir;

    //Camera rotation
    float cameraRotateSensitivity=10.0f;
    glm::vec3 *deltaLook=new glm::vec3(0);
    if (keys_down[6]) deltaLook->x=+1;
    if (keys_down[7]) deltaLook->x=-1;
    if (keys_down[8]) deltaLook->y=+1;
    if (keys_down[9]) deltaLook->y=-1;
    *deltaLook=*deltaLook*cameraRotateSensitivity*deltaTime;
    (scene->camera->t->rotation)=scene->camera->t->rotation+*deltaLook;

    if (keys_down[10]){
        scene->IdentifyObjects(projectFolder);
        scene->camera->SetAspectRatio(window->aspect);
    }
    scene->camera->CalculateVP();
}

void GameLoop(){
    bool shouldEnd=false;
    SDL_Event e;
    glm::mat4 *VP;
    int keys_down[NUM_KEYS];
    uint i;
    float deltaTime=1/60.f,startTime;
    while(!shouldEnd){
        startTime=SDL_GetTicks();
        while (SDL_PollEvent(&e)!=0){
            if (e.type==SDL_QUIT)
                shouldEnd=true;
            if (e.type==SDL_KEYDOWN) {
                for (i=0; i<sizeof(keys)/sizeof(e.key.keysym.sym); i++) {
                    if (e.key.keysym.sym==keys[i]) keys_down[i]=1;
                }
            }
            if (e.type==SDL_KEYUP) {
                for (i=0; i<sizeof(keys)/sizeof(e.key.keysym.sym); i++) {
                    if (e.key.keysym.sym==keys[i]) keys_down[i]=0;
                }
            }
        }
        HandleInput(keys_down,deltaTime);
        RenderScene(VP);
        deltaTime=(SDL_GetTicks()-startTime)/1000;
        //fprintf(stderr,"%f\n",(1/deltaTime));
    }
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
    
    window->End();
    return 1;
}
