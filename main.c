// =============================================================================
// === main.c - The Combined Story and Simulation Application (Nuklear + GLFW GL2) ===
// =============================================================================

// --- Header Order: Standard C -> POSIX -> External Libs -> Project Libs ---

// Standard C/POSIX Headers
#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <errno.h>
#include <limits.h>

// Nuklear Implementation (needs to be defined before including headers)
#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT

#define NK_IMPLEMENTATION
#define NK_GLFW_GL2_IMPLEMENTATION

// External Library Headers
#include <GLFW/glfw3.h>
#include "GUI_lib/nuklear.h"
#include "GUI_lib/nuklear_glfw_gl2.h"

// Project-Specific Headers
#include "humans.h"
#include "game_config.h"
#include "story_data.h"
#include "characters.h"
#include "logger.h"
#include "events.h"
#include "shared_data.h"
#include "forced_story.h"
#include "app_state.h"
#include "player.h"
#include "Player/situation_gui.h"
#include "calculations.h"

#define WINDOW_WIDTH 1400
#define WINDOW_HEIGHT 700


// --- Global Data ---
GuiSharedData g_shared_data;
pthread_mutex_t g_data_mutex;
pthread_cond_t g_story_cond;
pthread_mutex_t g_story_mutex;
struct PlayerStat theplayer;
struct HumanPopulation world_stat;
struct Human_Data human_data;
struct Kingdom kingdoms[NUM_KINGDOMS];
bool g_story_position_changed = false;

// --- Helper Functions ---
static void update_story_and_fade(AppState *state) {
    pthread_mutex_lock(&g_data_mutex);
    g_shared_data.current_story_chapter = state->current_chapter_index;
    g_shared_data.current_story_paragraph = state->current_paragraph_index;
    pthread_mutex_unlock(&g_data_mutex);

    pthread_mutex_lock(&g_story_mutex);
    g_story_position_changed = true;
    pthread_cond_signal(&g_story_cond);
    pthread_mutex_unlock(&g_story_mutex);
    
    state->fade_state = -1; // Start fade-out
}

static const nk_rune extended_glyph_ranges[] = {
    0x0020, 0x007E,  // Basic Latin (standard ASCII)
    0x2018, 0x2019,  // ‘ and ’ (single smart quotes)
    0x201C, 0x201D,  // “ and ” (double smart quotes)
    0x2014, 0x2014,  // — (em dash)
    0
};

// --- Styling Function (from your template) ---
static void set_dark_theme(struct nk_context *ctx) {
    struct nk_color table[NK_COLOR_COUNT];
    struct nk_color background_dark = nk_rgb(RGB_BACKGROUND_DARK);
    struct nk_color background_light = nk_rgb(RGB_BACKGROUND_LIGHT);
    struct nk_color text_color = nk_rgb(RGB_TEXT_COLOR);
    struct nk_color border_color = nk_rgb(RGB_BORDER_COLOR);
    table[NK_COLOR_TEXT] = text_color;
    table[NK_COLOR_WINDOW] = background_dark;
    table[NK_COLOR_HEADER] = background_dark;
    table[NK_COLOR_BORDER] = border_color;
    table[NK_COLOR_BUTTON] = background_dark;
    table[NK_COLOR_BUTTON_HOVER] = background_light;
    table[NK_COLOR_BUTTON_ACTIVE] = background_light;
    table[NK_COLOR_TOGGLE] = background_light;
    table[NK_COLOR_TOGGLE_HOVER] = background_light;
    table[NK_COLOR_TOGGLE_CURSOR] = text_color;
    table[NK_COLOR_SELECT] = background_light;
    table[NK_COLOR_SELECT_ACTIVE] = background_light;
    table[NK_COLOR_SLIDER] = background_light;
    table[NK_COLOR_SLIDER_CURSOR] = border_color;
    table[NK_COLOR_SLIDER_CURSOR_HOVER] = nk_rgb(RGB_CURSOR_HOVER_COLOR);
    table[NK_COLOR_SLIDER_CURSOR_ACTIVE] = nk_rgb(RGB_CURSOR_ACTIVE_COLOR);
    table[NK_COLOR_PROPERTY] = background_light;
    table[NK_COLOR_EDIT] = background_light;
    table[NK_COLOR_EDIT_CURSOR] = text_color;
    table[NK_COLOR_COMBO] = background_light;
    table[NK_COLOR_CHART] = background_light;
    table[NK_COLOR_CHART_COLOR] = text_color;
    table[NK_COLOR_CHART_COLOR_HIGHLIGHT] = nk_rgb(RGB_CHART_HIGHLIGHT);
    table[NK_COLOR_SCROLLBAR] = background_light;
    table[NK_COLOR_SCROLLBAR_CURSOR] = border_color;
    table[NK_COLOR_SCROLLBAR_CURSOR_HOVER] = nk_rgb(RGB_CURSOR_HOVER_COLOR);
    table[NK_COLOR_SCROLLBAR_CURSOR_ACTIVE] = nk_rgb(RGB_CURSOR_ACTIVE_COLOR);
    table[NK_COLOR_TAB_HEADER] = background_dark;
    nk_style_from_table(ctx, table);

    // Fine-tuning
    float rounding = 4.0f;
    ctx->style.window.padding = nk_vec2(10, 10);
    ctx->style.window.group_padding = nk_vec2(10, 10);
    ctx->style.button.border = 0.0f;
    ctx->style.button.rounding = rounding;
    ctx->style.combo.rounding = rounding;
    ctx->style.edit.rounding = rounding;
    ctx->style.property.rounding = rounding;
    ctx->style.tab.rounding = rounding;
    ctx->style.scrollv.rounding = rounding;
    ctx->style.scrollh.rounding = rounding;
    ctx->style.window.rounding = rounding;
}

