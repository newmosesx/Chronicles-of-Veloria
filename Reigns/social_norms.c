// file: social_norms.c

#include <stdio.h>
#include <stdlib.h>
#include "../market.h"
#include "../humans.h"
#include "../logger.h"
#include "../game_config.h"


// --- Handles military recruitment and its costs ---
void recruit_soldiers(struct Kingdom *kingdom, struct Human_Data *data) {
    if (!kingdom->is_active || kingdom->population == 0) return;

    // Only recruit if unrest is moderate, as a show of force.
    if (kingdom->unrest_level < DISSENT_THRESHOLD / 2) return;

    // Try to recruit a small number of troops each day
    int recruits_wanted = 5 + (kingdom->unrest_level / 20);
    int recruited_count = 0;

    for (int i = 0; i < data->count && recruited_count < recruits_wanted; i++) {
        if (data->humans[i].alive == 1 && data->humans[i].kingdom_id == kingdom->id &&
            data->humans[i].job > 0 && data->humans[i].job <= JOB_BLACKSMITH) {
            int unit_choice = rand() % 3;
            if (unit_choice == 0 && kingdom->metal >= COST_SWORDSMAN_METAL) {
                kingdom->metal -= COST_SWORDSMAN_METAL;
                data->humans[i].job = JOB_SWORDSMAN;
                recruited_count++;
            } else if (unit_choice == 1 && kingdom->wood >= COST_ARCHER_WOOD) {
                kingdom->wood -= COST_ARCHER_WOOD;
                data->humans[i].job = JOB_ARCHER;
                recruited_count++;
            } else if (unit_choice == 2 && kingdom->metal >= COST_CAVALRY_METAL && kingdom->food >= COST_CAVALRY_FOOD) {
                kingdom->metal -= COST_CAVALRY_METAL;
                kingdom->food -= COST_CAVALRY_FOOD;
                data->humans[i].job = JOB_CAVALRY;
                recruited_count++;
            }
        }
    }
}

/**
 * @brief Collects taxes from the population, increasing kingdom treasury and citizen unrest.
 */
void collect_taxes(struct Kingdom *kingdom, struct Human_Data *data)
{
    if (!kingdom->is_active || kingdom->population == 0) return;
    int total_tax_collected = 0;

    for (int i = 0; i < data->count; i++) {
        if (data->humans[i].alive == 1 && data->humans[i].kingdom_id == kingdom->id) {
            if (data->humans[i].bronze >= TAX_RATE_PER_PERSON) {
                data->humans[i].bronze -= TAX_RATE_PER_PERSON;
                total_tax_collected += TAX_RATE_PER_PERSON;
            }
        }
    }
    if (kingdom->divine_penalty_timer_days > 0) {
        total_tax_collected = (int)(total_tax_collected * kingdom->divine_tax_modifier);
    }
    kingdom->treasury += total_tax_collected;
    kingdom->unrest_level += UNREST_GAIN_FROM_TAXES;
}

/**
 * @brief Assigns jobs to the entire initial population to ensure a stable start.
 * This version creates a standing army from the outset.
 */
void initial_job_assignment(struct Human_Data *data) {
    const int archer_pct = 5;
    const int cavalry_pct = 5;
    const int swordsman_pct = 5;
    const int blacksmith_pct = 10;
    const int miner_pct = 10;
    const int lumberjack_pct = 15;
    int general_count = 0;
    for (int i = 0; i < data->count; i++) {
        if (data->humans[i].alive == 1) {
            int job_roll = rand() % 100;

            if (job_roll < archer_pct) { // 5% Archers
                data->humans[i].job = JOB_ARCHER;
            } else if (job_roll < archer_pct + cavalry_pct) { // 5% Cavalry
                data->humans[i].job = JOB_CAVALRY;
            } else if ((job_roll < cavalry_pct+archer_pct+swordsman_pct)){ // 5% Swordsmen
                data->humans[i].job = JOB_SWORDSMAN;
                if (general_count < INITIAL_GENERAL_LIMIT && (rand() % 100 < GENERAL_SPAWN_CHANCE_PERCENT)) {
                    data->humans[i].is_general = 1;
                    general_count++;
                }
            } else if (job_roll < blacksmith_pct) { // 10% Blacksmiths
                data->humans[i].job = JOB_BLACKSMITH;
            } else if (job_roll < miner_pct) { // 10% Miners
                data->humans[i].job = JOB_MINER;
            } else if (job_roll < lumberjack_pct) { // 15% Lumberjacks
                data->humans[i].job = JOB_LUMBERJACK;
            } else { // 50% Farmers
                data->humans[i].job = JOB_FARMER;
            }
        }
    }
    printf("Initial job assignments complete. The empire's army, workforce, and %d generals are ready!\n", general_count);
}

