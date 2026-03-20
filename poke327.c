#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "queue.h"
#include "heap.h"


                      //  GATE,       PATH,       SGRASS,     TGRASS,     ARCTIC,     LAKE,       FOREST,     MNTN,       PCNTR,      PMART,      BOULDER
int cost_table[5][11] = {{10         ,10         ,10         ,20         ,25         ,__INT_MAX__,__INT_MAX__,__INT_MAX__,10         ,10         ,__INT_MAX__}, //PC
                         {__INT_MAX__,10         ,10         ,15         ,20         ,__INT_MAX__,15         ,15         ,50         ,50         ,__INT_MAX__}, //HIKER
                         {__INT_MAX__,10         ,10         ,20         ,25         ,__INT_MAX__,__INT_MAX__,__INT_MAX__,50         ,50         ,__INT_MAX__}, //RIVAL
                         {__INT_MAX__,__INT_MAX__,__INT_MAX__,__INT_MAX__,__INT_MAX__,7          ,__INT_MAX__,__INT_MAX__,__INT_MAX__,__INT_MAX__,__INT_MAX__}, //SWIMMER
                         {__INT_MAX__,10         ,10         ,20         ,25         ,__INT_MAX__,__INT_MAX__,__INT_MAX__,50         ,50         ,__INT_MAX__}};//OTHER


int place_npcs(map *m, int num_trainers, heap *h);
void init_terminal(void);
void output_map(map *m);
void move_npc(map *m, character *c, world *w);
int dijiksra(int dist_map[HEIGHT][WIDTH], map *m, terrain *pc_pos, enum char_type npc);
void print_dist_map(int dist_map[HEIGHT][WIDTH]);


