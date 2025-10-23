// file: kingdoms.c

#include <stdio.h>
#include <stdlib.h>
#include "../humans.h"
#include "../logger.h" // For logging events
#include "../game_config.h"
#include "../shared_data.h"

/**
 * @brief Creates a specified number of new humans and assigns them a job and kingdom.
 * This directly expands the main human array.
 */
void create_new_humans_as_job(int count, int job_id, int kingdom_id, struct Human_Data *data) {
    if (count <= 0) return;

    // --- Safely reallocate memory if needed ---
    int required_capacity = data->count + count;
    if (required_capacity > data->capacity) {
        int new_capacity = required_capacity * HUMAN_ARRAY_GROWTH_FACTOR;
        void *temp_ptr = realloc(data->humans, new_capacity * sizeof(struct Human_Stats));
        if (temp_ptr == NULL) {
            fprintf(stderr, "Error: Failed to reallocate memory for divine reinforcements.\n");
            return;
        }
        data->humans = temp_ptr;
        data->capacity = new_capacity;
    }

    // --- Create the new humans at the end of the array ---
    int start_index = data->count;
    for (int i = start_index; i < start_index + count; i++) {
        struct Human_Stats human;
        human.name = "Divine Recruit";
        human.health = 100;
        human.hunger = 100;
        human.job = job_id;
        human.is_general = 0; // Divine recruits are not leaders
        human.bronze = STARTING_BRONZE;
        human.alive = 1;
        human.kingdom_id = kingdom_id;
        data->humans[i] = human;
    }
    data->count += count; // Update the total count
}

void recalculate_kingdom_populations(struct Kingdom kingdoms[], struct Human_Data *data) {
    for (int i = 0; i < NUM_KINGDOMS; i++) {
        if (kingdoms[i].is_active) {
            kingdoms[i].population = 0;
        }
    }
    for (int i = 0; i < data->count; i++) {
        if (data->humans[i].alive == 1) {
            int kingdom_id = data->humans[i].kingdom_id;
            if (kingdom_id >= 0 && kingdom_id < NUM_KINGDOMS) {
                kingdoms[kingdom_id].population++;
            }
        }
    }
}

/**
 * @brief Initializes the world with a single, unified Empire and dormant successor states.
 */
