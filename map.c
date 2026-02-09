#include <stdio.h>
#include <stdlib.h>

#include "queue.h"
#include "map.h"


int init_map(map *m) {    
    int x, y;

    for (y = 0; y < HEIGHT; y++) {
        for (x = 0; x < WIDTH; x++) {
            m->tiles[y][x].val = (!x || !y || x == WIDTH-1 || y == HEIGHT-1)? BOULDER: -1;

            m->tiles[y][x].up = (x)? &m->tiles[y-1][x] : NULL;
            m->tiles[y][x].down = (x < WIDTH-1)? &m->tiles[y+1][x] : NULL;
            m->tiles[y][x].left = (y)? &m->tiles[y][x-1] : NULL;
            m->tiles[y][x].right = (y < HEIGHT-1)? &m->tiles[y][x+1] : NULL;
        }
    }
    
    m->pN = -1;
    m->pS = -1;
    m->pE = -1;
    m->pW = -1;

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

    enum terrain_type regions[] = {CLEARING, TALL_GRASS, CLEARING, LAKE, ARCTIC, TALL_GRASS, LAKE, FOREST, BOULDER};
    int num_regions = 9;

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

    for (i = min; i < max; i++) {
        m->tiles[i][intersection].val = PATH;
    }

    intersection = HEIGHT/10 + rand()%(HEIGHT - HEIGHT/5);

    x = m->pN;
    for (i = 0; i < HEIGHT; i++) {
        if (i == intersection) x = m->pS;
        m->tiles[i][x].val = PATH;
    }

    min = ((m->pN) < (m->pS) ? (m->pN) : (m->pS));
    max = ((m->pN) > (m->pS) ? (m->pN) : (m->pS));

    for (i = min; i < max; i++) {
        m->tiles[intersection][i].val = PATH;
    }

    return 0;
}

int print_map(map *m) {
    int i, j;

    for (i = 0; i < HEIGHT; i++) {
        for (j = 0; j < WIDTH; j++) {
            if (m->tiles[i][j].val == -1)
                printf("%c", ' ');
            else {
                switch (m->tiles[i][j].val) {
                case PATH:
                    printf("%c", '#');
                    break;
                case CLEARING:
                    printf("%c", '.');
                    break;
                case TALL_GRASS:
                    printf("%c", ':');
                    break;
                case ARCTIC:
                    printf("%c", '*');
                    break;
                case LAKE:
                    printf("%c", '~');
                    break;
                case FOREST:
                    printf("%c", '^');
                    break;
                case BOULDER:
                    printf("%c", '%');
                    break;
                default:
                    break;
                }
            }
        }
        printf("\n");
    }

    return 0;
}

