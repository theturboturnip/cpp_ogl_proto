#define GL_GLEXT_PROTOTYPES
#define GLM_FORCE_RADIANS
#include <SDL.h> 
#include <SDL_opengl.h>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtx/transform.hpp>
#include <cstdlib>
#include "loader.h"
#include "hud.h"
#include "game_gl.h"

#define NUM_KEYS 20
static int keys[NUM_KEYS] = {
SDLK_d, SDLK_a,
SDLK_x, SDLK_z,
SDLK_s, SDLK_w,
SDLK_r, SDLK_q,
SDLK_o, SDLK_p,
SDLK_m, SDLK_n,
SDLK_i, SDLK_j,
SDLK_f, SDLK_g,
SDLK_h, SDLK_b,
SDLK_1, SDLK_2
};

class c_main
{
public:
    c_main(int screen_width, int screen_height);
    ~c_main();
    void checkSDLError(void);
    int Init(void);
    void MainLoop(void);
    void LoopForever(void);
    void Exit(void);
    int CreateWindow(int width, int height);

    c_game_gl     *game_gl;
    SDL_Window    *window;
    SDL_GLContext glContext;
    int keys_down[NUM_KEYS];
    bool shouldEnd;
    int screen_width, screen_height;
    c_hud *hud;
    int hud_active;
    int prev_time_out;
    int last_time_out;
};

c_main::c_main(int screen_width, int screen_height)
{
    int i;
    this->screen_width = screen_width;
    this->screen_height = screen_height;
    window = NULL;
    shouldEnd = false;
    for (i=0; i<NUM_KEYS; i++) {
        keys_down[i] = 0;
    }
    hud_active = 0;
}

void
c_main::checkSDLError(void)
{
	const char *error;
    error = SDL_GetError();
	if (*error != '\0') {
		fprintf(stderr,"SDL Error: %s\n", error);
		SDL_ClearError();
	}
}

int c_main::Init(void)
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        return 0;
    }

    SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 8 );
    SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 8 );
    SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 8 );
    SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, 8 );

    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 3 );
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    checkSDLError();

    return 1;
}

void
c_main::Exit(void)
{
    if (window) {
        SDL_DestroyWindow(window);
    }
    SDL_Quit();
}

int
c_main::CreateWindow(int width, int height)
{
    int flags; 

    flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
    window = SDL_CreateWindow("OpenGL Test",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,width, height, flags);
    checkSDLError();

    if (!window) {
        return 0;
    }

    hud = new c_hud(width,height);

    glContext=  SDL_GL_CreateContext(window);
    if (!glContext) {
        return 0;
    }

    if (0) {
        int major, minor;
        SDL_GL_GetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, &major );
        SDL_GL_GetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, &minor );
        checkSDLError();
        fprintf(stderr, "Using OpenGL version %d.%d\n",major,minor);
    }

    if (hud->init() == 0) {
        fprintf(stderr, "Failed to initialize HUD\n");
    }

    game_gl = new c_game_gl(width,height);
    game_gl->body_pos    = glm::vec3(4.0,3.0,3.0);
    game_gl->body_facing = glm::vec3(-1.0,-1.0,0.0);
    game_gl->body_facing = glm::normalize(game_gl->body_facing);
    game_gl->head_yaw = 0;
    game_gl->head_pitch = 0;
    game_gl->FOV = 45;
    return game_gl->init();
}

void
c_main::MainLoop(void)
{
    int i;
    SDL_Event e;
    int time_in;
    time_in = SDL_GetTicks();

    while (SDL_PollEvent(&e)!=0){
        if (e.type==SDL_QUIT)
            shouldEnd=true;
        if (e.type==SDL_KEYDOWN) {
            for (i=0; i<sizeof(keys)/sizeof(e.key.keysym.sym); i++) {
                if (e.key.keysym.sym==keys[i]) keys_down[i]=1;
            }
        }
        if (e.type==SDL_KEYUP) {
            for (i=0; i<sizeof(keys)/sizeof(e.key.keysym.sym); i++) {
                if (e.key.keysym.sym==keys[i]) keys_down[i]=0;
            }
        }
    }
    if (keys_down[0]) { game_gl->body_pos[0]-=0.1; }
    if (keys_down[1]) { game_gl->body_pos[0]+=0.1; }
    if (keys_down[2]) { game_gl->body_pos[1]-=0.1; }
    if (keys_down[3]) { game_gl->body_pos[1]+=0.1; }
    if (keys_down[4]) { game_gl->body_pos[2]-=0.1; }
    if (keys_down[5]) { game_gl->body_pos[2]+=0.1; }
    if (keys_down[6]) { game_gl->body_pos[0]=4.0; game_gl->body_pos[1]=3.0; game_gl->body_pos[2]=3.0; }
    if (keys_down[7]) { shouldEnd = true; }
    if (keys_down[8]) { game_gl->FOV -= 1; }
    if (keys_down[9]) { game_gl->FOV += 1; }
    if (keys_down[10]) { game_gl->head_yaw -= 1; }
    if (keys_down[11]) { game_gl->head_yaw += 1; }
    if (keys_down[12]) { game_gl->head_pitch -= 1; }
    if (keys_down[13]) { game_gl->head_pitch += 1; }
    if (keys_down[14]) { game_gl->body_facing = glm::rotate( game_gl->body_facing, glm::radians(2.0f), glm::vec3(0,0,1) ); }
    if (keys_down[15]) { game_gl->body_facing = glm::rotate( game_gl->body_facing, glm::radians(-2.0f), glm::vec3(0,0,1) ); }
    if (keys_down[16]) { game_gl->body_pos = game_gl->body_pos + game_gl->body_facing * 0.25f; }
    if (keys_down[17]) { game_gl->body_pos = game_gl->body_pos - game_gl->body_facing * 0.25f; }
    if (keys_down[18]) { hud_active = 1; }
    if (keys_down[19]) { hud_active = 0; }
    game_gl->head_yaw = 0.97*game_gl->head_yaw;
    game_gl->head_pitch = 0.97*game_gl->head_pitch;

    game_gl->draw();

    if (hud_active) {
        char text[256];
        int time_now;
        time_now = SDL_GetTicks();

        hud->clear();
        SDL_Color a={255, 255, 255};
        sprintf(text, "%5.2ffps time %dms", 1000.0f/(last_time_out-prev_time_out), (last_time_out - prev_time_out));
        hud->draw_text(100,100,text,a);
        hud->display();
    }

    SDL_GL_SwapWindow(window);
    prev_time_out = last_time_out;
    last_time_out = SDL_GetTicks();

}

void
c_main::LoopForever(void)
{
    while (!shouldEnd){
        MainLoop();
    }
}

int main(int argc,char *argv[]){
    int screen_width, screen_height;
    c_main *m;

    screen_width = 640;
    screen_height = 512;
    if (argc==3){
        screen_width  = atoi(argv[1]);
        screen_height = atoi(argv[2]);
    }

    m = new c_main(screen_width, screen_height);
    m->Init();
    if (!m->CreateWindow(screen_width, screen_height)) {
        fprintf(stderr,"Create window failed\n");
        return 4;
    }

    fprintf(stderr,"Looping forever\n");
    m->LoopForever();
    fprintf(stderr,"Done looping forever...\n");
    m->Exit();
    return 0;
}