void initialize_world_polities(struct Kingdom kingdoms[]) {
    log_event("A great Empire rises...");
    
    kingdoms[0] = (struct Kingdom){
        .id = 0, .name = "The Great Empire", .unrest_level = 0, .is_active = 1,
        .food = INITIAL_EMPIRE_FOOD, .wood = INITIAL_EMPIRE_WOOD, .stone = INITIAL_EMPIRE_STONE, .metal = INITIAL_EMPIRE_METAL, .army_morale = INITIAL_EMPIRE_MORALE,
        .treasury = INITIAL_EMPIRE_TREASURY,
        .story_skirmish_override = 0,
        .story_skirmish_chance_modifier = 1.0,
        .story_production_modifier = 1.0,
        .story_food_daily_cap = 0,
        .story_consumption_modifier = 1.0,
        .divine_tax_modifier = 1.0f,
        .divine_production_modifier = 1.0f,
        .divine_penalty_timer_days = 0,
        .can_use_divine_intervention = true
};

    // The 7 successor kingdoms are defined but inactive
    kingdoms[1] = (struct Kingdom){.id = 1, .name = "Esther", .food = INITIAL_SUCCESSOR_FOOD, .wood = INITIAL_SUCCESSOR_WOOD, .army_morale = INITIAL_SUCCESSOR_MORALE, .treasury = INITIAL_SUCCESSOR_TREASURY,
        .story_skirmish_override = 0,
        .story_skirmish_chance_modifier = 1.0,
        .story_production_modifier = 1.0,
        .story_food_daily_cap = 0
    };
    kingdoms[2] = (struct Kingdom){.id = 2, .name = "Magento", .is_active = 0, .food = INITIAL_SUCCESSOR_FOOD, .wood = INITIAL_SUCCESSOR_WOOD, .army_morale = INITIAL_SUCCESSOR_MORALE, .treasury = INITIAL_SUCCESSOR_TREASURY,
        .story_skirmish_override = 0,
        .story_skirmish_chance_modifier = 1.0,
        .story_production_modifier = 1.0,
        .story_food_daily_cap = 0
    };
    kingdoms[3] = (struct Kingdom){.id = 3, .name = "Nightingale", .is_active = 0, .food = INITIAL_SUCCESSOR_FOOD, .wood = INITIAL_SUCCESSOR_WOOD, .army_morale = INITIAL_SUCCESSOR_MORALE, .treasury = INITIAL_SUCCESSOR_TREASURY,
        .story_skirmish_override = 0,
        .story_skirmish_chance_modifier = 1.0,
        .story_production_modifier = 1.0,
        .story_food_daily_cap = 0
    };
    kingdoms[4] = (struct Kingdom){.id = 4, .name = "Lionheart", .is_active = 0, .food = INITIAL_SUCCESSOR_FOOD, .wood = INITIAL_SUCCESSOR_WOOD, .army_morale = INITIAL_SUCCESSOR_MORALE, .treasury = INITIAL_SUCCESSOR_TREASURY,
        .story_skirmish_override = 0,
        .story_skirmish_chance_modifier = 1.0,
        .story_production_modifier = 1.0,
        .story_food_daily_cap = 0
    };
    kingdoms[5] = (struct Kingdom){.id = 5, .name = "Rethmar", .is_active = 0, .food = INITIAL_SUCCESSOR_FOOD, .wood = INITIAL_SUCCESSOR_WOOD, .army_morale = INITIAL_SUCCESSOR_MORALE, .treasury = INITIAL_SUCCESSOR_TREASURY,
        .story_skirmish_override = 0,
        .story_skirmish_chance_modifier = 1.0,
        .story_production_modifier = 1.0,
        .story_food_daily_cap = 0
    };
    kingdoms[6] = (struct Kingdom){.id = 6, .name = "Tehran", .is_active = 0, .food = INITIAL_SUCCESSOR_FOOD, .wood = INITIAL_SUCCESSOR_WOOD, .army_morale = INITIAL_SUCCESSOR_MORALE, .treasury = INITIAL_SUCCESSOR_TREASURY,
        .story_skirmish_override = 0,
        .story_skirmish_chance_modifier = 1.0,
        .story_production_modifier = 1.0,
        .story_food_daily_cap = 0
    };
    kingdoms[7] = (struct Kingdom){.id = 7, .name = "Asfahan", .is_active = 0, .food = INITIAL_SUCCESSOR_FOOD, .wood = INITIAL_SUCCESSOR_WOOD, .army_morale = INITIAL_SUCCESSOR_MORALE, .treasury = INITIAL_SUCCESSOR_TREASURY,
        .story_skirmish_override = 0,
        .story_skirmish_chance_modifier = 1.0,
        .story_production_modifier = 1.0,
        .story_food_daily_cap = 0
    };
}

/**
 * @brief SHARED LOGIC for daily management of any active kingdom.
 * This version has been REFACTORED to be highly efficient to prevent GUI freezes.
 */
void manage_kingdom_daily(struct Kingdom *kingdom, struct Human_Data *data) {
    if (!kingdom->is_active) return;

    if (kingdom->divine_penalty_timer_days > 0) {
        kingdom->divine_penalty_timer_days--;
        if (kingdom->divine_penalty_timer_days == 0) {
            log_event("Divine collateral has been paid for %s. The economy returns to normal.", kingdom->name);
            kingdom->divine_tax_modifier = 1.0f;
            kingdom->divine_production_modifier = 1.0f;
        }
    }

    kingdom->can_use_divine_intervention = true;

    // --- Store "Before" State ---
    int unrest_before = kingdom->unrest_level;
    int pop_before = kingdom->population;
    int food_before = kingdom->food;
    int wood_before = kingdom->wood;
    int stone_before = kingdom->stone;
    int metal_before = kingdom->metal;

    // --- Run Daily Logic ---
    collect_taxes(kingdom, data);
    update_kingdom_unrest(kingdom, data);
    handle_recruitment_and_dissent(kingdom, data);
    recruit_soldiers(kingdom, data);
    EmpireAI(kingdom, data);

    // Morale Management (This is a daily check)
    if (kingdom->food > kingdom->population * MORALE_FOOD_SURPLUS_MULTIPLIER && kingdom->army_morale < 100) { kingdom->army_morale += MORALE_GAIN_FROM_SURPLUS; }
    if (kingdom->unrest_level > DISSENT_THRESHOLD && kingdom->army_morale > MINIMUM_MORALE_FOR_UNREST_LOSS) { kingdom->army_morale -= MORALE_LOSS_FROM_UNREST; }

    // After potential famine deaths, recount the population
    recalculate_kingdom_populations(kingdom, data);

}

