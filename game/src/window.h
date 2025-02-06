#pragma once
#include "GLFW/glfw3.h"
#include "nums.h"

typedef void (*window_key_cb)(i32 key, i32 scancode, i32 action, i32 mods);
typedef void (*window_mouse_cb)(f64 xpos, f64 ypos, i32 button, i32 action, i32 mods);
typedef void (*window_resize_cb)(u32 width, u32 height);

extern GLFWwindow *window;
extern bool key_down;
extern i32 key_code;
extern i32 key_scancode;
extern i32 key_action;
extern i32 key_mods;

extern bool mouse_down;
extern i32 mouse_button;
extern i32 mouse_action;
extern i32 mouse_mods;
extern f64 mouse_xpos;
extern f64 mouse_ypos;

void window_init(void);
void window_cleanup(void);
void window_close(void);
bool window_should_close(void);
void window_poll_events(void);
f32 window_aspect_ratio(void);
bool window_is_cursor_visible(void);
void window_set_cursor_visible(bool visible);

