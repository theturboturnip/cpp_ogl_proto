#ifndef __INC_WINDOW_H__
#define __INC_WINDOW_H__

#include "includes.h"
#include <fstream>

class GameWindow{ 
public: 
    GameWindow(int width=640,int height=480,const char *name="Untitled",bool resizeable=false,bool verbose=true);
    void CheckError(bool checkSDL=true,bool checkGL=true);
    //void UseShaderProgram(GLuint program);
    void SetClearColor(float r,float g,float b,float a=1.0f);
    void End(void);
    void ClearWindow(void);
    void ApplyResolution(void);
    void Flip(void);
    float aspect;
private:
    int SCREEN_WIDTH,SCREEN_HEIGHT;
    const char *SCREEN_NAME;
    //float SCREEN_FOV;
    bool VERBOSE,IS_ATTEMPTING;
    SDL_Window *SCREEN;
    SDL_GLContext GL_CONTEXT;
    void StartAttempt(char *attemptString);
    void EndAttempt(bool success=true);
    //glm::mat4 CalculateProjectionMatrix();
};

#endif

