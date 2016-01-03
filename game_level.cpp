#include "game_level.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#define MOVESPEED 2
#define GRAVITY 2
#define JUMPSPEED 2

c_game_level::c_game_level(void)
{
    dimensions[0] = 20;
    dimensions[1] = 20;
    dimensions[2] = 20;
    cubes = NULL;
    reset_cubes = NULL;
    player.x_m_8 = 5*8;
    player.y_m_8 = 0*8;
    player.z_m_8 = 3*8;
}

void
c_game_level::size(int a, int b, int c)
{
    if (cubes) free(cubes);
    cubes = NULL;

    dimensions[0] = a;
    dimensions[1] = b;
    dimensions[2] = c;
    cubes_size = sizeof(t_level_cube) * dimensions[0] * dimensions[1] * dimensions[2];
    cubes = (t_level_cube *)malloc(cubes_size);
}

c_game_level::~c_game_level(void)
{
    if (cubes) {
        free(cubes);
        cubes = NULL;
    }
}

void
c_game_level::set_cube(int x, int y, int z, t_level_cube cube)
{
    if (!cubes) return;
    if (bound_dimension(x,0) &&
        bound_dimension(y,1) &&
        bound_dimension(z,2)) {
        *(cube_of_pos(x,y,z)) = cube;
    }
}

void
c_game_level::fill_region(int corner[3], int sizes[3], t_level_cube cube)
{
    int i, j, k;
    if (!cubes) return;
    for (i=corner[0]; i<sizes[0]+corner[0]; i++) {
        if (!bound_dimension(i,0)) continue;
        for (j=corner[1]; j<sizes[1]+corner[1]; j++) {
            if (!bound_dimension(j,1)) continue;
            for (k=corner[2]; k<sizes[2]+corner[2]; k++) {
                set_cube(i,j,k,cube);
            }
        }
    }
}

static void
load_error(const char *level_filename, FILE *level_file, const char *error)
{
    fprintf(stderr, "Failed to load level file '%s': %s\n", level_filename, error);
    fprintf(stderr, "Reached character %ld\n", ftell(level_file) );
}

static void
skip_whitespace(FILE *file)
{
    int ch;

    while (!feof(file)) {
        ch = fgetc(file);
        if (!isspace(ch)) {
            ungetc(ch, file);
            return;
        }
    }
    return;
}

static int
skip_separator(const char *level_filename, FILE *level_file)
{
    int ch;

    skip_whitespace(level_file);
    if (feof(level_file)) return 0;
    ch = fgetc(level_file);
    if (ch!='-') {
        load_error(level_filename, level_file, "Bad separator");
        ungetc(ch, level_file);
        return 0;
    }
    if (fgetc(level_file)!='-') load_error(level_filename, level_file, "Bad separator");
    if (fgetc(level_file)!='-') load_error(level_filename, level_file, "Bad separator");
    while (!feof(level_file)) {
        ch = fgetc(level_file);
        if (ch=='\n') break;
    }
    return 1;
}
static int
is_separator(FILE *file)
{
    int ch;

    skip_whitespace(file);
    if (feof(file)) return 0;
    ch = fgetc(file);
    if (ch!='-') {
        ungetc(ch, file);
        return 0;
    }
    ungetc(ch, file);
    return 1;
}

struct file_char_map {
    struct file_char_map *next;
    int ch;
    int mesh;
    int texture;
    t_level_cube cube;
};

int
c_game_level::load_from_file(const char *level_filename)
{
    FILE *level_file = fopen(level_filename, "r");
    int dimensions[3];
    int z;
    struct file_char_map *mapping;
    mapping = NULL;

    if (level_file==NULL) {
        fprintf(stderr, "Failed to open level file '%s'\n", level_filename);
        return 0;
    }

    fscanf(level_file, "%d %d %d\n",dimensions, dimensions+1, dimensions+2);

    size(dimensions[0],dimensions[1],dimensions[2]);

    if (!skip_separator(level_filename, level_file)) return 0;
    while (!is_separator(level_file)) {
        char ch;
        int mesh, texture;
        int things, flags;
        char flag_string[64];
        struct file_char_map *new_mapping;

        skip_whitespace(level_file);
        things = fscanf(level_file,"%c%d.%d.%s\n",&ch,&mesh,&texture,&flag_string[0]);
        if (things<3) {
            load_error(level_filename, level_file, "Trying to load <ch><mesh>.<texture>[.<flags>]");
            return 0;
        }
        flags = 0;
        if (things==4) {
            if (strchr(flag_string,'<')) flags |= TLC_IS_LEFT_CONVEYOR;
            if (strchr(flag_string,'>')) flags |= TLC_IS_RIGHT_CONVEYOR;
            if (strchr(flag_string,'@')) flags |= TLC_IS_BRICK;
            if (strchr(flag_string,'!')) flags |= TLC_IS_NOT_AIR;
            fprintf(stderr,"%s:%d\n",flag_string,flags);
        }
        new_mapping = (struct file_char_map *)malloc(sizeof(struct file_char_map));
        if (!new_mapping) {
            fprintf(stderr, "Failed to malloc room for level file '%s'\n", level_filename);
            return 0;
        }
        new_mapping->next = mapping;
        mapping = new_mapping;
        new_mapping->ch = ch;
        new_mapping->mesh = mesh;
        new_mapping->texture = texture;
        new_mapping->cube = flags | (mesh<<8) | (texture<<0);
        fprintf(stderr, "Char '%c' is mesh %d texture %d cube %d\n", new_mapping->ch, new_mapping->mesh, new_mapping->texture, new_mapping->cube );
    }

    for (z=dimensions[2]-1; z>=0; z--) {
        int y;
        if (!skip_separator(level_filename, level_file)) return 0;
        for (y=0; y<dimensions[1]; y++) {
            int x;
            skip_whitespace(level_file);
            if (fgetc(level_file)!='@') {
                char buffer[256];
                sprintf(buffer, "Expected @ at start of description for z=%d y=%d",z,y);
                load_error(level_filename, level_file, buffer);
                return 0;
            }
            for (x=0; x<dimensions[0]; x++) {
                int ch;
                struct file_char_map *ptr;
                set_cube(x,y,z,0);
                ch = fgetc(level_file);
                if (ch==' ') continue;
                for (ptr=mapping; ptr; ptr=ptr->next) {
                    if (ptr->ch==ch) {
                        set_cube(x,y,z,ptr->cube);
                        break;
                    }
                }
            }
        }
    }
    if (!skip_separator(level_filename, level_file)) return 0;
    while (mapping) {
        struct file_char_map *ptr;
        ptr = mapping;
        mapping = mapping->next;
        free(ptr);
    }

    reset(1);
    reset(0);
    return 1;
}      

