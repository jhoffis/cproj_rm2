#include "gm.h"
#include "scenes.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "nums.h" // For your custom types and U32_MAX
#include "timer_util.h"

round_info_t gm_current_round;

i32         gm_starting_money = 100;
u16         gm_starting_inflation = 20;
u32         gm_std_income = 200;
u32         gm_max_income = 0;
u8          gm_max_income_type = 0;
const char* gm_max_income_types = "None\nLinear";
u32         gm_tracklength = 245;
u32         gm_tracklength_increase = 0;
u8          gm_tracklength_increaser_type = 1;
const char* gm_tracklength_increaser_names = "None\nDynamic\nLinear\nPercent";
u32         gm_end_goal = 4;
u8          gm_end_goal_type = 0;
const char* gm_end_goal_names = "Most points by round\nAhead by points\nFirst to points";
bool        gm_catch_up_enabled = true;
u8          gm_catch_up_value = 3;
u8          gm_loser_money_type = 0;
const char* gm_loser_money_types = "None\n+10%\n+15%";
u8          gm_slow_time_type = 0;
const char* gm_slow_time_types = "None\n-25%\n-33%\n-50%\n";
u8          gm_slow_time_time_type = 0;
const u32   gm_slow_time_time_values[] = {5000, 2500, 1000};
const char* gm_slow_time_time_types = "5000ms\n2500ms\n1000ms\n";

void gm_init(void) {
    gm_current_round = (round_info_t) {
        .round = 1,
        .tracklength = gm_tracklength
    };
}


// Randomizer function focused on high entropy
f64 features_random(f64 max) {
    // Use multiple entropy sources for better randomness
    const u32 seed = (u32) timer_now_millis();
    const u32 counter = rand();
    
    // Mix multiple sources of randomness
    u32 r1 = rand();
    u32 r2 = rand();
    
    // Create chaos by combining sources
    u64 mixed = ((u64)r1 << 32 | r2) ^ ((u64)clock() << 16);
    mixed ^= (u64)counter * seed;
    
    // Further scramble with bit operations
    mixed = (mixed ^ (mixed >> 12)) ^ (mixed << 25);
    
    // Normalize to 0-1 range and multiply by max
    return ((f64)(mixed % U32_MAX) / (f64)U32_MAX) * max;
}

i32 new_race_goal() {
    const auto rounds = gm_current_round.round;
    i32 new_length;
    f64 time_aim = 0.25 * rounds + 15;

    f64 time_max_aim = gm_current_round.last_player_hit_max_speed ? 15 : 30;
    if (time_aim > time_max_aim)
        time_aim = time_max_aim;

    // printf("time_aim: %f\n", time_aim);
    // printf("worst_time: %d\n", worst_time);
    f64 up_or_down;
    if (gm_current_round.last_player_time < 500)
        up_or_down = 2.5 + features_random(1.25);
    else if (gm_current_round.last_player_time < 2000)
        up_or_down = 2.25 + features_random(0.95);
    else if (gm_current_round.last_player_time < 5000)
        up_or_down = 1.75 + features_random(0.75);
    else if (gm_current_round.last_player_time < time_aim * 1000)
        up_or_down = 1.25 + features_random(0.5);
    else
        up_or_down = 0.25 + features_random(0.75);

    new_length = (i32) round(floor((f64) gm_current_round.tracklength * up_or_down / 10.0) * 10.0) + 60;
    
    gm_current_round.last_player_hit_max_speed = false;
    gm_current_round.last_player_time = U32_MAX;
    // gm_current_round.prev_race_length = new_length;
    return new_length;
}



void finish_control(void) {
    // first check
    for (size_t i = 0; i < num_players; i++) {
        auto player = &all_players[i];
        if (player->racing_round < player->finished_round) {
            fprintf(stderr, "Error: racing_round is less than finished_round for player %zu, %d vs. %d\n", 
                    i, 
                    player->racing_round,
                    player->finished_round);
            return;
        }
        
        if (player->racing_round != player->finished_round) {
            printf("player nr. %zu is still in the race\n", i);
            return;
        }
    }
    
    // all are finished
    for (size_t i = 0; i < num_players; i++) {
        auto player = &all_players[i];
        const i32 total_income = gm_std_income;
        player->bank.money += total_income;
        player->bank.money_added = total_income;
        printf("adding money %d\n", total_income);
    }

    if (gm_current_round.round == gm_end_goal) {
        change_scene(scene_win, false);            
        return;
    }

    gm_current_round.round++;
    new_race_goal();
}

void finalize_init_player(player_t* player) {
    player->bank.money = gm_starting_money;
    player->bank.inflation = gm_starting_inflation;
}
