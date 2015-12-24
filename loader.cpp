#define GL_GLEXT_PROTOTYPES
#include "loader.h"
#include <iostream>
#include <fstream>
#include <SDL2/SDL_opengl.h>
#include <SDL2_image/SDL_image.h>
using namespace std;

//All importing functions give a GLuint for referencing the loaded object.
//If the import fails, it will return 0.
//When using texture loaded using LoadImageFromFile it will be vertically flipped. Combat this by flipping y texture coordinates when importing model.

/*

Shader Loading

 */

GLuint LoadShaderFromFile(string pathString,GLenum shaderType){
  //Read all text
  fprintf(stderr, "Attempting shader load from %s...",pathString.c_str());
  const char *path=pathString.c_str();
  string varyingShaderCode;
  ifstream file (path,ios::in);
  if (file.is_open()){
    string line;
    while(getline(file,line))
      varyingShaderCode+=("\n"+line);
    file.close();
  }else{
    fprintf(stderr, " Failure\nFile couldn't be read\n");
    return 0;
  }
  const char *shaderCode=varyingShaderCode.c_str();
  //Compile text
  GLuint ShaderID=glCreateShader(shaderType);
  glShaderSource(ShaderID, 1, &shaderCode,NULL);
  glCompileShader(ShaderID);
  //Make sure compile was successful
  GLint compileResult;
  int compileInfoLength;
  glGetShaderiv(ShaderID,GL_COMPILE_STATUS,&compileResult);
  glGetShaderiv(ShaderID,GL_INFO_LOG_LENGTH,&compileInfoLength);
  if (compileResult==GL_FALSE){
    //Shader has warnings or has failed
    char ErrorMessage[compileInfoLength+1];
    glGetShaderInfoLog(ShaderID,compileInfoLength,NULL,ErrorMessage);
    fprintf(stderr," Failure\n%s\n",ErrorMessage);
    return 0;
  }
  fprintf(stderr," Success\n");
  //Send Shader ID
  return ShaderID;
}

GLuint LoadShadersIntoProgram(string vertexPath,string fragPath,GLuint program){
  if(program==0)
    program=glCreateProgram();
  GLuint vertexShader=LoadShaderFromFile(vertexPath,GL_VERTEX_SHADER);
  if(vertexShader!=0) 
    glAttachShader(program,vertexShader);
  else return 0;
  GLuint fragShader=LoadShaderFromFile(fragPath,GL_FRAGMENT_SHADER);
  if(fragShader!=0) 
    glAttachShader(program,fragShader);
  else return 0;
  glLinkProgram(program);
  //Check program has linked properly
  GLint linkResult=GL_FALSE;
  int linkInfoLength;
  glGetProgramiv(program,GL_LINK_STATUS,&linkResult);
  glGetProgramiv(program,GL_INFO_LOG_LENGTH, &linkInfoLength);
  if (linkResult==GL_FALSE){
    //Program linkage has failed
    char ErrorMessage[linkInfoLength+1];
    glGetProgramInfoLog(program,linkInfoLength,NULL,ErrorMessage);
    fprintf(stderr,"%s\n",ErrorMessage);
    return 0;
  }
  glDetachShader(program,vertexShader);
  glDetachShader(program,fragShader);
  glDeleteShader(vertexShader);
  glDeleteShader(fragShader);
  return program;
}

/*

Texture Loading

 */

GLuint LoadTextureFromFile(string imagePath, GLuint imageType){
  fprintf(stderr,"Attempting image load from %s...",imagePath.c_str());
  SDL_Surface *surface=IMG_Load(imagePath.c_str());
  if (surface==NULL){
    //SDL couldn't load image
    fprintf(stderr, " Failure\n%s\n", SDL_GetError());
    return 0;
  }
  //Generate an OpenGL texture to return
  GLuint TextureID;
  glGenTextures(1,&TextureID);
  glBindTexture(GL_TEXTURE_2D,TextureID);
  glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,surface->w,surface->h,0,imageType,GL_UNSIGNED_BYTE,surface->pixels);
  //Set filtering mode for when pixels smaller and bigger than screen
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
  SDL_FreeSurface(surface);
  fprintf(stderr," Success\n");
  return TextureID;
}
