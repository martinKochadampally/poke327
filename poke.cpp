#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <ncurses.h>
#include <vector>

#include "queue.h"
#include "heap.h"
#include "map.h"

int cost_table[5][11] = {{10         ,10         ,10         ,20         ,25         ,__INT_MAX__,__INT_MAX__,__INT_MAX__,10         ,10         ,__INT_MAX__},
                         {__INT_MAX__,10         ,10         ,15         ,20         ,__INT_MAX__,15         ,15         ,50         ,50         ,__INT_MAX__},
                         {__INT_MAX__,10         ,10         ,20         ,25         ,__INT_MAX__,__INT_MAX__,__INT_MAX__,50         ,50         ,__INT_MAX__},
                         {__INT_MAX__,__INT_MAX__,__INT_MAX__,__INT_MAX__,__INT_MAX__,7          ,__INT_MAX__,__INT_MAX__,__INT_MAX__,__INT_MAX__,__INT_MAX__},
                         {__INT_MAX__,10         ,10         ,20         ,25         ,__INT_MAX__,__INT_MAX__,__INT_MAX__,50         ,50         ,__INT_MAX__}};

World *w;

int place_npcs(Map *m, int num_trainers, heap *h);
void init_terminal(void);
void output_map(Map *m);
int move_pc(Map *m, Character *player, int dy, int dx);
void move_npc(Map *m, Character *c);
int dijiksra(int dist_map[HEIGHT][WIDTH], Map *m, terrain *pc_pos, enum char_type npc);
void trainer_list(Map *m, Character *pc, int NUM_TRAINERS);
void enter_building(const char *building_name);
void battle_interface(Character *c);


