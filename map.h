#ifndef MAP_H
#define MAP_H

#define HEIGHT 21
#define WIDTH 80
// #define HEIGHT 10
// #define WIDTH 10
#define TERRAIN_COUNT 6 // Number of terrain elements excluding the path.

enum terrain_type{
    PATH = 0,
    CLEARING = 1,
    TALL_GRASS = 2,
    ARCTIC = 4,
    LAKE = 8,
    FOREST = 16,
    BOULDER = 32,
    POKECENTER = 64,
    POKEMART = 65
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
    int pos_x, pos_y; // Location of the map in the world.
} map;

int init_map(map *m, int pos_x, int pos_y);
int seed_map(map *m);
int place_paths_and_buildings(map *m);
int place_buildings(map *m, int start, int intersection, enum terrain_type VAL, int upperbound);
int print_map(map *m);


#endif