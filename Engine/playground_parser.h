#ifndef __INCLUDED_CONFIG__
#define __INCLUDED_CONFIG__
#include "includes.h"
#include <string>
#include <vector>

using namespace std;
class PlaygroundFile{
public:
    PlaygroundFile(const char* filename);
    PlaygroundFile(ifstream *file);
    void ParseFile(ifstream *file);
    vector<string> *keys,*values;
    string IdentifyValue(const char* key);
private:
    void SplitString(string &str,char delim,vector<string> *v);
};
    
#endif
