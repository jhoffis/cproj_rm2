#include "player.h"
#include "play/car.h"

player_t my_player;

void init_player(void) {
    my_player = (player_t) {0};
    my_player.rep_id = create_rep(1, false);
    rep_t *rep = &reps[my_player.rep_id];
    my_player.rep = rep;
    stats_reset(&my_car, rep, 0, 0);
}

