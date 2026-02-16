#ifndef MAP_H
#define MAP_H

#define HEIGHT 21
#define WIDTH 80
// #define HEIGHT 10
// #define WIDTH 10
#define TERRAIN_COUNT 6 // Number of terrain elements excluding the path.

enum terrain_type{
    EMPTY = -1,
    PATH,
    CLEARING,
    TALL_GRASS,
    ARCTIC,
    LAKE,
    FOREST,
    BOULDER,
    POKECENTER,
    POKEMART
};

typedef struct tile{
    struct tile *top_left;
    struct tile *up;
    struct tile *top_right;
    struct tile *right;
    struct tile *bottom_right;
    struct tile *down;
    struct tile *bottom_left;
    struct tile *left;
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
int place_buildings(map *m, int intersection, int path, enum terrain_type VAL, int upperbound);
int print_map(map *m);


#endif