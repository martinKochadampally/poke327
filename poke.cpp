#include <cstdio>
#include <cstdlib>
#include <string>
#include <ctime>
#include <iostream>
#include <ncurses.h>
#include <vector>

#include "queue.h"
#include "heap.h"
#include "map.h"
#include "pokemon.h"

#define TRY_NEIGHBOR(dir, ny, nx) \
    if (t->dir && cost_table[npc%10][t->dir->val] != __INT_MAX__) { \
        new_cost = cost + cost_table[npc%10][t->dir->val]; \
        if (new_cost < dist_map[ny][nx]) { \
            dist_map[ny][nx] = new_cost; \
            if (ref_table[ny][nx]) decrease_key(&h, ref_table[ny][nx], new_cost); \
            else if (!(ref_table[ny][nx] = heap_insert(&h, t->dir, new_cost))) return -1; \
        } \
    }

int cost_table[6][12] = {
  /* Gate       , Path       , S Grass    , T Grass    , Arctic     , Lake       , Forest     , Mountain   , PokeCenter , PokeMart   , Boulder    , Black Market */
    {10,          10         , 10         , 20         , 25         , __INT_MAX__, __INT_MAX__, __INT_MAX__, 10         , 10         , __INT_MAX__, 10         }, // PC.
    {__INT_MAX__, 10         , 10         , 15         , 20         , __INT_MAX__, 15         , 15         , 50         , 50         , __INT_MAX__, 50         }, // Hiker.
    {__INT_MAX__, 10         , 10         , 20         , 25         , __INT_MAX__, __INT_MAX__, __INT_MAX__, 50         , 50         , __INT_MAX__, 50         }, // Rival.
    {__INT_MAX__, __INT_MAX__, __INT_MAX__, __INT_MAX__, __INT_MAX__, 7          , __INT_MAX__, __INT_MAX__, __INT_MAX__, __INT_MAX__, __INT_MAX__, __INT_MAX__}, // Swimmer.
    {__INT_MAX__, 10         , 10         , 20         , 25         , __INT_MAX__, __INT_MAX__, __INT_MAX__, 50         , 50         , __INT_MAX__, 50         }, // Other.
    {10         , 10         , 5          , 7          , 10         , __INT_MAX__, __INT_MAX__, 15         , __INT_MAX__, 5          , 5          , 5          }  // Police.
};

World *w;
int num_trainers;
bool police_caught_player = false;

int  place_npcs(Map *m, int num_trainers);
Map *get_or_create_map(int world_x, int world_y);
void init_terminal(void);
void output_map(Map *m);
int  move_pc(Map **m, int *world_x, int *world_y, Character *player, int dy, int dx);
void move_npc(Map *m, Character *c);
int  dijiksra(int dist_map[HEIGHT][WIDTH], Map *m, terrain *pc_pos, enum char_type npc);
void trainer_list(Map *m, Character *pc, int NUM_TRAINERS);
void enter_building(Character *c, const char *building_name);
void battle_interface(Character *pc, Character *npc);
void fly_command(Map **m, int *world_x, int *world_y, Character *pc);
Map *get_or_create_map(int world_x, int world_y);
void starter_selection(Character *pc);
void bag_interface(Character *pc);
bool wild_encounter(Character *pc, Pokemon *p);
void stun_grenade_handler(Map *m, Character *pc);
void steal_handler(Map *m, Character *pc);
void blackmarket_interface(Character *c, Map *m);
int spawn_police(Map *m, Character *pc);
int spawn_police_npc(Map *m, Character *pc);
void check_police_collision(Character *pc, Map *m);
void output_map_flash(Map *m, bool flash[HEIGHT][WIDTH]);
bool is_pc_defeated(Character *pc);
void game_over_screen();
void arrested_screen();
// bool battle_loop(Character* pc_char, NPC* npc_char, Pokemon* wild_p);
bool battle_loop(Character *pc_char, Character *npc_char, Pokemon *wild_p);

int main(int argc, char *argv[]) {
    // Error Handling Variables.
    int retval = 0;
    const char *errmsg = NULL;

    // Importing Infromation from Database
    if (argc == 2) {
        std::string db_path = get_db_path();
        if (db_path.empty()) {
            std::cerr << "Could not find pokedex database." << std::endl;
            return 1;
        }

        if (!strcmp(argv[1], "pokemon")) {
            std::vector<pokemon*> v;
            parse_pokemon(v, db_path);
            for (auto p : v) { p->print(); delete p; }
            return 0;
        } else if (!strcmp(argv[1], "moves")) {
            std::vector<moves*> v;
            parse_moves(v, db_path);
            for (auto m : v) { m->print(); delete m; }
            return 0;
        } else if (!strcmp(argv[1], "pokemon_moves")) {
            std::vector<pokemon_moves*> v;
            parse_pokemon_moves(v, db_path);
            for (auto pm : v) { pm->print(); delete pm; }
            return 0;
        } else if (!strcmp(argv[1], "pokemon_species")) {
            std::vector<pokemon_species*> v;
            parse_pokemon_species(v, db_path);
            for (auto ps : v) { ps->print(); delete ps; }
            return 0;
        } else if (!strcmp(argv[1], "experience")) {
            std::vector<experience*> v;
            parse_experience(v, db_path);
            for (auto e : v) { e->print(); delete e; }
            return 0;
        } else if (!strcmp(argv[1], "type_names")) {
            std::vector<type_names*> v;
            parse_type_names(v, db_path);
            for (auto tn : v) { tn->print(); delete tn; }
            return 0;
        } else if (!strcmp(argv[1], "pokemon_stats")) {
            std::vector<pokemon_stats*> v;
            parse_pokemon_stats(v, db_path);
            for (auto ps : v) { ps->print(); delete ps; }
            return 0;
        } else if (!strcmp(argv[1], "stats")) {
            std::vector<stats*> v;
            parse_stats(v, db_path);
            for (auto s : v) { s->print(); delete s; }
            return 0;
        } else if (!strcmp(argv[1], "pokemon_types")) {
            std::vector<pokemon_types*> v;
            parse_pokemon_types(v, db_path);
            for (auto pt : v) { pt->print(); delete pt; }
            return 0;
        } else {
            std::cerr << "Invalid database name: " << argv[1] << std::endl;
            return 1;
        }
    }

    // World Traversal Variables.
    int world_x, world_y;
    Map *m;
    Character *c;

    // Game Loop Variables.
    int input = ' ';
    Character *pc = NULL;
    int cost;

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
        errmsg = "Invalid argument(s).";
        retval = -2;
        goto ret_err;
    }

    // Generating World.
    if (!(w = new World())) { 
        errmsg = "Creating world failed."; retval = -3; 
        goto ret_err; 
    }

    srand(time(NULL));


    load_pokemon_data();

    world_x = world_y = 200;

    // Generate starting map.

    if (!get_or_create_map(world_x, world_y)) {
        errmsg = "Failed to create map.";
        retval = -4;
        goto cleanup;
    }

    // Place PC on starting map.

    if (w->maps[world_y][world_x]->place_pc(&pc)) {
        errmsg = "Failed to place player character.";
        retval = -4;
        goto cleanup;
    }


    dijiksra(w->hiker_dist_map, w->maps[world_y][world_x], &w->maps[world_y][world_x]->t[pc->y][pc->x], HIKER);
    dijiksra(w->rival_dist_map, w->maps[world_y][world_x], &w->maps[world_y][world_x]->t[pc->y][pc->x], RIVAL);

    // Add PC to the map's heap.
    pc->next_turn = 0;
    heap_insert(&w->maps[world_y][world_x]->char_heap, pc, pc->next_turn);


    init_terminal();


    starter_selection(pc);

    
    m = w->maps[world_y][world_x];
    c = pc;

    do {
        clear();

        if (c->type == PC_TYPE) {
            switch (input) {
                case '7': case 'y': 
                    move_pc(&m, &world_x, &world_y, c, -1, -1); 
                    break;
                case '8': case 'k': 
                    move_pc(&m, &world_x, &world_y, c, -1,  0); 
                    break;
                case '9': case 'u': 
                    move_pc(&m, &world_x, &world_y, c, -1,  1); 
                    break;
                case '6': case 'l': 
                    move_pc(&m, &world_x, &world_y, c,  0,  1); 
                    break;
                case '3': case 'n': 
                    move_pc(&m, &world_x, &world_y, c,  1,  1); 
                    break;
                case '2': case 'j': 
                    move_pc(&m, &world_x, &world_y, c,  1,  0); 
                    break;
                case '1': case 'b': 
                    move_pc(&m, &world_x, &world_y, c,  1, -1); 
                    break;
                case '4': case 'h': 
                    move_pc(&m, &world_x, &world_y, c,  0, -1); 
                    break;
                case '>':
                    if (m->t[c->y][c->x].val == POKEMART)
                        enter_building(c, "Pokemart");
                    else if (m->t[c->y][c->x].val == POKECENTER)
                        enter_building(c, "Pokemon Center");
                    else if (m->t[c->y][c->x].val == BLACKMARKET)
                        blackmarket_interface(c, m);
                    else
                        mvprintw(0, 0, "There is no building to enter. You must be standing on a 'M', 'C' or a 'B'.");
                    goto update_changes;
                case 't':
                    trainer_list(m, c, num_trainers);
                    goto update_changes;
                case 'B':
                    bag_interface(c);
                    goto update_changes;
                case 'g':
                    stun_grenade_handler(m, c);
                    goto update_changes;
                case 's':
                    steal_handler(m, c);
                    goto update_changes;
                case 'f':
                    fly_command(&m, &world_x, &world_y, c);
                    goto update_changes;
                case '5': 
                case ' ': 
                case '.': 
                    break;
                default: goto update_changes;
            }
        } else {
            move_npc(m, c);
            if (police_caught_player) {
                break;
            }
        }

        // Update turn queue.
        cost = cost_table[c->type % 10][m->t[c->y][c->x].val];
        c->next_turn = cost + m->current_time;
        if (!heap_insert(&m->char_heap, c, c->next_turn)) {
            errmsg = "Failed to add character to heap."; 
            retval = -4; 
            goto cleanup; 
        }

        // Get next character.
        if (heap_extract_min(&m->char_heap, (void **)&c, &m->current_time)) {
            errmsg = "Failed getting next character."; 
            retval = -5; 
            goto cleanup; 
        }

        if (c->type == PC_TYPE) pc = (PC *)c;

        update_changes:
            output_map(m);
            mvprintw(22, 0, "Position: (%d, %d)  ", world_x - 200, world_y - 200);
            if (c->type == PC_TYPE) {
                int money = ((PC *) c)->money;
                std::string smoney = std::to_string(money);
                mvprintw(22, (WIDTH - (smoney.length() + 1)), "$%d", money);
                input = getch();
                // Check for game over
                if (is_pc_defeated(pc)) {
                    game_over_screen();
                    input = 'Q';
                }
            }
            refresh();

    } while (input != 'Q');

    if (police_caught_player) {
        arrested_screen();
    }

    endwin();

    cleanup:
        // Free all visited maps and their characters.
        for (int y = 0; y <= 400; y++) {
            for (int x = 0; x <= 400; x++) {
                if (!w->maps[y][x]) continue;
                Map *m = w->maps[y][x];
                // Free all characters on the map.
                for (int i = 0; i < HEIGHT; i++)
                    for (int j = 0; j < WIDTH; j++)
                        if (m->ch[i][j]) { delete m->ch[i][j]; m->ch[i][j] = NULL; }
                delete m;
                w->maps[y][x] = NULL;
            }
        }
        delete w;
        free_pokemon_data();

    ret_err:
        if (errmsg) fprintf(stderr, "Error: %s\n", errmsg);
        return retval;
}

