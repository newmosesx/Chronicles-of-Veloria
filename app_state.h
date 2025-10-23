#ifndef APP_STATE_H
#define APP_STATE_H

#include <stdbool.h> // For the 'bool' type
#include "shared_data.h" // Because AppState contains a 'GuiSharedData' struct

// This is the SINGLE, AUTHORITATIVE definition for AppState.
// Any file that needs to know what AppState is will include this header.
typedef struct AppState {
    int current_chapter_index;
    int current_paragraph_index;
    float story_opacity;
    int fade_state; // 0=idle, 1=fading in, -1=fading out
    GuiSharedData sim_data;
    bool *character_window_open;
    bool *has_reached_end_of_chapter;
    int log_autoscroll_state; // 0=idle, 1=content added, 2=ready to scroll
    int last_log_write_index;
    bool show_policies_window;
    bool is_modal_active;
} AppState;

#endif // APP_STATE_H