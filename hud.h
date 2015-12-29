#ifndef __INC_HUD_H__
#define __INC_HUD_H__

#include <SDL.h> 
#include <SDL_opengl.h>
#include <SDL_ttf.h> 

class c_hud
{
public:
    c_hud(int width, int height);
    ~c_hud(void);
    int init(void);
    void clear(void);
    void draw_text(int x, int y, const char *text, SDL_Color c);
    void display(void);

    int width;
    int height;
    struct {
        TTF_Font    *font;
        SDL_Surface *surface;
    } SDL;
    struct {
        GLuint program_id;
        GLuint coords_id;
        GLuint texture_id;
        GLuint surface_texture;
    } GL;

};

#endif // __INC_HUD_H__

