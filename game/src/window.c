#include "window.h"
#include "GLFW/glfw3.h"
#include "game_state.h"
#include "input_handler.h"
#include "renderer.h"
#include <stdio.h>
#include <stdlib.h>

GLFWwindow *window;
bool key_down = false;
i32 key_code = 0;
i32 key_scancode = 0;
i32 key_action = 0;
i32 key_mods = 0;

bool mouse_down = false;
i32 mouse_button = 0;
i32 mouse_action = 0;
i32 mouse_mods = 0;
f64 mouse_xpos = 0;
f64 mouse_ypos = 0;

static void framebuffer_size_callback(GLFWwindow* window, int w, int h) {
    game_state.window.width = w;
    game_state.window.height = h;
    gfx_update_viewport();
    resize_cb(w, h);
}

static void key_input_cb(GLFWwindow *window, i32 key, i32 scancode, i32 action, i32 mods) {
    key_code = key;
    key_scancode = scancode;
    key_action = action;
    key_mods = mods;
    key_cb(key, scancode, action, mods);
}

static void mouse_btn_cb(GLFWwindow *window, i32 button, i32 action, i32 mods) {
    mouse_button = button;
    mouse_action = action;
    mouse_mods = mods;
    mouse_down = action != GLFW_RELEASE;
    mouse_cb(mouse_xpos, mouse_ypos, button, action, mods);
}

static void mouse_pos_cb(GLFWwindow *window, f64 xpos, f64 ypos) {
    mouse_xpos = xpos;
    mouse_ypos = game_state.window.height - ypos;
    mouse_cb(xpos, mouse_ypos, mouse_button, -1, mouse_mods);
}

static void window_pos_cb(GLFWwindow *window, i32 xpos, i32 ypos) {
    game_state.window.x = xpos;
    game_state.window.y = ypos;
}


void window_init(void) {

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

    glfwSetKeyCallback(window, key_input_cb);
    glfwSetMouseButtonCallback(window, mouse_btn_cb);
    glfwSetCursorPosCallback(window, mouse_pos_cb);
    glfwSetWindowPosCallback(window, window_pos_cb); 

    window_set_cursor_visible(!game_state.free_cam);
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
