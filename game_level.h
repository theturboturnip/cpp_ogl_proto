typedef unsigned int t_level_cube;
typedef struct {
    int x_m_8;
    int y_m_8;
    int z_m_8;
} t_level_position;

class c_game_level {
public:
    c_game_level(void);
    ~c_game_level();
    int dimensions[3];
    void set_cube(int x, int y, int z, t_level_cube cube);
    void fill_region(int corner[0], int sizes[0], t_level_cube cube);
    void size(int a, int b, int c);
    int load_from_file(const char *level_filename);
    inline int bound_dimension(int pos, int d) {
        if (!cubes) return 0;
        if (pos<0) return 0;
        if (pos>=dimensions[d]) return 0;
        return 1;
    }
    inline t_level_cube *cube_of_pos(int x, int y, int z) {
        return cubes + x + dimensions[0]*(y+dimensions[1]*z);
    }

    t_level_cube *cubes;
    t_level_position player;
};

