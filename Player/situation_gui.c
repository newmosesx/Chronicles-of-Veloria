#include "../game_config.h"
#include "../GUI_lib/nuklear.h" // We need the full header for styling
#include "../app_state.h"
#include "../shared_data.h"
#include "../logger.h"
#include <pthread.h>

void draw_player_actions_panel(struct nk_context *ctx, struct AppState *state) {
    
    // --- MANAGE POLICIES BUTTON (with the lock) ---
    nk_layout_row_dynamic(ctx, 40, 1);
    if (nk_button_label(ctx, "Manager")) {
        state->show_policies_window = true; // We still use the same state flag to open it
    }
}