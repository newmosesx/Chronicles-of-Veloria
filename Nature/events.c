// file: events.c

#include <stdio.h>
#include <stdlib.h>

#include "../events.h"
#include "../logger.h"
#include "../game_config.h"

// --- Helper Functions ---

/**
 * @brief Kills a specified number of random, non-military citizens in a kingdom.
 */
static void kill_random_civilians(struct Kingdom *kingdom, struct Human_Data *data, int count) {
    if (count <= 0) return;
    int casualties = 0;
    int attempts = 0;
    while (casualties < count && attempts < data->count * 2) {
        int rand_idx = rand() % data->count;
        // Target is alive, belongs to the kingdom, and is a civilian (job 1-5)
        if (data->humans[rand_idx].alive == 1 &&
            data->humans[rand_idx].kingdom_id == kingdom->id &&
            data->humans[rand_idx].job >= 1 && data->humans[rand_idx].job <= 5)
        {
            data->humans[rand_idx].alive = 0;
            casualties++;
        }
        attempts++;
    }
    log_event(" -> A disaster has claimed the lives of %d civilians in %s.\n", casualties, kingdom->name);
}


// --- Event Implementations ---

static void event_bountiful_harvest(struct Kingdom *kingdom) {
    int food_gain = HARVEST_BASE_FOOD_GAIN + (kingdom->population * HARVEST_POPULATION_FOOD_MULTIPLIER);
    kingdom->food += food_gain;
    log_event("EVENT: A bountiful harvest in %s\n", kingdom->name);
}

static void event_discovery_of_gold(struct Kingdom *kingdom, struct Human_Data *data) {
    log_event("EVENT: A vein of gold discovery in %s!\n", kingdom->name);
    for (int i = 0; i < data->count; i++) {
        if (data->humans[i].alive == 1 && data->humans[i].kingdom_id == kingdom->id) {
            data->humans[i].bronze += GOLD_DISCOVERY_BRONZE_BONUS; // Give a nice bonus to everyone
        }
    }
}

static void event_plague(struct Kingdom *kingdom, struct Human_Data *data) {
    log_event("EVENT: Sickness and decay!");
    int deaths = kingdom->population * PLAGUE_POPULATION_LOSS_PERCENT; // 10% of the population will perish
    kill_random_civilians(kingdom, data, deaths);
    kingdom->unrest_level += PLAGUE_UNREST_GAIN;
}

/**
 * @brief A severe drought strikes the kingdom, ruining the harvest and causing unrest.
 * This new version directly reduces food production for the day to zero.
 */
static void event_drought(struct Kingdom *kingdom, struct Human_Data *data) {
    log_event("EVENT: Heat wave, the fields are dust!");

    kingdom->food = kingdom->food*0.15; // keep 15% of total food

    if (kingdom->food < 0) kingdom->food=0;

    // Increase the unrest penalty to reflect the severity.
    kingdom->unrest_level += DROUGHT_UNREST_GAIN;
}

static void event_barbarian_raids(struct Kingdom *kingdom, struct Human_Data *data) {
    log_event("EVENT: Barbarians raiding !");
    int deaths = kingdom->population * BARBARIAN_POPULATION_LOSS_PERCENT; // 2% of population lost
    kill_random_civilians(kingdom, data, deaths);
    kingdom->wood = (int)(kingdom->wood * BARBARIAN_RAID_RESOURCE_LOSS_PERCENT); // Lose 20% of wood
    kingdom->stone = (int)(kingdom->stone * BARBARIAN_RAID_RESOURCE_LOSS_PERCENT); // Lose 20% of stone
    kingdom->unrest_level += BARBARIAN_RAID_UNREST_GAIN;
}

static void event_political_intrigue(struct Kingdom *kingdom) {
    log_event("EVENT: A plot is uncovered in the court!", kingdom->name);
    kingdom->unrest_level += POLITICAL_INTRIGUE_UNREST_GAIN;
}


// --- Main Event Trigger ---

void trigger_random_event(struct Kingdom *kingdom, struct Human_Data *data) {
    // Only active kingdoms with a population can have events
    if (!kingdom->is_active || kingdom->population < 50) {
        return;
    }

    if ((rand() % 100) >= DAILY_RANDOM_EVENT_CHANCE_PERCENT) {
        return; // Nothing happens today.
    }
    int event_id = rand() % TOTAL_RANDOM_EVENTS;

    switch (event_id) {
        case 0:
            event_bountiful_harvest(kingdom);
            break;
        case 1:
            event_discovery_of_gold(kingdom, data);
            break;
        case 2:
            event_plague(kingdom, data);
            break;
        case 3:
            event_drought(kingdom, data);
            break;
        case 4:
            event_barbarian_raids(kingdom, data);
            break;
        case 5:
            event_political_intrigue(kingdom);
            break;
    }
}