int main(int argc, char *argv[]) {
    int retval = 0;
    const char *errmsg = NULL;

    int num_trainers;
    queue visited;
    int y, x;
    int input = ' ';

    heap char_heap;
    Character *c;
    int current_time = 0, cost;

    if (argc == 1) {
        num_trainers = 10;
    } else if (argc == 3 && !strcmp("--numtrainers", argv[1])) {
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

    w = new World();
    if (!w) {
        errmsg = "Creating world failed.";
        retval = -3;
        goto ret_err;
    }

    queue_init(&visited);
    heap_init(&char_heap);
    srand(time(NULL));

    y = x = 200;

    w->maps[y][x] = new Map(x, y, NULL, NULL, NULL, NULL);
    if (!w->maps[y][x]) {
        errmsg = "Creating a map failed.";
        retval = -5;
        goto free_visited_maps;
    }

    if (queue_enqueue(&visited, x, y)) {
        errmsg = "Couldn't mark map as visited.";
        retval = -6;
        goto free_visited_maps;
    }

    if (w->maps[y][x]->seed()) {
        errmsg = "Seeding failed.";
        retval = -7;
        goto free_visited_maps;
    }

    w->maps[y][x]->place_paths_and_buildings();

    place_npcs(w->maps[y][x], num_trainers, &char_heap);

    if (w->maps[y][x]->place_pc(&c)) {
        errmsg = "place_pc failed\n";
        retval = -8;
        goto free_characters;
    }

    dijiksra(w->hiker_dist_map, w->maps[y][x], &w->maps[y][x]->t[c->y][c->x], HIKER);
    dijiksra(w->rival_dist_map, w->maps[y][x], &w->maps[y][x]->t[c->y][c->x], RIVAL);

    init_terminal();

    do {
        clear();

        if (c->type == PC_TYPE) {
            switch (input) {
                case '7': case 'y': move_pc(w->maps[y][x], c, -1, -1); break;
                case '8': case 'k': move_pc(w->maps[y][x], c, -1,  0); break;
                case '9': case 'u': move_pc(w->maps[y][x], c, -1,  1); break;
                case '6': case 'l': move_pc(w->maps[y][x], c,  0,  1); break;
                case '3': case 'n': move_pc(w->maps[y][x], c,  1,  1); break;
                case '2': case 'j': move_pc(w->maps[y][x], c,  1,  0); break;
                case '1': case 'b': move_pc(w->maps[y][x], c,  1, -1); break;
                case '4': case 'h': move_pc(w->maps[y][x], c,  0, -1); break;
                case '>':
                    if (w->maps[y][x]->t[c->y][c->x].val == POKEMART)
                        enter_building("Pokemart");
                    else if (w->maps[y][x]->t[c->y][c->x].val == POKECENTER)
                        enter_building("Pokemon Center");
                    else
                        mvprintw(0, 0, "You must be standing on a 'M' or 'C' to enter.");
                    goto update_changes;
                case 't':
                    trainer_list(w->maps[y][x], c, num_trainers);
                    goto update_changes;
                case '5': case ' ': case '.': break;
                default: goto update_changes;
            }
        } else {
            move_npc(w->maps[y][x], c);
        }

        cost = cost_table[c->type % 10][w->maps[y][x]->t[c->y][c->x].val];
        c->next_turn = cost + current_time;
        if (!heap_insert(&char_heap, c, c->next_turn)) {
            errmsg = "Adding to character heap failed.";
            retval = -9;
            goto close_ncurses;
        }

        heap_extract_min(&char_heap, (void **)&c, &current_time);

        update_changes:
            output_map(w->maps[y][x]);
            if (c->type == PC_TYPE) input = getch();
            refresh();

    } while (input != 'Q');

    close_ncurses:
        endwin();
        if (c) delete c;

    free_characters:
        while (!heap_extract_min(&char_heap, (void **)&c, &current_time)) {
            delete c;
        }
        heap_destroy(&char_heap);

    free_visited_maps:
        while (!queue_dequeue(&visited, &x, &y)) {
            delete w->maps[y][x];
        }
        queue_destroy(&visited);
        delete w;

    free_world:
        ;

    ret_err:
        if (errmsg) fprintf(stderr, "Error: %s\n", errmsg);
        return retval;
}


int place_npcs(Map *m, int num_trainers, heap *h) {
    int i, x, y;
    NPC *c;

    for (i = 0; i < num_trainers; i++) {
        char sym;
        char_type type;

        if (i == 0) {
            type = HIKER; sym = 'h';
        } else if (i == 1) {
            type = RIVAL; sym = 'r';
        } else {
            int r = rand() % 6;
            switch (r) {
                case 0: type = HIKER;    sym = 'h'; break;
                case 1: type = RIVAL;    sym = 'r'; break;
                case 2: type = PACER;    sym = 'p'; break;
                case 3: type = WANDERER; sym = 'w'; break;
                case 4: type = SENTRY;   sym = 's'; break;
                case 5: type = EXPLORER; sym = 'e'; break;
                default: type = OTHER;   sym = 'o'; break;
            }
        }

        do {
            x = 1 + rand() % (WIDTH - 2);
            y = 1 + rand() % (HEIGHT - 2);
        } while (m->ch[y][x] || m->t[y][x].val == GATE || m->t[y][x].val == POKECENTER ||
                 m->t[y][x].val == POKEMART || cost_table[type % 10][m->t[y][x].val] == __INT_MAX__);

        c = new NPC(sym, type, x, y);
        c->seq_num = i + 1;
        c->next_turn = 0;

        if (type == PACER || type == WANDERER) {
            if (rand() % 2) { c->dir[0] = (rand() % 2) ? 1 : -1; c->dir[1] = 0; }
            else             { c->dir[0] = 0; c->dir[1] = (rand() % 2) ? 1 : -1; }
        } else {
            c->dir[0] = (rand() % 3) - 1;
            c->dir[1] = (rand() % 3) - 1;
        }

        if (!heap_insert(h, c, c->next_turn)) {
            delete c;
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
    init_pair(COLOR_YELLOW,  COLOR_YELLOW,  COLOR_BLACK);
    init_pair(COLOR_GREEN,   COLOR_GREEN,   COLOR_BLACK);
    init_pair(COLOR_WHITE,   COLOR_WHITE,   COLOR_BLACK);
    init_pair(COLOR_BLUE,    COLOR_BLUE,    COLOR_BLACK);
    init_pair(COLOR_RED,     COLOR_RED,     COLOR_BLACK);
    init_pair(COLOR_MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(COLOR_BLACK,   COLOR_BLACK,   COLOR_BLACK);
}

void output_map(Map *m) {
    int y, x;

    for (y = 0; y < HEIGHT; y++) {
        for (x = 0; x < WIDTH; x++) {
            if (m->ch[y][x]) {
                attron(COLOR_PAIR(COLOR_MAGENTA));
                mvprintw(y + 1, x, "%c", m->ch[y][x]->symbol);
            } else {
                switch (m->t[y][x].val) {
                    case GATE:       
                        attron(COLOR_PAIR(COLOR_YELLOW));  
                        mvprintw(y+1, x, "#"); 
                        break;
                    case PATH:       
                        attron(COLOR_PAIR(COLOR_YELLOW));  
                        mvprintw(y+1, x, "#"); 
                        break;
                    case CLEARING:   
                        attron(COLOR_PAIR(COLOR_GREEN));   
                        mvprintw(y+1, x, "."); 
                        break;
                    case TALL_GRASS: 
                        attron(COLOR_PAIR(COLOR_GREEN));   
                        mvprintw(y+1, x, ":"); 
                        break;
                    case ARCTIC:     
                        attron(COLOR_PAIR(COLOR_WHITE));   
                        mvprintw(y+1, x, "*"); 
                        break;
                    case LAKE:       
                        attron(COLOR_PAIR(COLOR_BLUE));    
                        mvprintw(y+1, x, "~"); 
                        break;
                    case FOREST:     
                        attron(COLOR_PAIR(COLOR_GREEN));   
                        mvprintw(y+1, x, "^"); 
                        break;
                    case MOUNTAIN:   
                        attron(COLOR_PAIR(COLOR_WHITE));   
                        mvprintw(y+1, x, "%c", '%'); 
                        break;
                    case POKECENTER: 
                        attron(COLOR_PAIR(COLOR_RED));     
                        mvprintw(y+1, x, "C"); 
                        break;
                    case POKEMART:   
                        attron(COLOR_PAIR(COLOR_RED));     
                        mvprintw(y+1, x, "M"); 
                        break;
                    case BOULDER:    
                        attron(COLOR_PAIR(COLOR_WHITE));   
                        mvprintw(y+1, x, "%c", '%'); 
                        break;
                    default:         
                        attron(COLOR_PAIR(COLOR_BLACK));   
                        mvprintw(y+1, x, " "); 
                        break;
                }
            }
        }
    }
    attron(COLOR_PAIR(COLOR_WHITE));
}

int dijiksra(int dist_map[HEIGHT][WIDTH], Map *m, terrain *pc_pos, enum char_type npc) {
    heap h;
    terrain *t;
    int y, x, size, cost, new_cost;
    heap_item *ref_table[HEIGHT][WIDTH] = {{NULL}};

    heap_init(&h);

    for (y = 0; y < HEIGHT; y++) {
        for (x = 0; x < WIDTH; x++) {
            if (pc_pos->x == x && pc_pos->y == y) {
                if (!(ref_table[y][x] = heap_insert(&h, &m->t[y][x], 0))) return -1;
                dist_map[y][x] = 0;
                continue;
            }
            dist_map[y][x] = __INT_MAX__;
        }
    }

    while (!heap_size(&h, &size) && size) {
        heap_extract_min(&h, (void **)&t, &cost);
        y = t->y;
        x = t->x;

        if (t->NW && cost_table[npc%10][t->NW->val] != __INT_MAX__) {
            new_cost = cost + cost_table[npc%10][t->NW->val];
            if (new_cost < dist_map[y-1][x-1]) {
                dist_map[y-1][x-1] = new_cost;
                if (ref_table[y-1][x-1]) decrease_key(&h, ref_table[y-1][x-1], new_cost);
                else if (!(ref_table[y-1][x-1] = heap_insert(&h, t->NW, new_cost))) return -1;
            }
        }
        if (t->N && cost_table[npc%10][t->N->val] != __INT_MAX__) {
            new_cost = cost + cost_table[npc%10][t->N->val];
            if (new_cost < dist_map[y-1][x]) {
                dist_map[y-1][x] = new_cost;
                if (ref_table[y-1][x]) decrease_key(&h, ref_table[y-1][x], new_cost);
                else if (!(ref_table[y-1][x] = heap_insert(&h, t->N, new_cost))) return -1;
            }
        }
        if (t->NE && cost_table[npc%10][t->NE->val] != __INT_MAX__) {
            new_cost = cost + cost_table[npc%10][t->NE->val];
            if (new_cost < dist_map[y-1][x+1]) {
                dist_map[y-1][x+1] = new_cost;
                if (ref_table[y-1][x+1]) decrease_key(&h, ref_table[y-1][x+1], new_cost);
                else if (!(ref_table[y-1][x+1] = heap_insert(&h, t->NE, new_cost))) return -1;
            }
        }
        if (t->E && cost_table[npc%10][t->E->val] != __INT_MAX__) {
            new_cost = cost + cost_table[npc%10][t->E->val];
            if (new_cost < dist_map[y][x+1]) {
                dist_map[y][x+1] = new_cost;
                if (ref_table[y][x+1]) decrease_key(&h, ref_table[y][x+1], new_cost);
                else if (!(ref_table[y][x+1] = heap_insert(&h, t->E, new_cost))) return -1;
            }
        }
        if (t->SE && cost_table[npc%10][t->SE->val] != __INT_MAX__) {
            new_cost = cost + cost_table[npc%10][t->SE->val];
            if (new_cost < dist_map[y+1][x+1]) {
                dist_map[y+1][x+1] = new_cost;
                if (ref_table[y+1][x+1]) decrease_key(&h, ref_table[y+1][x+1], new_cost);
                else if (!(ref_table[y+1][x+1] = heap_insert(&h, t->SE, new_cost))) return -1;
            }
        }
        if (t->S && cost_table[npc%10][t->S->val] != __INT_MAX__) {
            new_cost = cost + cost_table[npc%10][t->S->val];
            if (new_cost < dist_map[y+1][x]) {
                dist_map[y+1][x] = new_cost;
                if (ref_table[y+1][x]) decrease_key(&h, ref_table[y+1][x], new_cost);
                else if (!(ref_table[y+1][x] = heap_insert(&h, t->S, new_cost))) return -1;
            }
        }
        if (t->SW && cost_table[npc%10][t->SW->val] != __INT_MAX__) {
            new_cost = cost + cost_table[npc%10][t->SW->val];
            if (new_cost < dist_map[y+1][x-1]) {
                dist_map[y+1][x-1] = new_cost;
                if (ref_table[y+1][x-1]) decrease_key(&h, ref_table[y+1][x-1], new_cost);
                else if (!(ref_table[y+1][x-1] = heap_insert(&h, t->SW, new_cost))) return -1;
            }
        }
        if (t->W && cost_table[npc%10][t->W->val] != __INT_MAX__) {
            new_cost = cost + cost_table[npc%10][t->W->val];
            if (new_cost < dist_map[y][x-1]) {
                dist_map[y][x-1] = new_cost;
                if (ref_table[y][x-1]) decrease_key(&h, ref_table[y][x-1], new_cost);
                else if (!(ref_table[y][x-1] = heap_insert(&h, t->W, new_cost))) return -1;
            }
        }
    }

    for (int i = 0; i < HEIGHT; i++)
        for (int j = 0; j < WIDTH; j++)
            ref_table[i][j] = NULL;

    heap_destroy(&h);
    return 0;
}

int move_pc(Map *m, Character *player, int dy, int dx) {
    int y = dy + player->y;
    int x = dx + player->x;
    const char *errmsg;

    if (m->ch[y][x]) {
        NPC *target = (NPC *)m->ch[y][x];
        if (!target->is_defeated) battle_interface(target);
        return -1;
    }

    if (cost_table[PC_TYPE % 10][m->t[y][x].val] < __INT_MAX__ && m->t[y][x].val != GATE) {
        m->ch[player->y][player->x] = NULL;
        m->ch[y][x] = player;
        player->y = y;
        player->x = x;

        dijiksra(w->hiker_dist_map, m, &m->t[player->y][player->x], HIKER);
        dijiksra(w->rival_dist_map, m, &m->t[player->y][player->x], RIVAL);
    } else if (m->t[y][x].val == GATE) {
        mvprintw(0, 0, "Gates are currently locked!");
        return -2;
    } else {
        switch (m->t[y][x].val) {
            case MOUNTAIN: errmsg = "mountainous regions"; break;
            case LAKE:     errmsg = "water";               break;
            case FOREST:   errmsg = "forests";             break;
            case BOULDER:  errmsg = "a boulder";           break;
            default:       errmsg = NULL;                  break;
        }
        mvprintw(0, 0, "You can't walk through %s!", errmsg);
        return -3;
    }

    return 0;
}

void move_npc(Map *m, Character *c) {
    int dy, dx, new_y, new_x;
    int next_x = c->x, next_y = c->y;
    int min_dist = __INT_MAX__;
    NPC *npc = (NPC *)c;

    char_type type = npc->is_defeated ? EXPLORER : c->type;

    if (type == SENTRY) return;

    if (type == HIKER || type == RIVAL) {
        int (*dist_map)[WIDTH] = (type == HIKER) ? w->hiker_dist_map : w->rival_dist_map;

        for (dy = -1; dy <= 1; dy++) {
            for (dx = -1; dx <= 1; dx++) {
                if (dx == 0 && dy == 0) continue;
                new_y = c->y + dy;
                new_x = c->x + dx;
                if (m->ch[new_y][new_x] && m->ch[new_y][new_x]->type == PC_TYPE) {
                    if (!npc->is_defeated) battle_interface(c);
                    return;
                }
                if (dist_map[new_y][new_x] < min_dist && !m->ch[new_y][new_x]) {
                    min_dist = dist_map[new_y][new_x];
                    next_x = new_x;
                    next_y = new_y;
                }
            }
        }
    } else if (type == PACER) {
        new_x = c->x + c->dir[0];
        new_y = c->y + c->dir[1];
        if (m->ch[new_y][new_x] || cost_table[OTHER%10][m->t[new_y][new_x].val] == __INT_MAX__ || m->t[new_y][new_x].val == GATE) {
            c->dir[0] *= -1;
            c->dir[1] *= -1;
        }
        next_x = c->x + c->dir[0];
        next_y = c->y + c->dir[1];
    } else if (type == WANDERER) {
        new_x = c->x + c->dir[0];
        new_y = c->y + c->dir[1];
        if (m->t[new_y][new_x].val != m->t[c->y][c->x].val || m->ch[new_y][new_x] || m->t[new_y][new_x].val == GATE) {
            if (rand() % 2) { c->dir[0] = (rand() % 2) ? 1 : -1; c->dir[1] = 0; }
            else             { c->dir[0] = 0; c->dir[1] = (rand() % 2) ? 1 : -1; }
        } else {
            next_x = new_x;
            next_y = new_y;
        }
    } else if (type == EXPLORER) {
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

void trainer_list(Map *m, Character *pc, int NUM_TRAINERS) {
    int i, j, dy, dx, count = 0;
    std::vector<Character *> trainers(NUM_TRAINERS);
    int offset = 0, key = 0, max_display = 20;
    const char *vert[] = {"north", "south"};
    const char *hori[] = {"west",  "east"};
    int v, h;

    for (i = 0; i < HEIGHT; i++) {
        for (j = 0; j < WIDTH; j++) {
            if (m->ch[i][j] && m->ch[i][j]->type != PC_TYPE) {
                trainers[count++] = m->ch[i][j];
            }
        }
    }

    while (key != 27) {
        clear();
        mvprintw(0, 0, "Trainer List:");
        mvprintw(1, 0, "%-5s | %-20s", "Sym", "Relative Position");
        mvprintw(2, 0, "------------------------------------");

        for (i = 0; i < max_display && (i + offset) < count; i++) {
            Character *tnr = trainers[i + offset];
            dy = pc->y - tnr->y;
            dx = pc->x - tnr->x;
            v = (dy > 1) ? 0 : 1;
            h = (dx > 1) ? 0 : 1;
            mvprintw(i + 3, 0, "  %c   | %d %s and %d %s", tnr->symbol, abs(dy), vert[v], abs(dx), hori[h]);
        }

        if (offset + max_display < count) mvprintw(23, 0, "-- More Below (Down Arrow) --");
        if (offset > 0)                   mvprintw(2, 30, "-- More Above (Up Arrow) --");

        refresh();
        key = getch();
        if (key == KEY_UP && offset > 0) offset--;
        else if (key == KEY_DOWN && (offset + max_display) < count) offset++;
    }
}

void battle_interface(Character *c) {
    int input = 0;
    NPC *npc = (NPC *)c;
    while (input != 27) {
        clear();
        mvprintw(10, 0, "A WILD TRAINER APPEARS!");
        mvprintw(12, 0, "Trainer: %c", c->symbol);
        mvprintw(14, 0, "BATTLE PLACEHOLDER. Press ESC to win.");
        refresh();
        input = getch();
    }
    npc->is_defeated = true;
}