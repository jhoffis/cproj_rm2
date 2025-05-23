#pragma once
#include "play/bank.h"
#include "play/body.h"
#include "play/rep.h"
#define PLAYERS_MAX 8

typedef struct {
    u8 player_index;
    bank_t bank;
    rep_t *rep;
    u8 rep_id;
    body_t body;
    f32_v2 map_pos;
} player_t;
extern player_t *my_player;
extern player_t all_players[PLAYERS_MAX];
extern u8 num_players;

void init_player(void);
