#include <iostream>
#include <climits>
#include <string>
#include <cstdlib>
#include <sys/stat.h>
#include <fstream>
#include <sstream>
#include <vector>
#include "pokemon.h"

std::string get_db_path() {
    struct stat sb;

    std::string path1 = "/share/cs327/pokedex/pokedex/data/csv/";
    if (stat(path1.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode)) {
        return path1;
    }

    char *home = getenv("HOME");
    if (home) {
        std::string path2 = std::string(home) + "/.poke327/pokedex/pokedex/data/csv/";
        if (stat(path2.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode)) {
            return path2;
        }
    }

    return "";
}

static std::string next_token(std::stringstream &ss) {
    std::string s;
    if (!std::getline(ss, s, ',')) {
        return "";
    }
    return s;
}

static int to_int(std::string s) {
    if (s.empty()) {
        return INT_MAX;
    }
    return std::stoi(s);
}

void parse_pokemon(std::vector<pokemon*> &v, std::string path) {
    std::ifstream f(path + "pokemon.csv");
    if (!f.is_open()) return;
    std::string line;
    std::getline(f, line); // Skip header
    while (std::getline(f, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        int id = to_int(next_token(ss));
        std::string identifier = next_token(ss);
        int species_id = to_int(next_token(ss));
        int height = to_int(next_token(ss));
        int weight = to_int(next_token(ss));
        int experience = to_int(next_token(ss));
        int order = to_int(next_token(ss));
        int is_default = to_int(next_token(ss));
        v.push_back(new pokemon(id, identifier, species_id, height, weight, experience, order, is_default));
    }
}

void parse_moves(std::vector<moves*> &v, std::string path) {
    std::ifstream f(path + "moves.csv");
    if (!f.is_open()) return;
    std::string line;
    std::getline(f, line);
    while (std::getline(f, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        int id = to_int(next_token(ss));
        std::string identifier = next_token(ss);
        int generation_id = to_int(next_token(ss));
        int type_id = to_int(next_token(ss));
        int power = to_int(next_token(ss));
        int pp = to_int(next_token(ss));
        int accuracy = to_int(next_token(ss));
        int priority = to_int(next_token(ss));
        int target_id = to_int(next_token(ss));
        int damage_class_id = to_int(next_token(ss));
        int effect_id = to_int(next_token(ss));
        int effect_chance = to_int(next_token(ss));
        int contest_type_id = to_int(next_token(ss));
        int contest_effect_id = to_int(next_token(ss));
        int super_contest_effect_id = to_int(next_token(ss));
        v.push_back(new moves(id, identifier, generation_id, type_id, power, pp, accuracy, priority, target_id, damage_class_id, effect_id, effect_chance, contest_type_id, contest_effect_id, super_contest_effect_id));
    }
}

void parse_pokemon_moves(std::vector<pokemon_moves*> &v, std::string path) {
    std::ifstream f(path + "pokemon_moves.csv");
    if (!f.is_open()) return;
    std::string line;
    std::getline(f, line);
    while (std::getline(f, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        int pokemon_id = to_int(next_token(ss));
        int version_group_id = to_int(next_token(ss));
        int move_id = to_int(next_token(ss));
        int pokemon_move_method_id = to_int(next_token(ss));
        int level = to_int(next_token(ss));
        int order = to_int(next_token(ss));
        v.push_back(new pokemon_moves(pokemon_id, version_group_id, move_id, pokemon_move_method_id, level, order));
    }
}

void parse_pokemon_species(std::vector<pokemon_species*> &v, std::string path) {
    std::ifstream f(path + "pokemon_species.csv");
    if (!f.is_open()) return;
    std::string line;
    std::getline(f, line);
    while (std::getline(f, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        int id = to_int(next_token(ss));
        std::string identifier = next_token(ss);
        int generation_id = to_int(next_token(ss));
        int evolves_from_species_id = to_int(next_token(ss));
        int evolution_chain_id = to_int(next_token(ss));
        int color_id = to_int(next_token(ss));
        int shape_id = to_int(next_token(ss));
        int habitat_id = to_int(next_token(ss));
        int gender_rate = to_int(next_token(ss));
        int capture_rate = to_int(next_token(ss));
        int base_happiness = to_int(next_token(ss));
        int is_baby = to_int(next_token(ss));
        int hatch_counter = to_int(next_token(ss));
        int has_gender_differences = to_int(next_token(ss));
        int growth_rate_id = to_int(next_token(ss));
        int forms_switchable = to_int(next_token(ss));
        int is_legendary = to_int(next_token(ss));
        int is_mythical = to_int(next_token(ss));
        int order = to_int(next_token(ss));
        int conquest_order = to_int(next_token(ss));
        v.push_back(new pokemon_species(id, identifier, generation_id, evolves_from_species_id, evolution_chain_id, color_id, shape_id, habitat_id, gender_rate, capture_rate, base_happiness, is_baby, hatch_counter, has_gender_differences, growth_rate_id, forms_switchable, is_legendary, is_mythical, order, conquest_order));
    }
}

void parse_experience(std::vector<experience*> &v, std::string path) {
    std::ifstream f(path + "experience.csv");
    if (!f.is_open()) return;
    std::string line;
    std::getline(f, line);
    while (std::getline(f, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        int growth_rate_id = to_int(next_token(ss));
        int level = to_int(next_token(ss));
        int exp = to_int(next_token(ss));
        v.push_back(new experience(growth_rate_id, level, exp));
    }
}

void parse_type_names(std::vector<type_names*> &v, std::string path) {
    std::ifstream f(path + "type_names.csv");
    if (!f.is_open()) return;
    std::string line;
    std::getline(f, line);
    while (std::getline(f, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        int type_id = to_int(next_token(ss));
        int local_language_id = to_int(next_token(ss));
        std::string name = next_token(ss);
        v.push_back(new type_names(type_id, local_language_id, name));
    }
}

void parse_pokemon_stats(std::vector<pokemon_stats*> &v, std::string path) {
    std::ifstream f(path + "pokemon_stats.csv");
    if (!f.is_open()) return;
    std::string line;
    std::getline(f, line);
    while (std::getline(f, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        int pokemon_id = to_int(next_token(ss));
        int stat_id = to_int(next_token(ss));
        int base_stat = to_int(next_token(ss));
        int effort = to_int(next_token(ss));
        v.push_back(new pokemon_stats(pokemon_id, stat_id, base_stat, effort));
    }
}

void parse_stats(std::vector<stats*> &v, std::string path) {
    std::ifstream f(path + "stats.csv");
    if (!f.is_open()) return;
    std::string line;
    std::getline(f, line);
    while (std::getline(f, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        int id = to_int(next_token(ss));
        int damage_class_id = to_int(next_token(ss));
        std::string identifier = next_token(ss);
        int is_battle_only = to_int(next_token(ss));
        int game_index = to_int(next_token(ss));
        v.push_back(new stats(id, damage_class_id, identifier, is_battle_only, game_index));
    }
}

void parse_pokemon_types(std::vector<pokemon_types*> &v, std::string path) {
    std::ifstream f(path + "pokemon_types.csv");
    if (!f.is_open()) return;
    std::string line;
    std::getline(f, line);
    while (std::getline(f, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        int pokemon_id = to_int(next_token(ss));
        int type_id = to_int(next_token(ss));
        int slot = to_int(next_token(ss));
        v.push_back(new pokemon_types(pokemon_id, type_id, slot));
    }
}

void parse_type_efficacy(std::vector<type_efficacy*> &v, std::string path) {
    std::ifstream f(path + "type_efficacy.csv");
    if (!f.is_open()) return;
    std::string line;
    std::getline(f, line);
    while (std::getline(f, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        int damage_type_id = to_int(next_token(ss));
        int target_type_id = to_int(next_token(ss));
        int damage_factor = to_int(next_token(ss));
        v.push_back(new type_efficacy(damage_type_id, target_type_id, damage_factor));
    }
}

void pokemon::print() {
    std::cout << (id == INT_MAX ? "" : std::to_string(id)) << ","
              << identifier << ","
              << (species_id == INT_MAX ? "" : std::to_string(species_id)) << ","
              << (height == INT_MAX ? "" : std::to_string(height)) << ","
              << (weight == INT_MAX ? "" : std::to_string(weight)) << ","
              << (experience == INT_MAX ? "" : std::to_string(experience)) << ","
              << (order == INT_MAX ? "" : std::to_string(order)) << ","
              << (is_default == INT_MAX ? "" : std::to_string(is_default)) << std::endl;
}

void moves::print() {
    std::cout << (id == INT_MAX ? "" : std::to_string(id)) << ","
              << identifier << ","
              << (generation_id == INT_MAX ? "" : std::to_string(generation_id)) << ","
              << (type_id == INT_MAX ? "" : std::to_string(type_id)) << ","
              << (power == INT_MAX ? "" : std::to_string(power)) << ","
              << (pp == INT_MAX ? "" : std::to_string(pp)) << ","
              << (accuracy == INT_MAX ? "" : std::to_string(accuracy)) << ","
              << (priority == INT_MAX ? "" : std::to_string(priority)) << ","
              << (target_id == INT_MAX ? "" : std::to_string(target_id)) << ","
              << (damage_class_id == INT_MAX ? "" : std::to_string(damage_class_id)) << ","
              << (effect_id == INT_MAX ? "" : std::to_string(effect_id)) << ","
              << (effect_chance == INT_MAX ? "" : std::to_string(effect_chance)) << ","
              << (contest_type_id == INT_MAX ? "" : std::to_string(contest_type_id)) << ","
              << (contest_effect_id == INT_MAX ? "" : std::to_string(contest_effect_id)) << ","
              << (super_contest_effect_id == INT_MAX ? "" : std::to_string(super_contest_effect_id)) << std::endl;
}

void pokemon_moves::print() {
    std::cout << (pokemon_id == INT_MAX ? "" : std::to_string(pokemon_id)) << ","
              << (version_group_id == INT_MAX ? "" : std::to_string(version_group_id)) << ","
              << (move_id == INT_MAX ? "" : std::to_string(move_id)) << ","
              << (pokemon_move_method_id == INT_MAX ? "" : std::to_string(pokemon_move_method_id)) << ","
              << (level == INT_MAX ? "" : std::to_string(level)) << ","
              << (order == INT_MAX ? "" : std::to_string(order)) << std::endl;
}

void pokemon_species::print() {
    std::cout << (id == INT_MAX ? "" : std::to_string(id)) << ","
              << identifier << ","
              << (generation_id == INT_MAX ? "" : std::to_string(generation_id)) << ","
              << (evolves_from_species_id == INT_MAX ? "" : std::to_string(evolves_from_species_id)) << ","
              << (evolution_chain_id == INT_MAX ? "" : std::to_string(evolution_chain_id)) << ","
              << (color_id == INT_MAX ? "" : std::to_string(color_id)) << ","
              << (shape_id == INT_MAX ? "" : std::to_string(shape_id)) << ","
              << (habitat_id == INT_MAX ? "" : std::to_string(habitat_id)) << ","
              << (gender_rate == INT_MAX ? "" : std::to_string(gender_rate)) << ","
              << (capture_rate == INT_MAX ? "" : std::to_string(capture_rate)) << ","
              << (base_happiness == INT_MAX ? "" : std::to_string(base_happiness)) << ","
              << (is_baby == INT_MAX ? "" : std::to_string(is_baby)) << ","
              << (hatch_counter == INT_MAX ? "" : std::to_string(hatch_counter)) << ","
              << (has_gender_differences == INT_MAX ? "" : std::to_string(has_gender_differences)) << ","
              << (growth_rate_id == INT_MAX ? "" : std::to_string(growth_rate_id)) << ","
              << (forms_switchable == INT_MAX ? "" : std::to_string(forms_switchable)) << ","
              << (is_legendary == INT_MAX ? "" : std::to_string(is_legendary)) << ","
              << (is_mythical == INT_MAX ? "" : std::to_string(is_mythical)) << ","
              << (order == INT_MAX ? "" : std::to_string(order)) << ","
              << (conquest_order == INT_MAX ? "" : std::to_string(conquest_order)) << std::endl;
}

void experience::print() {
    std::cout << (growth_rate_id == INT_MAX ? "" : std::to_string(growth_rate_id)) << ","
              << (level == INT_MAX ? "" : std::to_string(level)) << ","
              << (exp == INT_MAX ? "" : std::to_string(exp)) << std::endl;
}

void type_names::print() {
    std::cout << (type_id == INT_MAX ? "" : std::to_string(type_id)) << ","
              << (local_language_id == INT_MAX ? "" : std::to_string(local_language_id)) << ","
              << name << std::endl;
}

void pokemon_stats::print() {
    std::cout << (pokemon_id == INT_MAX ? "" : std::to_string(pokemon_id)) << ","
              << (stat_id == INT_MAX ? "" : std::to_string(stat_id)) << ","
              << (base_stat == INT_MAX ? "" : std::to_string(base_stat)) << ","
              << (effort == INT_MAX ? "" : std::to_string(effort)) << std::endl;
}

void stats::print() {
    std::cout << (id == INT_MAX ? "" : std::to_string(id)) << ","
              << (damage_class_id == INT_MAX ? "" : std::to_string(damage_class_id)) << ","
              << identifier << ","
              << (is_battle_only == INT_MAX ? "" : std::to_string(is_battle_only)) << ","
              << (game_index == INT_MAX ? "" : std::to_string(game_index)) << std::endl;
}

void pokemon_types::print() {
    std::cout << (pokemon_id == INT_MAX ? "" : std::to_string(pokemon_id)) << ","
              << (type_id == INT_MAX ? "" : std::to_string(type_id)) << ","
              << (slot == INT_MAX ? "" : std::to_string(slot)) << std::endl;
}

void type_efficacy::print() {
    std::cout << (damage_type_id == INT_MAX ? "" : std::to_string(damage_type_id)) << ","
              << (target_type_id == INT_MAX ? "" : std::to_string(target_type_id)) << ","
              << (damage_factor == INT_MAX ? "" : std::to_string(damage_factor)) << std::endl;
}

std::vector<pokemon *> pokemon_db;
std::vector<moves *> moves_db;
std::vector<pokemon_moves *> pokemon_moves_db;
std::vector<pokemon_species *> pokemon_species_db;
std::vector<experience *> experience_db;
std::vector<type_names *> type_names_db;
std::vector<pokemon_stats *> pokemon_stats_db;
std::vector<stats *> stats_db;
std::vector<pokemon_types *> pokemon_types_db;
std::vector<type_efficacy *> type_efficacy_db;

void load_pokemon_data() {
    std::string path = get_db_path();
    if (path.empty()) {
        std::cerr << "Error: Could not find pokedex database in /share/cs327/ or ~/.poke327/" << std::endl;
        exit(1);
    }
    parse_pokemon(pokemon_db, path);
    parse_moves(moves_db, path);
    parse_pokemon_moves(pokemon_moves_db, path);
    parse_pokemon_species(pokemon_species_db, path);
    parse_experience(experience_db, path);
    parse_type_names(type_names_db, path);
    parse_pokemon_stats(pokemon_stats_db, path);
    parse_stats(stats_db, path);
    parse_pokemon_types(pokemon_types_db, path);
    parse_type_efficacy(type_efficacy_db, path);

    if (pokemon_db.empty() || pokemon_moves_db.empty()) {
        std::cerr << "Error: Database files found but they appear to be empty or malformed." << std::endl;
        exit(1);
    }
}

void free_pokemon_data() {
    for (auto p : pokemon_db) delete p;
    for (auto m : moves_db) delete m;
    for (auto pm : pokemon_moves_db) delete pm;
    for (auto ps : pokemon_species_db) delete ps;
    for (auto e : experience_db) delete e;
    for (auto tn : type_names_db) delete tn;
    for (auto ps : pokemon_stats_db) delete ps;
    for (auto s : stats_db) delete s;
    for (auto pt : pokemon_types_db) delete pt;
    for (auto te : type_efficacy_db) delete te;
}

Pokemon::Pokemon(int level) : level(level) {
    if (pokemon_db.empty()) {
        std::cerr << "Error: pokemon_db is empty." << std::endl;
        exit(1);
    }
    int index = rand() % pokemon_db.size();
    pokemon_species_id = pokemon_db[index]->id;
    
    for (int i = 0; i < 6; i++) {
        iv[i] = rand() % 16;
    }
    
    shiny = (rand() % 8192 == 0);
    gender = (rand() % 2 == 0) ? gender_male : gender_female;

    int base_stats[6] = {0};
    for (auto ps : pokemon_stats_db) {
        if (ps->pokemon_id == pokemon_species_id) {
            if (ps->stat_id >= 1 && ps->stat_id <= 6) {
                base_stats[ps->stat_id - 1] = ps->base_stat;
            }
        }
    }
    
    stat[stat_hp] = ((base_stats[0] + iv[0]) * 2 * level) / 100 + level + 10;
    for (int i = 1; i < 6; i++) {
        stat[i] = ((base_stats[i] + iv[i]) * 2 * level) / 100 + 5;
    }
    current_hp = stat[stat_hp];
    
    std::vector<int> possible_moves;
    int search_level = level;
    while (possible_moves.empty() && search_level <= 100) {
        for (auto pm : pokemon_moves_db) {
            if (pm->pokemon_id == pokemon_species_id && pm->pokemon_move_method_id == 1 && pm->level <= search_level) {
                possible_moves.push_back(pm->move_id);
            }
        }
        if (possible_moves.empty()) search_level++;
    }

    if (possible_moves.empty()) {
        for (auto pm : pokemon_moves_db) {
            if (pm->pokemon_id == pokemon_species_id) {
                possible_moves.push_back(pm->move_id);
            }
        }
    }

    if (possible_moves.empty()) {
        for (auto m : moves_db) {
            possible_moves.push_back(m->id);
        }
    }

    for (int i = 0; i < 4; i++) pokemon_move_id[i] = -1;

    if (!possible_moves.empty()) {
        for (int i = (int)possible_moves.size() - 1; i > 0; i--) {
            int j = rand() % (i + 1);
            std::swap(possible_moves[i], possible_moves[j]);
        }
        int count = std::min((int)possible_moves.size(), 4);
        for (int i = 0; i < count; i++) {
            pokemon_move_id[i] = possible_moves[i];
        }
    }
}

Pokemon::Pokemon(int species_id, int level) : pokemon_species_id(species_id), level(level) {
    for (int i = 0; i < 6; i++) {
        iv[i] = rand() % 16;
    }
    
    shiny = (rand() % 8192 == 0);
    gender = (rand() % 2 == 0) ? gender_male : gender_female;

    int base_stats[6] = {0};
    for (auto ps : pokemon_stats_db) {
        if (ps->pokemon_id == pokemon_species_id) {
            if (ps->stat_id >= 1 && ps->stat_id <= 6) {
                base_stats[ps->stat_id - 1] = ps->base_stat;
            }
        }
    }
    
    stat[stat_hp] = ((base_stats[0] + iv[0]) * 2 * level) / 100 + level + 10;
    for (int i = 1; i < 6; i++) {
        stat[i] = ((base_stats[i] + iv[i]) * 2 * level) / 100 + 5;
    }
    current_hp = stat[stat_hp];
    
    std::vector<int> possible_moves;
    int search_level = level;
    while (possible_moves.empty() && search_level <= 100) {
        for (auto pm : pokemon_moves_db) {
            if (pm->pokemon_id == pokemon_species_id && pm->pokemon_move_method_id == 1 && pm->level <= search_level) {
                possible_moves.push_back(pm->move_id);
            }
        }
        if (possible_moves.empty()) search_level++;
    }

    if (possible_moves.empty()) {
        for (auto pm : pokemon_moves_db) {
            if (pm->pokemon_id == pokemon_species_id) {
                possible_moves.push_back(pm->move_id);
            }
        }
    }

    if (possible_moves.empty()) {
        for (auto m : moves_db) {
            possible_moves.push_back(m->id);
        }
    }

    for (int i = 0; i < 4; i++) pokemon_move_id[i] = -1;

    if (!possible_moves.empty()) {
        for (int i = (int)possible_moves.size() - 1; i > 0; i--) {
            int j = rand() % (i + 1);
            std::swap(possible_moves[i], possible_moves[j]);
        }
        int count = std::min((int)possible_moves.size(), 4);
        for (int i = 0; i < count; i++) {
            pokemon_move_id[i] = possible_moves[i];
        }
    }
}

const char *Pokemon::get_species() const {
    for (auto p : pokemon_db) {
        if (p->id == pokemon_species_id) return p->identifier.c_str();
    }
    return "Unknown";
}

const char *Pokemon::get_move(int i) const {
    if (i < 0 || i > 3 || pokemon_move_id[i] == -1) return "";
    for (auto m : moves_db) {
        if (m->id == pokemon_move_id[i]) return m->identifier.c_str();
    }
    return "Unknown";
}

int calculate_damage(Pokemon *attacker, Pokemon *defender, int move_idx) {
    int move_id = attacker->get_move_id(move_idx);
    if (move_id == -1) return 0;
    
    moves *m = NULL;
    for (auto move : moves_db) {
        if (move->id == move_id) {
            m = move;
            break;
        }
    }
    if (!m || m->power == INT_MAX) return 1; // Minimum 1 damage for non-damaging/missing moves if we hit

    double level = attacker->get_level();
    double power = m->power;
    double attack = attacker->get_atk();
    double defense = defender->get_def();
    
    // Critical
    double critical = 1.0;
    int base_speed = 0;
    for (auto ps : pokemon_stats_db) {
        if (ps->pokemon_id == attacker->get_species_id() && ps->stat_id == 6) {
            base_speed = ps->base_stat;
            break;
        }
    }
    if (rand() % 256 < (base_speed / 2)) {
        critical = 1.5;
    }
    
    // Random
    double random = (rand() % 16 + 85) / 100.0;
    
    // STAB
    double stab = 1.0;
    for (auto pt : pokemon_types_db) {
        if (pt->pokemon_id == attacker->get_species_id() && pt->type_id == m->type_id) {
            stab = 1.5;
            break;
        }
    }
    
    // Type effectiveness
    double type = 1.0;
    std::vector<int> defender_types;
    for (auto pt : pokemon_types_db) {
        if (pt->pokemon_id == defender->get_species_id()) {
            defender_types.push_back(pt->type_id);
        }
    }
    for (int t_id : defender_types) {
        for (auto te : type_efficacy_db) {
            if (te->damage_type_id == m->type_id && te->target_type_id == t_id) {
                type *= (te->damage_factor / 100.0);
            }
        }
    }
    
    int damage = (int)((((((2.0 * level) / 5.0) + 2.0) * power * (attack / defense)) / 50.0) + 2.0) * critical * random * stab * type;
    
    if (damage < 1) damage = 1;
    
    return damage;
}

