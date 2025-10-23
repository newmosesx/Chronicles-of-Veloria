#ifndef PLAYER_H
#define PLAYER_H

struct PlayerStat
{
    const char* current_player_name;

    //stats
    int current_player_smart;
    int current_player_damage;
    int current_player_defense;
    int current_player_health;
    int current_player_speed;
    int current_player_hunger;
    int current_player_level;
    double current_player_expirience;

    //armor
    int current_player_head;
    int current_player_torso;
    int current_player_legs;
    int current_player_foots;

    //items
    int current_player_right;
    int current_player_left;

    //status
    int alive; // 0 dead - 1 alive
};

void player_setup(struct PlayerStat *theplayer);

#endif // PLAYER_H