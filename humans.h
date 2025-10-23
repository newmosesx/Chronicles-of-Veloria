// file: humans.h

#ifndef HUMANS_H
#define HUMANS_H

#include <string.h>
#include <stdbool.h>
#include "shared_data.h"

// Represents a single political entity
struct Kingdom
{
    int id;
    const char* name;
    int population;
    int unrest_level;
    int is_active;

    int food;
    int wood;
    int stone;
    int metal;
    int weapons;
    int treasury; // Stored in bronze coins
    
    // --- Military Fields ---
    int army_morale; // 0-100, affects combat effectiveness

    // Story control fields
    int story_skirmish_override; // 0=normal, 1=force, -1=prevent
    float story_skirmish_chance_modifier; // multiply base chance
    float story_production_modifier; // multiply all production
    int story_food_daily_cap; // 0=no cap, else max food per day
    float story_consumption_modifier;

    float divine_tax_modifier;           // Multiplier for tax collection (e.g., 0.75f)
    float divine_production_modifier;    // Multiplier for resource gathering
    int divine_penalty_timer_days;       // How many days the penalty remains
    bool can_use_divine_intervention;    // Limits usage to once per day
};

struct HumanPopulation
{
    int prev_human_population;
    int human_population;
    int deaths;
    int growth;
    float human_br;
};

struct Human_Stats
{
    const char* name;

    //stats
    int smart;
    int damage;
    int health;
    int speed;
    int defense;
    int hunger;
    int level;
    double expirience;

    //social
    int job;// (1) Farmer, (2) Butcher, (3) Lumberjack, (4) Miner, (5) Blacksmith, (6) Soldier, (7) Rebel
    int kingdom_id; // Which kingdom this human belongs to. 0 is the Empire.
    int bronze;
    int quirks[3];

    //armor
    int head;
    int torso;
    int legs;
    int foots;

    //items
    int right;
    int left;

    //status
    int alive; // 0 dead - 1 alive
    
    // --- Military Fields ---
    int is_general; // 0 for no, 1 for yes
};

struct Human_Data {
    struct Human_Stats *humans;
    int count;
    int capacity;
};


// Forward declarations of functions defined in other .c files.
// This tells the compiler that these functions exist and what they look like.
// The linker will connect them all together later.
void inflict_casualties(struct Human_Data *data, int kingdom_id, int job_id, int count);
void* simulation_thread_func(void* arg);
void update_all_kingdom_details_for_gui(struct Kingdom kingdoms[], struct Human_Data *data, GuiSharedData *target_data);
void life(struct HumanPopulation*);
void initialize_world_polities(struct Kingdom*);
void initialize_population(struct Human_Data*);
void initial_job_assignment(struct Human_Data*);
void calculate_stable_population_changes(struct Kingdom*, int, int*, int*);
void alive_status(int, struct Human_Data*);
void persona(int, struct HumanPopulation*, struct Human_Data*, int);
void trigger_hourly_skirmish(struct Kingdom*, struct Human_Data*);
void occupation(struct HumanPopulation*, struct Human_Data*);
void dailyneed(struct Kingdom *kingdom, struct HumanPopulation *world_stat, struct Human_Data *data);
void payments(struct HumanPopulation*, struct Human_Data*);
void recalculate_kingdom_populations(struct Kingdom*, struct Human_Data*);
void manage_empire(struct Kingdom*, struct Human_Data*);
void manage_kingdom_daily(struct Kingdom*, struct Human_Data*);
void trigger_random_event(struct Kingdom*, struct Human_Data*);
void compact_dead_humans(struct Human_Data*);
void run_ai_governor_decision(struct Kingdom *kingdom, struct Human_Data *data);
void recruit_soldiers(struct Kingdom *kingdom, struct Human_Data *data);
int birth_rate(struct HumanPopulation *world_stat, double percentage);
void occupation(struct HumanPopulation *world_stat, struct Human_Data *data);
void payments(struct HumanPopulation *world_stat, struct Human_Data *data);
void collect_taxes(struct Kingdom *kingdom, struct Human_Data *data);
void update_kingdom_unrest(struct Kingdom *kingdom, struct Human_Data *data);
void handle_recruitment_and_dissent(struct Kingdom *kingdom, struct Human_Data *data);
void recruit_soldiers(struct Kingdom *kingdom, struct Human_Data *data);
void EmpireAI(struct Kingdom *kingdom, struct Human_Data *data);

#endif // HUMANS_H