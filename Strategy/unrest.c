// file: unrest.c

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "../humans.h"
#include "../logger.h"
#include "../game_config.h"

/**
 * @brief Kills a specified number of random, living people of a certain job in a kingdom.
 * This is a utility function for battles and starvation.
 * @param data The main human data array.
 * @param kingdom_id The kingdom where the casualties occur.
 * @param job_id The job of the people to target (e.g., JOB_SWORDSMAN).
 * @param count The number of people to kill.
 */
void inflict_casualties(struct Human_Data *data, int kingdom_id, int job_id, int count) {
    if (count <= 0) return;

    int casualties_inflicted = 0;
    int attempts = 0; // Safety break to prevent infinite loops
    
    // This is a simple but effective randomization method for large populations.
    while (casualties_inflicted < count && attempts < data->count * 3) {
        int random_index = rand() % data->count;
        
        if (data->humans[random_index].alive == 1 &&
            data->humans[random_index].kingdom_id == kingdom_id &&
            data->humans[random_index].job == job_id)
        {
            data->humans[random_index].alive = 0;
            casualties_inflicted++;
        }
        attempts++;
    }
}


void regroup_soldiers(int *soldier_count, int residual_soldier_damage,
                    int residual_soldier_health, int residual_soldier_defense, 
                    int *soldier_health, int *soldier_damage, int *soldier_defense)
{
    if (*soldier_count <= 0) return; // Prevent division by zero
    float soldier_dmg_approx = (float)residual_soldier_damage / *soldier_count;
    float soldier_hlt_approx = (float)residual_soldier_health / *soldier_count;
    float soldier_def_approx = (float)residual_soldier_defense / *soldier_count;
    
    // Check for division by zero
    float soldiers_count_approx = (soldier_hlt_approx > 0) ? (float)*soldier_health / soldier_hlt_approx : 0;

    soldiers_count_approx = floorf(soldiers_count_approx);

    *soldier_damage = soldiers_count_approx * soldier_dmg_approx;
    *soldier_health = soldiers_count_approx * soldier_hlt_approx;
    *soldier_defense = soldiers_count_approx * soldier_def_approx;
    log_event("The army lost %d soldiers.", (int)(*soldier_count - soldiers_count_approx));
    *soldier_count = soldiers_count_approx;
}


void surprise_attack(int *soldier_health, int rebel_damage)
{
    *soldier_health -= rebel_damage * ELEMENT_OF_SURPRISE;
}


void tank_mode(int *soldier_health, int residual_soldier_health, int *soldier_defense)
{
    if (*soldier_health < residual_soldier_health * 0.5)
    {
        int defense_to_health = *soldier_defense * 0.5;
        *soldier_defense -= defense_to_health;
        *soldier_health += defense_to_health;
    }
}


void soldier_attack(int *soldier_damage, int *rebel_defense, int *rebel_health)
{
    if (*rebel_defense < *soldier_damage) 
    {
        int damage_spillover = *soldier_damage - *rebel_defense;
        *rebel_defense = 0; // Defense is broken
        *rebel_health -= damage_spillover; 
    } else 
    {
        *rebel_defense -= *soldier_damage;
    }
}


void regroup_rebels(int *rebel_count, int residual_rebel_damage,
                    int residual_rebel_health, int residual_rebel_defense, 
                    int *rebel_health, int *rebel_damage, int *rebel_defense)
{
    if (*rebel_count <= 0) return; // Prevent division by zero
    float rebel_dmg_approx = (float)residual_rebel_damage / *rebel_count;
    float rebel_hlt_approx = (float)residual_rebel_health / *rebel_count;
    float rebel_def_approx = (float)residual_rebel_defense / *rebel_count;

    // Check for division by zero
    float rebels_count_approx = (rebel_hlt_approx > 0) ? (float)*rebel_health / rebel_hlt_approx : 0;

    rebels_count_approx = floorf(rebels_count_approx);
    *rebel_damage = rebels_count_approx * rebel_dmg_approx;
    *rebel_health = rebels_count_approx * rebel_hlt_approx;
    *rebel_defense = rebels_count_approx * rebel_def_approx;
    log_event("The rebels lost %d fighters.", (int)(*rebel_count - rebels_count_approx));
    *rebel_count = rebels_count_approx;
}


void rebel_attack(int rebel_damage, int *soldier_defense, int *soldier_health)
{
    if ((*soldier_defense * ORAGINIZED_COMMAND) > rebel_damage)
    {
        *soldier_defense -= rebel_damage;
    } else
    {
        int damage_spillover = rebel_damage - (*soldier_defense * ORAGINIZED_COMMAND);
        *soldier_defense = 0; // Defense is broken
        *soldier_health -= damage_spillover;
    }
}


