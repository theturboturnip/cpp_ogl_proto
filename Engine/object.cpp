#include "object.h"
#include "includes.h"

Object::Object(glm::vec3 *pos,glm::vec3 *rot,glm::vec3 *scale,Mesh *m,Material *material){
    t=new Transform(pos,rot,scale);
    mesh=m;
    mat=material;
}

void Object::Draw(glm::mat4 *VP){
    glm::mat4 MVP=(*VP)*(t->Evaluate());
    mat->Apply(MVP);
    mesh->Draw();
}