/*
    This is where the user interface adn things related to it will be implemented.
        Takes arg: --numtrainers
*/
int main(int argc, char *argv[]) {
    int retval = 0;             // This is what main will return.
    char *errmsg = NULL;        // If an error occurs, this stores error message.

    // Holds --numtrainers value:
    int NUM_NPCS = 10;
    
    // World Traversing Vars:
    world *w;                   // Stores out world.
    queue visited;              // Stores all the maps that we have visited, so that we can free them.

    // User Input Variable:
    char *input = NULL;
 
    // Movement:
    // map *m;
    int y, x;

    // Character Movement Vars:
    heap char_heap;             // Stores all the characters in the current map.
    character *player = NULL;   // Pointer to the Player Character.
    // character *c;
    // int current_time;
    // int new_y, new_x, dy, dx;
    
    // Checks for the -numtrainers tag.
    if (argc == 3 || argc == 1) {
        if (argc == 3) {
            if (!strcmp("--numtrainers", argv[1])) {
                NUM_NPCS = atoi(argv[2]);
                if (NUM_NPCS < 1) { 
                    errmsg = "The value of numtrainers should be a positive integer greater than 0.";
                    retval = -1;
                    goto ret_err;
                }
            }
        }
    } else {
        errmsg = "Invalid argument(s). Should be in the form: \n './poke327' or './poke327 --numtrainers <Positive Integer>'";
        retval = -2;
        goto ret_err;
    }

    // Creates the world.
    if ( !(w = malloc(sizeof (world) )) ) {
        errmsg = "Mallocing world failed.";
        retval = -3;
        goto ret_err;
    }

    // Initializing the world.
    if (world_init(w)) {
        errmsg = "Initializing world faild.";
        retval = -4;
        goto free_world;
    }

    // Creates a queue;
    queue_init(&visited);
    // Initializes a heap to keep track of whos turn it is.
    heap_init(&char_heap);

    srand(time(NULL));

    // Sets the current world to (0,0).
    y = x = 200;

    // Mallocing space for the map.
    if (!(w->maps[y][x] = malloc(sizeof (map)))) {
        errmsg = "Mallocing a map failed.";
        retval = -5;
        goto free_visited_maps;
    }

    // Add the map to the queue so we know which worlds we have visited.
    if (queue_enqueue(&visited, x, y)) {
        errmsg = "Couldn't mark map as visited.";
        retval = -6;
        goto free_visited_maps;
    }
    
    // Initiate the map and its gates depending on the srroundoing maps.
    init_map(w->maps[y][x], x, y, w->maps[y-1][x], w->maps[y+1][x], w->maps[y][x+1], w->maps[y][x-1]);

    // Seeding the terrain of the map.
    if (seed_map(w->maps[y][x])) {
        errmsg = "Seeding failed.";
        retval = -7;
        goto free_visited_maps;
    }
    
    // Placing the paths and buildings on the map.
    place_paths_and_buildings(w->maps[y][x]);
    if (place_pc(w->maps[y][x], &player)) {
        errmsg = "place_pc failed\n";
        retval = -8;
        goto free_player;
    }

    // Randomly placing NPCs on the map.
    place_npcs(w->maps[y][x], NUM_NPCS, &char_heap);

    // Adding each character to a heap keep track of turns.
    heap_insert(&char_heap, player, player->next_turn);

    if (!(input = malloc(10))) {
        errmsg = "Mallocing space for input failed.";
        retval = -9;
        goto destroy_heap;
    }

    // Using NCURSES lib, the following will output PC and NPC movements.
    init_terminal(); // don't use printf until closing ncurses.

    do {
        // Clears text from previous run.
        clear();

        switch (*input)
        {
        case "y":
            /* code */
            break;
        
        default:
            break;
        }

        // Prints out the updated map.
        output_map(w->maps[y][x]);

        // Fetches input.
        getnstr(input, 10);

        // Updates new changes onto the screen.
        refresh();
    } while (strcmp("q", input));

    // Setting error message to null so that a false error doesn't get outputed.
    errmsg = NULL;
    endwin();

    free(input);

    // Clean Up Code:
    destroy_heap:
        heap_destroy(&char_heap);

    free_player:
        free(player->p);
        free(player);

    free_visited_maps:
        while (!queue_dequeue(&visited, &x, &y)) {
            free(w->maps[y][x]);
        }
    // destroy_queue:
        queue_destroy(&visited);
    // destroy_world:
        world_destroy(w);

    free_world:
        free(w);

    ret_err:
        if (errmsg) {
            fprintf(stderr, "Error: %s\n", errmsg);
        }
        return retval;
}


    // while (1) {
    //     heap_extract_min(&char_heap, (void **)&c, &current_time);

    //     if (c->p) {
    //         dx = (rand() % 3) - 1;
    //         dy = (rand() % 3) - 1;
    //         new_x = c->x + dx;
    //         new_y = c->y + dy;

    //         if (new_y > 0 && new_y < HEIGHT - 1 && new_x > 0 && new_x < WIDTH - 1 && 
    //             cost_table[PC%10][w->maps[y][x]->t[new_y][new_x].val] != __INT_MAX__ &&
    //             w->maps[y][x]->t[new_y][new_x].val != GATE && !w->maps[y][x]->ch[new_y][new_x]) {
    //             w->maps[y][x]->ch[c->y][c->x] = NULL;
    //             c->x = new_x; 
    //             c->y = new_y;
    //             w->maps[y][x]->ch[c->y][c->x] = c;
    //         }

    //         dijiksra(w->hiker_dist_map, w->maps[y][x], &w->maps[y][x]->t[c->y][c->x], HIKER);
    //         dijiksra(w->rival_dist_map, w->maps[y][x], &w->maps[y][x]->t[c->y][c->x], RIVAL);

    //         print_map(w->maps[y][x]);
    //         printf("Current time: %d\n", current_time);
    //         usleep(250000);

    //     } else {
    //         move_npc( w->maps[y][x], c, w);
    //     }

    //     int cost = cost_table[c->type % 10][w->maps[y][x]->t[c->y][c->x].val];
    //     c->next_turn = current_time + cost;
    //     if ( !heap_insert(&char_heap, c, c->next_turn) ) {
    //         errmsg = "Adding to character heap failed.";
    //         retval = -10;
    //         goto destroy_heap;
    //     }
    // }


