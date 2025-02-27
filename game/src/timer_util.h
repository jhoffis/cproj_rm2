#pragma once
#include "nums.h"
#define TIMER_TICK 25

void timer_init(void);
u64 timer_now_millis(void);
u64 timer_now_nanos(void);
u64 timer_now_utc(void);

void timer_update_delta(void);
f64 timer_delta(void);
