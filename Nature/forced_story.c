// file: forced_story.c

#include <stdio.h>
#include "../forced_story.h"
#include "../logger.h"
#include "../shared_data.h"


int ch_1_3 = 0;
int ch_1_7 = 0;
int ch_1_9 = 0;

int ch_8_0 = 0;
int ch_8_12 = 0;
int ch_8_26 = 0;

void update_all_kingdom_details_for_gui(struct Kingdom kingdoms[], struct Human_Data *data, GuiSharedData *target_data);


// --- Helper Function to add people with a specific job ---
// This makes scripting events much cleaner.
static void add_people_with_job(int count, int job_id, int kingdom_id, struct Human_Data *data) {
    if (count <= 0) return;

    int converted = 0;
    // Convert ANY living person in the kingdom, regardless of current job
    for (int i = 0; i < data->count && converted < count; i++) {
        if (data->humans[i].alive == 1 && data->humans[i].kingdom_id == kingdom_id) {
            data->humans[i].job = job_id;
            converted++;
        }
    }
}

void force_gui_update_now(struct Kingdom kingdoms[], struct Human_Data *data) {
    pthread_mutex_lock(&g_data_mutex);
    update_all_kingdom_details_for_gui(kingdoms, data, &g_shared_data);
    pthread_mutex_unlock(&g_data_mutex);
}

// Story control functions
static void set_skirmish_control(struct Kingdom *kingdom, int override, float chance_modifier) {
    kingdom->story_skirmish_override = override;
    kingdom->story_skirmish_chance_modifier = chance_modifier;
}

static void set_production_control(struct Kingdom *kingdom, float modifier, int food_cap) {
    kingdom->story_production_modifier = modifier;
    kingdom->story_food_daily_cap = food_cap;
}

// --- The Main Event Switchboard ---
// This is where you, the author, will add your scripted events.
// TEST VERSION - This will trigger effects on the very first paragraph
void apply_story_effects(int chapter_index, int paragraph_index, struct Kingdom kingdoms[], struct Human_Data *data) {
    struct Kingdom *empire = &kingdoms[0];

    // if it dosen't, is because is not applying fast enough. If you wait it should apply.
    // Would be better to call this function after clicking.
    if (chapter_index == 0) { // Chapter 1
        if (paragraph_index == 3 && ch_1_3 == 0) {
            log_event("The crossroads meeting stirs the populace...");
            empire->unrest_level += 1;
            add_people_with_job(15, JOB_REBEL, empire->id, data);
            force_gui_update_now(kingdoms, data);
            ch_1_3 = 1;
        }
        else if (paragraph_index == 7 && ch_1_7 == 0) {
            log_event("News of twisted beasts spreads panic!");
            empire->unrest_level += 1;
            add_people_with_job(20, JOB_REBEL, empire->id, data);
            force_gui_update_now(kingdoms, data);
            ch_1_7 = 1;
        }
        else if (paragraph_index == 9 && ch_1_9 == 0) {
            log_event("The discovery of created monsters... terrifies the people!");
            empire->army_morale -= 2;
            empire->unrest_level += 5;
            add_people_with_job(30, JOB_REBEL, empire->id, data);
            force_gui_update_now(kingdoms, data);
            ch_1_9 = 1;
        }
    }
    else if (chapter_index == 7) { // Chapter 8
        if (paragraph_index == 0 && ch_8_0 == 0) {
            log_event("The rebels gain a new charismatic leader!");
            empire->unrest_level += 100;
            add_people_with_job(1200, JOB_REBEL, empire->id, data);
            force_gui_update_now(kingdoms, data);
            set_skirmish_control(empire, -1, 1.0);
            ch_8_0 = 1;
        }
        else if (paragraph_index >= 1 && paragraph_index <= 12 && ch_8_12 == 0) {
            set_skirmish_control(empire, -1, 1.0);
            ch_8_12 = 1;
        }
        else if (paragraph_index >= 13 && paragraph_index <= 26 && ch_8_26 == 0) {
            force_skirmish(360, 480, kingdoms, data);
            force_gui_update_now(kingdoms, data);
            ch_8_26 = 1;
        }
    }
}

// Stop any fights for 1 hour (5secs)- set_skirmish_control(empire, -1, 1.0);
// Halve production and cap food at 2000 - set_production_control(empire, 0.5, 2000);
// Triple skirmish chance - set_skirmish_control(empire, 0, 3.0);