// Returns an existing map or generates a new one.
Map *get_or_create_map(int world_x, int world_y) {
    if (world_x < 0 || world_x > 400 || world_y < 0 || world_y > 400) return NULL;

    if (w->maps[world_y][world_x]) return w->maps[world_y][world_x];

    Map *N = (world_y > 0)   ? w->maps[world_y-1][world_x] : NULL;
    Map *S = (world_y < 400) ? w->maps[world_y+1][world_x] : NULL;
    Map *E = (world_x < 400) ? w->maps[world_y][world_x+1] : NULL;
    Map *W = (world_x > 0)   ? w->maps[world_y][world_x-1] : NULL;


    if ( !(w->maps[world_y][world_x] = new Map(world_x, world_y, N, S, E, W)) ) {
        return NULL;
    }


    w->maps[world_y][world_x]->seed();

    w->maps[world_y][world_x]->place_paths_and_buildings();

    place_npcs(w->maps[world_y][world_x], num_trainers);

    // Generate blackmarket stock
    int distance = abs(world_x - 200) + abs(world_y - 200);
    if (distance > 50) {
        int min_l, max_l;
        if (distance <= 200) {
            min_l = 1;
            max_l = (distance < 2) ? 1 : distance / 2;
        } else {
            min_l = (distance - 200) / 2;
            max_l = 100;
        }
        if (min_l < 1) min_l = 1;
        if (max_l < min_l) max_l = min_l;

        int num_stock = 3 + (rand() % 3);  // 3-5 pokemon
        for (int i = 0; i < num_stock; i++) {
            int level = (min_l == max_l) ? min_l : min_l + rand() % (max_l - min_l + 1);
            w->maps[world_y][world_x]->blackmarket_stock.push_back(new Pokemon(level));
        }
    }

    return w->maps[world_y][world_x];
}

bool is_pc_defeated(Character *pc) {
    if (!pc || pc->type != PC_TYPE) return false;
    PC *player = (PC *)pc;
    for (size_t i = 0; i < player->pokemon.size(); i++) {
        if (player->pokemon[i]->get_current_hp() > 0) {
            return false;
        }
    }
    return true;
}

void game_over_screen() {
    clear();
    attrset(A_BOLD);
    int center_x = (WIDTH) / 2;
    int line = 8;
    
    mvprintw(line++, center_x, "========== GAME OVER ==========");
    line++;
    mvprintw(line++, center_x - 12, "You have been defeated!");
    line++;
    mvprintw(line++, center_x - 17, "All of your Pokémon have fainted.");
    line += 2;
    mvprintw(line++, center_x - 12, "Press any key to exit...");
    
    attrset(A_NORMAL);
    refresh();
    getch();
}

void arrested_screen() {
    clear();
    attrset(A_BOLD);
    int center_x = (WIDTH) / 2;
    int line = 8;
    
    mvprintw(line++, center_x, "========== GAME OVER ==========");
    line++;
    mvprintw(line++, center_x - 14, "You were arrested by police!");
    line++;
    mvprintw(line++, center_x - 5, "Game Over.");
    line += 2;
    mvprintw(line++, center_x - 12, "Press any key to exit...");
    
    attrset(A_NORMAL);
    refresh();
    getch();
}

