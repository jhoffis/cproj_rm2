#pragma once

#include "camera.h"
#include "nums.h"
#include "renderer.h"
#include "scenes.h"

typedef enum {
    minimized, windowed, maximized, fullscreen_no_border, fullscreen_true,
} window_state_enum;

typedef struct {
    u32 width, height, x, y;
    f32 aspect;
    window_state_enum state;
} window_state_t;

typedef struct {
    bool initialize;
    bool free_cam;
    camera3D_t cam3D;
    camera2D_t cam2D;
    gfx_method render_method;
    window_state_t window;
    scene_t current_scene;
} game_state_t;

extern game_state_t game_state;
