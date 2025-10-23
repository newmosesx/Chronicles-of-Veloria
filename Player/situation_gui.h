#ifndef SITUATION_GUI_H
#define SITUATION_GUI_H

// --- Forward Declarations ---
// We promise the compiler that these struct types exist somewhere.
// Since we are only using POINTERS to them in this header, this is all the
// compiler needs to know. It doesn't need to know their size or members.
struct nk_context;
struct AppState;

// This is the public contract. This is the only thing the outside world
// needs to know about this module.
void draw_player_actions_panel(struct nk_context *ctx, struct AppState *state);

#endif // SITUATION_GUI_H