int declare_result(int rebel_count, int soldier_count, int residual_rebel_count, int residual_soldier_count)
{
    if (soldier_count <= 0 && rebel_count <= 0) {
        log_event("The battle ended with no victor.. Just blood.."); 
        return 2; // End the battle
    }

    if (soldier_count >= rebel_count) {
        if (rebel_count >= residual_soldier_count * 0.2 && soldier_count < residual_soldier_count * 0.3) {
            return 3; // Continue the battle
        } else {
            log_event("Victory of the Empire!: Rebel Retreat"); 
            return 2; // End the battle
        }
    } else { // rebel_count >= soldier_count
        if (soldier_count >= residual_rebel_count * 0.2 && rebel_count < residual_rebel_count * 0.3) {
            return 3; // Continue the battle
        } else {
            log_event("Victory of the Rebels!: Soldiers Retreat"); 
            return 2; // End the battle
        }
        return 3; // Continue the battle
    }
}
// NEXT UPDATE: ALL FOOD, EQUIPMENT, etc BOUGHT BY THE POPULATION. 30% of the money goes in the treasury.

/**
 * @brief Triggers a random skirmish between the kingdom's army and rebels.
 * This should be called hourly.
 */
void trigger_hourly_skirmish(struct Kingdom *kingdom, struct Human_Data *data) {
    if (!kingdom->is_active) return;
    int modified_chance = (int)(HOURLY_SKIRMISH_BASE_CHANCE_PERCENT + ((kingdom->unrest_level+1)/10));
    
    // Check story override first
    if (kingdom->story_skirmish_override == -1) return; // Story prevents skirmish
    if (kingdom->story_skirmish_override == 1) {
        // Story forces skirmish - skip chance roll
    } else {
        // Normal chance, modified by story
        modified_chance = (int)(HOURLY_SKIRMISH_BASE_CHANCE_PERCENT * kingdom->story_skirmish_chance_modifier);
    }
    
    // Reset override after use
    kingdom->story_skirmish_override = 0;
    
    // 12% chance per hour to trigger a skirmish
    if ((rand() % 100) < HOURLY_SKIRMISH_BASE_CHANCE_PERCENT) {
        int soldier_health = 0; int rebel_health = 0;
        int soldier_damage = 0; int rebel_damage = 0;  
        int soldier_defense = 0; int rebel_defense = 0;
        int soldier_count = 0; int rebel_count = 0;
        int general_count = 0; int leader_count = 0;

        int swordsman_count = 0;
        int archer_count = 0;
        int cavalry_count = 0;


        for (int i = 0; i < data->count; i++)
        {
            if (data->humans[i].alive == 1 && data->humans[i].kingdom_id == kingdom->id) {
                switch (data->humans[i].job)
                {
                case JOB_SWORDSMAN:
                    swordsman_count++;
                    soldier_health += data->humans[i].health;
                    soldier_damage += data->humans[i].damage;
                    soldier_defense += data->humans[i].defense;
                    if (data->humans[i].is_general){general_count++;}
                    break;
                
                case JOB_ARCHER:
                    archer_count++;
                    soldier_health += data->humans[i].health;
                    soldier_damage += data->humans[i].damage;
                    soldier_defense += data->humans[i].defense;
                    if (data->humans[i].is_general){general_count++;}
                    break;

                case JOB_CAVALRY:
                    cavalry_count++;
                    soldier_health += data->humans[i].health;
                    soldier_damage += data->humans[i].damage;
                    soldier_defense += data->humans[i].defense;
                    if (data->humans[i].is_general){general_count++;}
                    break;
                
                case JOB_REBEL:
                    rebel_count++;
                    rebel_health += data->humans[i].health;
                    rebel_damage += data->humans[i].damage;
                    rebel_defense += data->humans[i].defense;
                    if (data->humans[i].is_general){leader_count++;}
                } // REMINDER: Body equipment should enhance defense. E.g. iron boots gives +2 defense
            }
        }
 
        soldier_count = swordsman_count + archer_count + cavalry_count;

        // A skirmish can only happen if both sides have people to fight
        if (soldier_count == 0 || rebel_count == 0) {
            return;
        }

        int rebel_fighters = (int)(rebel_count*(float)(REBELS_IN_SKIRMISH));
        int soldier_fighters = (int)(soldier_count*(float)(SOLDIERS_IN_SKIRMISH));
        rebel_damage = (int)(rebel_damage*(float)(REBELS_IN_SKIRMISH));
        soldier_damage = (int)(soldier_damage*(float)(SOLDIERS_IN_SKIRMISH));
        rebel_health = (int)(rebel_health*(float)(REBELS_IN_SKIRMISH));
        soldier_health = (int)(soldier_health*(float)(SOLDIERS_IN_SKIRMISH));
        rebel_defense = (int)(rebel_defense*(float)(REBELS_IN_SKIRMISH));
        soldier_defense = (int)(soldier_defense*(float)(SOLDIERS_IN_SKIRMISH));

        int residual_soldier_health = soldier_health; int residual_rebel_health = rebel_health;
        int residual_soldier_damage = soldier_damage; int residual_rebel_damage = rebel_damage;
        int residual_soldier_defense = soldier_defense; int residual_rebel_defense = rebel_defense;
        int residual_soldier_count = soldier_fighters; int residual_rebel_count = rebel_fighters;
        int battle_state = 3;

        log_event("SKIRMISH! A small battle erupts!");
        log_event("%d Imperials and %d rebels!", soldier_fighters, rebel_fighters);

        if (leader_count > 0) {
            rebel_damage *= (1.0f + (REBEL_LEADER_COMBAT_BONUS - 1.0f) * leader_count);
        }

        if (general_count > 0) {
            soldier_damage *= (1.0f + (GENERAL_COMBAT_BONUS - 1.0f) * general_count);
        }
        
        while(battle_state==3){
            // Rebels alwyas attack first. They have the element of surprise. Dealing more damage.
            surprise_attack(&soldier_health, rebel_damage);
            tank_mode(&soldier_health, residual_soldier_health, &soldier_defense);
            regroup_soldiers(&soldier_fighters, residual_soldier_damage,
                            residual_soldier_health, residual_soldier_defense, 
                            &soldier_health, &soldier_damage, &soldier_defense);
            soldier_attack(&soldier_damage, &rebel_defense, &rebel_health);
            regroup_rebels(&rebel_fighters, residual_rebel_damage,
                            residual_rebel_health, residual_rebel_defense, 
                            &rebel_health, &rebel_damage, &rebel_defense);
            rebel_attack(rebel_damage, &rebel_defense, &rebel_health);
            regroup_soldiers(&soldier_fighters, residual_soldier_damage,
                            residual_soldier_health, residual_soldier_defense, 
                            &soldier_health, &soldier_damage, &soldier_defense);
            battle_state = declare_result(rebel_fighters, soldier_fighters, residual_rebel_count, residual_soldier_count);
        }

        int total_soldier_casualties = residual_soldier_count - soldier_fighters;
        int total_rebel_casualties = residual_rebel_count - rebel_fighters;

        if (total_soldier_casualties > 0 && soldier_count > 0) {
            // Distribute casualties proportionally
            int swordsman_casualties = round(total_soldier_casualties * ((float)swordsman_count / soldier_count));
            int archer_casualties = round(total_soldier_casualties * ((float)archer_count / soldier_count));
            int cavalry_casualties = round(total_soldier_casualties * ((float)cavalry_count / soldier_count));

            // Use the simple, correct tool to inflict the casualties
            inflict_casualties(data, kingdom->id, JOB_SWORDSMAN, swordsman_casualties);
            inflict_casualties(data, kingdom->id, JOB_ARCHER, archer_casualties);
            inflict_casualties(data, kingdom->id, JOB_CAVALRY, cavalry_casualties);
        }

        inflict_casualties(data, kingdom->id, JOB_REBEL, total_rebel_casualties);
        
        // Skirmishes affect morale
        kingdom->army_morale--;
        if (kingdom->army_morale < 0) kingdom->army_morale = 0;
    }
}

