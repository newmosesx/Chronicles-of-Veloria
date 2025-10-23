// file: shared_data.h

#ifndef SHARED_DATA_H
#define SHARED_DATA_H

#include <stdbool.h>
#include <pthread.h>
#include "game_config.h"

// --- A struct to hold all GUI-relevant data for ONE kingdom ---
typedef struct {
    bool is_active;
    int population;
    int unrest_level;
    int army_morale;
    int treasury;

    // Raw resource counts
    int food;
    int wood;
    int stone;
    int metal;
    
    // Detailed job counts
    int job_counts[10]; // Index corresponds to JOB_* defines

} GuiKingdomData;


// --- The main shared data struct ---
typedef struct {
    // High-level stats are still here
    char world_population[50];
    char current_hour[20];
    char civil_war_status[50];
    
    // --- The big upgrade ---
    // Instead of many separate arrays, we have one clean array of structs.
    GuiKingdomData kingdoms[NUM_KINGDOMS];

    // These story-related fields are fine as they are
    int current_story_chapter;
    int current_story_paragraph;

} GuiSharedData;
// The 'extern' keyword is a promise to the compiler:
// "These variables exist and are defined in ONE .c file somewhere else.
// You don't need to create memory for them, just know that they exist."
extern GuiSharedData g_shared_data;
extern pthread_mutex_t g_data_mutex;

// Condition variable to signal story changes immediately
extern pthread_cond_t g_story_cond;
extern pthread_mutex_t g_story_mutex;
extern bool g_story_position_changed;

#endif // SHARED_DATA_H