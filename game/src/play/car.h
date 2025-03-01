#pragma once

#include "nums.h"
#include "rep.h"

typedef struct {
    bool throttle;
    bool clutch;
    bool nos_on;
    bool tireboost_on;
    bool nos_down_pressed;
    bool sequential_shift;
    bool sound_barrier_broken;
    bool has_done_start_boost;
    bool turbo_blow_on;
    bool redlined_this_gear;
    bool grinding;

    u32 nos_bottle_amount_left;
    u32 gear;
    u32 finish_speed;

    u64 tireboost_time_left;
    u64 last_time_release_throttle;
    u64 grinding_time;

    f64 speed;
    f64 distance;
    f64 spool;
    f64 spdinc;
    f64 brake;
    f64 throttle_percent;
    f64 clutch_percent; // If < 1 then clutch engaged
    f64 rpm;
    f64 rpm_goal;
    f64 grinding_current;
    f64 percent_turbo_supercharger;

    u64 nos_times_left[24];
    u64 nos_times_from[24]; // FIXME not infinite amount of nos
    
    f64 stats[ASPECTS_COUNT];
} car_stats_t;
extern car_stats_t my_car;

void reset_my_car(void);
void reset_stats(car_stats_t *stats, rep_t *r);
bool has_turbo(car_stats_t *stats);
bool has_two_step(car_stats_t *stats);
u64 get_nos_time_left(car_stats_t *s, size_t i);
void shift_seq(bool up);







