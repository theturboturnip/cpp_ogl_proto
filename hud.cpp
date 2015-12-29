#define GL_GLEXT_PROTOTYPES
#define GLM_FORCE_RADIANS
#include <SDL.h> 
#include <SDL_opengl.h>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include "loader.h"
#include "hud.h"

const float coords[8] = { 0.0f, 0.0f,
                          1.0f, 0.0f,
                          0.0f,  1.0f,
                          1.0f,  1.0f };

c_hud::c_hud(int hud_width, int hud_height)
{
    width  = hud_width;
    height = hud_height;
}

int
c_hud::init(void)
{
    SDL.surface = SDL_CreateRGBSurface(0,width,height,32,0xff,0xff<<8,0xff<<16,0xff<<24);
    if (!SDL.surface) return 0;

    GL.program_id = LoadShadersIntoProgram("hud_vertex_shader.glsl","hud_fragment_shader.glsl");
    if (GL.program_id == 0) {
        fprintf(stderr,"HUD Shader load failed\n");
        return 0;
    }

    GL.texture_id = glGetUniformLocation(GL.program_id,"textureSampler");

    TTF_Init();
    SDL.font = TTF_OpenFont("/Library/Fonts/Tahoma.ttf", 24);
    //SDL.font = TTF_OpenFont("FreeSans.ttf", 24);
    if (SDL.font == NULL) {
        fprintf(stderr,"HUD font load failed\n");
        return 0;
    }

    glGenBuffers(1,&GL.coords_id);
    glBindBuffer(GL_ARRAY_BUFFER,GL.coords_id);
    glBufferData(GL_ARRAY_BUFFER,sizeof(coords),coords,GL_STATIC_DRAW);
    glGenTextures(1,&GL.surface_texture);
    return 1;
}

void
c_hud::clear(void)
{
    SDL_FillRect(SDL.surface, NULL, SDL_MapRGB(SDL.surface->format, 0, 0, 0));
}

void
c_hud::draw_text(int x, int y, const char *text, SDL_Color c)
{
    SDL_Surface *text_surface;
    SDL_Rect dest_rect; // only x and y are used

    if (!SDL.font) return;
    // Write text to surface
    text_surface = TTF_RenderText_Solid(SDL.font,text,c);
    dest_rect.x = x;
    dest_rect.y = y;
    SDL_BlitSurface(text_surface, NULL, SDL.surface, &dest_rect);
}

void
c_hud::display(void)
{
    glUseProgram(GL.program_id);
    glActiveTexture(GL_TEXTURE0); // Uset texture unit 0 throughout
    glEnableVertexAttribArray(0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBindTexture(GL_TEXTURE_2D,GL.surface_texture);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,SDL.surface->w,SDL.surface->h,0,GL_RGBA,GL_UNSIGNED_BYTE,SDL.surface->pixels);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glBindBuffer(GL_ARRAY_BUFFER, GL.coords_id);
    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,0,NULL);
    glDrawArrays(GL_TRIANGLE_STRIP,0,4);

    glDisable(GL_BLEND);
    glDisableVertexAttribArray(0);
}
