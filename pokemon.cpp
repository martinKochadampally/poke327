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

    // 1. Check /share/cs327
    std::string path1 = "/share/cs327/pokedex/pokedex/data/csv/";
    if (stat(path1.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode)) {
        return path1;
    }

    // 2. Check $HOME/.poke327
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
