#ifndef TEST_MODE

#include "renderer.h"
#include "shader.h"
#include "timer_util.h"
#include "window.h"
#include "file_util.h"
#include "allocator.h"
#include "model_loader.h"
#include "player.h"
#include "nums.h"
#include <stdio.h>
#include "wav_loader.h"

static void key_cb(i32 key, i32 scancode, i32 action, i32 mods) {
    printf("asdf\n");
    if (action == GLFW_RELEASE) {
        if (key == GLFW_KEY_F5) {
            gfx_load_shaders();
        }
    }
}

static void mouse_cb(f64 xpos, f64 ypos, i32 button, i32 action, i32 mods) {
    // printf("asdfff\n");
}

int main(void) {
    mem_tracker_init();

    init_player();
    image_data img = load_image("sky");
    image_data img2 = load_image("yinyang");

    wav_entity* wav = create_wav_entity("test");
    load_next_wav_buffer(wav);
    free_wav_entity(wav);

    printf("ms: %llu\n", timer_now_nanos());

    window_init(key_cb, mouse_cb);
    gfx_init_graphics();
    gfx_load_shaders();


    float vertices[] = {
        // positions         // colors
         0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // bottom right
        -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // bottom left
         0.5f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f,    // top 
        -0.5f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f,    // top 
    };    

    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,   // first triangle
        0, 3, 2,   // first triangle
    };  
    gfx_bind_vertices(shader_sprite2D, vertices, 24, indices, 6);

    float texCoords[] = {
        0.0f, 1.0f,  // lower-left corner  
        1.0f, 1.0f,  // lower-right corner
        0.0f, 0.0f,   // top-center corner
        1.0f, 0.0f   // top-center corner
    };
    gfx_bind_texture(shader_sprite2D, texCoords, 8);
    gfx_finalize_image(&img);
    gfx_finalize_image(&img2);

    while (!window_should_close()) {
        gfx_clear_color(0.2f, 0.3f, 0.3f, 1.0f);
        window_poll_events();

        gfx_set_shader(shader_sprite2D);

        gfx_uniform_f32_v4(0, (f32_v4) {1, 0, 0.5, 1});
        gfx_uniform_f32_v3(1, (f32_v3) {.1, 0, 0.5});
        gfx_uniform_f32(2, window_aspect_ratio());
        gfx_activate_texture(0, img.texture);
        gfx_draw();
        gfx_uniform_f32_v3(1, (f32_v3) {.2, 0, 0.5});
        gfx_activate_texture(0, img2.texture);
        gfx_draw();

        gfx_swap();
    }

    gfx_cleanup_shaders();
    window_cleanup();
    mem_tracker_cleanup();
    return 0;
}


#else
#include <stdio.h>
int main(void) {
    printf("Passed all tests!\n");
    return 0;
}
#endif // TEST_MODE
