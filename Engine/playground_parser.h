#ifndef __INCLUDED_CONFIG__
#define __INCLUDED_CONFIG__
#include "includes.h"
#include "object.h"
#include "mesh.h"
#include <string>
#include <vector>

using namespace std;
class PlaygroundFile{
public:
    PlaygroundFile(const char* filename);
    PlaygroundFile(ifstream *file);
    void ParseFile(ifstream *file);
    vector<string> *keys,*values,*lines;
    string IdentifyValue(const char* key);
    void SplitString(string &str,char delim,vector<string> *v);
};

class PlaygroundScene : public PlaygroundFile{
public:
    PlaygroundScene(const char* filename);
    PlaygroundScene(ifstream *file);
    vector<Object>* IdentifyObjects(const char* projectFolder); 
    glm::vec3* stov3(string s);
};
    
#endif
