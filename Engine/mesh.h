#ifndef __INC_MESH_H__
#define __INC_MESH_H__

#include "includes.h"
#include "loader.h"
/*#include "object.h"
#include "playground_parser.h"
#include "window.h"
#include "game.h"*/

class Mesh{
public:
    Mesh(std::string modelPath="");
    void Draw();
protected:
    GLuint model[3],VertexArrayID;
    unsigned int vertexNum;
};

Mesh* LoadMesh(const char* name, const char* projectFolder);

#endif
