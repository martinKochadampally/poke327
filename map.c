#include <stdio.h>
#include <stdlib.h>

#include "queue.h"
#include "map.h"

int world_init(world *w) {
    int y, x;

    for (y = 0; y < 401; y++) {
        for (x = 0; x < 401; x++) {
            w->maps[y][x] = NULL;
        }
    } 

    w->hiker_dist_map = malloc(sizeof(int[HEIGHT][WIDTH]));
    w->rival_dist_map = malloc(sizeof(int[HEIGHT][WIDTH]));
    if (!w->hiker_dist_map || !w->rival_dist_map) return -1;
    return 0;
}

void world_destroy(world *w) {
    int i, j;
    for (i = 0; i < HEIGHT; i++) {
        for (j = 0; j < WIDTH; j++) {
            w->hiker_dist_map[i][j] = 0;
            w->rival_dist_map[i][j] = 0;
        }
    }
    free(w->hiker_dist_map);
    free(w->rival_dist_map);
}


int init_map(map *m, int pos_x, int pos_y) {    
    int x, y;

    for (y = 0; y < HEIGHT; y++) {
        for (x = 0; x < WIDTH; x++) {
            m->t[y][x].x = x; //
            m->t[y][x].y = y; //
            m->t[y][x].val = (!x || !y || x == WIDTH-1 || y == HEIGHT-1)? BOULDER : EMPTY;
            m->ch[y][x] = NULL;

            // up, down, left, right
            m->t[y][x].N = (y)? &m->t[y-1][x] : NULL;
            m->t[y][x].S = (y < HEIGHT-1)? &m->t[y+1][x] : NULL;
            m->t[y][x].W = (x)? &m->t[y][x-1] : NULL;
            m->t[y][x].E = (x < WIDTH-1)? &m->t[y][x+1] : NULL;

            // diagonals
            m->t[y][x].NW = (y && x)? &m->t[y-1][x-1] : NULL;
            m->t[y][x].NE = (y && x < WIDTH-1)? &m->t[y-1][x+1] : NULL;
            m->t[y][x].SW = (y < HEIGHT-1 && x)? &m->t[y+1][x-1] : NULL;
            m->t[y][x].SE = (y < HEIGHT-1 && x < WIDTH-1)? &m->t[y+1][x+1] : NULL;
        }
    }
    
    m->pos_x = pos_x;
    m->pos_y = pos_y;
    m->pN = m->pS = m->pE = m->pW = -1;

    return 0;
}

/*

*/
int seed_map(map *m) {
    int i, x, y, size;
    queue q;
    terrain *current;

    if (!m) return -1;

    queue_init(&q);

    enum terrain_type regions[] = {CLEARING, TALL_GRASS, CLEARING, LAKE, ARCTIC, TALL_GRASS, CLEARING, FOREST, MOUNTAIN};
    int num_regions = sizeof(regions) / sizeof(enum terrain_type);

    for (i = 0; i < num_regions; i++) {
        x = 1 + rand() % (WIDTH - 2);
        y = 1 + rand() % (HEIGHT - 2);

        if (m->t[y][x].val != EMPTY) {
            i--;
            continue;
        }

        m->t[y][x].val = regions[i];
        if (queue_enqueue(&q, x, y)) return -1;
    }

    while (!queue_size(&q, &size) && size) {
        queue_dequeue(&q, &x, &y);
        current = &m->t[y][x];

        if (current->N && current->N->val == EMPTY){
            current->N->val = current->val;
            if (queue_enqueue(&q, x, y-1)) {
                return -1;
            }
        }
        if (current->S && current->S->val == EMPTY){
            current->S->val = current->val;
            if (queue_enqueue(&q, x, y+1)) {
                return -1;
            }
        }
        if (current->W && current->W->val == EMPTY){
            current->W->val = current->val;
            if (queue_enqueue(&q, x-1, y)) {
                return -1;
            }
        }
        if (current->E && current->E->val == EMPTY){
            current->E->val = current->val;
            if (queue_enqueue(&q, x+1, y)) {
                return -1;
            }
        }
    }

    return 0;
}

