#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../humans.h"
#include "../logger.h"
#include "../game_config.h"

// Forward declaration for the function we'll create in forced_story.c
void force_skirmish(int imperial_combatants, int rebel_combatants, struct Kingdom kingdoms[], struct Human_Data *data);

/**
 * @brief Core logic for running a single battle between two groups.
 * @param kingdom The kingdom where the battle takes place.
 * @param data The global human data.
 * @param imperial_fighters The number of imperial soldiers in this skirmish.
 * @param rebel_fighters The number of rebels in this skirmish.
 */
static void run_battle(struct Kingdom *kingdom, struct Human_Data *data, int imperial_fighters, int rebel_fighters) {
    if (imperial_fighters <= 0 || rebel_fighters <= 0) return;

    // --- OPTIMIZATION: Combine loops for finding leaders ---
    int general_count = 0;
    int rebel_leader_count = 0;
    for (int i = 0; i < data->count; i++) {
        if (data->humans[i].alive == 1 &&
            data->humans[i].is_general == 1 &&
            data->humans[i].kingdom_id == kingdom->id)
        {
            if (data->humans[i].job >= JOB_SWORDSMAN && data->humans[i].job <= JOB_CAVALRY) {
                general_count++;
            } else if (data->humans[i].job == JOB_REBEL) {
                rebel_leader_count++;
            }
        }
    }

    // Calculate strength (base count + morale bonus)
    float morale_modifier = 0.5 + (kingdom->army_morale / 100.0);
    float imperial_strength = imperial_fighters * morale_modifier;
    float rebel_strength = rebel_fighters * REBEL_BASE_STRENGTH_MODIFIER;
    
    // --- Apply General bonus ---
    if (general_count > 0) {
        log_event("The Imperial army is led by %d General(s)", general_count);
        for (int i = 0; i < general_count; i++) {
            imperial_strength *= GENERAL_COMBAT_BONUS; // Each general adds a 15% bonus
        }
    }

    // --- Apply Rebel Leader bonus ---
    if (rebel_leader_count > 0) {
        log_event("The rebels are rallied by %d leaders!", rebel_leader_count);
        for (int i = 0; i < rebel_leader_count; i++) {
            rebel_strength *= REBEL_LEADER_COMBAT_BONUS; // Each leader adds a 25% bonus
        }
    }

    int imperial_casualties = 0;
    int rebel_casualties = 0;
    const char *winner_title;

    // Battle logic
    if (imperial_strength > rebel_strength) {
        winner_title = "Imperial Victory!";
        rebel_casualties = (int)(rebel_fighters * (0.6 + (rand() % 30) / 100.0));   // 60-90% losses
        imperial_casualties = (int)(imperial_fighters * (0.1 + (rand() % 20) / 100.0)); // 10-30% losses
        kingdom->army_morale += MORALE_GAIN_ON_VICTORY;;
        if(kingdom->army_morale > 100) kingdom->army_morale = 100;
    } else {
        winner_title = "Rebel Victory!";
        imperial_casualties = (int)(imperial_fighters * (0.5 + (rand() % 30) / 100.0)); // 50-80% losses
        rebel_casualties = (int)(rebel_fighters * (0.2 + (rand() % 20) / 100.0));   // 20-40% losses
        kingdom->army_morale -= MORALE_LOSS_ON_DEFEAT;
        if(kingdom->army_morale < 0) kingdom->army_morale = 0;
    }

    // Ensure casualties don't exceed fighters
    if (imperial_casualties > imperial_fighters) imperial_casualties = imperial_fighters;
    if (rebel_casualties > rebel_fighters) rebel_casualties = rebel_fighters;

    // Inflict the casualties
    inflict_casualties(data, kingdom->id, JOB_SWORDSMAN, imperial_casualties); // Simplified to one troop type for now
    inflict_casualties(data, kingdom->id, JOB_REBEL, rebel_casualties);

    // Log the detailed battle report
    log_event("Battle..!: %s", winner_title);
    log_event("Engaged: %d Imperials vs. %d Rebels", imperial_fighters, rebel_fighters);
    log_event("Survivors: %d Imperials | %d Rebels", imperial_fighters - imperial_casualties, rebel_fighters - rebel_casualties);

    log_event("Rumours spread of your defeat.");
}

/**
 * @brief Public function to allow the story module to force a battle.
 */
void force_skirmish(int imperial_combatants, int rebel_combatants, struct Kingdom kingdoms[], struct Human_Data *data) {    
    // Use the core battle logic
    run_battle(&kingdoms[0], data, imperial_combatants, rebel_combatants);
}