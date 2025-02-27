#include "rep.h"
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include <stdio.h>
#include <float.h>


// Global variables
rep_t reps[24];
bool reps_used[24];

static void set(u8 rep_id, aspects aspect, double val) {
    reps[rep_id].stats[aspect] = val;
}

static void set_stat(rep_t *r, aspects aspect, double val) {
    r->stats[aspect] = val;
}

static f64 get(u8 rep_id, aspects aspect) {
    return reps[rep_id].stats[aspect];
}

static f64 get_stat(rep_t *r, aspects aspect) {
    return r->stats[aspect];
}

static u8 create_temp(void) {
    uint8_t rep_id = 0;
    bool found = false;
    for (int i = 0; i < 24; i++) {
        if (reps_used[i]) continue;
        rep_id = (uint8_t)i;
        reps_used[i] = true;
        found = true;
        break;
    }
    printf("creating reps_used: %d\n", found);
    if (!found) {
        printf("DIDN'T FIND AVAILABLE REP!");
    }
    return rep_id;
}

u8 create_temp_rep(void) {
    return create_temp();
}

u8 create_rep(i8 name_id, bool random) {
    uint8_t rep_id = create_temp();
    reps[rep_id].name_id = name_id;
    reps[rep_id].random = random;

    printf("hello\n");

    set(rep_id, rep_spool, 1);
    set(rep_id, rep_turbo, 100);
    set(rep_id, rep_turbo_strength, 1);
    set(rep_id, rep_turbo_regen, 25);
    set(rep_id, rep_aero, 1);
    set(rep_id, rep_interest, 0.2);

    switch (name_id) {
        case 0:
            set(rep_id, rep_hp, 1330);
            set(rep_id, rep_kg, 1650);
            set(rep_id, rep_bar, 1.2);
            set(rep_id, rep_turbo_strength, 3);
            set(rep_id, rep_rpm_top, 5500);
            set(rep_id, rep_rpm_idle, 5500.0 / 4.5);
            set(rep_id, rep_gear_top, 4);
            set(rep_id, rep_spd_top, 155);
            set(rep_id, rep_tb_ms, 900);
            set(rep_id, rep_nos_ms, 500);
            break;
        case 1:
            set(rep_id, rep_hp, 2840);
            set(rep_id, rep_kg, 3050);
            set(rep_id, rep_rpm_top, 6500);
            set(rep_id, rep_rpm_idle, 6500.0 / 8.0);
            set(rep_id, rep_gear_top, 3);
            set(rep_id, rep_spd_top, 150);
            set(rep_id, rep_tb_area, 1);
            set(rep_id, rep_tb, 1);
            set(rep_id, rep_tb_ms, 1350);
            set(rep_id, rep_nos_ms, 500);
            break;
        case 2:
            set(rep_id, rep_hp, 900);
            set(rep_id, rep_kg, 1215);
            set(rep_id, rep_rpm_top, 10000);
            set(rep_id, rep_rpm_idle, 10000.0 / 8.0);
            set(rep_id, rep_gear_top, 5);
            set(rep_id, rep_spd_top, 150);
            set(rep_id, rep_tb_ms, 900);
            set(rep_id, rep_nos_ms, 600);
            set(rep_id, rep_nos_bottles, 1);
            set(rep_id, rep_nos, 1.4);
            break;
        case 3:
            set(rep_id, rep_hp, 1120);
            set(rep_id, rep_kg, 1450);
            set(rep_id, rep_rpm_top, 5000);
            set(rep_id, rep_rpm_idle, 5000.0 / 4.0);
            set(rep_id, rep_gear_top, 6);
            set(rep_id, rep_spd_top, 150);
            set(rep_id, rep_tb_ms, 700);
            set(rep_id, rep_nos_ms, 400);
            break;
        default:
            // TODO throw error or something
            break;
    }
    set(rep_id, rep_rpm_base_top, get(rep_id, rep_rpm_top));
    set(rep_id, rep_spd_base, get(rep_id, rep_spd_top));
    return rep_id;
}

void destroy_rep(uint8_t rep_id) {
    reps_used[rep_id] = false;
    printf("destroying reps_used: %d\n", rep_id);
}

void destroy_all_reps(void) {
    for (int i = 0; i < 24; i++) {
        reps_used[i] = false;
    }
    printf("destroying all reps_used\n");
}

void reset_rep(rep_t *rep) {
    if (get_stat(rep, rep_kg) < 50) {
        set_stat(rep, rep_kg, 50); // sjåføren
    }

    if (get_stat(rep, rep_aero) < 0) {
        set_stat(rep, rep_aero, 0);
    }

    // if (!rep.is(Rep.sequential) && rep.get(Rep.gear_top) > 10) {
    //     rep.set(Rep.gear_top, 10);
    // }

    // if (Double.isNaN(rep.get(Rep.kW))) {
    //     rep.set(Rep.kW, Double.MAX_VALUE);
    // }

    if (get_stat(rep, rep_rpm_idle) > get_stat(rep, rep_rpm_top) - 100) {
        set_stat(rep, rep_rpm_idle, get_stat(rep, rep_rpm_top) - 100);
    }

    // for (int i = 0; i < Rep.tb_area; i++) {
    //     if (rep.get(i) < 0d)
    //         rep.set(i, 0d);
    // }

    if (get_stat(rep, rep_spd_top) >= pow(10.07925285, 9)) {
        set_stat(rep, rep_spd_top, pow(10.07925285, 9));
    }
}

void set_rep_stats(uint8_t rep_id, uint32_t aspect, double val) {
    reps[rep_id].stats[aspect] = val;
}

bool is_random(uint8_t rep_id) {
    return reps[rep_id].random;
}

void set_random(uint8_t rep_id, bool val) {
    reps[rep_id].random = val;
}

int8_t get_name_id(uint8_t rep_id) {
    return reps[rep_id].name_id;
}

void set_name_id(uint8_t rep_id, int8_t name_id) {
    reps[rep_id].name_id = name_id;
}
//
// double turbo_hp(rep_t *r) {
//     return get_stat(r, rep_hp) / 4.0 * get_stat(r, rep_bar);
// }
//
// double get_turbo_hp(uint8_t rep_id) {
//     return turbo_hp(&reps[rep_id]);
// }

f64 get_total_hp(rep_t *r) {
    double hp_val = get_stat(r, rep_hp);
    double thp = 0; // turbo_hp(r);
    double rpm_t = get_stat(r, rep_rpm_top);
    double rpm_bt = get_stat(r, rep_rpm_base_top);

    double a = (hp_val + thp) * (rpm_t / rpm_bt);

    if (isinf(a)) {
        a = signbit(a) ? -DBL_MAX : DBL_MAX;
    }

    return a;
}

// double get_total_hp(uint8_t rep_id) {
//     double hp_val = get(rep_id, rep_hp);
//     double thp = 0; // get_turbo_hp(rep_id);
//     double rpm_t = get(rep_id, rep_rpm_top);
//     double rpm_bt = get(rep_id, rep_rpm_base_top);
//
//     double a = (hp_val + thp) * (rpm_t / rpm_bt);
//
//     if (isinf(a)) {
//         a = signbit(a) ? -DBL_MAX : DBL_MAX;
//     }
//
//     return a;
// }

void update_highest_speed(rep_t *r, double current_speed) {
    if (current_speed > get_stat(r, rep_spd_record))
        r->stats[rep_spd_record] = current_speed;
}