/*
    Places NPCs
*/
int place_npcs(map *m, int num_trainers, heap *h) {
    int i;
    for (i = 0; i < num_trainers; i++) {
        character *c = malloc(sizeof(*c));
        if (!c) return -1;

        c->seq_num = i + 1;
        c->next_turn = 0;

        if (i == 0) {
            c->type = HIKER;
            c->symbol = 'h';
        } else if (i == 1) {
            c->type = RIVAL;
            c->symbol = 'r';
        } else {
            int r = rand() % 6;
            switch (r) {
                case 0: c->type = HIKER;     c->symbol = 'h'; break;
                case 1: c->type = RIVAL;     c->symbol = 'r'; break;
                case 2: c->type = PACER;     c->symbol = 'p'; break;
                case 3: c->type = WANDERER;  c->symbol = 'w'; break;
                case 4: c->type = SENTRY;    c->symbol = 's'; break;
                case 5: c->type = EXPLORER;  c->symbol = 'e'; break;
            }
        }

        int x, y;
        do {
            x = 1 + rand() % (WIDTH - 2);
            y = 1 + rand() % (HEIGHT - 2);
        } while (m->ch[y][x] || m->t[y][x].val == GATE ||
                 cost_table[c->type % 10][m->t[y][x].val] == __INT_MAX__);

        c->x = x;
        c->y = y;
        m->ch[y][x] = c;

        if (c->type == PACER || c->type == WANDERER) {
            if (rand() % 2) {
                c->dir[0] = (rand() % 2) ? 1 : -1;
                c->dir[1] = 0;
            } else {
                c->dir[0] = 0;
                c->dir[1] = (rand() % 2) ? 1 : -1;
            }
        } else {
            c->dir[0] = (rand() % 3) - 1;
            c->dir[1] = (rand() % 3) - 1;
        }

        heap_insert(h, c, c->next_turn);
    }
    return 0;
}

void init_terminal(void)
{
  initscr();
  raw();
  noecho();
  curs_set(0);
  keypad(stdscr, TRUE);
}

