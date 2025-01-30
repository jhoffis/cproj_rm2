#pragma once
#include "nums.h"

typedef enum {
    anchor_bottom_left, anchor_bottom_right,
    anchor_top_left, anchor_top_right,
} sprite2D_anchor;

void sprite2D_init(void);
void sprite2D_cleanup(void);
void sprite2D_update_window_aspect(f32 aspect);
u32 sprite2D_create(const char *name, u32 scene_id, sprite2D_anchor anchor);
void sprite2D_visible(u32 id, bool visible);
void sprite2D_pos(u32 id, f32_v2 pos);
void sprite2D_camera_pos(f32_v2 pos);
void sprite2D_draw(void);
