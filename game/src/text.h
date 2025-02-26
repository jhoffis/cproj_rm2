#pragma once

#include "nums.h"
#include "sprite2d.h"

void init_text(void);
void reset_debug_pos(void);
void render_btn_text(const char *text, 
                 f32_v2 pos, 
                 f32 scale, 
                 f32_v3 color, 
                 f32_v2 img_sizes, 
                 sprite2D_anchor anchor);
void render_text(f32_v2 pos, 
                 sprite2D_anchor anchor, 
                 const char *text, ...);
void render_print(const char *input_text, ...);
