#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <SDL2/SDL_opengl.h>
#include <iostream>
#include <math.h>

const int SCREEN_WIDTH=640,SCREEN_HEIGHT=480;
SDL_Window* window=NULL;
SDL_Renderer* renderer=NULL;
bool shouldEnd=false;
SDL_Event e;
float clearDepth=1.0f,FOV=45.0f,nearZClip=0.1f,farZClip=100.0f;



float VerticalFOVFromHorizontalFOV(float hFOVDegrees,float aspect){
  //h=2*atan(tan(v/2)*aspect)
  //h/2=atan(tan(v/2)*aspect)
  //tan(h/2)=tan(v/2)*aspect
  //tan(v/2)=tan(h/2)/aspect
  //v=2*atan(tan(h/2)/aspect)
  float hFOVRadians=hFOVDegrees*M_PI/180;
  float vFOVRadians=2*atan(tan(hFOVRadians)/aspect);
  float vFOVDegrees=vFOVRadians*180/M_PI;
  return vFOVDegrees;
}

void ThrowSDLError(){
  std::cout << "SDL Error Encountered: "<<SDL_GetError() << "\n";
}

int CreateWindow(){
  if (SDL_Init(SDL_INIT_VIDEO)!= 0){
    ThrowSDLError();
    return 1;
  }
  SDL_RendererInfo displayRendererInfo;
  int flags=SDL_WINDOW_OPENGL&SDL_WINDOW_RESIZABLE;
  SDL_CreateWindowAndRenderer(SCREEN_WIDTH,
			      SCREEN_HEIGHT,
			      flags,
			      &window,
			      &renderer);
  SDL_GetRendererInfo(renderer,&displayRendererInfo);
  if (window==NULL || renderer==NULL){
    ThrowSDLError();
    return 1;
  }
  return 0;
}

void EndSDL(){
  SDL_DestroyWindow(window);
  SDL_Quit();
  return;
}

void MainLoop(){
  while (SDL_PollEvent(&e)!=0){
    if (e.type==SDL_QUIT)
      shouldEnd=true;
  }
}

int main(){
  if (CreateWindow()==1) return 1;
 
  while (!shouldEnd){
    MainLoop();
  }
  EndSDL();
  return 0;
}