/**
 * @brief Assigns a job to any living human who is currently unemployed.
 * This function iterates through the entire population and gives jobs to anyone
 * with job ID 0, ensuring the workforce is replenished as new people are born.
 */
void occupation(struct HumanPopulation *world_stat, struct Human_Data *data)
{
    // Loop through every single human in the world.
    for (int i = 0; i < data->count; i++)
    {
        // Find people who are alive but have no job.
        if (data->humans[i].alive == 1 && data->humans[i].job == 0)
        {
            // Assign them a random civilian job.
            // (1) Farmer, (2) Butcher, (3) Lumberjack, (4) Miner, (5) Blacksmith
            data->humans[i].job = (rand() % 8) + 1;
        }
    }
}

void commerce(struct Human_Data *data, int i)
{
    data->humans[i].hunger += (rand() % 40)+10;
    data->humans[i].bronze -= 15;
}

void payments(struct HumanPopulation *world_stat, struct Human_Data *data)
{
    // --- Iterate over actual data count, not world population estimate ---
    for (int i = 0; i < data->count; i++)
    {
        if (data->humans[i].alive == 1)
        {
            switch (data->humans[i].job) {
                case 1: data->humans[i].bronze += (rand() % 30) + 1; break; // Farmer
                case 2: data->humans[i].bronze += (rand() % 50) + 1; break; // Butcher
                case 3: data->humans[i].bronze += (rand() % 40) + 1; break; // Lumberjack
                case 4: data->humans[i].bronze += (rand() % 30) + 1; break; // Miner
                case 5: data->humans[i].bronze += (rand() % 85) + 1; break; // Blacksmith
                case 6:
                case 7:
                case 8: data->humans[i].bronze += (rand() % 41) + 20; break; // Military
                case 9: break; // Rebel
            }
        }
    }
}


/**
 * @brief Calculates daily resource production and consumption for a kingdom.
 * This version includes higher food consumption for military units.
 */
void apply_production_effects(struct Kingdom *kingdom) {

    // Apply story production modifiers
    if (kingdom->story_production_modifier != 1.0) {
        kingdom->food = (int)(kingdom->food * kingdom->story_production_modifier);
        kingdom->wood = (int)(kingdom->wood * kingdom->story_production_modifier);
        kingdom->stone = (int)(kingdom->stone * kingdom->story_production_modifier);
        kingdom->metal = (int)(kingdom->metal * kingdom->story_production_modifier);
    }

    if (kingdom->divine_penalty_timer_days > 0) {
        kingdom->food = (int)(kingdom->food * kingdom->divine_production_modifier);
        kingdom->wood = (int)(kingdom->wood * kingdom->divine_production_modifier);
        kingdom->stone = (int)(kingdom->stone * kingdom->divine_production_modifier);
        kingdom->metal = (int)(kingdom->metal * kingdom->divine_production_modifier);
    }
}

