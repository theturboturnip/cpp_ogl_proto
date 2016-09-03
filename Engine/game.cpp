#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>
//#include <sstream>
#include <vector>
#include "playground_parser.h"

//using namespace std;

int LoadProject(){
    //read config.cfg in projectFolder and create a GameWindow
    char configFileLoc[256];
    sprintf(configFileLoc,"%s/config.cfg",projectFolder);
    fprintf(stderr,"%s\n",configFileLoc);
    std::ifstream configFile(configFileLoc);
    //FILE *fp=fopen(configFileLoc,"r");
    //if(fp==NULL) {
    //    fprintf(stderr,"fopen didn't find anything\n");
    //    return 0;
    //}
    if (configFile==NULL){
        fprintf(stderr,"No config file found in project.\n");
        return 0;
    }
    //fclose(fp);
    int width=640,height=480;
    bool resizeable=true;
    const char* name="Window";
    PlaygroundFile *config=new PlaygroundFile(&configFile);
    std::string newName=config->IdentifyValue("WindowName");
    if(newName.compare("")!=0)
        name=newName.c_str();
    fprintf(stderr,"%s\n",name);
    
    window=new GameWindow(width,height,name,resizeable);
    return 1;
}

int main(int argc,char* argv[]){
    if (argc<2){
        fprintf(stderr, "No project folder found.\n");
        return 0;
    }
    projectFolder=argv[1];
    LoadProject();
    return 1;
}
