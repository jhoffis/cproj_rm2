#pragma once

#include "nums.h"
void init_btns(void);
void render_btn(char *text, f32_v2 pos, void (*func)(void));
void clear_hovered_btns(void);
void click_btn(void);
