#pragma once

#include "nums.h"
#include "sprite2d.h"
void init_btns(void);
void render_btn(char *text, f32_v2 pos, void (*func)(void), sprite2D_anchor anchor);
void clear_hovered_btns(void);
void click_btn(void);