/**
 * @brief Draws log entries from the global circular buffer one by one.
 * This ensures the parent nk_group can correctly calculate its content height.
 */
static void draw_log_from_circular_buffer(struct nk_context *ctx) {
    // Set a dynamic row layout so each label can determine its own height.
    nk_layout_row_dynamic(ctx, STARTING_ZERO, STARTING_ONE);

    // Safely read from the circular buffer
    pthread_mutex_lock(&g_event_log_mutex);
    int read_idx = g_log_read_index;
    while (read_idx != g_log_write_index) {
        if (g_log_entries[read_idx].timestamp > STARTING_ZERO) {
            // Using nk_label_wrap is safer for potentially long lines.
            nk_label_wrap(ctx, g_log_entries[read_idx].message);
        }
        read_idx = (read_idx + STARTING_ONE) % MAX_LOG_ENTRIES;
    }
    pthread_mutex_unlock(&g_event_log_mutex);
}

// --- The Main Application ---
int main(int argc, char **argv) {
    player_setup(&theplayer); // initalize player stats
    
    // --- All initialization code remains the same ---
    pthread_mutex_init(&g_data_mutex, NULL);
    init_logger();

    pthread_mutex_lock(&g_data_mutex);
    snprintf(g_shared_data.world_population, MAX_NUM_CHAR, "Population: 10000");
    snprintf(g_shared_data.current_hour, MAX_NUM_CHAR, "Day: 1, 00:00");
    snprintf(g_shared_data.civil_war_status, MAX_NUM_CHAR, "Status: Initializing...");
    for (int i = STARTING_ZERO; i < NUM_KINGDOMS; i++) {
        if (i == POSITION_ZERO) {
            g_shared_data.kingdoms[i].is_active = true;
            g_shared_data.kingdoms[i].population = 10000;
            g_shared_data.kingdoms[i].unrest_level = STARTING_ZERO;
            g_shared_data.kingdoms[i].treasury = INITIAL_EMPIRE_TREASURY;
        } else {
            g_shared_data.kingdoms[i].is_active = false;
            g_shared_data.kingdoms[i].population = STARTING_ZERO;
            g_shared_data.kingdoms[i].unrest_level = STARTING_ZERO;
            g_shared_data.kingdoms[i].treasury = STARTING_ZERO;
        }
    }
    pthread_mutex_unlock(&g_data_mutex);

    pthread_t sim_thread_id;
    printf("Starting simulation thread...\n");
    pthread_create(&sim_thread_id, NULL, simulation_thread_func, NULL);

    GLFWwindow *window;
    struct nk_context *ctx;
    struct nk_font *font_default = NULL, *font_large = NULL, *font_title = NULL;
    
    glfwInit();
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Chronicles of Veloria", NULL, NULL);
    glfwMakeContextCurrent(window);
    ctx = nk_glfw3_init(window, NK_GLFW3_INSTALL_CALLBACKS);
    
    struct nk_font_atlas *atlas;
    nk_glfw3_font_stash_begin(&atlas);
    struct nk_font_config config = nk_font_config(STARTING_ZERO);
    config.range = extended_glyph_ranges;
    const char *font_path = "civilization/Fonts/Cinzel-Bold.ttf";
    font_default = nk_font_atlas_add_from_file(atlas, font_path, 16.0f, &config);
    font_large = nk_font_atlas_add_from_file(atlas, font_path, 28.0f, &config);
    font_title = nk_font_atlas_add_from_file(atlas, font_path, 38.0f, &config);
    nk_glfw3_font_stash_end();

    if (!font_default || !font_large || !font_title) {
        fprintf(stderr, "Warning: Failed to load custom font '%s'. Falling back to default font.\n", font_path);
        nk_glfw3_font_stash_begin(&atlas);
        font_default = nk_font_atlas_add_default(atlas, 14.0f, STARTING_ZERO);
        font_large = nk_font_atlas_add_default(atlas, 24.0f, STARTING_ZERO);
        font_title = nk_font_atlas_add_default(atlas, 32.0f, STARTING_ZERO);
        nk_glfw3_font_stash_end();
    }
    
    nk_style_set_font(ctx, &font_default->handle);
    set_dark_theme(ctx);

    AppState state = {STARTING_ZERO};
    state.story_opacity = 1.0f;
    state.character_window_open = calloc(TOTAL_CHARACTERS, sizeof(bool));
    state.has_reached_end_of_chapter = calloc(TOTAL_CHAPTERS, sizeof(bool));
    if (story[POSITION_ZERO].paragraph_count == STARTING_ONE) { state.has_reached_end_of_chapter[POSITION_ZERO] = true; }
    if (state.character_window_open == NULL || state.has_reached_end_of_chapter == NULL) {
        fprintf(stderr, "Fatal: Could not allocate memory for UI state.\\n");
        return 1;
    }
    
    state.log_autoscroll_state = STARTING_ONE;
    pthread_mutex_lock(&g_event_log_mutex);
    state.last_log_write_index = g_log_write_index;
    pthread_mutex_unlock(&g_event_log_mutex);
    
    // --- 4. The Main Loop ---
    while(!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        nk_glfw3_new_frame();

        pthread_mutex_lock(&g_data_mutex);
        state.sim_data = g_shared_data;
        pthread_mutex_unlock(&g_data_mutex);
        
        if (state.fade_state == -STARTING_ONE) {
            state.story_opacity -= FADE_SPEED;
            if (state.story_opacity <= 0.0f) { state.story_opacity = 0.0f; state.fade_state = STARTING_ONE; }
        } else if (state.fade_state == STARTING_ONE) {
            state.story_opacity += FADE_SPEED;
            if (state.story_opacity >= 1.0f) { state.story_opacity = 1.0f; state.fade_state = STARTING_ZERO; }
        }

        // --- Step 1: Determine if ANY popup window is currently active ---
        bool is_any_popup_active = state.show_policies_window;
        if (!is_any_popup_active) {
            for (int i = STARTING_ZERO; i < TOTAL_CHARACTERS; i++) {
                if (state.character_window_open[i]) {
                    is_any_popup_active = true;
                    break;
                }
            }
        }

        // --- Step 2: Draw the main window. If a popup is active, lock the main window. ---
        nk_flags main_window_flags = NK_WINDOW_NO_SCROLLBAR;
        if (is_any_popup_active) {
            main_window_flags |= NK_WINDOW_NO_INPUT;
        }
        
        if (nk_begin(ctx, "Main", nk_rect(STARTING_ZERO, STARTING_ZERO, WINDOW_WIDTH, WINDOW_HEIGHT), main_window_flags)) {
            // --- All the code for drawing the Left, Center, and Right panels ---
            // --- This content is unchanged and remains here. ---
            nk_layout_row_template_begin(ctx, WINDOW_HEIGHT - STARTING_TWENTY);
            nk_layout_row_template_push_static(ctx, 350);
            nk_layout_row_template_push_variable(ctx, 750);
            nk_layout_row_template_push_static(ctx, 250);
            nk_layout_row_template_end(ctx);

            if (nk_group_begin(ctx, "LeftPanel", NK_WINDOW_NO_SCROLLBAR | NK_WINDOW_BORDER)) {
                nk_layout_row_dynamic(ctx, STARTING_TWENTY, STARTING_ONE);
                nk_label(ctx, state.sim_data.world_population, NK_TEXT_LEFT);
                nk_label(ctx, state.sim_data.current_hour, NK_TEXT_LEFT);
                nk_label(ctx, state.sim_data.civil_war_status, NK_TEXT_LEFT);
                nk_layout_row_dynamic(ctx, 10, STARTING_ONE); nk_spacer(ctx);
                nk_layout_row_dynamic(ctx, 300, STARTING_ONE);
                if (nk_group_begin(ctx, "Kingdoms", NK_WINDOW_BORDER)) {
                    for (int i = STARTING_ZERO; i < NUM_KINGDOMS; i++) {
                        if (state.sim_data.kingdoms[i].is_active) {
                            const char *title = (i==POSITION_ZERO) ? "The Great Empire" : "Successor Kingdom";
                            if (nk_tree_push_id(ctx, NK_TREE_TAB, title, (i==POSITION_ZERO ? NK_MAXIMIZED : NK_MINIMIZED), i)) {
                                char buffer[256];
                                nk_layout_row_dynamic(ctx, 15, STARTING_ONE);
                                snprintf(buffer, sizeof(buffer), "Population: %d", state.sim_data.kingdoms[i].population);
                                nk_label(ctx, buffer, NK_TEXT_LEFT);
                                snprintf(buffer, sizeof(buffer), "F: %d|W: %d|S: %d|M: %d|T: %d",
                                    state.sim_data.kingdoms[i].food,
                                    state.sim_data.kingdoms[i].wood, 
                                    state.sim_data.kingdoms[i].stone,
                                    state.sim_data.kingdoms[i].metal, 
                                    state.sim_data.kingdoms[i].treasury);
                                nk_label(ctx, buffer, NK_TEXT_LEFT);
                                int unrest = state.sim_data.kingdoms[i].unrest_level;
                                struct nk_color color = (unrest > REBELLION_THRESHOLD) ? nk_rgb(RGB_UNREST_RED) :
                                                        (unrest > DISSENT_THRESHOLD) ? nk_rgb(RGB_UNREST_YELLOW_LIGHT) : nk_rgb(RGB_UNREST_GRAY_LIGHT);
                                snprintf(buffer, sizeof(buffer), "Unrest: %d | Morale: %d", unrest, state.sim_data.kingdoms[i].army_morale);
                                nk_label_colored(ctx, buffer, NK_TEXT_LEFT, color);
                                int total_troops = state.sim_data.kingdoms[i].job_counts[JOB_SWORDSMAN] + state.sim_data.kingdoms[i].job_counts[JOB_ARCHER] + state.sim_data.kingdoms[i].job_counts[JOB_CAVALRY];
                                nk_labelf(ctx, NK_TEXT_LEFT, "Troops: %d | Rebels: %d", 
                                    total_troops, 
                                    state.sim_data.kingdoms[i].job_counts[JOB_REBEL]);
                                nk_tree_pop(ctx);
                            }
                        }
                    }
                    nk_group_end(ctx);
                }
                nk_layout_row_dynamic(ctx, 20, STARTING_ONE);
                nk_label(ctx, "Event Log", NK_TEXT_LEFT);
                nk_layout_row_dynamic(ctx, 250, STARTING_ONE);
                if (nk_group_begin(ctx, "LogGroup", NK_WINDOW_BORDER)) {
                    if (state.log_autoscroll_state == POSITION_TWO) { nk_group_set_scroll(ctx, "LogGroup", STARTING_ZERO, UINT_MAX); state.log_autoscroll_state = STARTING_ZERO; }
                    draw_log_from_circular_buffer(ctx);
                    pthread_mutex_lock(&g_event_log_mutex);
                    if (state.last_log_write_index != g_log_write_index) { state.log_autoscroll_state = STARTING_ONE; state.last_log_write_index = g_log_write_index; }
                    pthread_mutex_unlock(&g_event_log_mutex);
                    if (state.log_autoscroll_state == STARTING_ONE) { state.log_autoscroll_state = POSITION_TWO; }
                    nk_group_end(ctx);
                }
                nk_group_end(ctx);
            }
            if (nk_group_begin(ctx, "CenterPanel", NK_WINDOW_NO_SCROLLBAR)) {
                const Chapter *current_chapter = &story[state.current_chapter_index];
                struct nk_color text_color = nk_rgba(220, 220, 220, 255 * state.story_opacity);
                nk_style_push_font(ctx, &font_title->handle);
                nk_layout_row_dynamic(ctx, 45, STARTING_ONE); nk_label_colored(ctx, current_chapter->title, NK_TEXT_CENTERED, text_color);
                nk_style_pop_font(ctx);
                if (current_chapter->perspectives) {
                    const char* perspective = current_chapter->perspectives[state.current_paragraph_index];
                    if (perspective && strcmp(perspective, "Narrator") != 0) {
                        nk_layout_row_dynamic(ctx, 40, STARTING_ONE); nk_spacer(ctx);
                        nk_style_push_font(ctx, &font_large->handle);
                        nk_layout_row_dynamic(ctx, 30, STARTING_ONE); nk_label_colored(ctx, perspective, NK_TEXT_CENTERED, text_color);
                        nk_style_pop_font(ctx);
                    }
                }
                nk_layout_row_dynamic(ctx, 80, STARTING_ONE); nk_spacer(ctx);
                nk_style_push_font(ctx, &font_large->handle);
                float ratios[] = {0.15f, 0.70f, 0.15f};
                nk_layout_row(ctx, NK_DYNAMIC, 350, 3, ratios);
                nk_spacer(ctx);
                nk_label_colored_wrap(ctx, current_chapter->paragraphs[state.current_paragraph_index], text_color);
                nk_spacer(ctx);
                nk_style_pop_font(ctx);
                nk_group_end(ctx);
            }
            if (nk_group_begin(ctx, "RightPanel", NK_WINDOW_NO_SCROLLBAR)) {
                draw_player_actions_panel(ctx, &state);
                nk_layout_row_dynamic(ctx, 40, STARTING_ONE);
                if (nk_button_label(ctx, "Previous Chapter")) {
                    if (state.current_chapter_index > 0) {
                        state.current_chapter_index--; state.current_paragraph_index = STARTING_ZERO; update_story_and_fade(&state);
                    }
                }
                nk_layout_row_dynamic(ctx, 40, STARTING_ONE);
                bool is_unlocked = state.has_reached_end_of_chapter[state.current_chapter_index];
                if (is_unlocked) {
                    if (nk_button_label(ctx, "Next Chapter")) {
                        if (state.current_chapter_index < TOTAL_CHAPTERS - STARTING_ONE) {
                            state.current_chapter_index++; state.current_paragraph_index = STARTING_ZERO;
                            if (story[state.current_chapter_index].paragraph_count == STARTING_ONE) { state.has_reached_end_of_chapter[state.current_chapter_index] = true; }
                            update_story_and_fade(&state);
                        }
                    }
                } else {
                    struct nk_color disabled_bg = nk_rgb(40, 40, 40); struct nk_color disabled_text = nk_rgb(120, 120, 120);
                    nk_style_push_color(ctx, &ctx->style.button.normal.data.color, disabled_bg); nk_style_push_color(ctx, &ctx->style.button.hover.data.color, disabled_bg); nk_style_push_color(ctx, &ctx->style.button.active.data.color, disabled_bg);
                    nk_style_push_color(ctx, &ctx->style.button.text_normal, disabled_text); nk_style_push_color(ctx, &ctx->style.button.text_hover, disabled_text); nk_style_push_color(ctx, &ctx->style.button.text_active, disabled_text);
                    nk_button_label(ctx, "Next Chapter");
                    nk_style_pop_color(ctx); nk_style_pop_color(ctx); nk_style_pop_color(ctx); nk_style_pop_color(ctx); nk_style_pop_color(ctx); nk_style_pop_color(ctx);
                }
                nk_layout_row_dynamic(ctx, 20, STARTING_ONE); nk_spacer(ctx);
                nk_layout_row_dynamic(ctx, 40, STARTING_ONE);
                if (nk_combo_begin_label(ctx, "Characters", nk_vec2(nk_widget_width(ctx), 200))) {
                    nk_layout_row_dynamic(ctx, 25, STARTING_ONE);
                    for (int i=0; i < TOTAL_CHARACTERS; i++) {
                        if (nk_combo_item_label(ctx, characters[i].name, NK_TEXT_LEFT)) {
                            state.character_window_open[i] = true;
                        }
                    }
                    nk_combo_end(ctx);
                }
                float remaining_space = nk_window_get_content_region(ctx).h;
                nk_layout_row_dynamic(ctx, remaining_space - 280, STARTING_ONE); nk_spacer(ctx);
                nk_layout_row_dynamic(ctx, 40, STARTING_ONE);
                nk_style_push_color(ctx, &ctx->style.button.normal.data.color, nk_rgb(170, 40, 40)); nk_style_push_color(ctx, &ctx->style.button.hover.data.color, nk_rgb(180, 50, 50)); nk_style_push_color(ctx, &ctx->style.button.active.data.color, nk_rgb(190, 60, 60));
                nk_style_push_color(ctx, &ctx->style.button.text_normal, nk_rgb(255, 255, 255)); nk_style_push_color(ctx, &ctx->style.button.text_hover, nk_rgb(255, 255, 255)); nk_style_push_color(ctx, &ctx->style.button.text_active, nk_rgb(255, 255, 255));
                if (nk_button_label(ctx, "Quit")) { glfwSetWindowShouldClose(window, STARTING_ONE); }
                nk_style_pop_color(ctx); nk_style_pop_color(ctx); nk_style_pop_color(ctx); nk_style_pop_color(ctx); nk_style_pop_color(ctx); nk_style_pop_color(ctx);
                nk_group_end(ctx);
            }
        }
        nk_end(ctx); // The Main Window drawing ends here.

        // --- Step 3: Handle story progression clicks ONLY if no popup is active ---
        if (!is_any_popup_active) {
            if (state.fade_state == 0) {
                struct nk_rect center_panel_bounds = nk_rect(350.0f, 0.0f, WINDOW_WIDTH - 350.0f - 250.0f, WINDOW_HEIGHT);
                if (nk_input_is_mouse_click_in_rect(&ctx->input, NK_BUTTON_LEFT, center_panel_bounds)) {
                    if (state.current_paragraph_index < story[state.current_chapter_index].paragraph_count - STARTING_ONE) {
                        state.current_paragraph_index++;
                        if (state.current_paragraph_index == story[state.current_chapter_index].paragraph_count - STARTING_ONE) {
                            state.has_reached_end_of_chapter[state.current_chapter_index] = true;
                        }
                        update_story_and_fade(&state);
                    }
                } else if (nk_input_is_mouse_click_in_rect(&ctx->input, NK_BUTTON_RIGHT, center_panel_bounds)) {
                    if (state.current_paragraph_index > 0) {
                        state.current_paragraph_index--;
                        update_story_and_fade(&state);
                    }
                }
            }
            // For immidiate story effects.
            apply_story_effects(state.current_chapter_index, state.current_paragraph_index, kingdoms, &human_data);
        }

        if (state.show_policies_window) {
            // Draw dimming overlay
            if (nk_begin(ctx, "ModalContainer", nk_rect(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT), NK_WINDOW_BACKGROUND)) {
                struct nk_command_buffer *canvas = nk_window_get_canvas(ctx);
                struct nk_rect screen_bounds = nk_rect(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
                nk_fill_rect(canvas, screen_bounds, 0, nk_rgba(0, 0, 0, 150));
            }
            nk_end(ctx);
            
            // --- Define the centered position and new size for the Manager window ---
            int manager_width = 1000;
            int manager_height = 600;
            float manager_x = (WINDOW_WIDTH - manager_width) / 2.0f;
            float manager_y = (WINDOW_HEIGHT - manager_height) / 2.0f;

            // Draw the "Manager" window and handle its closure
            if (nk_begin(ctx, "Manager", nk_rect(manager_x, manager_y, manager_width, manager_height), NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_CLOSABLE | NK_WINDOW_TITLE)) {
                
                // --- Two Columns ---
                // We'll split the content area into a left and right side.
                // Left side for actions (300px), right side for the character sheet (variable).
                nk_layout_row_template_begin(ctx, 550); // Use a template for robust columns
                nk_layout_row_template_push_static(ctx, 300); // Left column width
                nk_layout_row_template_push_dynamic(ctx);     // Right column takes the rest
                nk_layout_row_template_end(ctx);

                // --- LEFT COLUMN: Governance & Edicts ---
                if (nk_group_begin(ctx, "EdictsGroup", NK_WINDOW_BORDER)) {
                    nk_layout_row_dynamic(ctx, 30, STARTING_ONE);
                    nk_label(ctx, "Governance & Edicts", NK_TEXT_CENTERED);
                    nk_layout_row_dynamic(ctx, 10, STARTING_ONE); // Spacer
                    
                    // --- EDICT 1: HOST A GRAND FESTIVAL (MOVED FROM situation_gui.c) ---
                    nk_layout_row_dynamic(ctx, 40, STARTING_ONE);
                    bool can_afford_festival = state.sim_data.kingdoms[POSITION_ZERO].treasury >= PLAYER_FESTIVAL_COST;

                    if (!can_afford_festival) {
                        struct nk_color disabled_bg = nk_rgb(40, 40, 40); struct nk_color disabled_text = nk_rgb(120, 120, 120);
                        nk_style_push_color(ctx, &ctx->style.button.normal.data.color, disabled_bg); nk_style_push_color(ctx, &ctx->style.button.hover.data.color, disabled_bg); nk_style_push_color(ctx, &ctx->style.button.active.data.color, disabled_bg);
                        nk_style_push_color(ctx, &ctx->style.button.text_normal, disabled_text); nk_style_push_color(ctx, &ctx->style.button.text_hover, disabled_text); nk_style_push_color(ctx, &ctx->style.button.text_active, disabled_text);
                    }

                    if (can_afford_festival) {
                        if (nk_button_label(ctx, "Host Festival (-50 Unrest)")) {
                            pthread_mutex_lock(&g_data_mutex);
                            if (g_shared_data.kingdoms[POSITION_ZERO].treasury >= PLAYER_FESTIVAL_COST) {
                                g_shared_data.kingdoms[POSITION_ZERO].treasury -= PLAYER_FESTIVAL_COST;
                                g_shared_data.kingdoms[POSITION_ZERO].unrest_level -= PLAYER_FESTIVAL_UNREST_REDUCTION;
                                if (g_shared_data.kingdoms[POSITION_ZERO].unrest_level < STARTING_ZERO) { g_shared_data.kingdoms[POSITION_ZERO].unrest_level = STARTING_ZERO; }
                                log_event("A grand festival is held! The people rejoice and unrest falls.");
                            }
                            pthread_mutex_unlock(&g_data_mutex);
                        }
                    } else {
                        nk_button_label(ctx, "Host Festival (150k)");
                    }

                    if (!can_afford_festival) {
                        nk_style_pop_color(ctx); nk_style_pop_color(ctx); nk_style_pop_color(ctx);
                        nk_style_pop_color(ctx); nk_style_pop_color(ctx); nk_style_pop_color(ctx);
                    }

                    // You can add future edicts here...
                    nk_group_end(ctx);
                }

                // --- RIGHT COLUMN: Emperor's Status ---
                if (nk_group_begin(ctx, "CharacterSheetGroup", NK_WINDOW_BORDER)) {
                    
                    nk_layout_row_dynamic(ctx, 30, STARTING_ONE);
                    nk_label(ctx, "Emperor's Status", NK_TEXT_CENTERED);

                    nk_layout_row_dynamic(ctx, 10, STARTING_ONE); // Spacer

                    // --- RPG Stats (using placeholder data for now) ---
                    nk_style_push_font(ctx, &font_large->handle);
                    nk_layout_row_dynamic(ctx, 35, STARTING_ONE);
                    nk_label(ctx, "Name: Emperor Valerius", NK_TEXT_LEFT);
                    nk_style_pop_font(ctx);

                    nk_layout_row_dynamic(ctx, 25, STARTING_ONE);
                    nk_label(ctx, "Health:",NK_TEXT_LEFT);

                    nk_layout_row_dynamic(ctx, 20, STARTING_ONE);
                    nk_prog(ctx, theplayer.current_player_health, PLAYER_HEALTH, NK_FIXED);

                    nk_layout_row_dynamic(ctx, 25, STARTING_ONE);
                    nk_label(ctx, "Strength: 15", NK_TEXT_LEFT);
                    nk_label(ctx, "Intellect: 18", NK_TEXT_LEFT);
                    nk_label(ctx, "Charisma: 16", NK_TEXT_LEFT);
                    nk_label(ctx, "Age: 42", NK_TEXT_LEFT);
                    nk_group_end(ctx);
                }

            } else {
                // reset state when the window is closed
                state.show_policies_window = false;
            }
            nk_end(ctx);
        }

        // --- Final Rendering ---
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        nk_glfw3_render(NK_ANTI_ALIASING_ON);
        glfwSwapBuffers(window);
    }

    // --- 5. Cleanup ---
    free(state.character_window_open);
    free(state.has_reached_end_of_chapter);
    nk_glfw3_shutdown();
    glfwDestroyWindow(window);
    glfwTerminate();
    pthread_mutex_destroy(&g_data_mutex);
    destroy_logger();

    return 0;
}


/**
 * @brief Gathers all detailed kingdom statistics for the GUI.
 * This is now a pure data copy, with no string formatting.
 */
void update_all_kingdom_details_for_gui(struct Kingdom kingdoms[], struct Human_Data *data, GuiSharedData *target_data) {
    // --- 1. First, calculate the job counts (this logic is still needed) ---
    // (We create a temporary array to hold the counts before copying)
    int temp_job_counts[NUM_KINGDOMS][10] = {STARTING_ZERO}; // Initialize all to zero
    for (int i = STARTING_ZERO; i < data->count; i++) {
        if (data->humans[i].alive == STARTING_ONE) {
            int k_id = data->humans[i].kingdom_id;
            int j_id = data->humans[i].job;
            if (k_id >= STARTING_ZERO && k_id < NUM_KINGDOMS && j_id >= STARTING_ZERO && j_id <= JOB_REBEL) {
                temp_job_counts[k_id][j_id]++;
            }
        }
    }

    // --- 2. Now, copy all raw data directly, kingdom by kingdom ---
    for (int k = STARTING_ZERO; k < NUM_KINGDOMS; k++) {
        // Direct struct-to-struct copying of raw data
        target_data->kingdoms[k].is_active = kingdoms[k].is_active;
        target_data->kingdoms[k].population = kingdoms[k].population;
        target_data->kingdoms[k].unrest_level = kingdoms[k].unrest_level;
        target_data->kingdoms[k].army_morale = kingdoms[k].army_morale;
        target_data->kingdoms[k].treasury = kingdoms[k].treasury;
        target_data->kingdoms[k].food = kingdoms[k].food;
        target_data->kingdoms[k].wood = kingdoms[k].wood;
        target_data->kingdoms[k].stone = kingdoms[k].stone;
        target_data->kingdoms[k].metal = kingdoms[k].metal;

        // Copy the job counts from our temporary array
        for (int j = STARTING_ZERO; j < 10; j++) {
            target_data->kingdoms[k].job_counts[j] = temp_job_counts[k][j];
        }
    }
}

// =============================================================================
// === SIMULATION THREAD (Runs in the background) ===
// =============================================================================

void* simulation_thread_func(void* arg) {
    int empire_has_fallen = STARTING_ZERO;

    life(&world_stat);
    initialize_world_polities(kingdoms);

    human_data.humans = malloc(world_stat.human_population * sizeof(struct Human_Stats));
    if (human_data.humans == NULL) {
        fprintf(stderr, "Memory allocation failed!\n");
        pthread_exit(NULL);
    }
    human_data.count = world_stat.human_population;

    initialize_population(&human_data);
    initial_job_assignment(&human_data);

    int sim_hour = STARTING_THREE;
    int sim_day = STARTING_ONE;

    // --- Main Simulation Loop ---
    while (STARTING_ONE) {

        int story_ch, story_p;
        pthread_mutex_lock(&g_data_mutex);
        story_ch = g_shared_data.current_story_chapter;
        story_p = g_shared_data.current_story_paragraph;
        pthread_mutex_unlock(&g_data_mutex);
        
        int population_at_hour_start = STARTING_ZERO;
        for(int i = STARTING_ZERO; i < NUM_KINGDOMS; i++) {
            if(kingdoms[i].is_active) population_at_hour_start += kingdoms[i].population;
        }
        if (population_at_hour_start == POSITION_ZERO) population_at_hour_start = world_stat.human_population;

        int new_births = STARTING_ZERO, new_deaths = STARTING_ZERO;
        calculate_stable_population_changes(&kingdoms[POSITION_ZERO], population_at_hour_start, &new_births, &new_deaths);

        apply_story_effects(story_ch, story_p, kingdoms, &human_data);

        alive_status(new_deaths, &human_data);
        persona(new_births, &world_stat, &human_data, empire_has_fallen);

        if (!empire_has_fallen) {
            trigger_hourly_skirmish(&kingdoms[POSITION_ZERO], &human_data);
        }

        // Determine which third of the population
        int hours_per_batch = DAY_IN_HOURS / BATCHES_PER_DAY; // 3 batches per day
        int current_batch = sim_hour / hours_per_batch;
        int population_third = human_data.count / BATCHES_PER_DAY;
        int start_index = current_batch * population_third;
        int end_index = (current_batch == STARTING_TWO) ? human_data.count : (current_batch + STARTING_ONE) * population_third;


        if (sim_hour >= WORK_START_HOUR && sim_hour < WORK_END_HOUR) {
             // We create a temporary struct to pass the subset of the population
            struct HumanPopulation world_stat_subset = world_stat;
            struct Human_Data human_data_subset;
            human_data_subset.humans = &human_data.humans[start_index];
            human_data_subset.count = end_index - start_index;

            // Run hourly tasks only for the current batch
            occupation(&world_stat_subset, &human_data_subset);
            dailyneed(&kingdoms[STARTING_ZERO], &world_stat_subset, &human_data_subset);

            // Distribute payments across the day for each batch
            if (current_batch == STARTING_ZERO && sim_hour == STARTING_SIX) { // First batch gets paid at hour 6
                payments(&world_stat_subset, &human_data_subset);
            } else if (current_batch == STARTING_ONE && sim_hour == STARTING_FOURTHEEN) { // Second batch at hour 14
                payments(&world_stat_subset, &human_data_subset);
            } else if (current_batch == STARTING_TWO && sim_hour == STARTING_TWENTY_TWO) { // Third batch at hour 22
                payments(&world_stat_subset, &human_data_subset);
            }
        }
        
        // Recalculate totals based on the entire population to ensure accuracy for GUI and daily events
        recalculate_kingdom_populations(kingdoms, &human_data);
        int true_total_population = STARTING_ZERO;
        for(int i=STARTING_ZERO; i < NUM_KINGDOMS; i++) {
            if(kingdoms[i].is_active) true_total_population += kingdoms[i].population;
        }
        world_stat.human_population = true_total_population;
        
        // Daily events still run at the end of the day on the whole population
        if (sim_hour == DAY_IN_HOURS-STARTING_ONE) {
            if (!empire_has_fallen) {
                manage_empire(&kingdoms[POSITION_ZERO], &human_data);
                trigger_random_event(&kingdoms[POSITION_ZERO], &human_data);
            } else {
                for (int i = STARTING_ONE; i < NUM_KINGDOMS; i++) {
                    if (kingdoms[i].is_active) {
                        manage_kingdom_daily(&kingdoms[i], &human_data);
                        trigger_random_event(&kingdoms[i], &human_data);
                    }
                }
            }
        }

        // =========================================================================
        // === GATHER AND UPDATE GUI DATA (CRITICAL SECTION) =======================
        // =========================================================================
        pthread_mutex_lock(&g_data_mutex);
        update_all_kingdom_details_for_gui(kingdoms, &human_data, &g_shared_data);
        snprintf(g_shared_data.world_population, MAX_NUM_CHAR, "World Pop: %d", world_stat.human_population);
        snprintf(g_shared_data.current_hour, MAX_NUM_CHAR, "Day %d, %02d:00", sim_day, sim_hour);
        if (!empire_has_fallen) { 
            snprintf(g_shared_data.civil_war_status, MAX_NUM_CHAR, "Status: The Empire Reigns"); 
        } else { 
            snprintf(g_shared_data.civil_war_status, MAX_NUM_CHAR, "Status: Age of Kingdoms"); 
        }
        pthread_mutex_unlock(&g_data_mutex);
        
        if (sim_hour % LOG_CLEAR_FREQUENCY_HOURS == POSITION_ZERO) { 
            clear_old_log_entries();
        }

        struct timespec sleep_time;
        sleep_time.tv_sec = SIMULATION_TICK_SECONDS;
        sleep_time.tv_nsec = STARTING_ZERO;
        nanosleep(&sleep_time, NULL);

        sim_hour++;
        if (sim_hour >= DAY_IN_HOURS) {
            sim_hour = STARTING_ZERO;
            sim_day++;
        }

        // Daily cleanup still happens once per day for the entire population
        if (sim_hour == POSITION_ZERO) {
            compact_dead_humans(&human_data);
        }
    }
    
    free(human_data.humans);
    return NULL;
}

/*
===========================================================================================
=== FUTURE DEVELOPMENT                                                                  ===
===========================================================================================

--- FUTURE IDEAS (PLAYER-DRIVEN) ---

3.  Player Agency as the Emperor
    -   The player could be cast as the Emperor, making decisions that directly affect
        the simulation. This provides a clear goal: stabilize the Empire against the odds.
    -   A "Policies" panel could be added where the player can:
        -   Set Tax Rates: (Low = less income, less unrest; High = more income, more unrest).
        -   Adjust Army Rations: (High = more food consumed, higher morale; Low = less food
          consumed, lower morale).
        -   Issue Edicts: Spend money for temporary bonuses, like "Festival" (lower unrest)
          or "Forced Conscription" (gain troops but spike unrest).
        
        The emperor dosen't need to stay in the capital or palace throughout the story. 
        -   Diffrent storylines involving the player moving around and facing adventures, ambushes, etc.

4.  Weather System
    -   Thic could add abnormalities, e.g. low production of all resources, army is less or more effective, etc.

5.  Random Storylins with new temporal characters
    -   s

6.  Dynamic Narrative (Simulation Affects Story)
    -   This is a long-term goal to make the story and simulation a true two-way street.
    -   The story could check the simulation state at the start of a chapter.
    -   Example: If a successor kingdom becomes powerful through good management and luck,
        a future chapter could dynamically generate text about their rise to prominence,
        or even introduce a new character from that kingdom.
    -   Example: If the player (as Emperor) manages to keep unrest extremely low, the story
        might branch to a narrative about rooting out hidden corruption rather than
        fighting an open rebellion.

--- SIMULATION PHILOSOSPHY NOTES ---

-   The Empire's stability is intentionally fragile. It is designed to be difficult, but
    not impossible, to keep from collapsing. The bigger it gets, the harder it is to manage.
-   Random events (`events.c`) are the primary source of major luck or chaos. A well-managed
    empire can survive a bout of bad luck, while a poorly-managed one can be pushed over
    the edge by a single drought or plague.
-   Rebels are the great equalizer. Even if the empire is large and wealthy, high unrest
    will spawn a significant rebel force, weakening the empire from within and making it
    vulnerable to external threats or further random events.
-   The story (`forced_story.c`) acts as the "hand of fate." It does not check if the
    empire is ready. When the narrative demands a major battle or a spike in unrest, it
    will happen, forcing the player to adapt to the unfolding plot.

*/