/**
 * @brief Updates the unrest level in a single kingdom.
 * This now runs HOURLY. Unrest decreases very slowly over time.
 */
void update_kingdom_unrest(struct Kingdom *kingdom, struct Human_Data *data) {
    if (!kingdom->is_active) return;

    // To balance the hourly call, we give it a 1 in 24 chance to decrease.
    // This averages out to about 1 point of unrest reduction per day.
    if (kingdom->unrest_level > 0 && (rand() % HOURLY_UNREST_DECAY_CHANCE_DIVISOR == 0)) {
        kingdom->unrest_level--;
    }
}


/**
 * @brief Manages dissent and military recruitment.
 * REVISED: Separates logic for civilians and soldiers to ensure both can become rebels.
 * Soldiers now have a slightly lower chance to defect.
 */
void handle_recruitment_and_dissent(struct Kingdom *kingdom, struct Human_Data *data) {
    if (!kingdom->is_active || kingdom->unrest_level <= DISSENT_THRESHOLD) return;

    int new_rebels_this_day = 0;
    const int max_new_rebels_per_day = MAX_NEW_REBELS_PER_DAY;
    int unrest_over_threshold = kingdom->unrest_level - DISSENT_THRESHOLD;
    if (unrest_over_threshold > MAX_UNREST_FOR_REBEL_CONVERSION) {
        unrest_over_threshold = MAX_UNREST_FOR_REBEL_CONVERSION;
    }

    for (int i = 0; i < data->count; i++) {
        if (data->humans[i].alive == 0 || data->humans[i].job == JOB_REBEL || data->humans[i].kingdom_id != kingdom->id) continue;
        if (new_rebels_this_day >= MAX_NEW_REBELS_PER_DAY) break; 

        bool is_soldier = (data->humans[i].job >= JOB_SWORDSMAN && data->humans[i].job <= JOB_CAVALRY);
        bool became_rebel = false;

        if (is_soldier) {
            int soldier_defection_chance = unrest_over_threshold * SOLDIER_DEFECTION_CHANCE_MODIFIER;
            if ((rand() % REBEL_CHANCE_DIVISOR) < soldier_defection_chance) { 
                kingdom->army_morale -= 5;
                data->humans[i].job = JOB_REBEL;
                became_rebel = true;
            }
        } else {
            if ((rand() % REBEL_CHANCE_DIVISOR) < unrest_over_threshold) { 
                data->humans[i].job = JOB_REBEL;
                became_rebel = true;
            } 
        }

        if (became_rebel) {
            new_rebels_this_day++;
            if (rand() % 100 < REBEL_LEADER_SPAWN_CHANCE_PERCENT) { 
                data->humans[i].is_general = 1;
            }
        }
    }

    if (kingdom->army_morale < 0) kingdom->army_morale = 0;
}

