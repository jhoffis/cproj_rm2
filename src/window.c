#include "window.h"
#include "GLFW/glfw3.h"
#include <stdio.h>
#include <stdlib.h>

GLFWwindow *window;

static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void window_init(void) {

    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\\n");
        exit(1);
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(800, 600, "OpenGL Window", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Failed to create GLFW window\\n");
        glfwTerminate();
        exit(1);
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);  

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


