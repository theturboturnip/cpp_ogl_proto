#define GL_GLEXT_PROTOTYPES
//#include <GL/gl.h>
//#include <GL/glext.h>
#include <SDL2/SDL.h> 
#include <SDL2/SDL_opengl.h>
#include <iostream>
#include "loadShader.cpp"
#include <stdlib.h>
#include <stdio.h>

int SCREEN_WIDTH=640,SCREEN_HEIGHT=480;
SDL_Window* window=NULL;
SDL_GLContext glContext;
bool shouldEnd=false;
SDL_Event e;
GLuint VertexArrayID,VertexBuffer,ShaderProgramID;

void checkSDLError(void)
{
	const char *error = SDL_GetError();
	if (*error != '\0')
	{
		fprintf(stderr,"SDL Error: %s\n", error);
		SDL_ClearError();
	}
}

void ThrowSDLError(void){
  std::cout << "SDL Error Encountered: "<<SDL_GetError() << "\n";
}

int CreateWindow(void){
  int major, minor;

   if (SDL_Init(SDL_INIT_EVERYTHING)!= 0){
    ThrowSDLError();
    return 1;
  }

  fprintf(stderr,"Setting GL basic attributes\n");
  SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 8 );
  SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 8 );
  SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 8 );
  SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, 8 );

  fprintf(stderr,"Setting GL version etc\n");
  SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
  SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 3 );
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  checkSDLError();

  int flags=SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
  window = SDL_CreateWindow("OpenGL Test",SDL_WINDOWPOS_UNDEFINED,0,SCREEN_WIDTH,SCREEN_HEIGHT,flags);
  if (window==NULL){
    ThrowSDLError();
    return 1;
  }
  glContext=  SDL_GL_CreateContext(window);
  fprintf(stderr,"glContext %p\n",glContext);
  if (!glContext) {
    return 1;
  }

  fprintf(stderr,"Getting GL attributes\n");
  SDL_GL_GetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, &major );
  SDL_GL_GetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, &minor );
  checkSDLError();
  fprintf(stderr,"SDL GL version %d.%d\n",major,minor);

  //std::cout << (glContext==NULL) ;a
  return 0;
}

void InitScene(void){
  glClearColor(0.0f,0.0f,0.4f,0.0f);
  glGenVertexArrays(1,&VertexArrayID);
  glBindVertexArray(VertexArrayID);
  GLfloat g_vertex_buffer_data[] = {
    -1.0f,1.0f,0.0f,
    1.0f,-1.0f,0.0f,
    0.0f,1.0f,0.0f};
  glGenBuffers(1,&VertexBuffer);
  glBindBuffer(GL_ARRAY_BUFFER,VertexBuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
  ShaderProgramID=LoadShaders("SimpleVertexShader.glsl","SimpleFragmentShader.glsl");
  
}
/*Basic object creation
 need vertex shader + fragment shader for entire object
 compile and store program
 load verticies from .obj to VBO and VAO
 To draw, use a Draw() function
*/

void EndSDL(void){
  SDL_DestroyWindow(window);
  SDL_Quit();
  return;
}

void MainLoop(void){
  while (SDL_PollEvent(&e)!=0){
    if (e.type==SDL_QUIT)
      shouldEnd=true;
  }
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glUseProgram(ShaderProgramID);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
  glVertexAttribPointer(0,
			3,
			GL_FLOAT,
			GL_FALSE,
			0,
			(void*)0);
  glDrawArrays(GL_TRIANGLES,0,3);
  glDisableVertexAttribArray(0);
  SDL_GL_SwapWindow(window);
}

int main(){
  if (CreateWindow()==1) return 1;
  InitScene();
  while (!shouldEnd){
    MainLoop();
  }
  EndSDL();
  return 0;
}
