#include "renderer.h"
#include <glad/glad.h> // hide opengl behind in here. In case we want to switch to vulkan later.
#include "window.h"
#include <stdio.h>
#include <stdlib.h>

void gfx_init_graphics(void) {
    // glfwGetProcAddress is specifically for opengl
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("Failed to initialize GLAD");
        exit(1);
    }    

    glViewport(0,0,800,600);
}

void gfx_clear_color(f32 r, f32 g, f32 b, f32 a) {
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT);
}

void gfx_swap(void) {
    glfwSwapBuffers(window);
}


