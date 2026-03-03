#include <string.h>

#include "heap.h"
#include "dijikstras.h"

                      //  GATE,       PATH,       SGRASS,     TGRASS,     ARCTIC,     LAKE,       FOREST,     MNTN,       PCNTR,      PMART,      BOULDER
int cost_table[5][11] = {{10         ,10         ,10         ,20         ,25         ,__INT_MAX__,__INT_MAX__,__INT_MAX__,10         ,10         ,__INT_MAX__}, //PC
                         {__INT_MAX__,10         ,10         ,15         ,20         ,__INT_MAX__,15         ,15         ,50         ,50         ,__INT_MAX__}, //HIKER
                         {__INT_MAX__,10         ,10         ,20         ,25         ,__INT_MAX__,__INT_MAX__,__INT_MAX__,50         ,50         ,__INT_MAX__}, //RIVAL
                         {__INT_MAX__,__INT_MAX__,__INT_MAX__,__INT_MAX__,__INT_MAX__,7          ,__INT_MAX__,__INT_MAX__,__INT_MAX__,__INT_MAX__,__INT_MAX__}, //SWIMMER
                         {__INT_MAX__,10         ,10         ,20         ,25         ,__INT_MAX__,__INT_MAX__,__INT_MAX__,50         ,50         ,__INT_MAX__}};//OTHER


/* We are just implementing dijikstra's algorithm here, so that npc 
know the shortest path to get to the Player Character. */
int dijiksra(int dist_map[HEIGHT][WIDTH], map *m, terrain *pc_pos, enum char_type npc) {
    heap h;
    terrain *t;
    int y, x;
    int size;
    npc %= 10;
    int cost, new_cost;

    heap_init(&h);

    // Sets all squares to a weight of infinity and adds the
    // player's square to the map.
    for (y = 0; y < HEIGHT; y++) {
        for (x = 0; x < WIDTH; x++) {
            if ((pc_pos)->x == x && (pc_pos)->y == y) {
                heap_insert(&h, &m->t[y][x], 0);
                dist_map[y][x] =  0;
                continue;
            }
            dist_map[y][x] =  __INT_MAX__;
        }
    }

    // Extracts the current min and adds its neighbours to the heap, or 
    // decreases the key of the node if it is already in the heap.
    while (!heap_size(&h, &size) && size) {
        heap_extract_min(&h, &t, &cost);

        y = t->y;
        x = t->x;

        if (t->NW && cost_table[npc][t->NW->val] != __INT_MAX__) {
            new_cost = cost + cost_table[npc][t->NW->val];
            if (new_cost < dist_map[y-1][x-1]) {
                dist_map[y-1][x-1] = new_cost;
                if (is_in_heap(&h, t->NW)) {
                    decrease_key(&h, t->NW, new_cost);
                } else {
                    heap_insert(&h, t->NW, new_cost);
                }
            }
        }
        if (t->N && cost_table[npc][t->N->val] != __INT_MAX__) {
            new_cost = cost + cost_table[npc][t->N->val];
            if (new_cost < dist_map[y-1][x]) {
                dist_map[y-1][x] = new_cost;
                if (is_in_heap(&h, t->N)) {
                    decrease_key(&h, t->N, new_cost);
                } else {
                    heap_insert(&h, t->N, new_cost);
                }
            }
        }
        if (t->NE && cost_table[npc][t->NE->val] != __INT_MAX__) {
            new_cost = cost + cost_table[npc][t->NE->val];
            if (new_cost < dist_map[y-1][x+1]) {
                dist_map[y-1][x+1] = new_cost;
                if (is_in_heap(&h, t->NE)) {
                    decrease_key(&h, t->NE, new_cost);
                } else {
                    heap_insert(&h, t->NE, new_cost);
                }
            }
        }
        if (t->E && cost_table[npc][t->E->val] != __INT_MAX__) {
            new_cost = cost + cost_table[npc][t->E->val];
            if (new_cost < dist_map[y][x+1]) {
                dist_map[y][x+1] = new_cost;
                if (is_in_heap(&h, t->E)) {
                    decrease_key(&h, t->E, new_cost);
                } else {
                    heap_insert(&h, t->E, new_cost);
                }
            }
        }
        if (t->SE && cost_table[npc][t->SE->val] != __INT_MAX__) {
            new_cost = cost + cost_table[npc][t->SE->val];
            if (new_cost < dist_map[y+1][x+1]) {
                dist_map[y+1][x+1] = new_cost;
                if (is_in_heap(&h, t->SE)) {
                    decrease_key(&h, t->SE, new_cost);
                } else {
                    heap_insert(&h, t->SE, new_cost);
                }
            }
        }
        if (t->S && cost_table[npc][t->S->val] != __INT_MAX__) {
            new_cost = cost + cost_table[npc][t->S->val];
            if (new_cost < dist_map[y+1][x]) {
                dist_map[y+1][x] = new_cost;
                if (is_in_heap(&h, t->S)) {
                    decrease_key(&h, t->S, new_cost);
                } else {
                    heap_insert(&h, t->S, new_cost);
                }
            }
        }
        if (t->SW && cost_table[npc][t->SW->val] != __INT_MAX__) {
            new_cost = cost + cost_table[npc][t->SW->val];
            if (new_cost < dist_map[y+1][x-1]) {
                dist_map[y+1][x-1] = new_cost;
                if (is_in_heap(&h, t->SW)) {
                    decrease_key(&h, t->SW, new_cost);
                } else {
                    heap_insert(&h, t->SW, new_cost);
                }
            }
        }
        if (t->W && cost_table[npc][t->W->val] != __INT_MAX__) {
            new_cost = cost + cost_table[npc][t->W->val];
            if (new_cost < dist_map[y][x-1]) {
                dist_map[y][x-1] = new_cost;
                if (is_in_heap(&h, t->W)) {
                    decrease_key(&h, t->W, new_cost);
                } else {
                    heap_insert(&h, t->W, new_cost);
                }
            }
        }
    }

    // Recursively destroys the heap.
    heap_destroy(&h);

    return 0;
}
