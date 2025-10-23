#include "../player.h"

void player_setup(struct PlayerStat *theplayer)
{
    // Status
    theplayer->alive = 1;

    // Stats
    theplayer->current_player_damage = 35;
    theplayer->current_player_health = 200;
    theplayer->current_player_defense = 40;
    theplayer->current_player_speed = 3; //1 - 10
    theplayer->current_player_hunger = 100;
    theplayer->current_player_level = 1;
    theplayer->current_player_expirience = 0;

    // Equipment
    theplayer->current_player_head = 2; // 2 for base iron
    theplayer->current_player_torso = 2;
    theplayer->current_player_legs = 2;
    theplayer->current_player_foots = 2;

    // Items
    theplayer->current_player_right = 0;
    theplayer->current_player_left = 0;
}