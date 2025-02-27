#include "car.h"
#define LEN_NOS 24

car_stats_t my_car;

void stats_reset(car_stats_t *stats, rep_t *r, f64 num_superchargers, f64 num_turbos) {
    for (int i = 0; i < ASPECTS_COUNT; i++) {
        stats->stats[i] = r->stats[i];
    }

    if (num_superchargers == 0) {
        stats->percent_turbo_supercharger = 1;
    } else if (num_superchargers > num_turbos) {
        stats->percent_turbo_supercharger = num_turbos / num_superchargers;
    } else {
        stats->percent_turbo_supercharger = num_superchargers / num_turbos;
    }

    stats->turbo_blow_on = false;
    stats->nos_down_pressed = false;
    // With space
    //    clutch = false;
    //    resistance = 0f;
    // Without space
    stats->grinding_time = 0;
    stats->grinding_current = 1;
    stats->throttle_percent = 0;
    stats->clutch = true;
    stats->clutch_percent = 1;
    stats->last_time_release_throttle = 0;
    stats->redlined_this_gear = false;
    stats->throttle = false;
    stats->nos_on = false;
    stats->sound_barrier_broken = false;
    stats->has_done_start_boost = false;
    stats->nos_bottle_amount_left = (u32)stats->stats[rep_nos_bottles];
    
    for (int i = 0; i < LEN_NOS; i++) { // FIXME hardcoded value for max amount of nos
        stats->nos_times_left[i] = 0;
        stats->nos_times_from[i] = 0;
    }
    
    stats->gear = 1;
    stats->speed = 0;
    stats->distance = 0;
    stats->spool = stats->stats[rep_spool_start];
    stats->spdinc = 0;
    stats->rpm = stats->stats[rep_rpm_idle];
    stats->rpm_goal = stats->rpm;
    stats->tireboost_time_left = 0;
    stats->sequential_shift = r->stats[rep_seq] != 0;

    stats->brake = 0;
}

bool has_turbo(car_stats_t *stats) {
    return stats->stats[rep_bar] > 0;
}

bool has_two_step(car_stats_t *stats) {
    return stats->stats[rep_two_step] != 0;
}

float nos_percentage_left(car_stats_t *s, u32 i, u64 now) {
    if (i >= LEN_NOS || s->nos_times_left[i] == 0)
        return 1.0f;
    if (s->nos_times_left[i] > now) {
        u64 timespan = s->nos_times_left[i] - now;
        return (float)timespan / (float)(s->nos_times_left[i] - s->nos_times_from[i]);
    }
    return 0;
}

// Exported function
float get_nos_percentage_left(u32 i, u64 now) {
    return nos_percentage_left(&my_car, i, now);
}

double tb_percentage_left(car_stats_t *s, u64 now) {
    double tb_max_time = s->stats[rep_tb_ms];
    if (s->tireboost_time_left <= now) {
        return 0;
    }
    double tb_time = (double)(s->tireboost_time_left - now);
    double percent = tb_time / tb_max_time;
    // printf("maxtime: %f, timeleft: %lu, tbTime: %f, now: %lu, percent: %f\n", 
    //        tb_max_time, s->tireboost_time_left, tb_time, now, percent);
    return fmax(percent, 0);
}

static void pop_nos_bottle(u64 from_time, u64 till_time) {
    car_stats_t *s = &my_car;

    for (u8 i = 0; i < LEN_NOS; i++) {
        if (s->nos_times_from[i] != 0) continue;

        s->nos_times_from[i] = from_time;
        s->nos_times_left[i] = till_time;
        s->nos_bottle_amount_left -= 1;
        return;
    }
}

bool pop_nos(u64 from_time) {
    car_stats_t *s = &my_car;
    if (s->nos_bottle_amount_left > 0) {
        pop_nos_bottle(from_time, from_time + s->stats[rep_nos_ms]);
        return true;
    }
    return false;
}

u64 get_nos_time_left(car_stats_t *s, size_t i) {
    if (i < LEN_NOS) {
        return s->nos_times_left[i];
    }
    return 0;
}

// Exported function
void change_last_nos_time_left(u64 val) {
    // OLD SOLUTION: if (nos_times_left.size() > 0)
    //     nos_times_left.set(nos_times_left.size() - 1, nos_times_left.get(nos_times_left.size() - 1) + val);
    car_stats_t *s = &my_car;
    if (s->nos_times_from[0] == 0) return;

    for (u8 i = 1; i < LEN_NOS; i++) {
        if (s->nos_times_from[i] != 0) continue;
        s->nos_times_left[i - 1] += val;
        return;  // Added return to match original logic
    }
}

void shift_seq(bool up) {
    if (up) {
        if (my_car.gear < my_car.stats[rep_gear_top]) my_car.gear++;
    } else {
        if (my_car.gear > 0) my_car.gear--;
    }
}
