typedef char t_level_cube;

class c_game_level {
public:
    c_game_level(void);
    ~c_game_level();
    int dimensions[3];
    t_level_cube *cubes;
    void set_cube(int x, int y, int z, t_level_cube cube);
    void fill_region(int corner[0], int sizes[0], t_level_cube cube);
    inline int bound_dimension(int pos, int d) {
        if (!cubes) return 0;
        if (pos<0) return 0;
        if (pos>=dimensions[d]) return 0;
        return 1;
    }
    inline t_level_cube *cube_of_pos(int x, int y, int z) {
        return cubes + x + dimensions[0]*(y+dimensions[1]*z);
    }
};

