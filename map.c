#include <stdio.h>
#include <stdlib.h>

#include "queue.h"
#include "map.h"


int init_map(map *m) {    
    int x, y;

    for (y = 0; y < HEIGHT; y++) {
        for (x = 0; x < WIDTH; x++) {
            m->tiles[y][x].val = (!x || !y || x == WIDTH-1 || y == HEIGHT-1)? BOULDER: -1;

            m->tiles[y][x].up = (y)? &m->tiles[y-1][x] : NULL;
            m->tiles[y][x].down = (y < HEIGHT-1)? &m->tiles[y+1][x] : NULL;
            m->tiles[y][x].left = (x)? &m->tiles[y][x-1] : NULL;
            m->tiles[y][x].right = (x < WIDTH-1)? &m->tiles[y][x+1] : NULL;
        }
    }
    
    m->pN = m->pS = m->pE = m->pW = -1;

    m->is_initialized = 0;

    return 0;
}

int seed_map(map *m) {
    int i, x, y, size;
    queue q;
    tile *current;

    if (m->is_initialized){
        return -1;
    }

    queue_init(&q);

    enum terrain_type regions[] = {CLEARING, TALL_GRASS, CLEARING, LAKE, ARCTIC, TALL_GRASS, LAKE, CLEARING, BOULDER};
    int num_regions = sizeof(regions) / sizeof(enum terrain_type);

    for (i = 0; i < num_regions; i++) {
        x = 1 + rand() % (WIDTH - 2);
        y = 1 + rand() % (HEIGHT - 2);

        if (m->tiles[y][x].val != -1) {
            i--;
            continue;
        }

        m->tiles[y][x].val = regions[i];
        if (queue_enqueue(&q, x, y)) return -1;
    }

    while (!queue_size(&q, &size) && size) {
        queue_dequeue(&q, &x, &y);
        current = &m->tiles[y][x];

        if (current->up && current->up->val == -1){
            current->up->val = current->val;
            if (queue_enqueue(&q, x, y-1)) {
                return -1;
            }
        }
        if (current->down && current->down->val == -1){
            current->down->val = current->val;
            if (queue_enqueue(&q, x, y+1)) {
                return -1;
            }
        }
        if (current->left && current->left->val == -1){
            current->left->val = current->val;
            if (queue_enqueue(&q, x-1, y)) {
                return -1;
            }
        }
        if (current->right && current->right->val == -1){
            current->right->val = current->val;
            if (queue_enqueue(&q, x+1, y)) {
                return -1;
            }
        }
    }

    return 0;
}

int place_paths(map *m) {
    int i;
    int x, y, intersection;
    int min, max;

    intersection = WIDTH/8 + rand()%(WIDTH - WIDTH/4);

    y = m->pE;
    for (i = 0; i < WIDTH; i++) {
        if (i == intersection) y = m->pW;
        m->tiles[y][i].val = PATH;
    }

    min = ((m->pE) < (m->pW) ? (m->pE) : (m->pW));
    max = ((m->pE) > (m->pW) ? (m->pE) : (m->pW));

    for (i = min; i <= max; i++) {
        m->tiles[i][intersection].val = PATH;
    }

    place_buildings(m, intersection, m->pE, POKECENTER, HEIGHT);

    intersection = HEIGHT/10 + rand()%(HEIGHT - HEIGHT/5);

    x = m->pN;
    for (i = 0; i < HEIGHT; i++) {
        if (i == intersection) x = m->pS;
        m->tiles[i][x].val = PATH;
    }

    min = ((m->pN) < (m->pS) ? (m->pN) : (m->pS));
    max = ((m->pN) > (m->pS) ? (m->pN) : (m->pS));

    for (i = min; i <= max; i++) {
        m->tiles[intersection][i].val = PATH;
    }

    place_buildings(m, intersection, m->pN, POKEMART, WIDTH);

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
    } while ( y <= 0 || y >= HEIGHT - 2 || x <= 0 || x >= WIDTH - 2 ||
        m->tiles[y][x].val == PATH || m->tiles[y+1][x+1].val == PATH || 
        m->tiles[y][x].val == POKECENTER || m->tiles[y+1][x+1].val == POKECENTER);

    m->tiles[y][x].val = m->tiles[y][x+1].val = m->tiles[y+1][x].val = m->tiles[y+1][x+1].val = VAL;

    return 0;
}

int print_map(map *m) {
    int i, j;
    char c;

    for (i = 0; i < HEIGHT; i++) {
        for (j = 0; j < WIDTH; j++) {
            if (m->tiles[i][j].val == -1)
                printf("%c", ' ');
            else {
                switch (m->tiles[i][j].val) {
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
                case BOULDER:
                    c = '%';
                    break;
                case POKECENTER:
                    c = 'C';
                    break;
                case POKEMART:
                    c = 'M';
                    break;
                }
            }
            printf("%c", c);
        }
        printf("\n");
    }

    return 0;
}

