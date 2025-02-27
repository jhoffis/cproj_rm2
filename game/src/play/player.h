#pragma once

#include "play/rep.h"
typedef struct {
    rep_t *rep;
    u8 rep_id;
} player_t;
extern player_t my_player;

void init_player(void);
