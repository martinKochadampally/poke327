#include <stdio.h>
#include <stdlib.h>

#include "queue.h"
#include "heap.h"
#include "map.h"

// ---- World ----

World::World() {
    int y, x;
    for (y = 0; y < 401; y++)
        for (x = 0; x < 401; x++)
            maps[y][x] = NULL;

    hiker_dist_map = new int[HEIGHT][WIDTH];
    rival_dist_map = new int[HEIGHT][WIDTH];
}

World::~World() {
    int i, j;
    for (i = 0; i < HEIGHT; i++)
        for (j = 0; j < WIDTH; j++) {
            hiker_dist_map[i][j] = 0;
            rival_dist_map[i][j] = 0;
        }
    delete[] hiker_dist_map;
    delete[] rival_dist_map;
}

// ---- Map ----

Map::Map(int pos_x, int pos_y, Map *N, Map *S, Map *E, Map *W) {
    int x, y;
    current_time = 0;
    heap_init(&char_heap);

    for (y = 0; y < HEIGHT; y++) {
        for (x = 0; x < WIDTH; x++) {
            t[y][x].x = x;
            t[y][x].y = y;
            t[y][x].val = (!x || !y || x == WIDTH-1 || y == HEIGHT-1) ? BOULDER : EMPTY;
            ch[y][x] = NULL;

            t[y][x].N  = (y) ? &t[y-1][x] : NULL;
            t[y][x].S  = (y < HEIGHT-1) ? &t[y+1][x] : NULL;
            t[y][x].W  = (x) ? &t[y][x-1] : NULL;
            t[y][x].E  = (x < WIDTH-1) ? &t[y][x+1] : NULL;

            t[y][x].NW = (y && x) ? &t[y-1][x-1] : NULL;
            t[y][x].NE = (y && x < WIDTH-1) ? &t[y-1][x+1] : NULL;
            t[y][x].SW = (y < HEIGHT-1 && x) ? &t[y+1][x-1] : NULL;
            t[y][x].SE = (y < HEIGHT-1 && x < WIDTH-1) ? &t[y+1][x+1] : NULL;
        }
    }

    this->pos_x = pos_x;
    this->pos_y = pos_y;
    pN = pS = pE = pW = -1;

    if (pos_y > 0 && N) pN = N->pS;
    else                 pN = 4 + rand() % (WIDTH - 8);

    if (pos_x < 400 && E) pE = E->pW;
    else                   pE = 4 + rand() % (HEIGHT - 8);

    if (pos_y < 400 && S) pS = S->pN;
    else                   pS = 4 + rand() % (WIDTH - 8);

    if (pos_x > 0 && W) pW = W->pE;
    else                 pW = 4 + rand() % (HEIGHT - 8);
}

Map::~Map() {
    heap_destroy(&char_heap);
}

int Map::seed() {
    int i, x, y, size;
    queue q;
    terrain *current;

    queue_init(&q);

    enum terrain_type regions[] = {CLEARING, TALL_GRASS, CLEARING, LAKE, ARCTIC, TALL_GRASS, CLEARING, FOREST, MOUNTAIN};
    int num_regions = sizeof(regions) / sizeof(enum terrain_type);

    for (i = 0; i < num_regions; i++) {
        x = 1 + rand() % (WIDTH - 2);
        y = 1 + rand() % (HEIGHT - 2);

        if (t[y][x].val != EMPTY) { i--; continue; }

        t[y][x].val = regions[i];
        if (queue_enqueue(&q, x, y)) return -1;
    }

    while (!queue_size(&q, &size) && size) {
        queue_dequeue(&q, &x, &y);
        current = &t[y][x];

        if (current->E && current->E->val == EMPTY) {
            current->E->val = current->val;
            if (queue_enqueue(&q, x+1, y)) return -1;
        }
        if (current->S && current->S->val == EMPTY) {
            current->S->val = current->val;
            if (queue_enqueue(&q, x, y+1)) return -1;
        }
        if (current->W && current->W->val == EMPTY) {
            current->W->val = current->val;
            if (queue_enqueue(&q, x-1, y)) return -1;
        }
        if (current->N && current->N->val == EMPTY) {
            current->N->val = current->val;
            if (queue_enqueue(&q, x, y-1)) return -1;
        }
    }

    return 0;
}

