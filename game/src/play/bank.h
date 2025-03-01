#pragma once
#include "nums.h"


typedef struct {
    i32 money;
    u16 inflation;
    u16 points;
    f64 money_achieved;
    i32 money_added;
    i16 points_added;
} bank_t;


void reset(bank_t *bank);
bool can_afford(bank_t bank, i32 cost);
bool buy(bank_t *bank, i32 cost);
void buy_forced(bank_t *bank, i32 cost);
void add_money(bank_t *bank, f64 amount);
void add_points(bank_t *bank, i16 amount);