void
c_game_level::reset(int set_reset)
{
    if (set_reset) {
        if (reset_cubes) free(reset_cubes);
        reset_cubes = (t_level_cube *)malloc(cubes_size);
        if (!reset_cubes) return;
        memcpy(reset_cubes, cubes, cubes_size);
        reset_player = player;
        return;
    }
    memcpy(cubes, reset_cubes, cubes_size);
    player = reset_player;
    jump_state = 0;
    state = 0;
    delta_x = 0;
    delta_z = 0;
    subtick = 0;
}

int
c_game_level::is_air_below(int x, int z)
{
    if (cube_of_pos((x+1)>>3,0,(z-1)>>3)[0] & TLC_IS_NOT_AIR) return 0;
    if (cube_of_pos((x+7)>>3,0,(z-1)>>3)[0] & TLC_IS_NOT_AIR) return 0;
    return 1;
}

static int jump_deltas[] = {
    0,-4,-4,-2,-2,-2,-2,-1,-1,-1,-1, 0,0, 1,1,1,1,2,2,2,2,4,4
};

void
c_game_level::tick(void)
{
    int jump_request;
    int new_x;
    jump_request = 0;
    subtick++;
    if (subtick < 2) return;
    subtick=0;

    if (state==0) { /* Must be at 'cube top level' to be in state 0 (on ground) */
        if (keys_down[0]) {
            delta_x = -MOVESPEED;
            if (keys_down[1]) {
                delta_x = 0;
            }
        }
        else if (keys_down[1]) {
            delta_x = MOVESPEED;
        }
        else { 
            t_level_cube cube_below;
            cube_below = cube_of_pos(player.x_m_8>>3,0,(player.z_m_8-1)>>3)[0];
            delta_x=0;
            if (cube_below & TLC_IS_LEFT_CONVEYOR)  delta_x=-1;
            if (cube_below & TLC_IS_RIGHT_CONVEYOR) delta_x=1;
        }
        if (keys_down[4]) {
            //Begin jump
            jump_state = sizeof(jump_deltas)/sizeof(int);
            state = 2;
        }
        if (is_air_below(player.x_m_8, player.z_m_8)) {
            //Falling
            state=1;
        }
        
        if (state==1) delta_x=0;
    }

    new_x = player.x_m_8 + delta_x;    
    if ((cube_of_pos((new_x)>>3,0,player.z_m_8>>3))[0] & TLC_IS_BRICK) {
        if (delta_x<0) delta_x=0;
    }
    if ((cube_of_pos((new_x+8)>>3,0,player.z_m_8>>3))[0] & TLC_IS_BRICK) {
        if (delta_x>0) delta_x=0;
    }
    player.x_m_8 = player.x_m_8 + delta_x;

    if (state==2){ /* Jumping */
        delta_z = jump_deltas[jump_state-1];
        jump_state--;
        if (jump_state==0){
            state=0; //Will be on ground next - except it may start falling then
        }
        if (delta_z<0) {
            if (((player.z_m_8 &7)==0) &&!is_air_below(player.x_m_8, player.z_m_8)) {
                state = 0;
                delta_z = 0;
            }
        }
    }
    
    if (state==1){ /* Falling */
        delta_z = -GRAVITY;
        delta_x = 0;
        if (((player.z_m_8 &7)==0) && !is_air_below(player.x_m_8, player.z_m_8)) {
            state=0;//On ground
            delta_z=0;
        }
    }
    player.z_m_8 += delta_z;
    if (player.z_m_8<0) player.z_m_8=1; 
    if (keys_down[6]) { reset(); }
}

void
c_game_level::display(char *buffer, int size)
{
    snprintf(buffer, size, "Player pos %d.%d , %d.%d\nState %d delta_x/z %d/%d",
             player.x_m_8>>3, player.x_m_8&7,
             player.z_m_8>>3, player.z_m_8&7,
             state, delta_x, delta_z);
}
