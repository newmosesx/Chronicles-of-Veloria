// forced_story.h

#ifndef FORCED_STORY_H
#define FORCED_STORY_H

#include "humans.h" // For access to Kingdom and Human_Data structs

/**
 * @brief Checks the current story location and applies any pre-planned game state changes.
 * This is the central function for narrative-driven events.
 * @param chapter_index The current chapter number from the story.
 * @param paragraph_index The current paragraph number within the chapter.
 * @param kingdoms The array of all kingdoms in the simulation.
 * @param data The array of all humans in the simulation.
 */
void apply_story_effects(int chapter_index, int paragraph_index, struct Kingdom kingdoms[], struct Human_Data *data);
void force_skirmish(int imperial_combatants, int rebel_combatants, struct Kingdom kingdoms[], struct Human_Data *data);

#endif // FORCED_STORY_H