#pragma once
#include "GLFW/glfw3.h"
#include "nums.h"

extern GLFWwindow *window;

void window_init(void);
void window_cleanup(void);
bool window_should_close(void);
void window_poll_events(void);
f32 window_aspect_ratio(void);
