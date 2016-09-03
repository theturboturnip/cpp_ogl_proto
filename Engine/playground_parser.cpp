#include "includes.h"
#include "playground_parser.h"
#include <fstream>
#include <string>
#include <vector>

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
    while (getline(*file,line)){
        splitline->clear();
        SplitString(line,'=',splitline);
        keys->push_back((*splitline)[0]);
        values->push_back((*splitline)[1]);
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
