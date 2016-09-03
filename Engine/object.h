#ifndef __INCLUDED_OBJECT__
#define __INCLUDED_OBJECT__
#include "includes.h"
#include "mesh.h"

class Object{
public:
    Object(glm::vec3 *pos,glm::vec3 *rot,glm::vec3 *scale,Mesh *m,Material *material);
    void Draw(glm::mat4 *VP);
    Transform *t;
    Mesh *mesh;
    Material *mat;
};

#endif
