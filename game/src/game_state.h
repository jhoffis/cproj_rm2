#pragma once

#include "nums.h"
typedef struct {
    int test;
    bool free_cam;
    f32_v4 cam_rot;
    f32_v4 cam_pos;
} game_state_t;

extern game_state_t game_state;