void output_map(map *m) {
    int y, x;
    char c;

    for (y = 0; y < HEIGHT; y++) {
        for (x = 0; x < WIDTH; x++) {
            if (m->ch[y][x]) {
                c = m->ch[y][x]->symbol;
            }
            else {
                switch (m->t[y][x].val) {
                    case GATE: c = '#'; 
                        break;
                    case PATH: c = '#';
                        break;
                    case CLEARING:c = '.';
                        break;
                    case TALL_GRASS:c = ':';
                        break;
                    case ARCTIC:c = '*';    
                        break;
                    case LAKE: c = '~';
                        break;
                    case FOREST: c = '^';
                        break;
                    case MOUNTAIN: c = '%'; 
                        break;
                    case POKECENTER: c = 'C'; 
                        break;
                    case POKEMART: c = 'M';
                        break;
                    case BOULDER: c = '%';
                        break;
                    default: c = ' ';
                        break;
                }
            }
            mvprintw(y + 1, x, "%c", c);
        }
    }
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

    heap_item *ref_table[HEIGHT][WIDTH] = {{NULL}};

    heap_init(&h);

    // Sets all squares to a weight of infinity and adds the
    // player's square to the map.
    for (y = 0; y < HEIGHT; y++) {
        for (x = 0; x < WIDTH; x++) {
            if ((pc_pos)->x == x && (pc_pos)->y == y) {
                if ( !(ref_table[y][x] = heap_insert(&h, &m->t[y][x], 0)) ){
                    return -1;
                }
                dist_map[y][x] =  0;
                continue;
            }
            dist_map[y][x] =  __INT_MAX__;
        }
    }

    // Extracts the current min and adds its neighbours to the heap, or 
    // decreases the key of the node if it is already in the heap.
    while (!heap_size(&h, &size) && size) {
        heap_extract_min(&h, (void **)&t, &cost);

        y = t->y;
        x = t->x;

        if (t->NW && cost_table[npc][t->NW->val] != __INT_MAX__) {
            new_cost = cost + cost_table[npc][t->NW->val];
            if (new_cost < dist_map[y-1][x-1]) {
                dist_map[y-1][x-1] = new_cost;
                if (ref_table[y-1][x-1]) {
                    decrease_key(&h, ref_table[y-1][x-1], new_cost);
                } else {
                    if (!(ref_table[y-1][x-1] = heap_insert(&h, t->NW, new_cost))) return -1;
                }
            }
        }
        if (t->N && cost_table[npc][t->N->val] != __INT_MAX__) {
            new_cost = cost + cost_table[npc][t->N->val];
            if (new_cost < dist_map[y-1][x]) {
                dist_map[y-1][x] = new_cost;
                if (ref_table[y-1][x]) {
                    decrease_key(&h, ref_table[y-1][x], new_cost);
                } else {
                    if (!(ref_table[y-1][x] = heap_insert(&h, t->N, new_cost))) return -1;
                }
            }
        }
        if (t->NE && cost_table[npc][t->NE->val] != __INT_MAX__) {
            new_cost = cost + cost_table[npc][t->NE->val];
            if (new_cost < dist_map[y-1][x+1]) {
                dist_map[y-1][x+1] = new_cost;
                if (ref_table[y-1][x+1]) {
                    decrease_key(&h, ref_table[y-1][x+1], new_cost);
                } else {
                    if (!(ref_table[y-1][x+1] = heap_insert(&h, t->NE, new_cost))) return -1;
                }
            }
        }
        if (t->E && cost_table[npc][t->E->val] != __INT_MAX__) {
            new_cost = cost + cost_table[npc][t->E->val];
            if (new_cost < dist_map[y][x+1]) {
                dist_map[y][x+1] = new_cost;
                if (ref_table[y][x+1]) {
                    decrease_key(&h, ref_table[y][x+1], new_cost);
                } else {
                    if (!(ref_table[y][x+1] = heap_insert(&h, t->E, new_cost))) return -1;
                }
            }
        }
        if (t->SE && cost_table[npc][t->SE->val] != __INT_MAX__) {
            new_cost = cost + cost_table[npc][t->SE->val];
            if (new_cost < dist_map[y+1][x+1]) {
                dist_map[y+1][x+1] = new_cost;
                if (ref_table[y+1][x+1]) {
                    decrease_key(&h, ref_table[y+1][x+1], new_cost);
                } else {
                    if (!(ref_table[y+1][x+1] = heap_insert(&h, t->SE, new_cost))) return -1;
                }
            }
        }
        if (t->S && cost_table[npc][t->S->val] != __INT_MAX__) {
            new_cost = cost + cost_table[npc][t->S->val];
            if (new_cost < dist_map[y+1][x]) {
                dist_map[y+1][x] = new_cost;
                if (ref_table[y+1][x]) {
                    decrease_key(&h, ref_table[y+1][x], new_cost);
                } else {
                    if (!(ref_table[y+1][x] = heap_insert(&h, t->S, new_cost))) return -1;
                }
            }
        }
        if (t->SW && cost_table[npc][t->SW->val] != __INT_MAX__) {
            new_cost = cost + cost_table[npc][t->SW->val];
            if (new_cost < dist_map[y+1][x-1]) {
                dist_map[y+1][x-1] = new_cost;
                if (ref_table[y+1][x-1]) {
                    decrease_key(&h, ref_table[y+1][x-1], new_cost);
                } else {
                    if (!(ref_table[y+1][x-1] = heap_insert(&h, t->SW, new_cost))) return -1;
                }
            }
        }
        if (t->W && cost_table[npc][t->W->val] != __INT_MAX__) {
            new_cost = cost + cost_table[npc][t->W->val];
            if (new_cost < dist_map[y][x-1]) {
                dist_map[y][x-1] = new_cost;
                if (ref_table[y][x-1]) {
                    decrease_key(&h, ref_table[y][x-1], new_cost);
                } else {
                    if (!(ref_table[y][x-1] = heap_insert(&h, t->W, new_cost))) return -1;
                }
            }
        }
    }

    // Recursively destroys the heap.
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            ref_table[i][j] = NULL;
        } 
    }

    heap_destroy(&h);

    return 0;
}