/**
 * @brief Checks if the number of rebels and soldiers is close enough to trigger all-out war.
 * @return 1 if war is triggered, 0 otherwise.
 */
int check_civil_war_trigger(struct Human_Data *data) {
    int rebel_count = 0;
    int soldier_count = 0;

    for (int i = 0; i < data->count; i++) {
        if (data->humans[i].alive == 1) {
            // Count ALL military units, not just swordsmen
            if (data->humans[i].job >= JOB_SWORDSMAN && data->humans[i].job <= JOB_CAVALRY) {
                soldier_count++;
            } else if (data->humans[i].job == JOB_REBEL) {
                rebel_count++;
            }
        }
    }
    
    // Trigger condition: There must be a significant number of rebels, and they must be
    // numerous enough to challenge the army (e.g., at least 75% of the army's size).
    if (rebel_count > CIVIL_WAR_MINIMUM_REBELS && rebel_count > (soldier_count * CIVIL_WAR_REBEL_TO_SOLDIER_RATIO)) {
        return 1;
    }

    return 0;
}

/**
 * @brief The special event for the Empire's collapse.
 * @return Returns 1 if the empire fell, 0 otherwise.
 */
int check_for_empire_collapse(struct Kingdom kingdoms[], struct Human_Data *data, int total_population) {
    if (!kingdoms[0].is_active || kingdoms[0].unrest_level < REBELLION_THRESHOLD) {
        return 0; // Not enough unrest, or empire already fell.
    }

    log_event("--- THE EMPIRE HAS FALLEN! ---");
    log_event("From the ashes, 7 new kingdoms arise!");

    kingdoms[0].is_active = 0; // The Empire is no more.

    // Activate the 7 new kingdoms. Names are assigned in kingdoms.c
    for (int i = 1; i <= 7; i++) {
        kingdoms[i].is_active = 1;
        kingdoms[i].population = 0;
    }

    // Reassign every living human to one of the new kingdoms.
    for (int i = 0; i < total_population; i++) {
        if (data->humans[i].alive == 1) {
            int new_kingdom_id = (rand() % 7) + 1;
            data->humans[i].kingdom_id = new_kingdom_id;

            if (data->humans[i].job == 6 || data->humans[i].job == 7) {
                data->humans[i].job = 1; // Former rebels/soldiers become farmers.
            }
        }
    }
    
    // Recalculate populations for the new kingdoms
    for(int i = 0; i < total_population; i++) {
        if(data->humans[i].alive == 1) {
            kingdoms[data->humans[i].kingdom_id].population++;
        }
    }

    return 1; // The empire has fallen.
}