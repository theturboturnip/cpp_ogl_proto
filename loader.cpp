#include <iostream>
#include <fstream>
#include <SDL2/SDL_opengl.h>
#include <png.h>
using namespace std;

/*

Shader Loading

 */

GLuint LoadShaderFromFile(string pathString,GLenum shaderType){
  //Read all text
  const char *path=pathString.c_str();
  string varyingShaderCode;
  ifstream file (path,ios::in);
  if (file.is_open()){
    string line;
    while(getline(file,line))
      varyingShaderCode+=("\n"+line);
    file.close();
  }else{
    fprintf(stderr, "File at %s couldn't be read, returning NULL",path);
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
    fprintf(stderr,"Shader %s failed %s\n",path,ErrorMessage);
    return 0;
  }
  //Send Shader ID
  return ShaderID;
}

GLuint LoadShadersIntoProgram(string vertexPath,string fragPath,GLuint program=0){
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

