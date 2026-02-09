#ifndef MAP_H
#define MAP_H

#define HEIGHT 21
#define WIDTH 80
#define TERRAIN_COUNT 6 // Number of terrain elements excluding the path.

enum terrain_type{
    PATH = 0,
    CLEARING = 1,
    TALL_GRASS = 2,
    ARCTIC = 4,
    LAKE = 8,
    FOREST = 16,
    BOULDER = 32, 
};

typedef struct tile{
    struct tile *up;
    struct tile *down;
    struct tile *left;
    struct tile *right;
    enum terrain_type val;
} tile;

typedef struct {
    tile tiles[HEIGHT][WIDTH];
    int pN, pS, pE, pW;
    int is_initialized; // 0 means it is intialized
} map;

int init_map(map *m);
int seed_map(map *m);
int place_paths(map *m);
int print_map(map *m);


#endif