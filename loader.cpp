#define GL_GLEXT_PROTOTYPES
#define GLM_FORCE_RADIANS

#include "loader.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_image.h>
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtx/transform.hpp>
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

GLuint LoadShadersIntoProgram(string vertexPath,string fragPath,GLuint program)
{
  if(program==0)
    program=glCreateProgram();
  GLuint vertexShader=LoadShaderFromFile(vertexPath,GL_VERTEX_SHADER);
  if(vertexShader==0)
    return 0;
  glAttachShader(program,vertexShader);
  GLuint fragShader=LoadShaderFromFile(fragPath,GL_FRAGMENT_SHADER);
  if(fragShader==0)
    return 0;
  glAttachShader(program,fragShader);
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

/*

Model Loading

*/

GLuint *LoadModelFromFile(string modelPath){
  //Assume model is a .obj
  fprintf(stderr, "Attempting model load from %s...",modelPath.c_str());
  FILE* modelFile=fopen(modelPath.c_str(),"r");
  if(modelFile==NULL)
    fprintf(stderr, " Failure\nCouldn't open file\n");
  std::vector<glm::vec3> verts,normals;
  vector<glm::vec2> uvs;
  vector< unsigned int > vertexIndices, uvIndices, normalIndices;
  int line=0;
  while (1){
    //Find first word of line
    char lineHeader[128];
    int res= fscanf(modelFile,"%s",lineHeader);
    if(res==EOF)
      break;
    if(strcmp(lineHeader,"vt")==0){
      //Process UV
      glm::vec2 uv;
      fscanf(modelFile,"%f %f\n",&uv.x,&uv.y);
      uvs.push_back(uv);
    }else if(strcmp(lineHeader,"vn")==0){
      //Process normal
      glm::vec3 normal;
      fscanf(modelFile,"%f %f %f\n", &normal.x,&normal.y,&normal.z);
      normals.push_back(normal);
    }else if(strcmp(lineHeader,"v")==0){
      //Process vertex
      glm::vec3 vertex;
      fscanf(modelFile,"%f %f %f\n",&vertex.x,&vertex.y,&vertex.z);
      verts.push_back(vertex);
    }else if(strcmp(lineHeader,"f")==0){
      //Process face
      unsigned int vertexIndex[3],uvIndex[3],normalIndex[3];
      int matches=fscanf(modelFile,"%d/%d/%d %d/%d/%d %d/%d/%d\n",
			 &vertexIndex[0],&uvIndex[0],&normalIndex[0],
			 &vertexIndex[1],&uvIndex[1],&normalIndex[1],
			 &vertexIndex[2],&uvIndex[2],&normalIndex[2]);
      if (matches!=9){
	fprintf(stderr," Failure\nLine %d couldn't be read\n",line);
	return 0;
      }
      vertexIndices.push_back(vertexIndex[0]);
      vertexIndices.push_back(vertexIndex[1]);
      vertexIndices.push_back(vertexIndex[2]);
      
      uvIndices.push_back(uvIndex[0]);
      uvIndices.push_back(uvIndex[1]);
      uvIndices.push_back(uvIndex[2]);

      normalIndices.push_back(normalIndex[0]);
      normalIndices.push_back(normalIndex[1]);
      normalIndices.push_back(normalIndex[2]);
    }
    line++;
    //Done processing, loop back
  }
  //Organize data into OpenGL compatible format
  unsigned int vertexIndicesLength=vertexIndices.size(),i;
  glm::vec3 organizedVerticies[vertexIndicesLength],
    organizedNormals[vertexIndicesLength];
  glm::vec2 organizedUVs[vertexIndicesLength];
  //Organize verticies
  for(i=0; i<vertexIndicesLength;i++){
    glm::vec3 vertex=verts[vertexIndices[i]-1];
    //-1 because .obj starts from 1
    organizedVerticies[i]=vertex;
  }
  //Organize normals
  for(i=0; i<vertexIndicesLength;i++){
    glm::vec3 normal=normals[normalIndices[i]-1];
    organizedNormals[i]=normal;
  }
  //Organize uvs
  for(i=0; i<vertexIndicesLength;i++){
    glm::vec2 uv=uvs[uvIndices[i]-1];
    organizedUVs[i]=uv;
  }
  //Data is organized, create array of buffer and finish
  GLuint buffers[3];
  //Vertex=0
  //UV=1
  //Normal=2
  glGenBuffers(3,buffers);
  //Fill vertex buffer
  glBindBuffer(GL_ARRAY_BUFFER,buffers[0]);
  glBufferData(GL_ARRAY_BUFFER,
	       sizeof(glm::vec3)*vertexIndicesLength,
	       &organizedVerticies[0],
	       GL_STATIC_DRAW);
  //Fill uv buffer
  glBindBuffer(GL_ARRAY_BUFFER,buffers[1]);
  glBufferData(GL_ARRAY_BUFFER,
	       sizeof(glm::vec2)*vertexIndicesLength,
	       &organizedUVs[0],
	       GL_STATIC_DRAW);
  //Fill normal buffer
  glBindBuffer(GL_ARRAY_BUFFER,buffers[2]);
  glBufferData(GL_ARRAY_BUFFER,
	       sizeof(glm::vec3)*vertexIndicesLength,
	       &organizedNormals[0],
	       GL_STATIC_DRAW);
  return buffers;
  
}
