#ifndef POKEMON_H
#define POKEMON_H

#include <iostream>
#include <string>
#include <vector>
#include <climits>

std::string get_db_path();

class pokemon {
    public:
        int id;
        std::string identifier;
        int species_id;
        int height;
        int weight;
        int experience;
        int order;
        int is_default;
    
        pokemon(int ID, std::string Identifier, int Species_ID, int Height, int Weight, int Experience, int Order, int Is_default) : 
            id(ID), identifier(Identifier), species_id(Species_ID), height(Height), weight(Weight), experience(Experience), order(Order), is_default(Is_default) 
            {}
        
        virtual ~pokemon() {}
        void print();
};

class moves {
    public:
        int id;
        std::string identifier;
        int generation_id;
        int type_id;	
        int power;	
        int pp;	
        int accuracy;
        int priority;	
        int target_id;	
        int damage_class_id;	
        int effect_id;	
        int effect_chance;	
        int contest_type_id;	
        int contest_effect_id;	
        int super_contest_effect_id;
        moves(int ID, std::string Identifier, int Generation_id, int Type_id, int Power, int Pp, int Accuracy, int Priority, int Target_id, int Damage_class_id, int Effect_id, int Effect_chance, int Contest_type_id, int Contest_effect_id, int Super_contest_effect_id) :
            id(ID), identifier(Identifier), generation_id(Generation_id), type_id(Type_id), power(Power), pp(Pp), accuracy(Accuracy), priority(Priority), target_id(Target_id), damage_class_id(Damage_class_id), effect_id(Effect_id), effect_chance(Effect_chance), contest_type_id(Contest_type_id), contest_effect_id(Contest_effect_id), super_contest_effect_id(Super_contest_effect_id)
            {}
        virtual ~moves() {}
        void print();
};

class pokemon_moves {
    public:
        int pokemon_id;
        int version_group_id, move_id;
        int pokemon_move_method_id;
        int level, order;
        pokemon_moves(int Pokemon_id, int Version_group_id, int Move_id, int Pokemon_move_method_id, int Level, int Order) :
            pokemon_id(Pokemon_id), version_group_id(Version_group_id), move_id(Move_id), pokemon_move_method_id(Pokemon_move_method_id), level(Level), order(Order)
            {}

        virtual ~pokemon_moves() {}
        void print();

};

class pokemon_species {
    public:
        int id; 
        std::string identifier;
        int generation_id, evolves_from_species_id,	evolution_chain_id,	color_id, shape_id,	habitat_id,	gender_rate, capture_rate, base_happiness, is_baby,	hatch_counter;	
        int has_gender_differences, growth_rate_id, forms_switchable, is_legendary, is_mythical, order, conquest_order;
        pokemon_species(int ID, std::string Identifier, int Generation_id, int Evolves_from_species_id, int Evolution_chain_id, int Color_id, int Shape_id, int Habitat_id, int Gender_rate, int Capture_rate, int Base_happiness, int Is_baby, int Hatch_counter, int Has_gender_differences, int Growth_rate_id, int Forms_switchable, int Is_legendary, int Is_mythical, int Order, int Conquest_order) :
            id(ID), identifier(Identifier), generation_id(Generation_id), evolves_from_species_id(Evolves_from_species_id), evolution_chain_id(Evolution_chain_id), color_id(Color_id), shape_id(Shape_id), habitat_id(Habitat_id), gender_rate(Gender_rate), capture_rate(Capture_rate), base_happiness(Base_happiness), is_baby(Is_baby), hatch_counter(Hatch_counter), has_gender_differences(Has_gender_differences), growth_rate_id(Growth_rate_id), forms_switchable(Forms_switchable), is_legendary(Is_legendary), is_mythical(Is_mythical), order(Order), conquest_order(Conquest_order)
            {}
        virtual ~pokemon_species() {}
        void print();
};

class experience {
    public:
        int growth_rate_id, level, exp;
        experience(int Growth_rate_id, int Level, int Exp) :
            growth_rate_id(Growth_rate_id), level(Level), exp(Exp)
            {}
        virtual ~experience() {}
        void print();

};

class type_names {
    public:
        int type_id, local_language_id;
        std::string name;
        type_names(int Type_id, int Local_language_id, std::string Name) :
            type_id(Type_id), local_language_id(Local_language_id), name(Name)
            {}
        virtual ~type_names() {}
        void print();
};

