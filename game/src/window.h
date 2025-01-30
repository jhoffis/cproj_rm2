#pragma once
#include "GLFW/glfw3.h"
#include "nums.h"

typedef void (*window_key_cb)(i32 key, i32 scancode, i32 action, i32 mods);
typedef void (*window_mouse_cb)(f64 xpos, f64 ypos, i32 button, i32 action, i32 mods);
typedef void (*window_resize_cb)(u32 width, u32 height);

extern GLFWwindow *window;

void window_init(window_key_cb kcb, window_mouse_cb mcb, window_resize_cb wrcb);
void window_cleanup(void);
void window_close(void);
bool window_should_close(void);
void window_poll_events(void);
f32 window_aspect_ratio(void);
bool window_is_cursor_visible(void);
void window_set_cursor_visible(bool visible);

