#ifndef POKEMON_H
#define POKEMON_H

#include <iostream>

class pokemon {
    private:
        int id;
        char *identifier;
        int species_id;
        int height;
        int weight;
        int experience;
        int order;
        int is_default;
    
    public:
        pokemon(int ID, char *Identifier, int Species_ID, int Height, int Weight, int Experience, int Order, int Is_default) : 
            id(ID), identifier(Identifier), species_id(Species_ID), height(Height), weight(Weight), experience(Experience), order(Order), is_default(Is_default) 
            {}
        
        virtual ~pokemon() {}
        void print();
};

class moves {
    private:
        int id;
        char *identifier;
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
    public:
        moves(int ID, char *Identifier, int Generation_id, int Type_id, int Power, int Pp, int Accuracy, int Priority, int Target_id, int Damage_class_id, int Effect_id, int Effect_chance, int Contest_type_id, int Contest_effect_id, int Super_contest_effect_id) :
            id(ID), identifier(Identifier), generation_id(Generation_id), type_id(Type_id), power(Power), pp(Pp), accuracy(Accuracy), priority(Priority), target_id(Target_id), damage_class_id(Damage_class_id), effect_id(Effect_id), effect_chance(Effect_chance), contest_type_id(Contest_type_id), contest_effect_id(Contest_effect_id), super_contest_effect_id(Super_contest_effect_id)
            {}
        virtual ~moves() {}
        void print();
};

class pokemon_moves {
    private:
        int pokemon_id;
        int version_group_id, move_id;
        int pokemon_move_method_id;
        int level, order;
    public:
        pokemon_moves(int Pokemon_id, int Version_group_id, int Move_id, int Pokemon_move_method_id, int Level, int Order) :
            pokemon_id(Pokemon_id), version_group_id(Version_group_id), move_id(Move_id), pokemon_move_method_id(Pokemon_move_method_id), level(Level), order(Order)
            {}

        virtual ~pokemon_moves() {}
        void print();

};

class pokemon_species {
    private:
        int id; 
        char *identifier;
        int generation_id, evolves_from_species_id,	evolution_chain_id,	color_id, shape_id,	habitat_id,	gender_rate, capture_rate, base_happiness, is_baby,	hatch_counter;	
        int has_gender_differences, growth_rate_id, forms_switchable, is_legendary, is_mythical, order, conquest_order;
    public:
        pokemon_species(int ID, char *Identifier, int Generation_id, int Evolves_from_species_id, int Evolution_chain_id, int Color_id, int Shape_id, int Habitat_id, int Gender_rate, int Capture_rate, int Base_happiness, int Is_baby, int Hatch_counter, int Has_gender_differences, int Growth_rate_id, int Forms_switchable, int Is_legendary, int Is_mythical, int Order, int Conquest_order) :
            id(ID), identifier(Identifier), generation_id(Generation_id), evolves_from_species_id(Evolves_from_species_id), evolution_chain_id(Evolution_chain_id), color_id(Color_id), shape_id(Shape_id), habitat_id(Habitat_id), gender_rate(Gender_rate), capture_rate(Capture_rate), base_happiness(Base_happiness), is_baby(Is_baby), hatch_counter(Hatch_counter), has_gender_differences(Has_gender_differences), growth_rate_id(Growth_rate_id), forms_switchable(Forms_switchable), is_legendary(Is_legendary), is_mythical(Is_mythical), order(Order), conquest_order(Conquest_order)
            {}
        virtual ~pokemon_species() {}
        void print();
};

class experience {
    private:
        int growth_rate_id, level, exp;
    public:
        experience(int Growth_rate_id, int Level, int Exp) :
            growth_rate_id(Growth_rate_id), level(Level), exp(Exp)
            {}
        virtual ~experience() {}
        void print();

};

class type_names {
    private:
        int type_id, local_language_id;
        char *name;
    public:
        type_names(int Type_id, int Local_language_id, char *Name) :
            type_id(Type_id), local_language_id(Local_language_id), name(Name)
            {}
        virtual ~type_names() {}
        void print();
};

class pokemon_stats {
    private:
        int pokemon_id,	stat_id, base_stat, effort;
    public:
        pokemon_stats(int Pokemon_id, int Stat_id, int Base_stat, int Effort) :
            pokemon_id(Pokemon_id), stat_id(Stat_id), base_stat(Base_stat), effort(Effort)
            {}
        virtual ~pokemon_stats() {}
        void print();
};

class stats {
    private:
        int id, damage_class_id;
        char *identifier;
        int is_battle_only, game_index;
    public:
        stats(int ID, int Damage_class_id, char *Identifier, int Is_battle_only, int Game_index) :
            id(ID), damage_class_id(Damage_class_id), identifier(Identifier), is_battle_only(Is_battle_only), game_index(Game_index)
            {}
        virtual ~stats() {}
        void print();
};

class pokemon_types {
    private:
        int pokemon_id, type_id, slot;
    public:
        pokemon_types(int Pokemon_id, int Type_id, int Slot) :
            pokemon_id(Pokemon_id), type_id(Type_id), slot(Slot)
            {}
        virtual ~pokemon_types() {}
        void print();
};



#endif
