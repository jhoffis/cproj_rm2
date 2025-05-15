#pragma once
#include "nums.h"

void race_init(u32 my_car);
void prepare_new_race(void);
void race_scene_render(void);
void race_key_cb(i32 key, i32 scancode, i32 action, i32 mods);