void starter_selection(Character *pc) {
    const char* title = " Rogue Style Pokemon Game ";
    const char* subtitle = "Choose your starter!";

    WINDOW *win = newwin(HEIGHT, WIDTH, 0, 0);
    keypad(win, TRUE);
    nodelay(win, FALSE);

    int starters[3];
    starters[0] = pokemon_db[rand() % pokemon_db.size()]->id;
    starters[1] = pokemon_db[rand() % pokemon_db.size()]->id;
    starters[2] = pokemon_db[rand() % pokemon_db.size()]->id;

    Pokemon *p0 = new Pokemon(starters[0], 1);
    Pokemon *p1 = new Pokemon(starters[1], 1);
    Pokemon *p2 = new Pokemon(starters[2], 1);

    Pokemon *choices[3] = {p0, p1, p2};
    int choice = 0;

    while (true) {
        werase(win);
        wborder(win, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE,ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);

        // Title - blue
        int title_x = (WIDTH - (int)strlen(title)) / 2;
        wattron(win, COLOR_PAIR(COLOR_BLUE) | A_BOLD);
        mvwprintw(win, 1, title_x, "%s", title);
        wattroff(win, COLOR_PAIR(COLOR_BLUE) | A_BOLD);

        // Subtitle - white
        int sub_x = (WIDTH - (int)strlen(subtitle)) / 2;
        wattron(win, COLOR_PAIR(COLOR_WHITE));
        mvwprintw(win, 2, sub_x, "%s", subtitle);
        wattroff(win, COLOR_PAIR(COLOR_WHITE));

        // Three names spread across row 4
        int name_cols[3] = { 4, 30, 56 };
        for (int i = 0; i < 3; i++) {
            const char *name = choices[i]->get_species();
            if (i == choice) {
                wattron(win, A_BOLD | COLOR_PAIR(COLOR_RED));
                mvwprintw(win, 4, name_cols[i], "-> %s", name);
                wattroff(win, A_BOLD | COLOR_PAIR(COLOR_RED));
            } else {
                wattron(win, COLOR_PAIR(COLOR_BLUE));
                mvwprintw(win, 4, name_cols[i], "   %s", name);
                wattroff(win, COLOR_PAIR(COLOR_BLUE));
            }
        }

        Pokemon *p = choices[choice];

        // Column x positions
        int col[3] = { 10, 35, 56 };

        // --- Column 1: Base stats ---
        const char *stat_names_1[3] = { "HP     ", "Attack ", "Defense" };
        int          stat_vals_1[3] = { p->get_hp(), p->get_atk(), p->get_def() };
        const char *stat_names_2[3] = { "Sp.Atk ", "Sp.Def ", "Speed  " };
        int          stat_vals_2[3] = { p->get_spatk(), p->get_spdef(), p->get_speed() };


        for (int r = 0; r < 3; r++) {
            wattron(win, COLOR_PAIR(COLOR_BLUE));
            mvwprintw(win, 7 + r, col[0], "%s: ", stat_names_1[r]);
            wattroff(win, COLOR_PAIR(COLOR_BLUE));
            wattron(win, COLOR_PAIR(COLOR_RED));
            mvwprintw(win, 7 + r, col[0] + 10, "%-4d", stat_vals_1[r]);
            wattroff(win, COLOR_PAIR(COLOR_RED));
        }

        for (int r = 0; r < 3; r++) {
            wattron(win, COLOR_PAIR(COLOR_BLUE));
            mvwprintw(win, 10 + r, col[0], "%s: ", stat_names_2[r]);
            wattroff(win, COLOR_PAIR(COLOR_BLUE));
            wattron(win, COLOR_PAIR(COLOR_RED));
            mvwprintw(win, 10 + r, col[0] + 10, "%-4d", stat_vals_2[r]);
            wattroff(win, COLOR_PAIR(COLOR_RED));
        }

        // --- Column 2: IVs ---
        const char *iv_names_1[3] = { "HP ", "Atk", "Def" };
        int          iv_vals_1[3] = { p->get_iv_hp(), p->get_iv_atk(), p->get_iv_def() };
        const char *iv_names_2[3] = { "SpA", "SpD", "Spe" };
        int          iv_vals_2[3] = { p->get_iv_spatk(), p->get_iv_spdef(), p->get_iv_speed() };

        wattron(win, COLOR_PAIR(COLOR_BLUE) | A_BOLD);
        mvwprintw(win, 6, col[1], "IVs");
        wattroff(win, COLOR_PAIR(COLOR_BLUE) | A_BOLD);

        for (int r = 0; r < 3; r++) {
            wattron(win, COLOR_PAIR(COLOR_BLUE));
            mvwprintw(win, 7 + r, col[1], "%s: ", iv_names_1[r]);
            wattroff(win, COLOR_PAIR(COLOR_BLUE));
            wattron(win, COLOR_PAIR(COLOR_RED));
            mvwprintw(win, 7 + r, col[1] + 5, "%-2d", iv_vals_1[r]);
            wattroff(win, COLOR_PAIR(COLOR_RED));
        }

        for (int r = 0; r < 3; r++) {
            wattron(win, COLOR_PAIR(COLOR_BLUE));
            mvwprintw(win, 11 + r, col[1], "%s: ", iv_names_2[r]);
            wattroff(win, COLOR_PAIR(COLOR_BLUE));
            wattron(win, COLOR_PAIR(COLOR_RED));
            mvwprintw(win, 11 + r, col[1] + 5, "%-2d", iv_vals_2[r]);
            wattroff(win, COLOR_PAIR(COLOR_RED));
        }

        // --- Column 3: Info, Gender, Shiny, Moves ---
        wattron(win, COLOR_PAIR(COLOR_BLUE) | A_BOLD);
        mvwprintw(win, 6, col[2], "Info");
        wattroff(win, COLOR_PAIR(COLOR_BLUE) | A_BOLD);

        wattron(win, COLOR_PAIR(COLOR_BLUE));
        mvwprintw(win, 7, col[2], "Gender : ");
        wattroff(win, COLOR_PAIR(COLOR_BLUE));
        wattron(win, COLOR_PAIR(COLOR_RED));
        mvwprintw(win, 7, col[2] + 9, "%s", p->get_gender_string());
        wattroff(win, COLOR_PAIR(COLOR_RED));

        wattron(win, COLOR_PAIR(COLOR_BLUE));
        mvwprintw(win, 8, col[2], "Shiny  : ");
        wattroff(win, COLOR_PAIR(COLOR_BLUE));
        wattron(win, COLOR_PAIR(COLOR_RED));
        mvwprintw(win, 8, col[2] + 9, "%s", p->is_shiny() ? "Yes!" : "No");
        wattroff(win, COLOR_PAIR(COLOR_RED));

        wattron(win, COLOR_PAIR(COLOR_BLUE) | A_BOLD);
        mvwprintw(win, 10, col[2], "Moves");
        wattroff(win, COLOR_PAIR(COLOR_BLUE) | A_BOLD);

        for (int m = 0; m < 4; m++) {
            const char *move = p->get_move(m);
            wattron(win, COLOR_PAIR(COLOR_RED));
            mvwprintw(win, 11 + m, col[2] + 2, "%-24s", (move && move[0]) ? move : "---");
            wattroff(win, COLOR_PAIR(COLOR_RED));
        }

        // Hint - white
        const char *hint = "< LEFT / RIGHT >  to browse    ENTER to confirm";
        wattron(win, COLOR_PAIR(COLOR_WHITE));
        mvwprintw(win, HEIGHT - 2, (WIDTH - (int)strlen(hint)) / 2, "%s", hint);
        wattroff(win, COLOR_PAIR(COLOR_WHITE));

        wrefresh(win);

        int ch = wgetch(win);
        if (ch == KEY_LEFT || ch == KEY_UP)
            choice = (choice + 2) % 3;
        else if (ch == KEY_RIGHT || ch == KEY_DOWN)
            choice = (choice + 1) % 3;
        else if (ch == KEY_ENTER || ch == '\n') {
            for (int i = 0; i < 3; i++) {
                if (i == choice) pc->pokemon.push_back(choices[i]);
                else             delete choices[i];
            }
            break;
        }
    }

    delwin(win);
    clear();
    refresh();
}

int place_npcs(Map *m, int num_trainers) {
    int i, x, y;

    for (i = 0; i < num_trainers; i++) {
        char sym;
        char_type type;

        if (i == 0)      { type = HIKER;    sym = 'h'; }
        else if (i == 1) { type = RIVAL;    sym = 'r'; }
        else {
            switch (rand() % 6) {
                case 0: type = HIKER;    sym = 'h'; break;
                case 1: type = RIVAL;    sym = 'r'; break;
                case 2: type = PACER;    sym = 'p'; break;
                case 3: type = WANDERER; sym = 'w'; break;
                case 4: type = SENTRY;   sym = 's'; break;
                case 5: type = EXPLORER; sym = 'e'; break;
                default: type = OTHER;   sym = 'o'; break;
            }
        }

        // Try random placement first
        bool placed = false;
        for (int attempts = 0; attempts < 1000; attempts++) {
            x = 1 + rand() % (WIDTH - 2);
            y = 1 + rand() % (HEIGHT - 2);
            if (!m->ch[y][x] &&
                m->t[y][x].val != GATE &&
                m->t[y][x].val != POKECENTER &&
                m->t[y][x].val != POKEMART &&
                cost_table[type % 10][m->t[y][x].val] != __INT_MAX__) {
                placed = true;
                break;
            }
        }

        // Fall back to exhaustive search
        if (!placed) {
            for (int ty = 1; ty < HEIGHT - 1 && !placed; ty++) {
                for (int tx = 1; tx < WIDTH - 1 && !placed; tx++) {
                    if (!m->ch[ty][tx] &&
                        m->t[ty][tx].val != GATE &&
                        m->t[ty][tx].val != POKECENTER &&
                        m->t[ty][tx].val != POKEMART &&
                        cost_table[type % 10][m->t[ty][tx].val] != __INT_MAX__) {
                        x = tx;
                        y = ty;
                        placed = true;
                    }
                }
            }
        }

        // Skip this NPC if no valid position exists
        if (!placed) continue;

        NPC *c = new NPC(sym, type, x, y);
        c->seq_num = i + 1;
        c->next_turn = 0;

        // Generate pokemon for NPC
        int distance = abs(m->pos_x - 200) + abs(m->pos_y - 200);
        int min_l, max_l;
        if (distance <= 200) {
            min_l = 1;
            max_l = (distance < 2) ? 1 : distance / 2;
        } else {
            min_l = (distance - 200) / 2;
            max_l = 100;
        }
        if (min_l < 1) min_l = 1;
        if (max_l < min_l) max_l = min_l;

        int level = (min_l == max_l) ? min_l : min_l + rand() % (max_l - min_l + 1);
        c->pokemon.push_back(new Pokemon(level));
        while (c->pokemon.size() < 6 && rand() % 100 < 60) {
            level = (min_l == max_l) ? min_l : min_l + rand() % (max_l - min_l + 1);
            c->pokemon.push_back(new Pokemon(level));
        }

        if (type == PACER || type == WANDERER) {
            if (rand() % 2) { c->dir[0] = (rand() % 2) ? 1 : -1; c->dir[1] = 0; }
            else             { c->dir[0] = 0; c->dir[1] = (rand() % 2) ? 1 : -1; }
        } else {
            c->dir[0] = (rand() % 3) - 1;
            c->dir[1] = (rand() % 3) - 1;
        }

        heap_insert(&m->char_heap, c, c->next_turn);
        m->ch[y][x] = c;
    }
    return 0;
}


bool wild_encounter(Character *pc, Pokemon *p) {
    return battle_loop(pc, NULL, p);
}