int Map::place_paths_and_buildings() {
    int i, x, y, intersection1, intersection2, min, max;
    int d = abs(pos_x - 200) + abs(pos_y - 200);
    int center = 0, mart = 0;

    if (d == 0) {
        center = mart = 1;
    } else {
        double p;
        if (d == 200)      p = 100;
        else if (d < 200)  p = ((-45.0 * d) / 200.0 + 50.0) / 100.0;
        else               p = 0.05;

        if ((rand() % 100) < (p * 100)) center = 1;
        if ((rand() % 100) < (p * 100)) mart = 1;
    }

    intersection1 = WIDTH/8 + rand() % (WIDTH - WIDTH/4);

    y = pW;
    for (i = 0; i < WIDTH; i++) {
        if (i == intersection1) y = pE;
        t[y][i].val = PATH;
    }

    min = (pE < pW) ? pE : pW;
    max = (pE > pW) ? pE : pW;
    for (i = min; i <= max; i++) t[i][intersection1].val = PATH;

    intersection2 = HEIGHT/10 + rand() % (HEIGHT - HEIGHT/5);

    x = pN;
    for (i = 0; i < HEIGHT; i++) {
        if (i == intersection2) x = pS;
        t[i][x].val = PATH;
    }

    min = (pN < pS) ? pN : pS;
    max = (pN > pS) ? pN : pS;
    for (i = min; i <= max; i++) t[intersection2][i].val = PATH;

    if (center) place_buildings(intersection1, pW, POKECENTER, HEIGHT);
    if (mart)   place_buildings(intersection2, pN, POKEMART, WIDTH);

    t[pW][0].val = t[pE][WIDTH-1].val = t[0][pN].val = t[HEIGHT-1][pS].val = GATE;

    WE_intersection = intersection1;
    NS_intersection = intersection2;

    return 0;
}

int Map::place_buildings(int intersection, int path, enum terrain_type VAL, int upperbound) {
    int tmp, x, y;

    do {
        x = 1 + rand() % (intersection - 2);
        if (path > upperbound / 4) y = path - 2;
        else                       y = path + 1;
        if (upperbound == WIDTH) { tmp = x; x = y; y = tmp; }
    } while (y <= 0 || y >= HEIGHT - 2
            || x <= 0 || x >= WIDTH - 2
            || t[y][x].val == PATH       || t[y+1][x+1].val == PATH
            || t[y][x].val == POKECENTER || t[y+1][x+1].val == POKECENTER
            || t[y][x].val == POKEMART   || t[y+1][x+1].val == POKEMART);

    t[y][x].val = t[y][x+1].val = t[y+1][x].val = t[y+1][x+1].val = VAL;
    return 0;
}

int Map::place_pc(Character **player) {
    int x;
    if (pos_y != 200 || pos_x != 200) return -1;
    x = WE_intersection + rand() % (WIDTH - 2 - WE_intersection);
    *player = new PC(x, pE);
    ch[pE][x] = *player;
    return 0;
}

int Map::print() {
    int i, j;
    char c;

    for (i = 0; i < HEIGHT; i++) {
        for (j = 0; j < WIDTH; j++) {
            if (ch[i][j]) {
                c = ch[i][j]->symbol;
            } else {
                switch (t[i][j].val) {
                    case GATE:       c = '#'; break;
                    case PATH:       c = '#'; break;
                    case CLEARING:   c = '.'; break;
                    case TALL_GRASS: c = ':'; break;
                    case ARCTIC:     c = '*'; break;
                    case LAKE:       c = '~'; break;
                    case FOREST:     c = '^'; break;
                    case MOUNTAIN:   c = '%'; break;
                    case POKECENTER: c = 'C'; break;
                    case POKEMART:   c = 'M'; break;
                    case BOULDER:    c = '%'; break;
                    default:         c = ' '; break;
                }
            }
            printf("%c", c);
        }
        printf("\n");
    }

    printf("position: (%i, %i)\n", (pos_x - 200), (pos_y - 200));
    return 0;
}