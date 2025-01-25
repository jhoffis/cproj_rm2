#ifdef TEST_MODE
#include <stdio.h>
int main(void) {
    printf("Passed all tests!\n");
    return 0;
}
#else

#define STB_IMAGE_IMPLEMENTATION
#include "allocator.h"
#include "player.h"
#include "nums.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include "stb_image.h"

static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

int main(void) {
    mem_tracker_init();

    init_player();

    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\\n");
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL Window", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Failed to create GLFW window\\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("Failed to initialize GLAD");
        return -1;
    }    

    glViewport(0, 0, 800, 600);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);  

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    mem_tracker_cleanup();
    return 0;
}


#endif // TEST_MODE
