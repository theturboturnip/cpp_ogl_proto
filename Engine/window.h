#ifndef __INC_WINDOW_H__
#define __INC_WINDOW_H__

#define GL_GEXT_PROTOTYPES
#define GLM_FORCE_RADIANS
#include <SDL.h>
#include <SDL_opengl.h>
#include <stdlib.h>
#include <stdio.h>
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtx/transform.hpp>

class GameWindow{
public: 
    GameWindow(int width=640,int height=480,float FOV=45,const char *name="Untitled",bool resizeable=false,bool zBuffer=true,bool cullBackFaces=true,bool verbose=true);
    void CheckError(bool checkSDL=true,bool checkGL=true);
    void UseShaderProgram(GLuint program);
    void SetClearColor(float r,float g,float b,float a=1.0f);
    void End(void);
    void ClearWindow(void);
    void Flip(void);
    glm::mat4 SetFOV(float newFOV);
    void SetZBuffer(bool newShouldZBuffer);
    void SetBackFaceCull(bool newBackFaceCull);
    glm::mat4 ProjectionMatrix;
    
private:
    int SCREEN_WIDTH,SCREEN_HEIGHT;
    const char *SCREEN_NAME;
    float SCREEN_FOV;
    bool Z_BUFFER_ENABLED,BACK_FACE_CULL,VERBOSE,IS_ATTEMPTING;
    SDL_Window *SCREEN;
    SDL_GLContext GL_CONTEXT;
    void StartAttempt(char *attemptString);
    void EndAttempt(bool success=true);
    glm::mat4 CalculateProjectionMatrix();
};

#endif
