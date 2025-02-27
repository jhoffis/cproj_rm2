#pragma once
#include "nums.h"

typedef enum {
    rep_hp, 
    rep_kg,
    rep_bar,
    rep_spd_top,
    rep_rpm_top,
    rep_rpm_idle,
    rep_turbo,
    rep_nos,
    rep_nos_ms,
    rep_tb,
    rep_tb_ms,
    // rep_tb_longify, // TODO make up some stuff
    // rep_tb_jump,
    rep_spool,
    rep_spool_start,
    rep_aero,
    rep_turbo_regen,
    rep_turbo_strength,
    rep_money_turn,
    rep_interest,
    // cannot be multiplied:
    rep_nos_bottles,
    rep_spd_base, 
    rep_rpm_base_idle,
    rep_rpm_base_top,
    rep_snos,
    rep_life,
    rep_gear_top,
    rep_nos_auto,
    rep_nos_push,
    rep_tb_area,
    rep_sticky_clutch,
    rep_seq,
    rep_nos_sound,
    rep_manual,
    rep_two_step, // FIXME Why is this here? It is a bool value that should not be added through upgrades... What is this struct really??? Isn'ẗ it the representation of your car? And
             // so on....
    rep_throttle,
    rep_spd_record, // FIXME Why is this here? HighestSpeed cannot be upgraded?!
    ASPECTS_COUNT
} aspects;

typedef struct {
    i8 name_id;
    bool random;
    f64 stats[ASPECTS_COUNT];
} rep_t;

extern rep_t reps[24];
extern bool reps_used[24];


u8 create_temp_rep(void);
u8 create_rep(i8 name_id, bool random);
f64 get_total_hp(rep_t *r);
void update_highest_speed(rep_t *r, double current_speed);



