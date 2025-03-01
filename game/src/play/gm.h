#pragma once
#include "nums.h"
#include "play/player.h"

// For each player to store as info about previous races.
typedef struct {
    u32 time;
} round_finish_info_t;
typedef struct {
    u8 round;
    u32 tracklength;
    bool last_player_hit_max_speed;
    u32 last_player_time;
} round_info_t;
extern round_info_t gm_current_round;

extern i32         gm_starting_money;
extern u16         gm_starting_inflation;
extern u32         gm_std_income;
extern u32         gm_max_income;
extern u8          gm_max_income_type;
extern const char* gm_max_income_types;
extern u32         gm_tracklength;
extern u32         gm_tracklength_increase;
extern u8          gm_tracklength_increaser_type;
extern const char* gm_tracklength_increaser_names;
extern u32         gm_end_goal;
extern u8          gm_end_goal_type;
extern const char* gm_end_goal_names;
extern bool        gm_catch_up_enabled;
extern u8          gm_catch_up_value;
extern u8          gm_loser_money_type;
extern const char* gm_loser_money_types;
extern u8          gm_slow_time_type;
extern const char* gm_slow_time_types;
extern u8          gm_slow_time_time_type;
extern const u32   gm_slow_time_time_values[];
extern const char* gm_slow_time_time_types;

void gm_init(void);
void finish_control(void);
void finalize_init_player(player_t* player);


