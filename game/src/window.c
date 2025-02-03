#include "window.h"
#include "GLFW/glfw3.h"
#include "game_state.h"
#include "renderer.h"
#include <stdio.h>
#include <stdlib.h>

GLFWwindow *window;
bool key_down = false;
i32 key_code = 0;
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
    game_state.window.width = w;
    game_state.window.height = h;
    gfx_update_viewport();
    external_wrcb(w, h);
}

static void key_cb(GLFWwindow *window, i32 key, i32 scancode, i32 action, i32 mods) {
    key_code = key;
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

static void window_pos_cb(GLFWwindow *window, i32 xpos, i32 ypos) {
    game_state.window.x = xpos;
    game_state.window.y = ypos;
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

    if (game_state.initialize) {
        game_state.window.width = 800;
        game_state.window.height = 600;
        game_state.window.state = windowed;
    }

    window = glfwCreateWindow(game_state.window.width, game_state.window.height, "OpenGL Window", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Failed to create GLFW window\\n");
        glfwTerminate();
        exit(1);
    }
    if (game_state.initialize) {
        i32 x, y;
        glfwGetWindowPos(window, &x, &y);
        game_state.window.x = x;
        game_state.window.y = y;
    } else {
        glfwSetWindowPos(window, game_state.window.x, game_state.window.y);
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);  

    glfwSetKeyCallback(window, key_cb);
    glfwSetMouseButtonCallback(window, mouse_btn_cb);
    glfwSetCursorPosCallback(window, mouse_pos_cb);
    glfwSetWindowPosCallback(window, window_pos_cb); 
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
    return (f32) game_state.window.height / (f32) game_state.window.width;
}

bool window_is_cursor_visible(void) {
    return glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL;
}

void window_set_cursor_visible(bool visible) {
    glfwSetInputMode(window, GLFW_CURSOR, visible ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
}
