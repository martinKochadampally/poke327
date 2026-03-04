#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "queue.h"
#include "heap.h"


                      //  GATE,       PATH,       SGRASS,     TGRASS,     ARCTIC,     LAKE,       FOREST,     MNTN,       PCNTR,      PMART,      BOULDER
int cost_table[5][11] = {{10         ,10         ,10         ,20         ,25         ,__INT_MAX__,__INT_MAX__,__INT_MAX__,10         ,10         ,__INT_MAX__}, //PC
                         {__INT_MAX__,10         ,10         ,15         ,20         ,__INT_MAX__,15         ,15         ,50         ,50         ,__INT_MAX__}, //HIKER
                         {__INT_MAX__,10         ,10         ,20         ,25         ,__INT_MAX__,__INT_MAX__,__INT_MAX__,50         ,50         ,__INT_MAX__}, //RIVAL
                         {__INT_MAX__,__INT_MAX__,__INT_MAX__,__INT_MAX__,__INT_MAX__,7          ,__INT_MAX__,__INT_MAX__,__INT_MAX__,__INT_MAX__,__INT_MAX__}, //SWIMMER
                         {__INT_MAX__,10         ,10         ,20         ,25         ,__INT_MAX__,__INT_MAX__,__INT_MAX__,50         ,50         ,__INT_MAX__}};//OTHER

void print_dist_map(int dist_map[HEIGHT][WIDTH]);
int dijiksra(int dist_map[HEIGHT][WIDTH], map *m, terrain *pc_pos, enum char_type npc);

/*
    % - immovable boulders and mountainous regions.
    ^ - forest.
    : - long grass.
    . - clearings.
    ~ - lake.
    * - artic.
    # - roads.
    Cs - are Pokemon Centers.
    Ms - are Pokemarts.
    @ - Player Character
*/


int main(int argc, char *argv[]) {
    world *w;
    queue visited;
    character *player = NULL;
    int NUM_NPCS = 10;
    int y, x;
    char *errmsg;
    // char input[20];
    // int new_x_pos, new_y_pos;

    if (argc == 3 || argc == 1) {
        if (argc == 3) {
            if (!strcmp("--numtrainers", argv[1])) {
                NUM_NPCS = atoi(argv[2]);
                if (strcmp(argv[2], "0") && NUM_NPCS == 0) {
                    errmsg = "The value of numtrainers should be an Integer.";
                    goto ret_err;
                }
                if (NUM_NPCS < 0) { 
                    errmsg = "The value of numtrainers should be greater than 0.";
                    goto ret_err;
                }
            }
        }
    } else {
        errmsg = "Invalid argument(s). Should be in the form: \n './poke327' or './poke327 --numtrainers <Positive Integer>'";
        goto ret_err;
    }

    if (!( w = malloc(sizeof(world)))) return -1;
    if (world_init(w)) return -1;

    queue_init(&visited);

    srand(time(NULL));
    
    // input[0] = 'f';
    // new_x_pos = 0;
    // new_y_pos = 0;

    y = x = 200;
    
    // while (input[0] != 'q') {
    //     if (input[0] == 'f' ) {
    //         x = new_x_pos + 200;
    //         y = new_y_pos + 200;
    //     } 
    //     else if (input[0] == 'n' && y > 0) y--;
    //     else if (input[0] == 's' && y < 400) y++;
    //     else if (input[0] == 'e' && x < 400) x++;
    //     else if (input[0] == 'w' && x > 0) x--;
    //     else {
    //         printf("Invalid command or edge of w reached!\n");
    //     }

        if (!w->maps[y][x]) {
            
            w->maps[y][x] = malloc(sizeof (map));

            if (!w->maps[y][x]) {
                fprintf(stderr, "Malloc failed.");
                return -1;
            }

            if (queue_enqueue(&visited, x, y)) {
                return -1;
            }

            init_map(w->maps[y][x], x, y);

            // Check north.
            if (y > 0 && w->maps[y - 1][x] != NULL) {
                w->maps[y][x]->pN = w->maps[y - 1][x]->pS;
            } else {
                w->maps[y][x]->pN = 4 + rand() % (WIDTH - 8);
            }

            // Check south.
            if (y < 400 && w->maps[y + 1][x] != NULL) {
                w->maps[y][x]->pS = w->maps[y + 1][x]->pN;
            } else {
                w->maps[y][x]->pS = 4 + rand() % (WIDTH - 8);
            }

            // Check west.
            if (x > 0 && w->maps[y][x - 1] != NULL) {
                w->maps[y][x]->pW = w->maps[y][x - 1]->pE;
            } else {
                w->maps[y][x]->pW = 4 + rand() % (HEIGHT - 8);
            }

            // Check East.
            if (x < 400 && w->maps[y][x + 1] != NULL) {
                w->maps[y][x]->pE = w->maps[y][x + 1]->pW;
            } else {
                w->maps[y][x]->pE = 4 + rand() % (HEIGHT - 8);
            }

            if (seed_map(w->maps[y][x])) {
                fprintf(stderr, "Seeding failed.");
                return -1;
            }
            
            place_paths_and_buildings(w->maps[y][x]);
            if (place_pc(w->maps[y][x], &player)) {
                fprintf(stderr, "place_pc failed\n");
                return -1;
            }
        }

        print_map(w->maps[y][x]);
        dijiksra(w->hiker_dist_map, w->maps[y][x], &w->maps[y][x]->t[player->y][player->x], HIKER);
        dijiksra(w->rival_dist_map, w->maps[y][x], &w->maps[y][x]->t[player->y][player->x], RIVAL);

    //     printf("Enter a command (n, s, e, w, f <x> <y>, q): ");

    //     if (!fgets(input, sizeof (input), stdin)) {
    //         return -1;
    //     }

    //     if (input[0] == 'f') {
    //         int temp_x, temp_y;
    //         if (sscanf(input, "f %d %d", &temp_x, &temp_y) == 2) {
    //             if (temp_x >= -200 && temp_x <= 200 && temp_y >= -200 && temp_y <= 200) {
    //                 new_x_pos = temp_x;
    //                 new_y_pos = temp_y;
    //             } else {
    //                 printf("Out of bounds! Stay between -200 and 200.\n");
    //             }
    //         }
    //     }
    // }
    
    // printf("Hiker Distance Map:");
    // print_dist_map(w->hiker_dist_map); 
    // printf("Rival Distance Map:");
    // print_dist_map(w->rival_dist_map); 


    while (!queue_dequeue(&visited, &x, &y)) {
        free(w->maps[y][x]);
    }

    queue_destroy(&visited);

    free(player->p);
    free(player);

    world_destroy(w);

    free(w);

    return 0;

    ret_err:
        printf("Error: %s\n", errmsg);
        return -1;
}


/* 
    We are just implementing dijikstra's algorithm here, so that npc 
    can know the shortest path to get to the Player Character. 
*/
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

void print_dist_map(int dist_map[HEIGHT][WIDTH]) {
    int x, y;
    for (y = 0; y < HEIGHT; y++) {
        for (x = 0; x < WIDTH; x++) {
            if (dist_map[y][x] == __INT_MAX__) {
                printf("   ");
            } else {
                printf("%02d ", dist_map[y][x] % 100);
            }
        }
        printf("\n");
    }
}