#ifndef MAP_H
#define MAP_H

#define HEIGHT 21
#define WIDTH 80

enum terrain_type{
    EMPTY = -1,
    GATE,
    PATH,
    CLEARING,
    TALL_GRASS,
    ARCTIC,
    LAKE,
    FOREST,
    MOUNTAIN,
    POKECENTER,
    POKEMART,
    BOULDER,
    NUM_TERRAINS
};

enum char_type {
    PC = 10,
    HIKER,
    RIVAL,
    SWIMMER,
    OTHER,
    NUM_NPCS
};

typedef struct terrain{
    struct terrain *NW;
    struct terrain *N;
    struct terrain *NE;
    struct terrain *E;
    struct terrain *SE;
    struct terrain *S;
    struct terrain *SW;
    struct terrain *W;
    int x, y;
    enum terrain_type val;
} terrain;

typedef struct
{
    struct pc *p;
    struct npc *npc;
    enum char_type type;
    char symbol;
    int seq_num;
    int next_turn;
    int x, y;
} character;


typedef struct {
    terrain t[HEIGHT][WIDTH];
    character *ch[HEIGHT][WIDTH];
    int pN, pS, pE, pW;
    int pos_x, pos_y; // Location of the map in the world.
} map;

typedef struct {
    map *maps[401][401];
    int (*hiker_dist_map)[WIDTH]; 
    int (*rival_dist_map)[WIDTH];
} world;

int init_world(world *w);
int init_map(map *m, int pos_x, int pos_y);
int seed_map(map *m);
int place_paths_and_buildings(map *m);
int place_buildings(map *m, int intersection, int path, enum terrain_type VAL, int upperbound);
int print_map(map *m);


#endif