// Handles gate traversal and normal movement.
int move_pc(Map **m_ptr, int *world_x, int *world_y, Character *player, int dy, int dx) {
    Map *m = *m_ptr;
    int ny = player->y + dy;
    int nx = player->x + dx;
    const char *errmsg;

    // Check for gate traversal.
    if (m->t[ny][nx].val == GATE) {
        int new_world_x = *world_x, new_world_y = *world_y;
        int new_px, new_py;

        // Determine which gate and new position in neighbor map.
        if (nx == 0) {
            // West gate
            if (*world_x == 0) { mvprintw(0, 0, "At the edge of the world!"); return -1; }
            new_world_x = *world_x - 1;
            new_world_y = *world_y;
            new_px = WIDTH - 2;
            new_py = player->y;
        } else if (nx == WIDTH - 1) {
            // East gate
            if (*world_x == 400) { mvprintw(0, 0, "At the edge of the world!"); return -1; }
            new_world_x = *world_x + 1;
            new_world_y = *world_y;
            new_px = 1;
            new_py = player->y;
        } else if (ny == 0) {
            // North gate
            if (*world_y == 0) { mvprintw(0, 0, "At the edge of the world!"); return -1; }
            new_world_x = *world_x;
            new_world_y = *world_y - 1;
            new_px = player->x;
            new_py = HEIGHT - 2;
        } else {
            // South gate
            if (*world_y == 400) { mvprintw(0, 0, "At the edge of the world!"); return -1; }
            new_world_x = *world_x;
            new_world_y = *world_y + 1;
            new_px = player->x;
            new_py = 1;
        }

        // Get or create the neighbor map.
        Map *new_map = get_or_create_map(new_world_x, new_world_y);
        if (!new_map) { mvprintw(0, 0, "Can't go there!"); return -1; }


        m->ch[player->y][player->x] = NULL;

        // Place PC on new map.
        player->x = new_px;
        player->y = new_py;
        new_map->ch[new_py][new_px] = player;

        // Add PC to new map's heap.
        player->next_turn = new_map->current_time;
        heap_insert(&new_map->char_heap, player, player->next_turn);

        // Update dijkstra for new map.
        dijiksra(w->hiker_dist_map, new_map, &new_map->t[player->y][player->x], HIKER);
        dijiksra(w->rival_dist_map, new_map, &new_map->t[player->y][player->x], RIVAL);

        *world_x = new_world_x;
        *world_y = new_world_y;
        *m_ptr = new_map;
        return 0;
    }

    // Normal movement.
    if (m->ch[ny][nx]) {
        NPC *target = (NPC *)m->ch[ny][nx];
        if (!target->is_defeated) battle_interface(player, target);
        return -1;
    }

    if (cost_table[PC_TYPE % 10][m->t[ny][nx].val] < __INT_MAX__) {
        m->ch[player->y][player->x] = NULL;
        m->ch[ny][nx] = player;
        player->y = ny;
        player->x = nx;

        dijiksra(w->hiker_dist_map, m, &m->t[player->y][player->x], HIKER);
        dijiksra(w->rival_dist_map, m, &m->t[player->y][player->x], RIVAL);

        // If we are standing on tall grass, check if pokemon is there.
        if (m->t[ny][nx].val == TALL_GRASS && rand() % 10 == 0) {
            int distance = abs(*world_x - 200) + abs(*world_y - 200);
            int min_l, max_l;
            if (distance <= 200) {
                min_l = 1;
                max_l = (distance < 2) ? 1 : distance / 2;
            } else {
                min_l = (distance - 200) / 2;
                max_l = 100;
            }
            if (min_l < 1) min_l = 1;
            if (max_l < min_l) max_l = min_l;

            int level = (min_l == max_l) ? min_l : min_l + rand() % (max_l - min_l + 1);
            Pokemon *p = new Pokemon(level);
            if (!wild_encounter(player, p)) delete p;
        }
    } else {
        switch (m->t[ny][nx].val) {
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
                errmsg = "there";               
                break;
        }
        mvprintw(0, 0, "You can't walk through %s!", errmsg);
        return -3;
    }

    return 0;
}


void fly_command(Map **m_ptr, int *world_x, int *world_y, Character *pc) {
    if ((*m_ptr)->is_wanted) {
        mvprintw(0, 0, "You can't fly - you're wanted on this map!");
        return;
    }

    char buf_x[16], buf_y[16];
    int target_x, target_y;

    echo();
    curs_set(1);

    mvprintw(0, 0, "Fly X (-200 to 200): ");
    clrtoeol();
    refresh();
    getnstr(buf_x, 15);

    mvprintw(0, 0, "Fly Y (-200 to 200): ");
    clrtoeol();
    refresh();
    getnstr(buf_y, 15);

    noecho();
    curs_set(0);

    mvprintw(0, 0, " ");
    clrtoeol();

    target_x = atoi(buf_x) + 200;
    target_y = atoi(buf_y) + 200;

    if (target_x < 0 || target_x > 400 || target_y < 0 || target_y > 400) {
        mvprintw(0, 0, "Invalid coordinates! Must be between -200 and 200.");
        return;
    }

    Map *old_map = *m_ptr;
    old_map->ch[pc->y][pc->x] = NULL;

    Map *new_map = get_or_create_map(target_x, target_y);
    if (!new_map) {
        mvprintw(0, 0, "Couldn't fly there!");
        old_map->ch[pc->y][pc->x] = pc;
        return;
    }

    int new_px = new_map->WE_intersection;
    int new_py = new_map->pW;

    pc->x = new_px;
    pc->y = new_py;
    new_map->ch[new_py][new_px] = pc;

    pc->next_turn = new_map->current_time;
    heap_insert(&new_map->char_heap, pc, pc->next_turn);

    dijiksra(w->hiker_dist_map, new_map, &new_map->t[pc->y][pc->x], HIKER);
    dijiksra(w->rival_dist_map, new_map, &new_map->t[pc->y][pc->x], RIVAL);

    *world_x = target_x;
    *world_y = target_y;
    *m_ptr = new_map;
}


void move_npc(Map *m, Character *c) {
    int dy, dx, new_y, new_x;
    int next_x = c->x, next_y = c->y;
    int min_dist = __INT_MAX__;
    NPC *npc = (NPC *)c;

    // Check if PC is on the map and if Police caught them
    if (c->type == POLICE) {
        for (dy = 0; dy < HEIGHT; dy++) {
            for (dx = 0; dx < WIDTH; dx++) {
                if (m->ch[dy][dx] && m->ch[dy][dx]->type == PC_TYPE) {
                    if (c->x == dx && c->y == dy) {
                        // PC caught by Police!
                        police_caught_player = true;
                        return;
                    }
                }
            }
        }
    }

    // Handle stun state
    if (npc->stun_turns_remaining > 0) {
        npc->stun_turns_remaining--;
        return;
    }

    char_type type = npc->is_defeated ? EXPLORER : c->type;

    if (type == SENTRY) return;

    if (type == HIKER || type == RIVAL || type == POLICE) {
        int (*dist_map)[WIDTH] = NULL;
        int current_dist = __INT_MAX__;
        
        if (type == HIKER) {
            dist_map = w->hiker_dist_map;
        } else if (type == RIVAL) {
            dist_map = w->rival_dist_map;
        } else if (type == POLICE) {
            // Police pathfind toward PC
            dist_map = new int[HEIGHT][WIDTH];
            for (int i = 0; i < HEIGHT; i++) {
                for (int j = 0; j < WIDTH; j++) {
                    dist_map[i][j] = __INT_MAX__;
                }
            }
            
            // Find PC position
            int pc_y = -1, pc_x = -1;
            for (int i = 0; i < HEIGHT; i++) {
                for (int j = 0; j < WIDTH; j++) {
                    if (m->ch[i][j] && m->ch[i][j]->type == PC_TYPE) {
                        pc_y = i;
                        pc_x = j;
                        break;
                    }
                }
                if (pc_y != -1) break;
            }
            
            if (pc_y == -1 || pc_x == -1) {
                delete[] dist_map;
                return;  // PC not on this map
            }
            
            // Run dijkstra from PC position
            dijiksra(dist_map, m, &m->t[pc_y][pc_x], POLICE);
        }

        current_dist = dist_map[c->y][c->x];

        for (dy = -1; dy <= 1; dy++) {
            for (dx = -1; dx <= 1; dx++) {
                if (dx == 0 && dy == 0) continue;
                new_y = c->y + dy;
                new_x = c->x + dx;
                if (new_y < 0 || new_y >= HEIGHT || new_x < 0 || new_x >= WIDTH) continue;
                if (m->ch[new_y][new_x] && m->ch[new_y][new_x]->type == PC_TYPE) {
                    if (type == POLICE) {
                        if (type == POLICE) delete[] dist_map;
                        police_caught_player = true;
                        return;
                    }
                    if (!npc->is_defeated) battle_interface(m->ch[new_y][new_x], c);
                    if (type == POLICE) delete[] dist_map;
                    return;
                }
                if (dist_map[new_y][new_x] < current_dist && dist_map[new_y][new_x] < min_dist && !m->ch[new_y][new_x]) {
                    min_dist = dist_map[new_y][new_x];
                    next_x = new_x;
                    next_y = new_y;
                }
            }
        }
        
        if (type == POLICE) delete[] dist_map;
    } else if (type == PACER) {
        new_x = c->x + c->dir[0];
        new_y = c->y + c->dir[1];
        if (m->ch[new_y][new_x] || cost_table[OTHER%10][m->t[new_y][new_x].val] == __INT_MAX__ || m->t[new_y][new_x].val == GATE) {
            c->dir[0] *= -1; c->dir[1] *= -1;
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
            next_x = new_x; next_y = new_y;
        }
    } else if (type == EXPLORER) {
        new_x = c->x + c->dir[0];
        new_y = c->y + c->dir[1];
        if (cost_table[c->type%10][m->t[new_y][new_x].val] == __INT_MAX__ || m->ch[new_y][new_x] || m->t[new_y][new_x].val == GATE) {
            c->dir[0] = (rand() % 3) - 1;
            c->dir[1] = (rand() % 3) - 1;
            if (c->dir[0] == 0 && c->dir[1] == 0) c->dir[0] = 1;
        } else {
            next_x = new_x; next_y = new_y;
        }
    }

    // NPCs cannot move into gates.
    if (m->t[next_y][next_x].val == GATE) return;

    if (!m->ch[next_y][next_x]) {
        m->ch[c->y][c->x] = NULL;
        c->x = next_x;
        c->y = next_y;
        m->ch[c->y][c->x] = c;
    }

    // NPC theft attempt (only for non-Police NPCs)
    if (c->type != POLICE) {
        if ((rand() % 100) < 2) {  // 2% chance
            // Find PC
            Character *pc = NULL;
            for (int i = 0; i < HEIGHT; i++) {
                for (int j = 0; j < WIDTH; j++) {
                    if (m->ch[i][j] && m->ch[i][j]->type == PC_TYPE) {
                        // Check if adjacent
                        int dy_adj = abs(i - c->y);
                        int dx_adj = abs(j - c->x);
                        if (dy_adj <= 1 && dx_adj <= 1) {
                            pc = m->ch[i][j];
                            break;
                        }
                    }
                }
                if (pc) break;
            }

            if (pc && ((PC *)pc)->pokemon.size() > 0) {
                PC *player = (PC *)pc;
                int stolen_idx = rand() % player->pokemon.size();
                Pokemon *target = player->pokemon[stolen_idx];

                if ((rand() % 100) < 30) {
                    // Caught!
                    npc->is_defeated = true;
                    mvprintw(0, 0, "A trainer tried to steal %s but was caught!", target->get_species());
                } else {
                    // Success!
                    player->pokemon.erase(player->pokemon.begin() + stolen_idx);
                    mvprintw(0, 0, "A trainer stole your %s!", target->get_species());
                }
            }
        }
    }
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
    init_pair(9,             COLOR_BLUE,    COLOR_RED);
    init_pair(COLOR_BLUE,    COLOR_BLUE,    COLOR_BLACK);
    init_pair(COLOR_RED,     COLOR_RED,     COLOR_BLACK);
    init_pair(COLOR_MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(COLOR_BLACK,   COLOR_BLACK,   COLOR_BLACK);
    init_pair(8,             COLOR_BLACK,   COLOR_WHITE);

    bkgd(COLOR_PAIR(COLOR_WHITE));
    refresh();
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
                    case GATE:       attron(COLOR_PAIR(COLOR_YELLOW)); mvprintw(y+1, x, "#"); break;
                    case PATH:       attron(COLOR_PAIR(COLOR_YELLOW)); mvprintw(y+1, x, "#"); break;
                    case CLEARING:   attron(COLOR_PAIR(COLOR_GREEN));  mvprintw(y+1, x, "."); break;
                    case TALL_GRASS: attron(COLOR_PAIR(COLOR_GREEN));  mvprintw(y+1, x, ":"); break;
                    case ARCTIC:     attron(COLOR_PAIR(COLOR_WHITE));  mvprintw(y+1, x, "*"); break;
                    case LAKE:       attron(COLOR_PAIR(COLOR_BLUE));   mvprintw(y+1, x, "~"); break;
                    case FOREST:     attron(COLOR_PAIR(COLOR_GREEN));  mvprintw(y+1, x, "^"); break;
                    case MOUNTAIN:   attron(COLOR_PAIR(COLOR_WHITE));  mvprintw(y+1, x, "%c", '%'); break;
                    case POKECENTER: attron(COLOR_PAIR(COLOR_RED));    mvprintw(y+1, x, "C"); break;
                    case POKEMART:   attron(COLOR_PAIR(COLOR_RED));    mvprintw(y+1, x, "M"); break;
                    case BOULDER:    attron(COLOR_PAIR(COLOR_WHITE));  mvprintw(y+1, x, "%c", '%'); break;
                    case BLACKMARKET: attron(COLOR_PAIR(COLOR_MAGENTA)); mvprintw(y+1, x, "B"); break;
                    default:         attron(COLOR_PAIR(COLOR_BLACK));  mvprintw(y+1, x, " "); break;
                }
            }
        }
    }
    attron(COLOR_PAIR(COLOR_WHITE));
}