// Add this new function to the bottom of kingdoms.c

/**
 * @brief A more advanced AI governor for managing a kingdom.
 * This AI uses a tiered decision-making process:
 * 1. CATASTROPHE AVERSION: Checks for immediate, game-ending threats (like starvation).
 * 2. REACTIVE MANAGEMENT: If no catastrophe, it identifies the most pressing current issue (unrest, weak military, low food) and addresses it.
 * 3. PROACTIVE MANAGEMENT: If the kingdom is stable, it works towards long-term goals, like building up the army to an ideal size.
 */
void EmpireAI(struct Kingdom *kingdom, struct Human_Data *data) {
    // --- 1. Intelligence Gathering Phase ---
    // Avoids action if the population is too small to matter.
    if (kingdom->population < 100) return;

    // Calculate key metrics
    int daily_food_consumption = kingdom->population + 1;
    float food_days_left = (daily_food_consumption > 0) ? (float)kingdom->food / daily_food_consumption : 999;
    int soldier_count = 0, rebel_count = 0;
    for (int i = 0; i < data->count; i++) {
        if (data->humans[i].alive == 1 && data->humans[i].kingdom_id == kingdom->id) {
            if (data->humans[i].job >= JOB_SWORDSMAN && data->humans[i].job <= JOB_CAVALRY) soldier_count++;
            if (data->humans[i].job == JOB_REBEL) rebel_count++;
        }
    }
    
    float military_ratio = (float)soldier_count / (float)(rebel_count + 1);
    float food_urgency = (food_days_left < AI_FOOD_DAYS_THRESHOLD) ? 1.0f - (food_days_left / AI_FOOD_DAYS_THRESHOLD) : 0.0f;
    float unrest_urgency = (float)kingdom->unrest_level / (float)REBELLION_THRESHOLD;
    float military_urgency = (military_ratio < 1.0f) ? 1.0f - military_ratio : 0.0f;

    // --- TIER 0: DIVINE INTERVENTION ---
    // The AI's last resort when it is rich but desperate.
    if (kingdom->can_use_divine_intervention && kingdom->treasury > AI_DIVINE_INTERVENTION_TREASURY_THRESHOLD) {
        // Condition 1: About to be militarily overrun
        if (military_urgency > 0.8f) {
            log_event("GOVERNOR AI: Our armies are collapsing! We pray for divine reinforcements!");
            kingdom->treasury -= DI_COST_REINFORCEMENTS;
            create_new_humans_as_job(DI_REINFORCEMENTS_COUNT, JOB_SWORDSMAN, kingdom->id, data);
            kingdom->divine_tax_modifier = DI_PENALTY_TAX_MODIFIER;
            kingdom->divine_penalty_timer_days = PENALTY_DURATION_DAYS;
            kingdom->can_use_divine_intervention = false; // Used its one miracle for the day
            return;
        }
        // Condition 2: Mass unrest threatening imminent collapse
        if (kingdom->unrest_level > DI_ABSOLUTION_UNREST_THRESHOLD && unrest_urgency > 0.7f) {
            log_event("GOVERNOR AI: The people threaten to tear down the walls! We beg for divine absolution!");
            kingdom->treasury -= DI_COST_ABSOLUTION;
            kingdom->unrest_level -= DI_ABSOLUTION_UNREST_REDUCTION;
            kingdom->army_morale -= DI_PENALTY_MORALE_DROP;
            if (kingdom->army_morale < 0) kingdom->army_morale = 0;
            kingdom->divine_penalty_timer_days = PENALTY_DURATION_DAYS; // Can apply a generic penalty if desired
            kingdom->can_use_divine_intervention = false;
            return;
        }
        // Condition 3: Catastrophic famine imminent
        if (food_urgency > 0.9f) {
            log_event("GOVERNOR: The granaries are empty and the people starve! We plead for a miracle of sustenance!");
            kingdom->treasury -= DI_COST_SUSTENANCE;
            kingdom->food += DI_SUSTENANCE_FOOD_AMOUNT;
            kingdom->divine_production_modifier = DI_PENALTY_PRODUCTION_MODIFIER;
            kingdom->divine_penalty_timer_days = PENALTY_DURATION_DAYS;
            kingdom->can_use_divine_intervention = false;
            return;
        }
    }


    // --- 2. Tier 1: Catastrophe Aversion ---
    // If we are about to starve, this is the ONLY priority. Nothing else matters.
    if (food_days_left < AI_CRITICAL_FOOD_DAYS_THRESHOLD) {
        log_event("GOVERNOR: Reassigning all available");
        int converted_workers = 0;
        for (int i = 0; i < data->count && converted_workers < AI_FARMER_CONVERSION_COUNT; i++) {
            if (data->humans[i].alive == 1 && data->humans[i].kingdom_id == kingdom->id) {
                // Convert non-essential jobs to farmers
                if (data->humans[i].job == JOB_LUMBERJACK || data->humans[i].job == JOB_MINER || data->humans[i].job == JOB_BLACKSMITH) {
                    data->humans[i].job = JOB_FARMER;
                    converted_workers++;
                }
            }
        }
        return; // Override all other logic
    }

    // --- 3. Tier 2: Reactive Problem Solving ---
    // Calculate urgency scores for major problems.

    // Find the most urgent problem
    float max_urgency = 0.0f;
    if (food_urgency > max_urgency) max_urgency = food_urgency;
    if (unrest_urgency > max_urgency) max_urgency = unrest_urgency;
    if (military_urgency > max_urgency) max_urgency = military_urgency;
    
    // Only act if the problem is significant enough to warrant a response.
    if (max_urgency > AI_ACTION_THRESHOLD) {
        if (military_urgency == max_urgency) {
            log_event("GOVERNOR: Prioritizing recruitment.");
            recruit_soldiers(kingdom, data);
            return;
        }
        if (unrest_urgency == max_urgency && kingdom->treasury >= AI_FESTIVAL_COST) {
            log_event("GOVERNOR: Hosting festivals to calm the populace.");
            kingdom->treasury -= AI_FESTIVAL_COST;
            kingdom->unrest_level -= AI_FESTIVAL_UNREST_REDUCTION;
            if (kingdom->unrest_level < 0) kingdom->unrest_level = 0;
            return;
        }
        if (food_urgency == max_urgency) {
            log_event("GOVERNOR: Assigning more workers to farms.");
            int converted_workers = 0;
            for (int i = 0; i < data->count && converted_workers < AI_FARMER_CONVERSION_COUNT / 2; i++) { // Less drastic than the catastrophe response
                if (data->humans[i].alive == 1 && data->humans[i].kingdom_id == kingdom->id && (data->humans[i].job == JOB_LUMBERJACK || data->humans[i].job == JOB_MINER)) {
                    data->humans[i].job = JOB_FARMER;
                    converted_workers++;
                }
            }
            return;
        }
    }

    // --- 4. Tier 3: Proactive Management ---
    // If there are no immediate crises, the AI works on long-term stability.
    // Its primary goal is to maintain a standing army relative to its population.
    float ideal_army_size = kingdom->population * AI_ARMY_SIZE_GOAL_PERCENT;
    if (soldier_count < ideal_army_size) {
        log_event("GOVERNOR: The kingdom is stable.");
        recruit_soldiers(kingdom, data); // Recruit slowly to reach the ideal size
    } else {
        // If all goals are met, the AI does nothing and saves resources.
        // You could add a log_event here for debugging if you want.
        // log_event("GOVERNOR AI: The kingdom is stable and well-defended. Conserving resources.");
    }
}

