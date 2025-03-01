#include "player.h"
#include "play/car.h"

player_t *my_player;
player_t all_players[PLAYERS_MAX];
u8 num_players;


void init_player(void) {
    auto p = (player_t) {0};
    p.player_index = 0;
    p.rep_id = create_rep(1, false);
    rep_t *rep = &reps[p.rep_id];
    p.rep = rep;

    num_players = 1;
    all_players[p.player_index] = p;
    my_player = &all_players[p.player_index];

    reset_my_car();
}

