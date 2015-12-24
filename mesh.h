#include <SDL2/SDL_opengl.h>
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtx/transform.hpp>

class Transform{
 public:
  glm::vec3 position,rotation,scale;
};

class Mesh{
 public:
  Mesh(string modelPath,string texturePath,Transform t);
  void Draw(glm::mat4 VP,GLuint shaderMatrixLocation);
 protected:
  Transform transform;
  GLuint model[3],texture;
}
