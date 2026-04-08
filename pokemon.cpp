#include <iostream>
#include <climits>
#include "pokemon.h"

void pokemon::print() {
    std::cout << (id == INT_MAX ? "" : std::to_string(id)) << ","
              << (identifier ? identifier : "") << ","
              << (species_id == INT_MAX ? "" : std::to_string(species_id)) << ","
              << (height == INT_MAX ? "" : std::to_string(height)) << ","
              << (weight == INT_MAX ? "" : std::to_string(weight)) << ","
              << (experience == INT_MAX ? "" : std::to_string(experience)) << ","
              << (order == INT_MAX ? "" : std::to_string(order)) << ","
              << (is_default == INT_MAX ? "" : std::to_string(is_default)) << std::endl;
}

void moves::print() {
    std::cout << (id == INT_MAX ? "" : std::to_string(id)) << ","
              << (identifier ? identifier : "") << ","
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
              << (identifier ? identifier : "") << ","
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
              << (name ? name : "") << std::endl;
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
              << (identifier ? identifier : "") << ","
              << (is_battle_only == INT_MAX ? "" : std::to_string(is_battle_only)) << ","
              << (game_index == INT_MAX ? "" : std::to_string(game_index)) << std::endl;
}

void pokemon_types::print() {
    std::cout << (pokemon_id == INT_MAX ? "" : std::to_string(pokemon_id)) << ","
              << (type_id == INT_MAX ? "" : std::to_string(type_id)) << ","
              << (slot == INT_MAX ? "" : std::to_string(slot)) << std::endl;
}
