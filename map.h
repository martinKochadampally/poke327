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
    PC_TYPE = 10,
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

class Character {
public:
    char symbol;
    char_type type;
    int x, y;
    int seq_num;
    int next_turn;
    int dir[2];

    Character(char symbol, char_type type, int x, int y) : symbol(symbol), type(type), x(x), y(y), seq_num(0), next_turn(0)
    {
        dir[0] = dir[1] = 0;
    }

    virtual ~Character() = 0;
    virtual void takeTurn(class Map *m) = 0;
};

inline Character::~Character() {}

class PC : public Character {
public:
    PC(int x, int y) : Character('@', char_type::PC_TYPE, x, y) {}
    ~PC() override {}
    void takeTurn(class Map *m) override {}
};

class NPC : public Character {
public:
    bool is_defeated;

    NPC(char symbol, char_type type, int x, int y)
        : Character(symbol, type, x, y), is_defeated(false) {}
    ~NPC() override {}
    void takeTurn(class Map *m) override {}
};

class Map {
public:
    terrain t[HEIGHT][WIDTH];
    Character *ch[HEIGHT][WIDTH];
    int pN, pS, pE, pW;
    int WE_intersection, NS_intersection;
    int pos_x, pos_y;
    heap char_heap;      // Per-map turn queue
    int current_time;    // Per-map time

    Map(int pos_x, int pos_y, Map *N, Map *S, Map *E, Map *W);
    ~Map();

    int seed();
    int place_paths_and_buildings();
    int place_buildings(int intersection, int path, enum terrain_type VAL, int upperbound);
    int place_pc(Character **player);
    int print();
};

class World {
public:
    Map *maps[401][401];
    int (*hiker_dist_map)[WIDTH];
    int (*rival_dist_map)[WIDTH];

    World();
    ~World();
};

#endif