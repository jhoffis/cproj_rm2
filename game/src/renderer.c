#include "renderer.h"
#include <glad/glad.h> // hide opengl behind in here. In case we want to switch to vulkan later.
#include "game_state.h"
#include "window.h"
#include <stdio.h>
#include <stdlib.h>

void gfx_init_graphics(void) {
    // glfwGetProcAddress is specifically for opengl
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("Failed to initialize GLAD");
        exit(1);
    }    
    
    gfx_update_viewport();
    
    // Enable depth testing
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);  // Smaller depth values are drawn in front
    
    glEnable(GL_CULL_FACE);  
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);  

    if (game_state.render_method == 0) {
        game_state.render_method = GFX_TRIANGLES;
    }
}

void gfx_update_viewport(void) {
    glViewport(0, 0, window_width, window_height);
}

void gfx_clear_color(f32 r, f32 g, f32 b, f32 a) {
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // Clear both color and depth buffers
}

void gfx_swap(void) {
    glfwSwapBuffers(window);
}


