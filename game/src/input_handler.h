#pragma once 
#include "nums.h"

#ifdef DEBUG
extern bool exit_hotreload;
#endif

extern f32 move_forward, move_back, move_left, move_right, move_up, move_down;

void key_cb(i32 key, i32 scancode, i32 action, i32 mods);
void mouse_cb(f64 xpos, f64 ypos, i32 button, i32 action, i32 mods);
void resize_cb(u32 width, u32 height);
