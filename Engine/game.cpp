#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>
//#include <sstream>
#include <vector>
#include "playground_parser.h"


//using namespace std;



bool LoadObject(const char *name){
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
}

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
    objects=new std::vector<Object>();
    std::string sceneName=config->IdentifyValue("StartScene");
    if(sceneName.compare("")==0){
        fprintf(stderr,"No starting scene was found so no objects will be loaded.\n");
        return 1;
    }
    char sceneFileLoc[256];
    sprintf(sceneFileLoc,"%s/Scenes/%s.scene",projectFolder,sceneName.c_str());
    fprintf(stderr,"Looking for scene file at %s\n",sceneFileLoc);
    PlaygroundScene *scene=new PlaygroundScene(sceneFileLoc);

    //Load Objects
    /*int objCount=0;
    std::string objCountStr=scene->IdentifyValue("ObjectCount");
    if (objCount.compare("")==0){
        fprintf(stderr,"ObjectCount not present in starting scene, assuming no objects.\n");
        return 1;
        }*/
    /*std::string objectName=scene->IdentifyValue("ObjectName");
      LoadObject(objectName.c_str());*/
    objects=scene->IdentifyObjects(projectFolder);
    return 1;
}





int GameLoop(){
    window->ClearWindow();
    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);
    //Basic idea:
    //Foreach object
        //Script update
        //Render
    //Object has a transform (model matrix+pos+rot+scale+parent) and a mesh (vertex data) and a material
    glm::mat4 VP=glm::perspective(90.0f,16.0f/9.0f,0.3f,50.0f);//*glm::translate(0.0f,0.0f,5.0f);
    for(uint i=0;i<objects->size();i++){
        (*objects)[i].Draw(&VP);
    }
	const char *error;
    error = SDL_GetError();
	if (*error != '\0') {
		fprintf(stderr,"SDL Error: %s\n", error);
		SDL_ClearError();
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