void output_map_flash(Map *m, bool flash[HEIGHT][WIDTH]) {
    int y, x;

    for (y = 0; y < HEIGHT; y++) {
        for (x = 0; x < WIDTH; x++) {
            if (flash[y][x]) {
                attron(COLOR_PAIR(8));
                if (m->ch[y][x]) {
                    mvprintw(y + 1, x, "%c", m->ch[y][x]->symbol);
                } else {
                    switch (m->t[y][x].val) {
                        case GATE:       mvprintw(y+1, x, "#"); break;
                        case PATH:       mvprintw(y+1, x, "#"); break;
                        case CLEARING:   mvprintw(y+1, x, "."); break;
                        case TALL_GRASS: mvprintw(y+1, x, ":"); break;
                        case ARCTIC:     mvprintw(y+1, x, "*"); break;
                        case LAKE:       mvprintw(y+1, x, "~"); break;
                        case FOREST:     mvprintw(y+1, x, "^"); break;
                        case MOUNTAIN:   mvprintw(y+1, x, "%c", '%'); break;
                        case POKECENTER: mvprintw(y+1, x, "C"); break;
                        case POKEMART:   mvprintw(y+1, x, "M"); break;
                        case BOULDER:    mvprintw(y+1, x, "%c", '%'); break;
                        case BLACKMARKET: mvprintw(y+1, x, "B"); break;
                        default:         mvprintw(y+1, x, " "); break;
                    }
                }
            } else if (m->ch[y][x]) {
                attron(COLOR_PAIR(COLOR_MAGENTA));
                mvprintw(y + 1, x, "%c", m->ch[y][x]->symbol);
            } else {
                switch (m->t[y][x].val) {
                    case GATE:       attron(COLOR_PAIR(COLOR_YELLOW)); mvprintw(y+1, x, "#"); break;
                    case PATH:       attron(COLOR_PAIR(COLOR_YELLOW)); mvprintw(y+1, x, "#"); break;
                    case CLEARING:   attron(COLOR_PAIR(COLOR_GREEN));  mvprintw(y+1, x, "."); break;
                    case TALL_GRASS: attron(COLOR_PAIR(COLOR_GREEN));  mvprintw(y+1, x, ":"); break;
                    case ARCTIC:     attron(COLOR_PAIR(COLOR_WHITE));  mvprintw(y+1, x, "*"); break;
                    case LAKE:       attron(COLOR_PAIR(COLOR_BLUE));   mvprintw(y+1, x, "~"); break;
                    case FOREST:     attron(COLOR_PAIR(COLOR_GREEN));  mvprintw(y+1, x, "^"); break;
                    case MOUNTAIN:   attron(COLOR_PAIR(COLOR_WHITE));  mvprintw(y+1, x, "%c", '%'); break;
                    case POKECENTER: attron(COLOR_PAIR(COLOR_RED));    mvprintw(y+1, x, "C"); break;
                    case POKEMART:   attron(COLOR_PAIR(COLOR_RED));    mvprintw(y+1, x, "M"); break;
                    case BOULDER:    attron(COLOR_PAIR(COLOR_WHITE));  mvprintw(y+1, x, "%c", '%'); break;
                    case BLACKMARKET: attron(COLOR_PAIR(COLOR_MAGENTA)); mvprintw(y+1, x, "B"); break;
                    default:         attron(COLOR_PAIR(COLOR_BLACK));  mvprintw(y+1, x, " "); break;
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
        y = t->y; x = t->x;

        TRY_NEIGHBOR(NW, y-1, x-1)
        TRY_NEIGHBOR(N,  y-1, x)
        TRY_NEIGHBOR(NE, y-1, x+1)
        TRY_NEIGHBOR(E,  y,   x+1)
        TRY_NEIGHBOR(SE, y+1, x+1)
        TRY_NEIGHBOR(S,  y+1, x)
        TRY_NEIGHBOR(SW, y+1, x-1)
        TRY_NEIGHBOR(W,  y,   x-1)
    }

    for (int i = 0; i < HEIGHT; i++)
        for (int j = 0; j < WIDTH; j++)
            ref_table[i][j] = NULL;

    heap_destroy(&h);
    return 0;
}

void stun_grenade_handler(Map *m, Character *pc_char) {
    PC *pc = (PC *)pc_char;
    if (pc->stun_grenades <= 0) {
        mvprintw(0, 0, "You have no stun grenades!");
        return;
    }

    bool flash[HEIGHT][WIDTH] = {{false}};
    bool stunned_any = false;

    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            int target_y = pc->y + dy;
            int target_x = pc->x + dx;
            if (target_y < 0 || target_y >= HEIGHT || target_x < 0 || target_x >= WIDTH) continue;
            flash[target_y][target_x] = true;
            if (m->ch[target_y][target_x] && m->ch[target_y][target_x]->type != PC_TYPE) {
                NPC *target = (NPC *)m->ch[target_y][target_x];
                target->stun_turns_remaining = 5;
                stunned_any = true;
            }
        }
    }

    clear();
    output_map_flash(m, flash);
    mvprintw(0, 0, "Stun grenade detonated!");
    refresh();
    napms(120);

    clear();
    output_map(m);
    mvprintw(0, 0, stunned_any ? "Nearby NPCs were stunned!" : "No NPCs were in range.");
    refresh();
    napms(120);

    if (stunned_any) {
        pc->stun_grenades--;
    }
}

void steal_handler(Map *m, Character *pc_char) {
    int key;

    NPC *npc = NULL;
    PC *pc = (PC *)pc_char;

    // Find adjacent stunned NPCs
    int dy, dx, target_y, target_x;
    NPC *target_npc = NULL;

    for (dy = -1; dy <= 1; dy++) {
        for (dx = -1; dx <= 1; dx++) {
            if (dy == 0 && dx == 0) continue;
            target_y = pc->y + dy;
            target_x = pc->x + dx;
            if (target_y < 0 || target_y >= HEIGHT || target_x < 0 || target_x >= WIDTH) continue;
            if (m->ch[target_y][target_x] && m->ch[target_y][target_x]->type != PC_TYPE) {
                npc = (NPC *)m->ch[target_y][target_x];
                if (npc->stun_turns_remaining > 0) {
                    target_npc = npc;
                    break;
                }
            }
        }
        if (target_npc) break;
    }

    if (!target_npc) {
        mvprintw(0, 0, "No stunned NPC adjacent to you!");
        return;
    }

    if (pc->stolen_pokemon.size() >= 3) {
        mvprintw(0, 0, "Your stolen Pokemon bag is full! (Max 3)");
        return;
    }

    // Show NPC's pokemon
    key = 0;
    clear();
    mvprintw(0, 0, "Select a Pokemon to steal from %c (ESC to cancel):", target_npc->symbol);
    for (size_t i = 0; i < target_npc->pokemon.size(); i++) {
        mvprintw(2 + i, 2, "%lu. %s (Lvl %d)", i + 1, target_npc->pokemon[i]->get_species(), target_npc->pokemon[i]->get_level());
    }
    refresh();
    key = getch();

    while (key != 27) {
        if (key >= '1' && (size_t)(key - '1') < target_npc->pokemon.size()) {
            int idx = key - '1';
            Pokemon *stolen = target_npc->pokemon[idx];
            
            // Remove from NPC's party
            target_npc->pokemon.erase(target_npc->pokemon.begin() + idx);
            
            // Add to player's stolen collection
            pc->stolen_pokemon.push_back(stolen);
            
            // Mark as wanted
            m->is_wanted = true;
            
            // Spawn police
            spawn_police(m, pc_char);
            
            mvprintw(15, 0, "You stole %s! The Police are onto you!", stolen->get_species());
            refresh();
            getch();
            break;
        }
    }
}

int spawn_police(Map *m, Character *pc) {
    // Ensure at least 2 police spawn (2-4 total)
    int num_police = 2 + (rand() % 3);  // 2-4 police
    
    for (int i = 0; i < num_police; i++) {
        spawn_police_npc(m, pc);
    }
    return 0;
}

int spawn_police_npc(Map *m, Character *pc) {
    int spawn_x = -1, spawn_y = -1;
    int attempts = 0;

    // Try to spawn at map edges
    while (attempts < 100) {
        int edge = rand() % 4;
        if (edge == 0) {  // Top
            spawn_y = 1;
            spawn_x = 1 + rand() % (WIDTH - 2);
        } else if (edge == 1) {  // Bottom
            spawn_y = HEIGHT - 2;
            spawn_x = 1 + rand() % (WIDTH - 2);
        } else if (edge == 2) {  // Left
            spawn_x = 1;
            spawn_y = 1 + rand() % (HEIGHT - 2);
        } else {  // Right
            spawn_x = WIDTH - 2;
            spawn_y = 1 + rand() % (HEIGHT - 2);
        }

        if (!m->ch[spawn_y][spawn_x] && cost_table[5][m->t[spawn_y][spawn_x].val] != __INT_MAX__) {
            break;
        }
        attempts++;
    }

    if (attempts >= 100) return -1;

    NPC *police = new NPC('P', POLICE, spawn_x, spawn_y);
    m->ch[spawn_y][spawn_x] = police;
    m->police_list.push_back(police);
    
    // Add to heap. Delay their first turn slightly so the player regains control
    // and the UI doesn't get flooded immediately after a theft. Shorter delay
    // so police can start chasing reasonably soon.
    police->next_turn = m->current_time + 20; // shorter delay
    heap_insert(&m->char_heap, police, police->next_turn);
    
    // Compute dijkstra for police
    int (*police_dist_map)[WIDTH] = new int[HEIGHT][WIDTH];
    dijiksra(police_dist_map, m, &m->t[pc->y][pc->x], POLICE);
    delete[] police_dist_map;

    return 0;
}

void blackmarket_interface(Character *c, Map *m) {
    PC *pc = (PC *)c;
    int input = 0;
    
    while (input != '3') {
        clear();
        mvprintw(1, 0, "Welcome to the Black Market.");
        mvprintw(2, 0, "Your Money: $%d", pc->money);
        mvprintw(4, 0, "1. Sell Stolen Pokemon");
        mvprintw(5, 0, "2. Buy Pokemon");
        mvprintw(6, 0, "3. Exit (<)");
        refresh();
        input = getch();

        if (input == '1') {
            // Sell stolen pokemon
            if (pc->stolen_pokemon.empty()) {
                mvprintw(8, 0, "You have no stolen Pokemon to sell!");
                refresh();
                getch();
            } else {
                int s_key = 0;
                while (s_key != 27) {
                    clear();
                    mvprintw(0, 0, "Select a Pokemon to sell (ESC to cancel):");
                    for (size_t i = 0; i < pc->stolen_pokemon.size(); i++) {
                        int price = pc->stolen_pokemon[i]->get_level() * 20;
                        mvprintw(2 + i, 2, "%lu. %s (Lvl %d) - $%d", i + 1, pc->stolen_pokemon[i]->get_species(), pc->stolen_pokemon[i]->get_level(), price);
                    }
                    refresh();
                    s_key = getch();
                    
                    if (s_key >= '1' && (size_t)(s_key - '1') < pc->stolen_pokemon.size()) {
                        int idx = s_key - '1';
                        int price = pc->stolen_pokemon[idx]->get_level() * 20;
                        pc->money += price;
                        
                        delete pc->stolen_pokemon[idx];
                        pc->stolen_pokemon.erase(pc->stolen_pokemon.begin() + idx);
                        
                        mvprintw(15, 0, "Sold for $%d! Total money: $%d", price, pc->money);
                        refresh();
                        getch();
                        break;
                    }
                }
            }
        } else if (input == '2') {
            // Buy pokemon
            if (m->blackmarket_stock.empty()) {
                mvprintw(8, 0, "This black market has no Pokemon to sell!");
                refresh();
                getch();
            } else {
                int b_key = 0;
                while (b_key != 27) {
                    clear();
                    mvprintw(0, 0, "Select a Pokemon to buy (ESC to cancel). Your money: $%d", pc->money);
                    for (size_t i = 0; i < m->blackmarket_stock.size(); i++) {
                        int price = m->blackmarket_stock[i]->get_level() * 50;
                        const char *afford = (pc->money >= price) ? "" : " (Can't afford)";
                        mvprintw(2 + i, 2, "%lu. %s (Lvl %d) - $%d%s", i + 1, m->blackmarket_stock[i]->get_species(), m->blackmarket_stock[i]->get_level(), price, afford);
                    }
                    refresh();
                    b_key = getch();
                    
                    if (b_key >= '1' && (size_t)(b_key - '1') < m->blackmarket_stock.size()) {
                        int idx = b_key - '1';
                        int price = m->blackmarket_stock[idx]->get_level() * 50;
                        
                        if (pc->money < price) {
                            mvprintw(15, 0, "You can't afford that!");
                            refresh();
                            getch();
                        } else {
                            pc->money -= price;
                            Pokemon *bought = m->blackmarket_stock[idx];
                            m->blackmarket_stock.erase(m->blackmarket_stock.begin() + idx);
                            
                            if (pc->pokemon.size() < 6) {
                                pc->pokemon.push_back(bought);
                                mvprintw(15, 0, "Purchased %s! Your money: $%d", bought->get_species(), pc->money);
                            } else {
                                pc->stored_pokemon.push_back(bought);
                                mvprintw(15, 0, "Party full! %s sent to storage. Your money: $%d", bought->get_species(), pc->money);
                            }
                            refresh();
                            getch();
                            break;
                        }
                    }
                }
            }
        }
    }
}

void enter_building(Character *c, const char *building_name) {
    PC *pc = (PC *)c;
    if (!strcmp(building_name, "Pokemart")) {
        pc->potions = 5;
        pc->revives = 5;
        pc->pokeballs = 5;
        pc->stun_grenades = 3;
    } else if (!strcmp(building_name, "Pokemon Center")) {
        for (auto p : pc->pokemon) {
            p->set_current_hp(p->get_hp());
        }
    }

    int input = 0;
    while (input != '<') {
        clear();
        if (!strcmp(building_name, "Pokemon Center")) {
            mvprintw(1, 0, "Welcome to the Pokemon Center!");
            mvprintw(2, 0, "1. Heal all Pokemon");
            mvprintw(3, 0, "2. Deposit Pokemon");
            mvprintw(4, 0, "3. Withdraw Pokemon");
            mvprintw(5, 0, "4. Exit (<)");
            mvprintw(6, 0, "Party: %zu/6  Storage: %zu", pc->pokemon.size(), pc->stored_pokemon.size());
            refresh();
            input = getch();

            if (input == '1') {
                // Heal all Pokemon
                for (auto p : pc->pokemon) {
                    p->set_current_hp(p->get_hp());
                }
                mvprintw(8, 0, "Your Pokemon have been fully healed!");
                refresh();
                getch();
            } else if (input == '2') {
                // Deposit Pokemon
                int d_key = 0;
                while (d_key != 27) {
                    clear();
                    mvprintw(1, 0, "Select a Pokemon to deposit (ESC to cancel):");
                    for (size_t i = 0; i < pc->pokemon.size(); i++) {
                        mvprintw(2 + i, 2, "%lu. %s (Lvl %d)", i + 1, pc->pokemon[i]->get_species(), pc->pokemon[i]->get_level());
                    }
                    refresh();
                    d_key = getch();
                    if (d_key >= '1' && (size_t)(d_key - '1') < pc->pokemon.size()) {
                        int idx = d_key - '1';
                        pc->stored_pokemon.push_back(pc->pokemon[idx]);
                        pc->pokemon.erase(pc->pokemon.begin() + idx);
                        mvprintw(15, 0, "Pokemon deposited!");
                        refresh();
                        getch();
                        break;
                    }
                }
            } else if (input == '3') {
                // Withdraw Pokemon
                if (pc->pokemon.size() >= 6) {
                    mvprintw(8, 0, "Your party is full!");
                    refresh();
                    getch();
                } else {
                    int w_key = 0;
                    while (w_key != 27) {
                        clear();
                        mvprintw(1, 0, "Select a Pokemon to withdraw (ESC to cancel):");
                        for (size_t i = 0; i < pc->stored_pokemon.size(); i++) {
                            mvprintw(2 + i, 2, "%lu. %s (Lvl %d)", i + 1, pc->stored_pokemon[i]->get_species(), pc->stored_pokemon[i]->get_level());
                        }
                        refresh();
                        w_key = getch();
                        if (w_key >= '1' && (size_t)(w_key - '1') < pc->stored_pokemon.size()) {
                            int idx = w_key - '1';
                            pc->pokemon.push_back(pc->stored_pokemon[idx]);
                            pc->stored_pokemon.erase(pc->stored_pokemon.begin() + idx);
                            mvprintw(15, 0, "Pokemon withdrawn!");
                            refresh();
                            getch();
                            break;
                        }
                    }
                }
            } else if (input == '4') {
                input = '<';
            }
        } else {
            mvprintw(1, 0, "Welcome to the %s!", building_name);
            if (!strcmp(building_name, "Pokemart")) {
                mvprintw(2, 0, "Your items have been restocked!");
            } else {
                mvprintw(2, 0, "Your Pokemon have been fully healed!");
            }
            mvprintw(4, 0, "Press '<' to exit.");
            refresh();
            input = getch();
        }
    }
}

void bag_interface(Character *c) {
    PC *pc = (PC *)c;
    int key = 0;
    while (key != 27) {
        clear();
        mvprintw(1, 10, "INVENTORY");
        mvprintw(2, 10, "Money: $%d", pc->money);
        mvprintw(3, 12, "1. Potions: %d", pc->potions);
        mvprintw(4, 12, "2. Revives: %d", pc->revives);
        mvprintw(5, 12, "3. Pokeballs: %d", pc->pokeballs);
        mvprintw(6, 12, "4. Stun Grenades: %d", pc->stun_grenades);
        mvprintw(8, 10, "Select an item to use on a Pokemon, or ESC to exit.");
        refresh();
        key = getch();

        if (key == '1') {
            if (pc->potions > 0) {
                // Select pokemon to heal
                int p_key = 0;
                while (p_key != 27) {
                    clear();
                    mvprintw(1, 10, "Select a Pokemon to use Potion on:");
                    for (size_t i = 0; i < pc->pokemon.size(); i++) {
                        mvprintw(3 + i, 12, "%lu. %s (HP: %d/%d)", i + 1, pc->pokemon[i]->get_species(), pc->pokemon[i]->get_current_hp(), pc->pokemon[i]->get_hp());
                    }
                    refresh();
                    p_key = getch();
                    if (p_key >= '1' && (size_t)(p_key - '1') < pc->pokemon.size()) {
                        int idx = p_key - '1';
                        if (pc->pokemon[idx]->get_current_hp() > 0 && pc->pokemon[idx]->get_current_hp() < pc->pokemon[idx]->get_hp()) {
                            int new_hp = pc->pokemon[idx]->get_current_hp() + 20;
                            if (new_hp > pc->pokemon[idx]->get_hp()) new_hp = pc->pokemon[idx]->get_hp();
                            pc->pokemon[idx]->set_current_hp(new_hp);
                            pc->potions--;
                            break;
                        }
                    }
                }
            }
        } else if (key == '2') {
            if (pc->revives > 0) {
                int p_key = 0;
                while (p_key != 27) {
                    clear();
                    mvprintw(1, 10, "Select a Pokemon to use Revive on:");
                    for (size_t i = 0; i < pc->pokemon.size(); i++) {
                        mvprintw(3 + i, 12, "%lu. %s (HP: %d/%d)", i + 1, pc->pokemon[i]->get_species(), pc->pokemon[i]->get_current_hp(), pc->pokemon[i]->get_hp());
                    }
                    refresh();
                    p_key = getch();
                    if (p_key >= '1' && (size_t)(p_key - '1') < pc->pokemon.size()) {
                        int idx = p_key - '1';
                        if (pc->pokemon[idx]->get_current_hp() == 0) {
                            pc->pokemon[idx]->set_current_hp(pc->pokemon[idx]->get_hp() / 2);
                            pc->revives--;
                            break;
                        }
                    }
                }
            }
        }
    }
}


void trainer_list(Map *m, Character *pc, int NUM_TRAINERS) {
    int i, j, dy, dx, count = 0;
    std::vector<Character *> trainers(NUM_TRAINERS);
    int offset = 0, key = 0, max_display = 20;
    const char *vert[] = {"north", "south"};
    const char *hori[] = {"west",  "east"};

    for (i = 0; i < HEIGHT; i++)
        for (j = 0; j < WIDTH; j++)
            if (m->ch[i][j] && m->ch[i][j]->type != PC_TYPE)
                trainers[count++] = m->ch[i][j];

    while (key != 27) {
        clear();
        mvprintw(0, 0, "Trainer List (ESC to close):");
        mvprintw(1, 0, "%-5s | %-20s", "Sym", "Relative Position");
        mvprintw(2, 0, "------------------------------------");

        for (i = 0; i < max_display && (i + offset) < count; i++) {
            Character *tnr = trainers[i + offset];
            dy = pc->y - tnr->y;
            dx = pc->x - tnr->x;
            mvprintw(i + 3, 0, "  %c   | %d %s and %d %s",
                     tnr->symbol, abs(dy), (dy > 0) ? vert[0] : vert[1],
                                  abs(dx), (dx > 0) ? hori[0] : hori[1]);
        }

        if (offset + max_display < count) mvprintw(23, 0, "-- More Below (Down Arrow) --");
        if (offset > 0)                   mvprintw(2, 30, "-- More Above (Up Arrow) --");

        refresh();
        key = getch();
        if (key == KEY_UP && offset > 0) offset--;
        else if (key == KEY_DOWN && (offset + max_display) < count) offset++;
    }
}

void battle_interface(Character *pc, Character *npc) {
    battle_loop(pc, npc, NULL);
}

bool battle_loop(Character *pc_char, Character *npc_char, Pokemon *wild_p) {
    PC *pc = (PC *)pc_char;
    std::vector<Pokemon *> npc_party;
    bool is_wild = (wild_p != NULL);
    
    if (is_wild) {
        npc_party.push_back(wild_p);
    } else {
        npc_party = npc_char->pokemon;
    }
    
    size_t pc_idx = 0;
    while (pc_idx < pc->pokemon.size() && pc->pokemon[pc_idx]->get_current_hp() == 0) pc_idx++;
    if (pc_idx == pc->pokemon.size()) return false;
    
    size_t npc_idx = 0;
    int escape_attempts = 0;
    
    while (true) {
        Pokemon *p_pc = pc->pokemon[pc_idx];
        Pokemon *p_npc = npc_party[npc_idx];
        
        clear();
        mvprintw(1, 10, "PC: %s (HP: %d/%d) vs %s: %s (HP: %d/%d)", 
                 p_pc->get_species(), p_pc->get_current_hp(), p_pc->get_hp(),
                 is_wild ? "Wild" : "NPC", p_npc->get_species(), p_npc->get_current_hp(), p_npc->get_hp());
        
        mvprintw(4, 12, "1. Fight");
        mvprintw(5, 12, "2. Bag");
        mvprintw(6, 12, "3. Run");
        mvprintw(7, 12, "4. Pokemon");
        refresh();
        
        int choice = getch();
        int pc_action = -1;
        int pc_move_idx = -1;
        int item_choice = -1;
        
        if (choice == '1') {
            while (true) {
                clear();
                mvprintw(1, 10, "Select a move:");
                for (int i = 0; i < 4; i++) {
                    const char *move_name = p_pc->get_move(i);
                    if (move_name[0]) mvprintw(3 + i, 12, "%d. %s", i + 1, move_name);
                }
                mvprintw(8, 12, "0. Back");
                refresh();
                int m_choice = getch();
                if (m_choice == '0') break;
                if (m_choice >= '1' && m_choice <= '4' && p_pc->get_move(m_choice - '1')[0]) {
                    pc_action = 0;
                    pc_move_idx = m_choice - '1';
                    break;
                }
            }
            if (pc_action == -1) continue;
        } else if (choice == '2') {
            while (true) {
                clear();
                mvprintw(1, 10, "Select an item:");
                mvprintw(3, 12, "1. Potion (%d)", pc->potions);
                mvprintw(4, 12, "2. Revive (%d)", pc->revives);
                mvprintw(5, 12, "3. Pokeball (%d)", pc->pokeballs);
                mvprintw(7, 12, "0. Back");
                refresh();
                int b_choice = getch();
                if (b_choice == '0') break;
                if (b_choice == '1' && pc->potions > 0) { pc_action = 1; item_choice = 1; break; }
                if (b_choice == '2' && pc->revives > 0) { pc_action = 1; item_choice = 2; break; }
                if (b_choice == '3' && pc->pokeballs > 0) { pc_action = 1; item_choice = 3; break; }
            }
            if (pc_action == -1) continue;
        } else if (choice == '3') {
            if (!is_wild) {
                mvprintw(9, 10, "You can't run from a trainer battle!");
                refresh(); getch(); continue;
            }
            pc_action = 2;
        } else if (choice == '4') {
            while (true) {
                clear();
                mvprintw(1, 10, "Select a Pokemon to switch to:");
                for (size_t i = 0; i < pc->pokemon.size(); i++) {
                    mvprintw(3 + i, 12, "%lu. %s (HP: %d/%d)", i + 1, pc->pokemon[i]->get_species(), pc->pokemon[i]->get_current_hp(), pc->pokemon[i]->get_hp());
                }
                mvprintw(10, 12, "0. Back");
                refresh();
                int s_choice = getch();
                if (s_choice == '0') break;
                if (s_choice >= '1' && (size_t)(s_choice - '1') < pc->pokemon.size()) {
                    int new_idx = s_choice - '1';
                    if ((size_t)new_idx == pc_idx) {
                        mvprintw(12, 10, "Already in battle!"); refresh(); getch(); continue;
                    }
                    if (pc->pokemon[new_idx]->get_current_hp() == 0) {
                        mvprintw(12, 10, "That Pokemon is fainted!"); refresh(); getch(); continue;
                    }
                    pc_action = 3;
                    pc_idx = new_idx;
                    break;
                }
            }
            if (pc_action == -1) continue;
        } else {
            continue;
        }
        
        int npc_move_idx = 0;
        std::vector<int> valid_npc_moves;
        for (int i = 0; i < 4; i++) {
            if (p_npc->get_move(i)[0]) valid_npc_moves.push_back(i);
        }
        npc_move_idx = valid_npc_moves[rand() % valid_npc_moves.size()];
        
        bool pc_first = true;
        if (pc_action == 0) {
            int pc_priority = 0;
            int npc_priority = 0;
            moves *m_pc = NULL, *m_npc = NULL;
            for (auto m : moves_db) {
                if (m->id == p_pc->get_move_id(pc_move_idx)) m_pc = m;
                if (m->id == p_npc->get_move_id(npc_move_idx)) m_npc = m;
            }
            if (m_pc) pc_priority = m_pc->priority;
            if (m_npc) npc_priority = m_npc->priority;
            
            if (pc_priority > npc_priority) pc_first = true;
            else if (npc_priority > pc_priority) pc_first = false;
            else {
                if (p_pc->get_speed() > p_npc->get_speed()) pc_first = true;
                else if (p_npc->get_speed() > p_pc->get_speed()) pc_first = false;
                else pc_first = (rand() % 2 == 0);
            }
        } else {
            pc_first = true;
        }
        
        // Execute turns
        for (int turn = 0; turn < 2; turn++) {
            bool current_pc = (turn == 0) ? pc_first : !pc_first;
            
            if (current_pc) {
                // PC's turn
                if (pc_action == 0) {
                    // Fight
                    moves *m_pc = NULL;
                    for (auto m : moves_db) {
                        if (m->id == p_pc->get_move_id(pc_move_idx)) {
                            m_pc = m;
                            break;
                        }
                    }
                    if (m_pc && (m_pc->accuracy == INT_MAX || rand() % 100 < m_pc->accuracy)) {
                        int damage = calculate_damage(p_pc, p_npc, pc_move_idx);
                        p_npc->set_current_hp(p_npc->get_current_hp() - damage);
                        if (p_npc->get_current_hp() < 0) p_npc->set_current_hp(0);
                        mvprintw(12, 10, "PC's %s uses %s! Did %d damage.", p_pc->get_species(), p_pc->get_move(pc_move_idx), damage);
                    } else {
                        mvprintw(12, 10, "PC's %s uses %s! It missed!", p_pc->get_species(), p_pc->get_move(pc_move_idx));
                    }
                } else if (pc_action == 1) {
                    // Bag
                    if (item_choice == 1) { // Potion
                        int old_hp = p_pc->get_current_hp();
                        int new_hp = old_hp + 20;
                        if (new_hp > p_pc->get_hp()) new_hp = p_pc->get_hp();
                        p_pc->set_current_hp(new_hp);
                        pc->potions--;
                        mvprintw(12, 10, "Used Potion! %s healed %d HP.", p_pc->get_species(), new_hp - old_hp);
                    } else if (item_choice == 2) { // Revive
                        mvprintw(12, 10, "Select a Pokemon to revive:");
                        refresh();
                        int r_choice = getch();
                        if (r_choice >= '1' && (size_t)(r_choice - '1') < pc->pokemon.size()) {
                            int r_idx = r_choice - '1';
                            if (pc->pokemon[r_idx]->get_current_hp() == 0) {
                                pc->pokemon[r_idx]->set_current_hp(pc->pokemon[r_idx]->get_hp() / 2);
                                pc->revives--;
                                mvprintw(14, 10, "%s was revived!", pc->pokemon[r_idx]->get_species());
                            } else {
                                mvprintw(14, 10, "Not fainted!");
                            }
                        }
                    } else if (item_choice == 3) { // Pokeball
                        if (!is_wild) {
                            mvprintw(12, 10, "Can't catch trainer's Pokemon!");
                        } else {
                            if (pc->pokemon.size() < 6) {
                                pc->pokemon.push_back(p_npc);
                                pc->pokeballs--;
                                mvprintw(12, 10, "Gotcha! %s was caught!", p_npc->get_species());
                                refresh(); getch(); return true;
                            } else {
                                pc->stored_pokemon.push_back(p_npc);
                                pc->pokeballs--;
                                mvprintw(12, 10, "Party full! %s sent to storage!", p_npc->get_species());
                                refresh(); getch(); return true;
                            }
                        }
                    }
                } else if (pc_action == 2) {
                    // Run
                    escape_attempts++;
                    int speed_wild = p_npc->get_speed();
                    int divisor = (speed_wild / 4);
                    if (divisor == 0) divisor = 1;
                    int odd_escape = ((p_pc->get_speed() * 32 / divisor) % 256) + 30 * escape_attempts;
                    if (rand() % 256 < odd_escape) {
                        mvprintw(12, 10, "Got away safely!");
                        refresh(); getch(); return false;
                    } else {
                        mvprintw(12, 10, "Couldn't escape!");
                    }
                } else if (pc_action == 3) {
                    mvprintw(12, 10, "Go! %s!", pc->pokemon[pc_idx]->get_species());
                    p_pc = pc->pokemon[pc_idx];
                }
            } else {
                // NPC's turn
                moves *m_npc = NULL;
                for (auto m : moves_db) {
                    if (m->id == p_npc->get_move_id(npc_move_idx)) {
                        m_npc = m;
                        break;
                    }
                }
                if (m_npc && (m_npc->accuracy == INT_MAX || rand() % 100 < m_npc->accuracy)) {
                    int damage = calculate_damage(p_npc, p_pc, npc_move_idx);
                    p_pc->set_current_hp(p_pc->get_current_hp() - damage);
                    if (p_pc->get_current_hp() < 0) p_pc->set_current_hp(0);
                    mvprintw(14, 10, "%s's %s uses %s! Did %d damage.", is_wild ? "Wild" : "NPC", p_npc->get_species(), p_npc->get_move(npc_move_idx), damage);
                } else {
                    mvprintw(14, 10, "%s's %s uses %s! It missed!", is_wild ? "Wild" : "NPC", p_npc->get_species(), p_npc->get_move(npc_move_idx));
                }
            }
            refresh();
            getch();
            
            if (p_npc->get_current_hp() == 0) {
                mvprintw(16, 10, "%s's %s fainted!", is_wild ? "Wild" : "NPC", p_npc->get_species());
                refresh(); getch();
                npc_idx++;
                if (npc_idx == npc_party.size()) {
                    mvprintw(18, 10, "You won the battle!");
                    if (!is_wild) {
                        ((NPC *)npc_char)->is_defeated = true;
                        // Calculate money reward: sum of all NPC pokemon levels * 10
                        int total_money = 0;
                        for (auto p : npc_party) {
                            total_money += p->get_level() * 10;
                        }
                        ((PC *)pc_char)->money += total_money;
                        mvprintw(19, 10, "Earned $%d!", total_money);
                    }
                    refresh(); getch(); return false;
                }
                p_npc = npc_party[npc_idx];
                break;
            }
            if (p_pc->get_current_hp() == 0) {
                mvprintw(16, 10, "PC's %s fainted!", p_pc->get_species());
                refresh(); getch();
                bool all_fainted = true;
                for (size_t i = 0; i < pc->pokemon.size(); i++) {
                    if (pc->pokemon[i]->get_current_hp() > 0) {
                        all_fainted = false;
                        break;
                    }
                }
                if (all_fainted) {
                    mvprintw(18, 10, "You lost the battle! All your Pokemon fainted.");
                    refresh(); getch(); return false;
                }
                while (true) {
                    clear();
                    mvprintw(1, 10, "Select a Pokemon to switch to:");
                    for (size_t i = 0; i < pc->pokemon.size(); i++) {
                        mvprintw(3 + i, 12, "%lu. %s (HP: %d/%d)", i + 1, pc->pokemon[i]->get_species(), pc->pokemon[i]->get_current_hp(), pc->pokemon[i]->get_hp());
                    }
                    refresh();
                    int s_choice = getch();
                    if (s_choice >= '1' && (size_t)(s_choice - '1') < pc->pokemon.size()) {
                        int new_idx = s_choice - '1';
                        if (pc->pokemon[new_idx]->get_current_hp() > 0) {
                            pc_idx = new_idx;
                            p_pc = pc->pokemon[pc_idx];
                            break;
                        }
                    }
                }
                break;
            }
        }
    }
    return false;
}