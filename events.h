// events.h

#ifndef EVENTS_H
#define EVENTS_H

#include "humans.h" // Gives us access to Kingdom and Human_Data structs

/**
 * @brief The main entry point for the event system.
 * This function has a chance to trigger a random event for a given kingdom.
 * @param kingdom A pointer to the specific kingdom being affected.
 * @param data A pointer to the global human data array.
 */
void trigger_random_event(struct Kingdom *kingdom, struct Human_Data *data);

#endif // EVENTS_H