void move_npc(map *m, character *c, world *w) {
    int dy, dx, new_y, new_x;
    int next_x = c->x;
    int next_y = c->y;
    int min_dist = __INT_MAX__;

    if (c->type == SENTRY) return;

    if (c->type == HIKER || c->type == RIVAL) {
        // Get dist maps.
        int (*dist_map)[WIDTH] = (c->type == HIKER) ? w->hiker_dist_map : w->rival_dist_map;
        
        for (dy = -1; dy <= 1; dy++) {
            for (dx = -1; dx <= 1; dx++) {
                if (dx == 0 && dy == 0) continue;
                new_y = c->y + dy;
                new_x = c->x + dx;
                
                if (dist_map[new_y][new_x] < min_dist) {
                    if (!m->ch[new_y][new_x]) {
                        min_dist = dist_map[new_y][new_x];
                        next_x = new_x;
                        next_y = new_y;
                    }
                }
            }
        }
    } 
    else if (c->type == PACER) {
        new_x = c->x + c->dir[0];
        new_y = c->y + c->dir[1];
        
        if (m->ch[new_y][new_x] || cost_table[OTHER%10][m->t[new_y][new_x].val] == __INT_MAX__ || m->t[new_y][new_x].val == GATE) {
            c->dir[0] *= -1; 
            c->dir[1] *= -1; 
        }
        next_x = c->x + c->dir[0];
        next_y = c->y + c->dir[1];
    }
    else if (c->type == WANDERER) {
        new_x = c->x + c->dir[0];
        new_y = c->y + c->dir[1];
        
        if (m->t[new_y][new_x].val != m->t[c->y][c->x].val || m->ch[new_y][new_x] || m->t[new_y][new_x].val == GATE) {
            if (rand() % 2) {
                c->dir[0] = (rand() % 2) ? 1 : -1; c->dir[1] = 0;
            } else {
                c->dir[0] = 0; c->dir[1] = (rand() % 2) ? 1 : -1;
            }
        } else {
            next_x = new_x;
            next_y = new_y;
        }
    }
    else if (c->type == EXPLORER) {
        new_x = c->x + c->dir[0];
        new_y = c->y + c->dir[1];
        
        if (cost_table[OTHER%10][m->t[new_y][new_x].val] == __INT_MAX__ || m->ch[new_y][new_x] || m->t[new_y][new_x].val == GATE) {
            c->dir[0] = (rand() % 3) - 1;
            c->dir[1] = (rand() % 3) - 1;
            if (c->dir[0] == 0 && c->dir[1] == 0) c->dir[0] = 1;
        } else {
            next_x = new_x;
            next_y = new_y;
        }
    }


    if (!m->ch[next_y][next_x]) {
        m->ch[c->y][c->x] = NULL;
        c->x = next_x;
        c->y = next_y;
        m->ch[c->y][c->x] = c;
    }
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



// int main(int argc, char *argv[]) {
//     world *w;
//     queue visited;
//     character *player = NULL;
//     int NUM_NPCS = 10;
//     int y, x;
//     int retval = 0;
//     char *errmsg = NULL;
//     // char input[20];
//     // int new_x_pos, new_y_pos;
//     if (argc == 3 || argc == 1) {
//         if (argc == 3) {
//             if (!strcmp("--numtrainers", argv[1])) {
//                 NUM_NPCS = atoi(argv[2]);
//                 if (NUM_NPCS < 1) { 
//                     errmsg = "The value of numtrainers should be a positive integer greater than 0.";
//                     retval = -1;
//                     goto ret_err;
//                 }
//             }
//         }
//     } else {
//         errmsg = "Invalid argument(s). Should be in the form: \n './poke327' or './poke327 --numtrainers <Positive Integer>'";
//         retval = -2;
//         goto ret_err;
//     }

//     if ( !(w = malloc(sizeof (world) )) ) {
//         errmsg = "Mallocing world failed.";
//         retval = -3;
//         goto ret_err;
//     }
//     if (world_init(w)) {
//         errmsg = "Initializing world faild.";
//         retval = -5;
//         goto free_world;
//     }

//     queue_init(&visited);

//     srand(time(NULL));
    
//     // input[0] = 'f';
//     // new_x_pos = 0;
//     // new_y_pos = 0;

//     y = x = 200;
    
//     // while (input[0] != 'q') {
//     //     if (input[0] == 'f' ) {
//     //         x = new_x_pos + 200;
//     //         y = new_y_pos + 200;
//     //     } 
//     //     else if (input[0] == 'n' && y > 0) y--;
//     //     else if (input[0] == 's' && y < 400) y++;
//     //     else if (input[0] == 'e' && x < 400) x++;
//     //     else if (input[0] == 'w' && x > 0) x--;
//     //     else {
//     //         printf("Invalid command or edge of w reached!\n");
//     //     }

//         if (!w->maps[y][x]) {
            
//             w->maps[y][x] = malloc(sizeof (map));

//             if (!w->maps[y][x]) {
//                 errmsg = "Mallocing a map failed.";
//                 retval = -6;
//                 goto free_visited_maps;
//             }

//             if (queue_enqueue(&visited, x, y)) {
//                 errmsg = "Couldn't mark map as visited.";
//                 retval = -7;
//                 goto free_visited_maps;
//             }

//             init_map(w->maps[y][x], x, y);

//             // Check north.
//             if (y > 0 && w->maps[y - 1][x] != NULL) {
//                 w->maps[y][x]->pN = w->maps[y - 1][x]->pS;
//             } else {
//                 w->maps[y][x]->pN = 4 + rand() % (WIDTH - 8);
//             }

//             // Check south.
//             if (y < 400 && w->maps[y + 1][x] != NULL) {
//                 w->maps[y][x]->pS = w->maps[y + 1][x]->pN;
//             } else {
//                 w->maps[y][x]->pS = 4 + rand() % (WIDTH - 8);
//             }

//             // Check west.
//             if (x > 0 && w->maps[y][x - 1] != NULL) {
//                 w->maps[y][x]->pW = w->maps[y][x - 1]->pE;
//             } else {
//                 w->maps[y][x]->pW = 4 + rand() % (HEIGHT - 8);
//             }

//             // Check East.
//             if (x < 400 && w->maps[y][x + 1] != NULL) {
//                 w->maps[y][x]->pE = w->maps[y][x + 1]->pW;
//             } else {
//                 w->maps[y][x]->pE = 4 + rand() % (HEIGHT - 8);
//             }

//             if (seed_map(w->maps[y][x])) {
//                 errmsg = "Seeding failed.";
//                 retval = -1;
//                 goto free_visited_maps;
//             }
            
//             place_paths_and_buildings(w->maps[y][x]);
//             if (place_pc(w->maps[y][x], &player)) {
//                 errmsg = "place_pc failed\n";
//                 retval = -1;
//                 goto free_player;
//             }
//         }
//         place_npcs(w->maps[y][x], NUM_NPCS);
//         print_map(w->maps[y][x]);
//         dijiksra(w->hiker_dist_map, w->maps[y][x], &w->maps[y][x]->t[player->y][player->x], HIKER);
//         dijiksra(w->rival_dist_map, w->maps[y][x], &w->maps[y][x]->t[player->y][player->x], RIVAL);

//     //     printf("Enter a command (n, s, e, w, f <x> <y>, q): ");

//     //     if (!fgets(input, sizeof (input), stdin)) {
//     //         return -1;
//     //     }

//     //     if (input[0] == 'f') {
//     //         int temp_x, temp_y;
//     //         if (sscanf(input, "f %d %d", &temp_x, &temp_y) == 2) {
//     //             if (temp_x >= -200 && temp_x <= 200 && temp_y >= -200 && temp_y <= 200) {
//     //                 new_x_pos = temp_x;
//     //                 new_y_pos = temp_y;
//     //             } else {
//     //                 printf("Out of bounds! Stay between -200 and 200.\n");
//     //             }
//     //         }
//     //     }
//     // }
    
//     // printf("Hiker Distance Map:");
//     // print_dist_map(w->hiker_dist_map); 
//     // printf("Rival Distance Map:");
//     // print_dist_map(w->rival_dist_map); 

//     free_player:
//         free(player->p);
//     // free_character:
//         free(player);

//     free_visited_maps:
//         while (!queue_dequeue(&visited, &x, &y)) {
//             free(w->maps[y][x]);
//         }
//     // destroy_queue:
//         queue_destroy(&visited);
//     // destroy_world:
//         world_destroy(w);

//     free_world:
//         free(w);

//     ret_err:
//         if (errmsg) {
//             fprintf(stderr, "Error: %s\n", errmsg);
//         }
//         return retval;
// }