#include "includes.h"
#include "playground_parser.h"
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include "loader.h"
#include "object.h"
#include "mesh.h"

using namespace std;
PlaygroundFile::PlaygroundFile(const char* filename){
    ifstream file(filename);
    ParseFile(&file);
}

PlaygroundFile::PlaygroundFile(ifstream *file){
    ParseFile(file);
}

void PlaygroundFile::ParseFile(ifstream *file){
    //ifstream file=*file_ptr;
    string line;
    vector<string> *splitline= new vector<string>();
    keys=new vector<string>();
    values=new vector<string>();
    lines=new vector<string>();
    while (getline(*file,line)){
        splitline->clear();
        SplitString(line,'=',splitline);
        if(splitline->size()<2){
            //If this isn't a key pair add a line anyway to keep keys,values and lines in sync
            keys->push_back("");
            values->push_back("");
        }else{
            keys->push_back((*splitline)[0]);
            values->push_back((*splitline)[1]);
        }
        lines->push_back(line);
    }
    delete splitline;
}
        
using namespace std;

void PlaygroundFile::SplitString(string& str, char delim,vector<string> *v) {
    auto i = 0;
    auto pos = str.find(delim);
    while (pos != string::npos) {
      v->push_back(str.substr(i, pos-i));
      i = ++pos;
      pos = str.find(delim, pos);

      if (pos == string::npos)
         v->push_back(str.substr(i, str.length()));
    }
}

string PlaygroundFile::IdentifyValue(const char* key){
    int size=keys->size();
    for(int i=0;i<size;i++){
        if ((*keys)[i].compare(key)==0){
            return (*values)[i];
        }
    }
    return "";
}

PlaygroundScene::PlaygroundScene(const char* filename) : PlaygroundFile(filename){
    //ifstream file(filename);
    //ParseFile(&file);
}

PlaygroundScene::PlaygroundScene(ifstream *file) : PlaygroundFile(file){
    ParseFile(file);
}

void PlaygroundScene::IdentifyObjects(const char *projectFolder){
    //Cycle through lines until we see OBJECT_START
    string line,key,value;
    bool has_object=false;
    glm::vec3 *pos,*rot,*scale;
    Material *mat;
    Mesh *mesh;
    Object *o;
    string type;
    objects=new vector<Object>();
    sLights=new vector<ShadowLight>();
    //vector<Object> *objects=new vector<Object*>();
    map<string,string> *data;
    for(uint i=0;i<lines->size();i++){
        line=(*lines)[i];
        if (line.compare("OBJECT_START")==0){
            has_object=true;
            pos=new glm::vec3(0);
            rot=new glm::vec3(0);
            scale=new glm::vec3(1,1,1);
            data=new map<string,string>();
            mat=NULL;
            mesh=NULL;
            type="";
        }else if (line.compare("OBJECT_END")==0){
            has_object=false;
            //Add all known data to the object
            if (type.compare("Camera")==0){
                camera=new Camera(pos,rot,scale,type.c_str(),data);
            }else if (type.compare("SLight")==0){
                ShadowLight *l=new ShadowLight(pos,rot,type.c_str(),data);
                sLights->push_back(*l);
            }else{
                o=new Object(pos,rot,scale,mesh,mat,type.c_str(),data);
                //Push object onto list
                objects->push_back(*o);
            }
        }else if (has_object){
            //Use this line to determine more data on our object
            key=(*keys)[i];
            value=(*values)[i];
            if (key.compare("Pos")==0){
                pos=stov3(value);
            }else if (key.compare("Rot")==0){
                rot=stov3(value);
            }else if (key.compare("Scale")==0){
                scale=stov3(value);
            }else if (key.compare("Mesh")==0){
                mesh=LoadMesh(value.c_str(),projectFolder);
            }else if (key.compare("Material")==0){
                mat=LoadMaterial(value.c_str(),projectFolder);
            }else if (key.compare("Type")==0){
                type=value;
            }else{
                data->emplace(key,value);
            }
        }else{
            key=(*keys)[i];
            value=(*values)[i];
            if (key.compare("ShadowMaterial")==0){
                shadowMat=LoadMaterial(value.c_str(),projectFolder);
            }
        }

    }
    if (has_object){
        fprintf(stderr,"Object leak detected in scene\n");
    }
    //return objects;
}

glm::vec3* PlaygroundScene::stov3(string s){
    vector<string> *vecConvUtil=new vector<string>();
    SplitString(s,',',vecConvUtil);
    glm::vec3 *toReturn=new glm::vec3(0);
    toReturn->x=stof((*vecConvUtil)[0]);
    toReturn->y=stof((*vecConvUtil)[1]);
    toReturn->z=stof((*vecConvUtil)[2]);
    return toReturn;
}