void consume_resources(struct Kingdom *kingdom, struct Human_Data *data, int i){
    // --- 2. Consumption Phase ---
    // Civilians eat 2 food. Military members eat 2 + an extra amount.
    
    if (kingdom->food > 1) {

        // People need to eat to heal themselves
        if (data->humans[i].hunger <= EAT_HUNGER_THRESHOLD) {
            if (data->humans[i].bronze >= FOOD_COST){
                data->humans[i].bronze -= FOOD_COST;
                kingdom->food -= 2;
                if (data->humans[i].job > 6 && data->humans[i].job < 9) kingdom->food -= MILITARY_EXTRA_FOOD_CONSUMPTION;
                data->humans[i].hunger += (rand() % 40)+10;
            } else {
                data->humans[i].health -= 5;
            }
        }
        //if (kingdom->unrest_level > WELL_FED_UNREST_REDUCTION_AMOUNT) kingdom->unrest_level -= WELL_FED_UNREST_REDUCTION_AMOUNT; // Well-fed people are happier
        // this will cause unrest to never increase. keep it commented for now.
    } else {
        log_event("!!! FAMINE in %s !!!", kingdom->name);
        kingdom->food = 0;
        kingdom->unrest_level += UNREST_GAIN_FROM_FAMINE;
        int deaths_from_starvation = (int)(kingdom->population * (FAMINE_POPULATION_LOSS_PERCENT / 100.0f));
        if (deaths_from_starvation < 1 && kingdom->population > 0) deaths_from_starvation = 1;

        log_event(" -> %d people have died from starvation!", deaths_from_starvation);
        
        // --- STARVATION LOOP ---
        if (data->humans[i].alive == 1 && data->humans[i].kingdom_id == kingdom->id && data->humans[i].hunger > 0) {
            data->humans[i].hunger -= 10;
        } else if (data->humans[i].hunger < 0){
            data->humans[i].alive = 0;
        }
        }
}

void dailyneed(struct Kingdom *kingdom, struct HumanPopulation *world_stat, struct Human_Data *data)
{
    int military_count = 0;
    int food_produced = 0; int stone_produced = 0;
    int wood_produced = 0; int metal_produced = 0;

    // --- Iterate over actual data count ---
    for (int i = 0; i < data->count; i++)
    {
        if (data->humans[i].alive == 1)
        {
            if (data->humans[i].health <= 0) {
                data->humans[i].alive = 0;
                continue;
            }

            // Working consumes health and hunger
            if (data->humans[i].health > 10){
                int work = rand() % 2;
                if (work >= 1) {
                    switch (data->humans[i].job) {
                        case JOB_FARMER:
                            data->humans[i].health -= 5;
                            data->humans[i].hunger -= 10;
                            food_produced += (rand() % FARMER_FOOD_PRODUCTION);
                            break;
                        case JOB_BUTCHER:
                              data->humans[i].health -= 10;
                            data->humans[i].hunger -= 15;
                            food_produced += (rand() % BUTCHER_MEAT_PRODUCTION);
                            break;
                        case JOB_LUMBERJACK:
                            data->humans[i].health -= 15;
                            data->humans[i].hunger -= 20;
                            wood_produced += (rand() % LUMBERJACK_WOOD_PRODUCTION)+1;
                            break;
                        case JOB_MINER:
                            data->humans[i].health -= 30;
                            data->humans[i].hunger -= 35;
                            if (rand() % 100 < MINER_METAL_CHANCE_PERCENT) {
                                metal_produced += (rand() % MINER_METAL_PRODUCTION)+1;
                            } else {
                                stone_produced += (rand() % MINER_STONE_PRODUCTION)+1;
                            }
                            break;
                        case JOB_BLACKSMITH:
                            if (kingdom->metal > 2){
                                data->humans[i].health -= 20;
                                data->humans[i].hunger -= 25;
                                kingdom->metal -= BLACKSMITH_METAL_NEEDS;
                            } else {data->humans[i].hunger -= 10;}
                            break;
                        case JOB_SWORDSMAN:
                        case JOB_ARCHER:
                        case JOB_CAVALRY: // The army needs to train.
                            data->humans[i].health -= 5;
                            data->humans[i].hunger -= 10;
                        case JOB_REBEL:
                            break;
                    }
                } else {
                    data->humans[i].health += 20; // Preventing helps more
                }
            }else {
                data->humans[i].health += 5; // Didn't prevent well, slower recovery.
            }

            // Should add a logic specific for rebels.
            if (data->humans[i].job != JOB_REBEL) consume_resources(kingdom, data, i);

        }
    }

    kingdom->food += food_produced;
    kingdom->wood += wood_produced;
    kingdom->stone += stone_produced;
    kingdom->metal += metal_produced;

    apply_production_effects(kingdom);

    // Apply story food cap // CHECK ON THIS MIGHT SOLVE THE STRANGE FOOD BEHAVIOUR YOU'VE BEN HAVING TO STABILISE THINGS
    if (kingdom->story_food_daily_cap > 0 && kingdom->food > kingdom->story_food_daily_cap) {
        kingdom->food = kingdom->story_food_daily_cap;
    }
}



