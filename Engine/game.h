#ifndef __INC_GAME_H__
#define __INC_GAME_H__

#include "includes.h"
#include "loader.h"
#include "object.h"
#include "mesh.h"
#include "playground_parser.h"
#include "playground_scene.h"
#include "window.h"

const char* projectFolder;
int main(int argc,char* argv[]);
GameWindow *window=NULL;
PlaygroundScene *scene;

#endif
