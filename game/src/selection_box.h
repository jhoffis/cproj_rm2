#pragma once
#include "nums.h"

void sel_box_init(void);
void sel_box_render(void);
void sel_box_start_dragging(const f32_v2 world_pos);
void sel_box_drag(const f32_v2 world_pos);
void sel_box_stop_dragging(bool select);
bool sel_box_is_visible(void);
void sel_box_destroy(void);

