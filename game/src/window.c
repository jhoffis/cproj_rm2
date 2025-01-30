#include "window.h"
#include "GLFW/glfw3.h"
#include <stdio.h>
#include <stdlib.h>

GLFWwindow *window;
u32 window_width, window_height;
bool key_down = false;
i32 key_key = 0;
i32 key_scancode = 0;
i32 key_action = 0;
i32 key_mods = 0;

i32 mouse_button = 0;
i32 mouse_action = 0;
i32 mouse_mods = 0;
f64 mouse_xpos = 0;
f64 mouse_ypos = 0;

static window_key_cb external_kcb;
static window_mouse_cb external_mcb;
static window_resize_cb external_wrcb;

static void framebuffer_size_callback(GLFWwindow* window, int w, int h) {
    window_width = w;
    window_height = h;
    glViewport(0, 0, window_width, window_height);
    external_wrcb(w, h);
}

static void key_cb(GLFWwindow *window, i32 key, i32 scancode, i32 action, i32 mods) {
    key_key = key;
    key_scancode = scancode;
    key_action = action;
    key_mods = mods;
    external_kcb(key, scancode, action, mods);
}

static void mouse_btn_cb(GLFWwindow *window, i32 button, i32 action, i32 mods) {
    mouse_button = button;
    mouse_action = action;
    mouse_mods = mods;
    external_mcb(mouse_xpos, mouse_ypos, button, action, mods);
}

static void mouse_pos_cb(GLFWwindow *window, f64 xpos, f64 ypos) {
    mouse_xpos = xpos;
    mouse_ypos = ypos;
    external_mcb(xpos, ypos, mouse_button, mouse_action, mouse_mods);
}


void window_init(window_key_cb kcb, window_mouse_cb mcb, window_resize_cb wrcb) {
    external_kcb = kcb;
    external_mcb = mcb;
    external_wrcb = wrcb;

    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\\n");
        exit(1);
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window_width = 800;
    window_height = 600;

    window = glfwCreateWindow(window_width, window_height, "OpenGL Window", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Failed to create GLFW window\\n");
        glfwTerminate();
        exit(1);
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);  

    glfwSetKeyCallback(window, key_cb);
    glfwSetMouseButtonCallback(window, mouse_btn_cb);
    glfwSetCursorPosCallback(window, mouse_pos_cb);
}

void window_close(void) {
    glfwSetWindowShouldClose(window, true);
}

void window_cleanup(void) {
    glfwDestroyWindow(window);
    glfwTerminate();
}

bool window_should_close(void) {
    return glfwWindowShouldClose(window);
}

void window_poll_events(void) {
    glfwPollEvents();
}

f32 window_aspect_ratio(void) {
    return (f32) window_height / (f32) window_width;
}

bool window_is_cursor_visible(void) {
    return glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL;
}

void window_set_cursor_visible(bool visible) {
    glfwSetInputMode(window, GLFW_CURSOR, visible ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
}
