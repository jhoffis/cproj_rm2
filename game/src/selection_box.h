#pragma once
#include "nums.h"

void sel_box_init(void);
void sel_box_render(void);
void sel_box_start_dragging(f32 x, f32 y);
void sel_box_stop_dragging(bool select);
bool sel_box_is_visible(void);
void sel_box_destroy(void);

