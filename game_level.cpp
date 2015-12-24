#include "game_level.h"
#include <stdlib.h>

c_game_level::c_game_level(void)
{
    dimensions[0] = 20;
    dimensions[1] = 20;
    dimensions[2] = 20;
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
    for (i=corner[0]; i<sizes[0]+corner[0]; i++) {
        if (!bound_dimension(i,0)) continue;
        for (j=corner[1]; j<sizes[1]+corner[1]; j++) {
            if (!bound_dimension(j,1)) continue;
            for (k=corner[2]; k<sizes[2]+corner[2]; k++) {
                set_cube(i,j,k,cube);
            }
        }
    }
    dimensions[0] = 20;
    dimensions[1] = 20;
    dimensions[2] = 20;
    cubes = (t_level_cube *)malloc(sizeof(t_level_cube) * dimensions[0] * dimensions[1] * dimensions[2]);
}
