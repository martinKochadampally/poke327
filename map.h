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
};

enum char_type {
    PC = 10,
    HIKER,
    RIVAL,
    SWIMMER,
    OTHER,
    PACER = 24,
    WANDERER = 34,
    SENTRY = 44,
    EXPLORER = 54
};

typedef struct terrain{
    struct terrain *NW, *N, *NE, *E;
    struct terrain *SE, *S, *SW, *W;
    int x, y;
    enum terrain_type val;
} terrain;

typedef struct pc{
    int strength;
} npc;

typedef struct npc{
    int strength;
} pc;

typedef struct {
    pc *p;
    npc *npc;
    char symbol;
    int seq_num;
    int next_turn;
    int x, y;
    int dir[1];
    enum char_type type;
} character;


typedef struct {
    terrain t[HEIGHT][WIDTH];
    character *ch[HEIGHT][WIDTH];
    int pN, pS, pE, pW;
    int WE_intersection, NS_intersection;
    int pos_x, pos_y; // Location of the map in the world.
} map;

typedef struct {
    map *maps[401][401];
    int (*hiker_dist_map)[WIDTH]; 
    int (*rival_dist_map)[WIDTH];
} world;

int world_init(world *w);
void world_destroy(world *w);
int init_map(map *m, int pos_x, int pos_y);
int seed_map(map *m);
int place_paths_and_buildings(map *m);
int place_buildings(map *m, int intersection, int path, enum terrain_type VAL, int upperbound);
int place_pc(map *m, character **player);
int place_npcs(map *m, int NUM_NPCs);
int print_map(map *m);



#endif