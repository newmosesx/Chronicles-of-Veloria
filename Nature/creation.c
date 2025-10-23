#include <stdio.h>
#include <stdlib.h>
#include "../humans.h"
#include "../game_config.h"

void life(struct HumanPopulation *world) {
    world->human_population = INITIAL_POPULATION;
}

/*
With my GODLY power I move the dead guys to the back.
of the pointer. Using this sorting algorithm. (Forgot the name)
*/
void compact_dead_humans(struct Human_Data *data) {
    int write_index = 0;
    
    // Compact: move all living humans to the front
    for (int read_index = 0; read_index < data->count; read_index++) {
        if (data->humans[read_index].alive == 1) {
            if (write_index != read_index) {
                data->humans[write_index] = data->humans[read_index];
            }
            write_index++;
        }
    }
    
    // Update count to only living humans
    int old_count = data->count;
    data->count = write_index;
    
    // No need to reallocate here, we can just use less of the existing buffer.
    // The buffer will grow again naturally via the persona function when needed.
    //printf("Compacted humans: %d -> %d (removed %d dead)\n", 
    //       old_count, data->count, old_count - data->count);
}

/*
The first humans are created magically. I wonder who did it.
data->count should be the global human population in integer.
*/
void initialize_population(struct Human_Data *data)
{
    data->capacity = data->count; // Initial capacity is the starting population
    // Loop through the entire starting population
    for (int i = 0; i < data->count; i++)
    {
        struct Human_Stats human;
        human.name = "Adam"; 
        human.health = 200;
        human.hunger = 100;
        human.speed = (rand() % 30)+1;
        human.damage = (rand() % 30)+1;
        human.defense = (rand() % 30)+1;
        human.smart = (rand() % 30)+1;
        human.job = 0; // Unemployed
        human.is_general = 0;
        human.bronze = STARTING_BRONZE;
        human.alive = 1;
        human.kingdom_id = 0;
        data->humans[i] = human;
    }
}

/*
This calculates deaths and births.
Death by "Natural Causes" = Because so I felt like it.
Birth by "Food Surplus" = Food production is going wild.
*/
void calculate_stable_population_changes(struct Kingdom *kingdom, int current_population, int *out_births, int *out_deaths)
{
    if (current_population <= 0) {
        *out_births = 0;
        *out_deaths = 0;
        return;
    }

    // 1. Calculate Deaths from Natural Causes.
    float daily_natural_deaths = (current_population * DAILY_NATURAL_DEATH_RATE_PER_1000) / DAYS_IN_MONTH;
    float hourly_natural_deaths = daily_natural_deaths / ACTIVE_HOURS_PER_DAY;
    static float death_fraction = 0.0;
    death_fraction += hourly_natural_deaths;
    *out_deaths = (int)death_fraction;
    if (*out_deaths > 0) {
        death_fraction -= *out_deaths;
    }

    // 2. Calculate Births based on Food Surplus.
    int food_surplus = kingdom->food - current_population;
    if (food_surplus < 0) food_surplus = 0;
    float daily_births_from_surplus = food_surplus / FOOD_SURPLUS_PER_BIRTH;
    float hourly_births = daily_births_from_surplus / ACTIVE_HOURS_PER_DAY;
    static float birth_fraction = 0.0;
    birth_fraction += hourly_births;
    *out_births = (int)birth_fraction;
    if (*out_births > 0) {
        birth_fraction -= *out_births;
    }

    if (current_population < POPULATION_GROWTH_FLOOR && *out_deaths > *out_births) {
        *out_deaths = *out_births; 
    }
}

void persona(int new_births, struct HumanPopulation *world_stat, struct Human_Data *data, int empire_has_fallen)
{
    if (new_births <= 0) return;

    // --- Capacity-based reallocation ---
    int required_capacity = data->count + new_births;
    if (required_capacity > data->capacity) {
        int new_capacity = required_capacity * HUMAN_ARRAY_GROWTH_FACTOR; // Grow by 50% to avoid frequent reallocs
        void *temp_ptr = realloc(data->humans, new_capacity * sizeof(struct Human_Stats));
        if (temp_ptr == NULL) {
            fprintf(stderr, "Error: Failed to reallocate memory for new humans.\n");
            return; 
        }
        data->humans = temp_ptr;
        data->capacity = new_capacity;
    }

    int start_index = data->count;
    data->count += new_births;
    world_stat->human_population = data->count;

    for (int i = start_index; i < data->count; i++)
    {
        struct Human_Stats human;
        human.name = "Adam"; 
        human.health = 200;
        human.hunger = 100;
        human.speed = rand() % 10;
        human.damage = rand() % 10;
        human.defense = rand() % 10;
        human.smart = rand() % 10;
        human.job = 0; // Unemployed
        human.is_general = 0;
        human.bronze = STARTING_BRONZE;
        human.alive = 1;
        human.quirks[0] = rand() % 2;
        human.quirks[1] = rand() % 2;
        human.quirks[2] = rand() % 2;
        
        if (empire_has_fallen) {
            human.kingdom_id = (rand() % 7) + 1;
        } else {
            human.kingdom_id = 0;
        }
        data->humans[i] = human;
    }
}

/**
 * @brief Marks a specific number of living humans as deceased.
 */
void alive_status(int deaths_to_inflict, struct Human_Data *data)
{
    if (deaths_to_inflict <= 0) return;

    int casualties = 0;
    // This has better cache performance and avoids hitting the same dead person multiple times.
    int start_index = rand() % data->count;
    for (int i = 0; i < data->count; i++) {
        if (casualties >= deaths_to_inflict) break;

        int current_index = (start_index + i) % data->count;
        if (data->humans[current_index].alive == 1) {
            data->humans[current_index].alive = 0;
            data->humans[current_index].job = 0;
            casualties++;
        }
    }
}