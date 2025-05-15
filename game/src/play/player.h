#pragma once
#include "play/bank.h"
#include "play/rep.h"
#define PLAYERS_MAX 8

#define BODY_GENDER 0x00000001
#define BODY_GENDER_MALE 0
#define BODY_GENDER_FEMALE 1
#define BODY_RACE   0x0000000E

typedef struct {
    u32 info;
} body_t;

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