// --- Unique Kingdom Entry Points ---

void manage_empire(struct Kingdom *self, struct Human_Data *data) {
    if (!self->is_active) return;
    manage_kingdom_daily(self, data);
}

void EstherKingdom(struct Kingdom *self, struct Human_Data *data) {
    if (!self->is_active) return;
    // This is where you could add unique behavior for Esther
    manage_kingdom_daily(self, data);
}

void MagentanoKingdom(struct Kingdom *self, struct Human_Data *data) {
    if (!self->is_active) return;
    manage_kingdom_daily(self, data);
}

void NightingaleKingdom(struct Kingdom *self, struct Human_Data *data) {
    if (!self->is_active) return;
    manage_kingdom_daily(self, data);
}

void LionheartKingdom(struct Kingdom *self, struct Human_Data *data) {
    if (!self->is_active) return;
    manage_kingdom_daily(self, data);
}

void RethmarKingdom(struct Kingdom *self, struct Human_Data *data) {
    if (!self->is_active) return;
    manage_kingdom_daily(self, data);
}

void TehranKingdom(struct Kingdom *self, struct Human_Data *data) {
    if (!self->is_active) return;
    manage_kingdom_daily(self, data);
}

void AsfahanKingdom(struct Kingdom *self, struct Human_Data *data) {
    if (!self->is_active) return;
    manage_kingdom_daily(self, data);
}