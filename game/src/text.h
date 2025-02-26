#pragma once

#include "nums.h"
#include "sprite2d.h"

void init_text(void);
void render_text(const char *text, f32_v2 pos, f32 scale, f32_v3 color, sprite2D_anchor anchor);
