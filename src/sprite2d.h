#pragma once
#include "nums.h"

u32 sprite2D_create(const char *name, u32 scene_id);
void sprite2D_visible(u32 id, bool visible);
void sprite2D_pos(u32 id, f32_v2 pos);
void sprite2D_draw(void);
