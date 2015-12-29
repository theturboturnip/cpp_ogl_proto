#include "game_level.h"
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

c_game_level::c_game_level(void)
{
    dimensions[0] = 20;
    dimensions[1] = 20;
    dimensions[2] = 20;
    cubes = NULL;
}

void
c_game_level::size(int a, int b, int c)
{
    if (cubes) free(cubes);
    cubes = NULL;

    dimensions[0] = a;
    dimensions[1] = b;
    dimensions[2] = c;
    cubes = (t_level_cube *)malloc(sizeof(t_level_cube) * dimensions[0] * dimensions[1] * dimensions[2]);
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
        struct file_char_map *new_mapping;

        skip_whitespace(level_file);
        if (fscanf(level_file,"%c%d.%d\n",&ch,&mesh,&texture)!=3) {
            load_error(level_filename, level_file, "Trying to load <ch><mesh>.<texture>");
            return 0;
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
        new_mapping->cube = (mesh<<8) | (texture<<0);
        fprintf(stderr, "Char '%c' is mesh %d texture %d cube %d\n", new_mapping->ch, new_mapping->mesh, new_mapping->texture, new_mapping->cube );
    }

    for (z=0; z<dimensions[2]; z++) {
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
    return 1;
}      
