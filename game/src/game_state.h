#pragma once

#include "nums.h"
#include "scenes.h"

typedef enum {
    minimized, windowed, maximized, fullscreen_no_border, fullscreen_true,
} window_state_enum;

typedef struct {
    u32 width, height, x, y;
    window_state_enum state;
} window_state_t;

typedef struct {
    bool initialize;
    bool free_cam;
    f32_v4 cam_rot;
    f32_v4 cam_pos;
    u32 render_method;
    window_state_t window;
    scene_t current_scene;
} game_state_t;

extern game_state_t game_state;