class pokemon_stats {
    public:
        int pokemon_id,	stat_id, base_stat, effort;
        pokemon_stats(int Pokemon_id, int Stat_id, int Base_stat, int Effort) :
            pokemon_id(Pokemon_id), stat_id(Stat_id), base_stat(Base_stat), effort(Effort)
            {}
        virtual ~pokemon_stats() {}
        void print();
};

class stats {
    public:
        int id, damage_class_id;
        std::string identifier;
        int is_battle_only, game_index;
        stats(int ID, int Damage_class_id, std::string Identifier, int Is_battle_only, int Game_index) :
            id(ID), damage_class_id(Damage_class_id), identifier(Identifier), is_battle_only(Is_battle_only), game_index(Game_index)
            {}
        virtual ~stats() {}
        void print();
};

class pokemon_types {
    public:
        int pokemon_id, type_id, slot;
        pokemon_types(int Pokemon_id, int Type_id, int Slot) :
            pokemon_id(Pokemon_id), type_id(Type_id), slot(Slot)
            {}
        virtual ~pokemon_types() {}
        void print();
};

enum pokemon_stat {
  stat_hp,
  stat_atk,
  stat_def,
  stat_spatk,
  stat_spdef,
  stat_speed
};

enum pokemon_gender {
  gender_female,
  gender_male
};

class Pokemon {
private:
  int pokemon_species_id;
  int level;
  int iv[6];
  int stat[6];
  int pokemon_move_id[4];
  int current_hp;
  pokemon_gender gender;
  bool shiny;
public:
  Pokemon(int level);
  Pokemon(int species_id, int level);
  int get_level() const { return level; }
  const char *get_species() const;
  int get_species_id() const { return pokemon_species_id; }
  int get_hp() const { return stat[stat_hp]; }
  int get_current_hp() const { return current_hp; }
  void set_current_hp(int hp) { current_hp = hp; }
  int get_atk() const { return stat[stat_atk]; }
  int get_def() const { return stat[stat_def]; }
  int get_spatk() const { return stat[stat_spatk]; }
  int get_spdef() const { return stat[stat_spdef]; }
  int get_speed() const { return stat[stat_speed]; }
  int get_iv_hp() const { return iv[stat_hp]; }
  int get_iv_atk() const { return iv[stat_atk]; }
  int get_iv_def() const { return iv[stat_def]; }
  int get_iv_spatk() const { return iv[stat_spatk]; }
  int get_iv_spdef() const { return iv[stat_spdef]; }
  int get_iv_speed() const { return iv[stat_speed]; }
  const char *get_gender_string() const { return gender == gender_female ? "female" : "male"; }
  bool is_shiny() const { return shiny; }
  const char *get_move(int i) const;
  int get_move_id(int i) const { return pokemon_move_id[i]; }
};

int calculate_damage(Pokemon *attacker, Pokemon *defender, int move_idx);

extern std::vector<pokemon *> pokemon_db;
extern std::vector<moves *> moves_db;
extern std::vector<pokemon_moves *> pokemon_moves_db;
extern std::vector<pokemon_species *> pokemon_species_db;
extern std::vector<experience *> experience_db;
extern std::vector<type_names *> type_names_db;
extern std::vector<pokemon_stats *> pokemon_stats_db;
extern std::vector<stats *> stats_db;
extern std::vector<pokemon_types *> pokemon_types_db;

class type_efficacy {
    public:
        int damage_type_id, target_type_id, damage_factor;
        type_efficacy(int Damage_type_id, int Target_type_id, int Damage_factor) :
            damage_type_id(Damage_type_id), target_type_id(Target_type_id), damage_factor(Damage_factor)
            {}
        virtual ~type_efficacy() {}
        void print();
};

extern std::vector<type_efficacy *> type_efficacy_db;

void load_pokemon_data();
void free_pokemon_data();

void parse_pokemon(std::vector<pokemon*> &v, std::string path);
void parse_moves(std::vector<moves*> &v, std::string path);
void parse_pokemon_moves(std::vector<pokemon_moves*> &v, std::string path);
void parse_pokemon_species(std::vector<pokemon_species*> &v, std::string path);
void parse_experience(std::vector<experience*> &v, std::string path);
void parse_type_names(std::vector<type_names*> &v, std::string path);
void parse_pokemon_stats(std::vector<pokemon_stats*> &v, std::string path);
void parse_stats(std::vector<stats*> &v, std::string path);
void parse_pokemon_types(std::vector<pokemon_types*> &v, std::string path);
void parse_type_efficacy(std::vector<type_efficacy*> &v, std::string path);

#endif
