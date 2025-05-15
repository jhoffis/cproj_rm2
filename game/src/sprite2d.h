#pragma once
#include "file_util.h"
#include "nums.h"

typedef enum {
    anchor_left, anchor_right, 
    anchor_mid, anchor_mid_bottom,
    anchor_top_left, anchor_top_right,
} sprite2D_anchor;

typedef struct {
    f32_v2 position;
    f32 scale;
    i32 hovered;
    bool visible;
    sprite2D_anchor anchor;
    image_data *img;
} sprite2D_t;
extern sprite2D_t *sprites;
typedef u32 s2D;

void sprite2D_init(void);
void sprite2D_cleanup(void);
void sprite2D_update_window_aspect(f32 aspect);
u32 sprite2D_create(const char *name, u32 scene_id);
void sprite2D_visible(s2D id, bool visible);
void sprite2D_scale(s2D id, f32 scale);
void sprite2D_pos(s2D id, f32_v2 pos);
void sprite2D_camera_pos(f32_v2 pos);
void sprite2D_draw(s2D id);
