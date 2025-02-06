#pragma once

#include "nums.h"
void init_btns(void);
void render_btn_func(char *text, void (*func)(void));
void render_btn(char *text, f32_v2 pos);
