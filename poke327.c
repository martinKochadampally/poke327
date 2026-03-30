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

// Stores out world.
world *w;

int place_npcs(map *m, int num_trainers, heap *h);
void init_terminal(void);
void output_map(map *m);
int move_pc(map *m, character *player, int dy, int dx);
void move_npc(map *m, character *c);
int dijiksra(int dist_map[HEIGHT][WIDTH], map *m, terrain *pc_pos, enum char_type npc);
void trainer_list(map *m, character *pc, int NUM_TRAINERS);
void enter_building(const char *building_name);
void battle_interface(character *c);



/*
    This is where the user interface adn things related to it will be implemented.
        Takes arg: --numtrainers
*/
int main(int argc, char *argv[]) {
    int retval = 0;             // This is what main will return.
    char *errmsg = NULL;        // If an error occurs, this stores error message.

    int num_trainers;          // Holds --numtrainers value.
    
    queue visited;              // Stores all the maps that we have visited, so that we can free them.
 
    // Movement:
    // map *m;
    int y, x;

    // User Input Variable:
    int input = ' ';

    // Character Movement Vars:
    heap char_heap;             // Stores all the characters in the current map.
    character *c;
    int current_time = 0, cost;
    
    // Checks for the -numtrainers tag.
    if ( argc == 1 ) {
        num_trainers = 10;
    }
    else if ( argc == 3 && !strcmp("--numtrainers", argv[1]) ) {
        num_trainers = atoi(argv[2]);
        if (num_trainers < 1) { 
            errmsg = "The value of numtrainers should be greater than 0.";
            retval = -1;
            goto ret_err;
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

    // Randomly placing NPCs on the map and adding each of them to the heap to keep track of whose turn it is.
    place_npcs(w->maps[y][x], num_trainers, &char_heap);

    if (place_pc(w->maps[y][x], &c)) {
        errmsg = "place_pc failed\n";
        retval = -8;
        goto free_characters;
    }

    dijiksra(w->hiker_dist_map, w->maps[y][x], &w->maps[y][x]->t[c->y][c->x], HIKER);
    dijiksra(w->rival_dist_map, w->maps[y][x], &w->maps[y][x]->t[c->y][c->x], RIVAL);

    // Using NCURSES lib, the following will output PC and NPC movements.
    init_terminal();

    do {
        // Clears text from previous run.
        clear();
        
        // If the current character is the PC.
        if (c->p) {
            switch (input) {
                case '7':
                case 'y':
                    move_pc(w->maps[y][x], c, -1, -1);
                    break;
                case '8':
                case 'k':
                    move_pc(w->maps[y][x], c, -1, 0);
                    break;
                case '9':
                case 'u':
                    move_pc(w->maps[y][x], c, -1, 1);
                    break;
                case '6':
                case 'l':
                    move_pc(w->maps[y][x], c, 0, 1);
                    break;
                case '3':
                case 'n':
                    move_pc(w->maps[y][x], c, 1, 1);
                    break;
                case '2':
                case 'j':
                    move_pc(w->maps[y][x], c, 1, 0);
                    break;
                case '1':
                case 'b':
                    move_pc(w->maps[y][x], c, 1, -1);
                    break;
                case '4':
                case 'h':
                    move_pc(w->maps[y][x], c, 0, -1);
                    break;
                case '>':
                    // Check if the tile the PC is currently standing on is a building
                    if (w->maps[y][x]->t[c->y][c->x].val == POKEMART) {
                        enter_building("Pokemart");
                    } 
                    else if (w->maps[y][x]->t[c->y][c->x].val == POKECENTER) {
                        enter_building("Pokemon Center");
                    } 
                    else {
                        mvprintw(0, 0, "You must be standing on a 'M' or 'C' to enter.");
                    }
                    // Redraw map and get new input after exiting building
                    output_map(w->maps[y][x]);
                    input = getch();
                    refresh();
                    continue;
                case 't':
                    trainer_list(w->maps[y][x], c, num_trainers);
                    output_map(w->maps[y][x]);
                    input = getch();
                    refresh();
                    continue;
                    break;
                case '5':
                case ' ':
                case '.':
                    break;
                default:
                    output_map(w->maps[y][x]);
                    input = getch();
                    refresh();
                    continue;
                    break;
            }

            // Prints out the updated map.
            output_map(w->maps[y][x]);

            // Fetches input.
            input = getch();
        }
        else {
            move_npc(w->maps[y][x], c);
        }

        // Adds current character to the heap with a updated next_turn value.
        cost = cost_table[c->type%10][w->maps[y][x]->t[c->y][c->x].val];
        c->next_turn = cost + current_time;
        if ( !heap_insert(&char_heap, c, c->next_turn) ) {
            errmsg = "Adding to character heap failed.";
            retval = -9;
            goto close_ncurses;
        }

        // Gets next character.
        heap_extract_min(&char_heap, (void **)&c, &current_time);

        // Updates new changes onto the screen.
        refresh();
    } while (input != 'Q');

    // Clean Up Code:
    close_ncurses:
        endwin();

        if (c) {
            if (c->p) 
                free(c->p);
            else 
                free(c->npc);
            free(c);
        }

    free_characters:
        while (!heap_extract_min(&char_heap, (void **)&c, &current_time)) {
            if (c->p) {
                free(c->p);
            }
            else {
                free(c->npc);
            }
            free(c);
        }
    
    // destroy_heap:
        heap_destroy(&char_heap);

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


/*
    Places NPCs
*/
int place_npcs(map *m, int num_trainers, heap *h) {
    int i;
    int x, y;
    for (i = 0; i < num_trainers; i++) {
        character *c = malloc(sizeof(*c));
        if (!c) return -1;

        c->seq_num = i + 1;
        c->next_turn = 0;
        c->p = NULL;

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

        do {
            x = 1 + rand() % (WIDTH - 2);
            y = 1 + rand() % (HEIGHT - 2);
        } while (m->ch[y][x] || m->t[y][x].val == GATE ||  m->t[y][x].val == POKECENTER|| 
                  m->t[y][x].val == POKEMART || cost_table[c->type % 10][m->t[y][x].val] == __INT_MAX__);

        c->x = x;
        c->y = y;

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

        if ( !(c->npc = malloc(sizeof (npc))) ) {
            free(c);
            return -1;
        }

        c->npc->is_defeated = 0;

        if (!heap_insert(h, c, c->next_turn)) {
            free(c->npc);
            free(c);
            return -1;
        }

        m->ch[y][x] = c;
    }
    return 0;
}

void init_terminal(void) {
    initscr();
    raw();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    start_color();
    init_pair(COLOR_YELLOW, COLOR_YELLOW, COLOR_BLACK); // PATH & GATE
    init_pair(COLOR_GREEN, COLOR_GREEN, COLOR_BLACK); // CLEARING & TALL GRASS
    init_pair(COLOR_WHITE, COLOR_WHITE, COLOR_BLACK); //
    init_pair(COLOR_BLUE, COLOR_BLUE, COLOR_BLACK); //
    init_pair(COLOR_RED, COLOR_RED, COLOR_BLACK); //
    init_pair(COLOR_MAGENTA, COLOR_MAGENTA, COLOR_BLACK); //
    init_pair(COLOR_BLACK, COLOR_BLACK, COLOR_BLACK); //
}

void output_map(map *m) {
    int y, x;

    for (y = 0; y < HEIGHT; y++) {
        for (x = 0; x < WIDTH; x++) {
            if (m->ch[y][x]) {
                attron(COLOR_PAIR(COLOR_MAGENTA));
                mvprintw(y + 1, x, "%c", m->ch[y][x]->symbol);
            }
            else {
                switch (m->t[y][x].val) {
                    case GATE: 
                        attron(COLOR_PAIR(COLOR_YELLOW));
                        mvprintw(y + 1, x, "#");
                        break;
                    case PATH: 
                        attron(COLOR_PAIR(COLOR_YELLOW));
                        mvprintw(y + 1, x, "#");
                        break;
                    case CLEARING:
                        attron(COLOR_PAIR(COLOR_GREEN));
                        mvprintw(y + 1, x, ".");
                        break;
                    case TALL_GRASS:
                        attron(COLOR_PAIR(COLOR_GREEN));
                        mvprintw(y + 1, x, ":");
                        break;
                    case ARCTIC:
                        attron(COLOR_PAIR(COLOR_WHITE));
                        mvprintw(y + 1, x, "*");   
                        break;
                    case LAKE: 
                        attron(COLOR_PAIR(COLOR_BLUE));
                        mvprintw(y + 1, x, "~");
                        break;
                    case FOREST: 
                        attron(COLOR_PAIR(COLOR_GREEN));
                        mvprintw(y + 1, x, "^");
                        break;
                    case MOUNTAIN: 
                        attron(COLOR_PAIR(COLOR_WHITE));
                        mvprintw(y + 1, x, "%c", '%');
                        break;
                    case POKECENTER: 
                        attron(COLOR_PAIR(COLOR_RED));
                        mvprintw(y + 1, x, "C");
                        break;
                    case POKEMART: 
                        attron(COLOR_PAIR(COLOR_RED));
                        mvprintw(y + 1, x, "M");
                        break;
                    case BOULDER: 
                        attron(COLOR_PAIR(COLOR_WHITE));
                        mvprintw(y + 1, x, "%c", '%');
                        break;
                    default: 
                        attron(COLOR_PAIR(COLOR_BLACK));
                        mvprintw(y + 1, x, " ");
                        break;
                }
            }
        }
    }
    attron(COLOR_PAIR(COLOR_WHITE));
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

    // Destroys the ref_table.
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            ref_table[i][j] = NULL;
        } 
    }

    // Recursively destroys the heap.
    heap_destroy(&h);

    return 0;
}

int move_pc(map *m, character *player, int dy, int dx) {
    int y, x;
    char* errmsg;
    
    y = dy + player->y;
    x = dx + player->x;

    if (m->ch[y][x]) {
        character *target = m->ch[y][x];
        
        if (!target->npc->is_defeated) {
            battle_interface(target);
        }
        return -1;
    }

    if (cost_table[PC%10][m->t[y][x].val] < __INT_MAX__ && !m->ch[y][x] && m->t[y][x].val != GATE) {
        m->ch[player->y][player->x] = NULL;
        m->ch[y][x] = player;
        player->y = y;
        player->x = x;

        dijiksra(w->hiker_dist_map, m, &m->t[player->y][player->x], HIKER);
        dijiksra(w->rival_dist_map, m, &m->t[player->y][player->x], RIVAL);
    } 
    else if (m->t[y][x].val == GATE) {
        mvprintw(0, 0, "Gates are currently locked!");
        return -2;
    }
    else {
        switch (m->t[y][x].val) {
            case MOUNTAIN:
                errmsg = "mountainous regions";
                break;
            case LAKE:
                errmsg = "water";
                break;
            case FOREST:
                errmsg = "forests";
                break;
            case BOULDER:
                errmsg = "a boulder";
                break;
            default:
                errmsg = NULL;
                break;
        }
        mvprintw(0, 0, "You can't walk through %s!", errmsg);
        return -3;
    }

    return 0;
}


void move_npc(map *m, character *c) {
    int dy, dx, new_y, new_x;
    int next_x = c->x;
    int next_y = c->y;
    int min_dist = __INT_MAX__;
    int type;

    type = (c->npc->is_defeated) ? EXPLORER : c->type;

    if (type == SENTRY) return;

    if (type == HIKER || type == RIVAL) {
        // Get dist maps.
        int (*dist_map)[WIDTH] = (type == HIKER) ? w->hiker_dist_map : w->rival_dist_map;
        
        for (dy = -1; dy <= 1; dy++) {
            for (dx = -1; dx <= 1; dx++) {
                if (dx == 0 && dy == 0) continue;
                new_y = c->y + dy;
                new_x = c->x + dx;
                if (m->ch[new_y][new_x] && m->ch[new_y][new_x]->p) {
                    if (!c->npc->is_defeated) {
                        battle_interface(c);
                    }
                    return;
                }
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
    else if (type == PACER) {
        new_x = c->x + c->dir[0];
        new_y = c->y + c->dir[1];
        
        if (m->ch[new_y][new_x] || cost_table[OTHER%10][m->t[new_y][new_x].val] == __INT_MAX__ || m->t[new_y][new_x].val == GATE) {
            c->dir[0] *= -1; 
            c->dir[1] *= -1; 
        }
        next_x = c->x + c->dir[0];
        next_y = c->y + c->dir[1];
    }
    else if (type == WANDERER) {
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
    else if (type == EXPLORER) {
        new_x = c->x + c->dir[0];
        new_y = c->y + c->dir[1];
        
        if (cost_table[c->type%10][m->t[new_y][new_x].val] == __INT_MAX__ || m->ch[new_y][new_x] || m->t[new_y][new_x].val == GATE) {
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

void enter_building(const char *building_name) {
    int input = 0;

    while (input != '<') {
        clear();
        mvprintw(1, 0, "Welcome to the %s!", building_name);
        mvprintw(2, 0, "This is a temporary placeholder.");
        mvprintw(3, 0, "Press '<' to exit.");
        refresh();
        input = getch();
    }
}

void trainer_list(map *m, character *pc, int NUM_TRAINERS) {
    int i, j, dy, dx, count = 0;
    character *trainers[NUM_TRAINERS];

    // Gets pointers to all NPCs
    for (i = 0; i < HEIGHT; i++) {
        for (j = 0; j < WIDTH; j++) {
            if (m->ch[i][j] && !m->ch[i][j]->p) {
                trainers[count++] = m->ch[i][j];
            }
        }
    }

    int offset = 0;
    int key = 0;
    int max_display = 20;

    while (key != 27) {
        clear();
        mvprintw(0, 0, "Trainer List:");
        mvprintw(1, 0, "%-5s | %-20s", "Sym", "Relative Position");
        mvprintw(2, 0, "------------------------------------");

        for (i = 0; i < max_display && (i + offset) < count; i++) {
            character *tnr = trainers[i + offset];
            
            // Calculate relative position
            dy = pc->y - tnr->y;
            dx = pc->x - tnr->x;

            char *vert = (dy >= 0) ? "north" : "south";
            char *hori = (dx >= 1) ? "west" : "east";

            mvprintw(i + 3, 0, "  %c   | %d %s and %d %s", tnr->symbol, abs(dy), vert, abs(dx), hori);
        }

        if (offset + max_display < count) mvprintw(23, 0, "-- More Below (Down Arrow) --");
        if (offset > 0) mvprintw(2, 30, "-- More Above (Up Arrow) --");

        refresh();
        key = getch();

        // Scrolling
        if (key == KEY_UP && offset > 0) {
            offset--;
        } else if (key == KEY_DOWN && (offset + max_display) < count) {
            offset++;
        }
    }
}

void battle_interface(character *c) {
    int input = 0;

    while (input != 27) {
        clear();
        mvprintw(10, 0, "A WILD TRAINER APPEARS!");
        mvprintw(12, 0, "Trainer: %c", c->symbol);
        mvprintw(14, 0, "BATTLE PLACEHOLDER. Press ESC to win.");
        refresh();
        input = getch();
    }
    c->npc->is_defeated = 1;
}