int place_paths_and_buildings(map *m) {
    int i;
    int x, y, intersection1, intersection2;
    int min, max;
    int d = abs(m->pos_x - 200) + abs(m->pos_y - 200);
    int center = 0;
    int mart = 0;

    if (d == 0) {
        center = mart = 1;
    } else {
        double p;
        if (d == 200) p = 100;
        else if (d < 200) {
            p = ((-45.0 * d) / 200.0 + 50.0) / 100.0;
        } else {
            p = 0.05;
        }

        if ((rand() % 100) < (p * 100)) center = 1;
        if ((rand() % 100) < (p * 100)) mart = 1;
    }


    intersection1 = WIDTH/8 + rand()%(WIDTH - WIDTH/4);

    y = m->pW;
    for (i = 0; i < WIDTH; i++) {
        if (i == intersection1) y = m->pE;
        m->t[y][i].val = PATH;
    }

    min = ((m->pE) < (m->pW) ? (m->pE) : (m->pW));
    max = ((m->pE) > (m->pW) ? (m->pE) : (m->pW));

    for (i = min; i <= max; i++) {
        m->t[i][intersection1].val = PATH;
    }

    intersection2 = HEIGHT/10 + rand()%(HEIGHT - HEIGHT/5);

    x = m->pN;
    for (i = 0; i < HEIGHT; i++) {
        if (i == intersection2) x = m->pS;
        m->t[i][x].val = PATH;
    }

    min = ((m->pN) < (m->pS) ? (m->pN) : (m->pS));
    max = ((m->pN) > (m->pS) ? (m->pN) : (m->pS));

    for (i = min; i <= max; i++) {
        m->t[intersection2][i].val = PATH;
    }

    if (center) place_buildings(m, intersection1, m->pW, POKECENTER, HEIGHT);

    if (mart) place_buildings(m, intersection2, m->pN, POKEMART, WIDTH);

    m->t[m->pW][0].val = m->t[m->pE][WIDTH-1].val = m->t[0][m->pN].val = m->t[HEIGHT-1][m->pS].val = GATE;

    m->WE_intersection = intersection1;
    m->NS_intersection = intersection2;

    return 0;
}

int place_buildings(map *m, int intersection, int path, enum terrain_type VAL, int upperbound) {
    int tmp;
    int x, y;

    do {
        x = 1 + rand()%(intersection-2);

        if (path > upperbound/4) y = path - 2;
        else y = path + 1;

        if (upperbound == WIDTH) {
            tmp = x;
            x = y;
            y = tmp;
        }
    } while (y <= 0 || y >= HEIGHT - 2          || x <= 0 || x >= WIDTH - 2      
            || m->t[y][x].val == PATH       || m->t[y+1][x+1].val == PATH
            || m->t[y][x].val == POKECENTER || m->t[y+1][x+1].val == POKECENTER
            || m->t[y][x].val == POKEMART   || m->t[y+1][x+1].val == POKEMART);

    m->t[y][x].val = m->t[y][x+1].val = m->t[y+1][x].val = m->t[y+1][x+1].val = VAL;

    return 0;
}


/*
 Places the Player Character on the map on the road and connects the given pointer to it;
*/
int place_pc(map *m, character **player) {
    int x;

    if (m->pos_y != 200 || m->pos_x != 200) return -1;

    x = m->WE_intersection + rand()%(WIDTH-2-m->WE_intersection);
    if (!(*player = malloc(sizeof (character)))) return -1;
    if (!((*player)->p = malloc(sizeof (pc)))) return -1;
    (*player)->p->strength = 50;
    
    (*player)->symbol = '@';
    (*player)->npc = NULL;
    (*player)->type = PC;
    (*player)->x = x;
    (*player)->y = m->pE;

    m->ch[m->pE][x] = *player;
    return 0;
}

/*
 Prints the map out if there is no charachter currently
 occupying that square.
*/
int print_map(map *m) {
    int i, j;
    char c;

    for (i = 0; i < HEIGHT; i++) {
        for (j = 0; j < WIDTH; j++) {
            if (m->ch[i][j]) {
                c = m->ch[i][j]->symbol;
            }
            else {
                switch (m->t[i][j].val) {
                    case GATE:
                        c = '#';
                        break;
                    case PATH:
                        c = '#';
                        break;
                    case CLEARING:
                        c = '.';
                        break;
                    case TALL_GRASS:
                        c = ':';
                        break;
                    case ARCTIC:
                        c = '*';
                        break;
                    case LAKE: 
                        c = '~';
                        break;
                    case FOREST:
                        c = '^';
                        break;
                    case MOUNTAIN:
                        c = '%';
                        break;
                    case POKECENTER:
                        c = 'C';
                        break;
                    case POKEMART:
                        c = 'M';
                        break;
                    case BOULDER:
                        c = '%';
                        break;
                    default:
                        c = ' ';
                        break;
                }
            }
            printf("%c", c);
        }
        printf("\n");
    }

    printf("position: (%i, %i)\n", (m->pos_x - 200), (m->pos_y - 200));

    return 0;
}