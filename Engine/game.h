#ifndef __INCLUDED_GAME__
#define __INCLUDED_GAME__

#include "includes.h"
#include "window.h"
#include "loader.h"
#include "object.h"
#include "mesh.h"

const char* projectFolder;
int main(int argc,char* argv[]);
GameWindow *window=NULL;
/*std::vector<Object*> *objects;
  Camera *camera;*/
PlaygroundScene *scene;

#endif
